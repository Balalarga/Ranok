#include "Parser.h"

#include <fmt/format.h>

#include "HardcodedConstructions.h"
#include "LibraryStorage.h"
#include "Utils/StringUtils.h"

namespace Ranok
{
const std::map<Token::Type, int> Parser::_operationPriorities
{
	{ Token::Type::Pipe, 1 },
	{ Token::Type::Ampersand, 1 },
	{ Token::Type::Plus, 2 },
	{ Token::Type::Minus, 2 },
	{ Token::Type::Star, 4 },
	{ Token::Type::Slash, 4 },
	{ Token::Type::Ampersand, 5 },
	{ Token::Type::Pipe, 5 },
	{ Token::Type::Hat, 8 },
};

const std::map<Parser::ReservedKeywordsTypes, std::string> Parser::_reservedKeywords
{
	{ ReservedKeywordsTypes::FunctionDef, "def" },
	{ ReservedKeywordsTypes::VariableDef, "var" },
	{ ReservedKeywordsTypes::ReturnStatement, "return" },
};
const std::map<Parser::ReservedFuncsTypes, std::string> Parser::_reservedFuncs
{
	{ ReservedFuncsTypes::MainFunc, "main" },
};

Parser::Parser()
{
	for (const auto& [name, lib] : LibraryStorage::GetLibs())
		AddGlobalData(lib);
}

ActionTree Parser::Parse(Lexer lexer)
{
	_errors.clear();
	
	ActionTree tree;
	tree.GlobalFactory() += _globalFactory;
	std::deque<ActionNodeFactory*> factoryStack( {&tree.GlobalFactory()} );
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::FunctionDef)))
		{
			lexer.Pop();
			if (!ParseFunction(lexer, factoryStack))
				break;
		}
		else if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::VariableDef)))
		{
			lexer.Pop();
			if (!ParseVariableDeclaration(lexer, factoryStack))
				break;
		}
		else
		{
			if (!ParseVariableDeclaration(lexer, factoryStack))
				break;
		}
		
		if (lexer.Peek().type == Token::Type::Semicolon)
			lexer.Pop();
	}
	factoryStack.pop_front();
	if (!factoryStack.empty())
		_errors.push_back({0, 0, "Scope nesting mismatch: expected 0, have " + std::to_string(factoryStack.size())});
	
	if (FunctionDeclarationNode* mainFunc = tree.GlobalFactory().FindFunction(_reservedFuncs.at(ReservedFuncsTypes::MainFunc)))
		tree.SetRoot(mainFunc);
	
	return tree;
}

int Parser::GetOperationPriority(Token::Type type)
{
	auto it = _operationPriorities.find(type);
	if (it == _operationPriorities.end())
		return -1;
	
	return it->second;
}

bool Parser::CheckToken(const Token& token, Token::Type expected)
{
	if (token.type == expected)
		return true;

	std::string descr = Token::TypeString(expected);
	for (auto& kv : Lexer::SymbolTypes)
	{
		if (kv.second == expected)
		{
			if (kv.first == '{')
				descr = "{{";
			else if (kv.first == '}')
				descr = "}}";
			else
				descr = kv.first;
			break;
		}
	}
	
	DumpTokenError("Expected \"" + descr + "\", but have \"{name}\" ({line}: {column})", token);
	return false;
}

std::set<std::string> Parser::GetReservedKeywords()
{
	std::set<std::string> words;
	for (auto& kv: _reservedKeywords)
		words.emplace(kv.second);
	return words;
}

std::set<std::string> Parser::GetReservedFuncwords()
{
	std::set<std::string> words;
	for (auto& kv: _reservedFuncs)
		words.emplace(kv.second);
	return words;
}

void Parser::DumpTokenError(const std::string& errFormat, const Token& token)
{
	_errors.push_back({token.line, token.column, fmt::format(fmt::runtime(errFormat), fmt::arg("name", token.string), fmt::arg("line", token.line), fmt::arg("column", token.column))});
}

void Parser::AddGlobalData(const ActionNodeFactory& factory)
{
	_globalFactory += factory;
}

ActionNode* Parser::ParseExpression(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	if (ActionNode* node = ParsePrimary(lexer, factories))
		return ParseBinary(node, lexer, factories);
	return nullptr;
}

