#include "Parser.h"

#include <fmt/format.h>

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
};

const std::map<Parser::ReservedNameTypes, std::string> Parser::_reservedWords
{
	{ ReservedNameTypes::FunctionDef, "def" },
	{ ReservedNameTypes::MainFunc, "main" },
	{ ReservedNameTypes::VariableDef, "var" },
	{ ReservedNameTypes::ReturnStatement, "return" },
};



ActionTree Parser::Parse(Lexer lexer)
{
	_errors.clear();
	
	ActionTree tree;
	std::deque<ActionNodeFactory*> factoryStack( {&tree.GlobalFactory()} );
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, _reservedWords.at(ReservedNameTypes::FunctionDef)))
			{
				lexer.Pop();
				if (!ParseFunction(lexer, factoryStack))
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "var"))
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
		else
		{
			break;
		}
	}
	factoryStack.pop_front();
	if (!factoryStack.empty())
		_errors.push_back("Scope nesting mismatch: expected 0, have " + std::to_string(factoryStack.size()));
	
	if (FunctionDeclarationNode* mainFunc = tree.GlobalFactory().FindFunction("main"))
		tree.SetRoot(mainFunc);
	else
		_errors.push_back("No main function in parsed code");
	
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

std::set<std::string> Parser::GetReservedWords()
{
	std::set<std::string> words;
	for (auto& kv: _reservedWords)
		words.emplace(kv.second);
	return words;
}

void Parser::DumpTokenError(const std::string& errFormat, const Token& token)
{
	_errors.push_back(fmt::format(fmt::runtime(errFormat), fmt::arg("name", token.string), fmt::arg("line", token.line), fmt::arg("column", token.column)));
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
		
		Token operation = lexer.Take();
		ActionNode* rhs = ParsePrimary(lexer, factories);
		
		if (!rhs)
			return nullptr;
		
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
		
		if (size && size != GetArraySize(val))
		{
			DumpTokenError("Array variable size is different from it's value ({line}: {column})", var);
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
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, _reservedWords.at(ReservedNameTypes::FunctionDef)))
			{
				lexer.Pop();
				if (!ParseFunction(lexer, factories))
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "return"))
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
			else if (StringUtils::Compare(lexer.Peek().string, "var"))
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
		else
		{
			break;
		}
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
		if (child && !IsArray(child))
			return factories.front()->Create<UnaryNode>(minus, child);
		DumpTokenError("Couldn't apply unary operator to array value at ({line}: {column})", minus);
	}
	
	return nullptr;
}

ActionNode* Parser::ParseWord(Lexer& lexer, std::deque<ActionNodeFactory*>& factories)
{
	if (GetReservedWords().contains(lexer.Peek().string))
	{
		DumpTokenError("Word \"{name}\" is reserved by language)", lexer.Peek());
		return nullptr;
	}
	
	Token name = lexer.Take();
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
	{
		lexer.Pop();
		std::vector<ActionNode*> args;
		while (lexer.Peek().type != Token::Type::ParenthesisClose)
		{
			ActionNode* arg = ParseExpression(lexer, factories);
			if (!arg)
				return nullptr;
			
			args.push_back(arg);
			if (lexer.Peek().type == Token::Type::ParenthesisClose)
				break;
			
			CheckToken(lexer.Take(), Token::Type::Comma);
		}
		lexer.Pop();
		for (auto i = factories.rbegin(); i != factories.rend(); ++i)
		{
			if (FunctionDeclarationNode* funcDecl = (*i)->FindFunction(name.string))
				return factories.front()->Create<FunctionCallNode>(funcDecl, args);
		}
		DumpTokenError("Function \"{name}\" not found ({line}: {column})", name);
		return nullptr;
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
			if (var && var->Type() == VariableType::Array)
				return factories.front()->Create<ArrayGetterNode>(var, node);
		}
		DumpTokenError("Array variable \"{name}\" not found ({line}: {column})", name);
		return nullptr;
	}
	
	for (auto i = factories.rbegin(); i != factories.rend(); ++i)
	{
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
	std::vector<ActionNode*> components;
	while (lexer.Peek().type != Token::Type::BraceClose)
	{
		ActionNode* node = ParseExpression(lexer, factories);
		if (!node)
			return nullptr;

		if (lexer.Peek().type != Token::Type::BraceClose)
			CheckToken(lexer.Take(), Token::Type::Comma);
		
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
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, _reservedWords.at(ReservedNameTypes::FunctionDef)))
			{
				lexer.Pop();
				if (!ParseFunction(lexer, factories))
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "return", false))
			{
				lexer.Pop();
				if (ActionNode* result = ParseExpression(lexer, factories))
					return result;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "var", false))
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
		else
		{
			break;
		}
	}
	
	return nullptr;
}
}