ActionNode* Parser::ParseBinary(ActionNode* lhs, Lexer& lexer, std::deque<ActionNodeFactory*>& factories, int priority)
{
	while (true)
	{
		int currPriority = GetOperationPriority(lexer.Peek().type);
		if (currPriority < priority)
			return lhs;
		
		if (ActionNode::IsArray(lhs))
		{
			DumpTokenError("Array operations not supported ({line}: {column})\n"
				"You must make some array variable and work with it's components", lexer.Peek());
			return nullptr;
		}
	
		Token operation = lexer.Take();
		ActionNode* rhs = ParsePrimary(lexer, factories);
		
		if (!rhs)
		{
			DumpTokenError("Couldn't get operand ({line}: {column})", lexer.Peek());
			return nullptr;
		}
		
		if (ActionNode::IsArray(rhs))
		{
			DumpTokenError("Array operations not supported ({line}: {column})\n"
				"You must make some array variable and work with it's components", lexer.Peek());
			return nullptr;
		}
		
		int newOperatorPriority = GetOperationPriority(lexer.Peek().type);
		if (currPriority < newOperatorPriority)
		{
			rhs = ParseBinary(rhs, lexer, factories, newOperatorPriority);
			if (!rhs)
				return nullptr;
		}
		
		lhs = factories.front()->Create<BinaryNode>(operation, lhs, rhs);
	}
}

VariableDeclarationNode* Parser::ParseVariableDeclaration(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	Token var = lexer.Take();
	if (Hardcoded::Get().VariableDeclarations.contains(var.string) ||
		Hardcoded::Get().FunctionDeclarations.contains(var.string))
	{
		DumpTokenError("Word \"{name}\" is reserved by language)", var);
		return nullptr;
	}
	
	if (lexer.Peek().type == Token::Type::BracketOpen)
	{
		size_t size = 0;
		lexer.Pop();
		if (lexer.Peek().type == Token::Type::Number)
		{
			ActionNode* val = ParseExpression(lexer, factories);
			DoubleNumberNode* num = val ? dynamic_cast<DoubleNumberNode*>(val) : nullptr;
			if (!num)
				return nullptr;
			size = static_cast<size_t>(num->Value());
		}
		CheckToken(lexer.Take(), Token::Type::BracketClose);
		
		if (!CheckToken(lexer.Peek(), Token::Type::Assign))
			return nullptr;
		lexer.Pop();
		
		ActionNode* val = ParseExpression(lexer, factories);
		if (!val)
			return nullptr;
		size_t valSize = ActionNode::GetArraySize(val);
		if (size && size != valSize)
		{
			DumpTokenError("Array variable \"{name}\" ({line}: {column}) size error: have "+std::to_string(size)+", expected: "+std::to_string(valSize), var);
			return nullptr;
		}
		
		return factories.front()->CreateVariable(var, val);
	}

	if (!CheckToken(lexer.Peek(), Token::Type::Assign))
		return nullptr;
	lexer.Pop();

	ActionNode* val = ParseExpression(lexer, factories);
	if (!val)
		return nullptr;
	
	return factories.front()->CreateVariable(var, val);
}

FunctionDeclarationNode* Parser::ParseFunction(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	if (Hardcoded::Get().VariableDeclarations.contains(lexer.Peek().string) ||
		Hardcoded::Get().FunctionDeclarations.contains(lexer.Peek().string))
	{
		DumpTokenError("Word \"{name}\" is reserved by language)", lexer.Peek());
		return nullptr;
	}
	
	if (!bAllowInnerFunctionDeclarations && factories.size() > 1)
	{
		DumpTokenError("Nested functions not supported for a while ({line}: {column})", lexer.Peek());
		return nullptr;
	}
	
	if (lexer.Peek().type != Token::Type::Word)
		return nullptr;
	
	Token name = lexer.Take();
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisOpen))
		return nullptr;
	
	auto func = factories.front()->TempCreateFunction(FunctionSignature(name));
	ActionNodeFactory& funcFactory = func->Get()->Factory();
	std::vector<ActionNode*> args;
	while (lexer.Peek().type == Token::Type::Word)
	{
		Token argName = lexer.Take();
		if (lexer.Peek().type == Token::Type::Comma)
		{
			func->Get()->Signature().Args().push_back(funcFactory.CreateVariable(argName, funcFactory.Create<DoubleNumberNode>(argName, 0)));
			lexer.Pop();
		}
		else if(lexer.Peek().type == Token::Type::ParenthesisClose)
		{
			func->Get()->Signature().Args().push_back(funcFactory.CreateVariable(argName, funcFactory.Create<DoubleNumberNode>(argName, 0)));
		}
		else if (lexer.Peek().type == Token::Type::BracketOpen)
		{
			lexer.Pop();
			if (!CheckToken(lexer.Peek(), Token::Type::Number))
				return nullptr;
			
			int arrSize = std::stoi(lexer.Take().string);
			if (arrSize <= 0)
				return nullptr;
			
			if (!CheckToken(lexer.Take(), Token::Type::BracketClose))
				return nullptr;
			
			func->Get()->Signature().Args().push_back(funcFactory.CreateVariable(argName, funcFactory.Create<ArrayNode>(argName, std::vector<ActionNode*>(arrSize, nullptr))));
		}
		if (lexer.Peek().type == Token::Type::Comma)
			lexer.Pop();
	}
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisClose) || !CheckToken(lexer.Take(), Token::Type::BraceOpen))
		return nullptr;
	
	factories.push_front(&funcFactory);
	
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::FunctionDef)))
		{
			lexer.Pop();
			if (!ParseFunction(lexer, factories))
				break;
		}
		else if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::ReturnStatement)))
		{
			lexer.Pop();
			if (ActionNode* result = ParseExpression(lexer, factories))
			{
				func->Get()->SetBody(result);
				while (lexer.Peek().type == Token::Type::Semicolon)
					lexer.Pop();
				if (CheckToken(lexer.Peek(), Token::Type::BraceClose))
				{
					lexer.Pop();
					break;
				}
			}
			else
			{
				break;
			}
		}
		else if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::VariableDef)))
		{
			lexer.Pop();
			if (!ParseVariableDeclaration(lexer, factories))
				break;
		}
		else
		{
			if (!ParseVariableDeclaration(lexer, factories))
				break;
		}
		
		while (lexer.Peek().type == Token::Type::Semicolon)
			lexer.Pop();
	}
	factories.pop_front();
	
	if (!func->Get()->Body())
		return nullptr;
	
	return func->Commit();
}

ActionNode* Parser::ParsePrimary(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{	
	if (lexer.Peek().type == Token::Type::Word)
		return ParseWord(lexer, factories);
	
	if (lexer.Peek().type == Token::Type::Number || lexer.Peek().type == Token::Type::Dot)
		return ParseNumber(lexer, factories);
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
		return ParseParentheses(lexer, factories);
	
	if (lexer.Peek().type == Token::Type::BraceOpen)
		return ParseArrayValues(lexer, factories);

	if (lexer.Peek().type == Token::Type::Minus)
	{
		Token minus = lexer.Take();
		ActionNode* child = ParsePrimary(lexer, factories);
		if (child && !ActionNode::IsArray(child))
			return factories.front()->Create<UnaryNode>(minus, child);
		DumpTokenError("Couldn't apply unary operator to array value at ({line}: {column})", minus);
	}
	
	return nullptr;
}

ActionNode* Parser::ParseWord(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	if (GetReservedKeywords().contains(lexer.Peek().string))
	{
		DumpTokenError("Word \"{name}\" is reserved by language)", lexer.Peek());
		return nullptr;
	}
	
	Token name = lexer.Take();
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
	{
		lexer.Pop();
		FunctionDeclarationNode* funcDecl{};
		for (auto i = factories.rbegin(); i != factories.rend(); ++i)
		{
			funcDecl = (*i)->FindFunction(name.string);
			if (funcDecl)
				break;
		}
		
		if (!funcDecl)
		{
			auto hardcodedFunc = Hardcoded::Get().FunctionDeclarations.find(name.string);
			if (hardcodedFunc != Hardcoded::Get().FunctionDeclarations.end())
			{
				funcDecl = hardcodedFunc->second;
			}
			else
			{
				DumpTokenError("Couldn't find function \"{name}\" declaration ({line}: {column})", name);
				return nullptr;
			}
		}
		
		std::vector<ActionNode*> args;
		while (lexer.Peek().type != Token::Type::ParenthesisClose)
		{
			ActionNode* arg = ParseExpression(lexer, factories);
			if (!arg)
				return nullptr;
			
			if (args.size() == funcDecl->Signature().Args().size())
			{
				DumpTokenError("Function \"{name}\" arguments size miss match ({line}: {column})", name);
				return nullptr;
			}
			bool sigArgIsArr = ActionNode::IsArray(funcDecl->Signature().Args()[args.size()]);
			bool argIsArr = ActionNode::IsArray(arg);
			if (sigArgIsArr != argIsArr)
			{
				DumpTokenError("Function \"{name}\" arguments type miss match ({line}: {column})", name);
				return nullptr;
			}
			
			args.push_back(arg);
			if (lexer.Peek().type == Token::Type::ParenthesisClose)
				break;
			
			CheckToken(lexer.Take(), Token::Type::Comma);
		}
		if (args.size() != funcDecl->Signature().Args().size())
		{
			DumpTokenError("Function \"{name}\" arguments size miss match ({line}: {column})", name);
			return nullptr;
		}
		lexer.Pop();

		return factories.front()->Create<FunctionCallNode>(funcDecl, args);
	}
	
	if (lexer.Peek().type == Token::Type::BracketOpen)
	{
		lexer.Pop();
		auto node = ParseExpression(lexer, factories);
		
		if (!node)
			return nullptr;
		
		CheckToken(lexer.Take(), Token::Type::BracketClose);
		
		for (auto i = factories.rbegin(); i != factories.rend(); ++i)
		{
			VariableDeclarationNode* var = (*i)->FindVariable(name.string);
			if (var && ActionNode::IsArray(var->Value()))
				return factories.front()->Create<ArrayGetterNode>(var, node);
		}
		
		auto hardcodedVar = Hardcoded::Get().VariableDeclarations.find(name.string);
		if (hardcodedVar != Hardcoded::Get().VariableDeclarations.end())
		{
			if (ActionNode::IsArray(hardcodedVar->second->Value()))
				return hardcodedVar->second->Value();
		}
		
		DumpTokenError("Array variable \"{name}\" not found ({line}: {column})", name);
		return nullptr;
	}
	
	for (auto i = factories.rbegin(); i != factories.rend(); ++i)
	{
		auto hardcodedVar = Hardcoded::Get().VariableDeclarations.find(name.string);
		if (hardcodedVar != Hardcoded::Get().VariableDeclarations.end())
			return hardcodedVar->second->Value();
		
		if (VariableDeclarationNode* varDecl = (*i)->FindVariable(name.string))
			return factories.front()->Create<VariableNode>(varDecl);
	}
	
	DumpTokenError("Variable \"{name}\" not found ({line}: {column})", name);
	return nullptr;
}

ActionNode* Parser::ParseParentheses(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	lexer.Pop();
	Token top = lexer.Peek();
	ActionNode* node = ParseExpression(lexer, factories);
	if (!node)
		return nullptr;
	
	CheckToken(lexer.Take(), Token::Type::ParenthesisClose);
	return node;
}

ActionNode* Parser::ParseArrayValues(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	lexer.Pop();
	Token top = lexer.Peek();
	Token lastArg = top;
	std::vector<ActionNode*> components;
	while (lexer.Peek().type != Token::Type::BraceClose)
	{
		ActionNode* node = ParseExpression(lexer, factories);
		if (!node)
			return nullptr;
		
		if (ActionNode::IsArray(node))
		{
			DumpTokenError("Array nesting is not supported: \"{name}\" ({line}: {column})", lastArg);
			return nullptr;
		}

		if (lexer.Peek().type != Token::Type::BraceClose)
			CheckToken(lexer.Take(), Token::Type::Comma);
		lastArg = lexer.Peek();
		components.push_back(node);
	}
	lexer.Pop();
	top.string = _unnamePrefix + std::to_string(++_unnamedCounter);
	return factories.front()->Create<ArrayNode>(top, components);
}

ActionNode* Parser::ParseNumber(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	Token number = lexer.Take();
	
	if(number.type == Token::Type::Dot)
	{
		if (lexer.Peek().type == Token::Type::Number)
		{
			Token num = lexer.Take();
			return factories.front()->Create<DoubleNumberNode>(num, std::stod("0." + num.string));
		}
		return nullptr;
	}
	
	if (lexer.Peek().type == Token::Type::Dot)
	{
		lexer.Pop();
		if (lexer.Peek().type == Token::Type::Number)
		{
			Token floatPart = lexer.Take();
			return factories.front()->Create<DoubleNumberNode>(number, std::stod(number.string + '.' + floatPart.string));
		}
		return factories.front()->Create<DoubleNumberNode>(number, std::stod(number.string));
	}
	
	return factories.front()->Create<DoubleNumberNode>(number, std::stoi(number.string));
}

ActionNode* Parser::ParseBody(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::FunctionDef)))
		{
			lexer.Pop();
			if (!ParseFunction(lexer, factories))
				break;
		}
		else if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::ReturnStatement), false))
		{
			lexer.Pop();
			if (ActionNode* result = ParseExpression(lexer, factories))
				return result;
		}
		else if (StringUtils::Compare(lexer.Peek().string, _reservedKeywords.at(ReservedKeywordsTypes::VariableDef), false))
		{
			lexer.Pop();
			if (!ParseVariableDeclaration(lexer, factories))
				break;
		}
		else
		{
			if (!ParseVariableDeclaration(lexer, factories))
				break;
		}
		
		if (lexer.Peek().type == Token::Type::Semicolon)
			lexer.Pop();
	}
	
	return nullptr;
}
}
