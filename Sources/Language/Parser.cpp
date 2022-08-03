#include "Parser.h"
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


ActionTree Parser::Parse(Lexer lexer)
{
	_errors.clear();
	
	ActionTree tree;
	std::stack<ActionNodeFactory*> factoryStack( {&tree.GlobalFactory()} );
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, "def"))
			{
				lexer.Pop();
				if (FunctionDeclarationNode* func = ParseFunction(lexer, factoryStack))
					tree.AddGlobalFunction(func);
				else
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "var"))
			{
				lexer.Pop();
				if (VariableDeclarationNode* var = ParseVariableDeclaration(lexer, factoryStack))
					tree.AddGlobalVariable(var);
				else
					break;
			}
			else
			{
				if (VariableDeclarationNode* var = ParseVariableDeclaration(lexer, factoryStack))
					tree.AddGlobalVariable(var);
				else
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
	factoryStack.pop();
	if (!factoryStack.empty())
		_errors.push_back("Scope nesting mismatch: expected 0, have " + std::to_string(factoryStack.size()));
	
	if (FunctionDeclarationNode* mainFunc = tree.GetGlobalFunction("main"))
		tree.SetRoot(mainFunc);
	else
		_errors.push_back(DumpToken(lexer.Peek()));
	
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
	
	_errors.push_back("Expected " + Token::TypeString(expected) + ", but have " + DumpToken(token));
	return false;
}

std::string Parser::DumpToken(const Token& token)
{
	return token.string+" (" + std::to_string(token.line) + ": " + std::to_string(token.column) + ")";
}

ActionNode* Parser::ParseExpression(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	if (ActionNode* node = ParsePrimary(lexer, factories))
		return ParseBinary(node, lexer, factories);
	return nullptr;
}

ActionNode* Parser::ParseBinary(ActionNode* lhs, Lexer& lexer, std::stack<ActionNodeFactory*>& factories, int priority)
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
		
		lhs = factories.top()->Create<BinaryNode>(operation.string, lhs, rhs);
	}
}

VariableDeclarationNode* Parser::ParseVariableDeclaration(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	Token var = lexer.Take();
	if (!CheckToken(lexer.Peek(), Token::Type::Assign))
		return nullptr;
	
	lexer.Pop();
	ActionNode* val = ParseExpression(lexer, factories);
	if (!val)
		return nullptr;
	
	return factories.top()->CreateVariable(var.string, val);
}

FunctionDeclarationNode* Parser::ParseFunction(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	if (lexer.Peek().type != Token::Type::Word)
		return nullptr;
	
	Token name = lexer.Take();
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisOpen))
		return nullptr;
	
	auto func = factories.top()->TempCreateFunction(FunctionSignature(name.string));
	ActionNodeFactory& funcFactory = func->Get()->Factory();
	std::vector<ActionNode*> args;
	while (lexer.Peek().type == Token::Type::Word)
	{
		Token argName = lexer.Take();
		if (lexer.Peek().type == Token::Type::Comma)
		{
			func->Get()->Signature().Args().push_back(funcFactory.CreateVariable(argName.string, funcFactory.Create<DoubleNumberNode>(0)));
			lexer.Pop();
		}
		else if(lexer.Peek().type == Token::Type::ParenthesisClose)
		{
			func->Get()->Signature().Args().push_back(funcFactory.CreateVariable(argName.string, funcFactory.Create<DoubleNumberNode>(0)));
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
			
			func->Get()->Signature().Args().push_back(funcFactory.CreateVariable(argName.string, funcFactory.Create<ArrayNode>(argName.string, std::vector<ActionNode*>(arrSize, nullptr))));
		}
	}
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisClose) || !CheckToken(lexer.Take(), Token::Type::BraceOpen))
		return nullptr;
	
	factories.push(&funcFactory);
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, "def"))
			{
				lexer.Pop();
				if (!ParseFunction(lexer, factories))
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "return"))
			{
				lexer.Pop();
				if (ActionNode* result = ParseExpression(lexer, factories))
					func->Get()->SetBody(result);
				else
					break;
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
			
			if (lexer.Peek().type == Token::Type::BraceClose)
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
	factories.pop();
	
	if (!func->Get()->Body())
	{
		_errors.push_back("");
		return nullptr;
	}
	
	return func->Commit();
}

ActionNode* Parser::ParsePrimary(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	if (lexer.Peek().type == Token::Type::Word)
		return ParseWord(lexer, factories);
	
	if (lexer.Peek().type == Token::Type::Number)
		return ParseDoubleNumber(lexer, factories);
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
		return ParseParentheses(lexer, factories);
	
	return nullptr;
}

ActionNode* Parser::ParseWord(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
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
		std::deque<ActionNodeFactory*> factoriesRaw = factories._Get_container();
		for (auto i = factoriesRaw.rbegin(); i != factoriesRaw.rend(); ++i)
		{
			if (FunctionDeclarationNode* funcDecl = (*i)->FindFunction(name.string))
				return factories.top()->Create<FunctionCallNode>(funcDecl, args);
		}
		_errors.push_back("Function "+ name.string +" not found");
		return nullptr;
	}
	
	if (lexer.Peek().type == Token::Type::BracketOpen)
	{
		lexer.Pop();
		auto node = ParseExpression(lexer, factories);
		
		if (!node)
			return nullptr;
		
		CheckToken(lexer.Take(), Token::Type::BracketClose);
		
		std::deque<ActionNodeFactory*> factoriesRaw = factories._Get_container();
		for (auto i = factoriesRaw.rbegin(); i != factoriesRaw.rend(); ++i)
		{
			VariableDeclarationNode* var = (*i)->FindVariable(name.string);
			if (var && var->Type() == VariableType::Array)
				return factories.top()->Create<ArrayGetterNode>(var, node);
		}
		_errors.push_back("Array variable "+ name.string +" not found");
		return nullptr;
	}
	
	std::deque<ActionNodeFactory*> factoriesRaw = factories._Get_container();
	for (auto i = factoriesRaw.rbegin(); i != factoriesRaw.rend(); ++i)
	{
		if (VariableDeclarationNode* varDecl = (*i)->FindVariable(name.string))
			return factories.top()->Create<VariableNode>(varDecl);
	}
	
	_errors.push_back("Variable "+ name.string +" not found");
	return nullptr;
}

ActionNode* Parser::ParseParentheses(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	lexer.Pop();
	Token top = lexer.Peek();
	ActionNode* node = ParseExpression(lexer, factories);
	if (!node)
		return nullptr;
	
	CheckToken(lexer.Take(), Token::Type::ParenthesisClose);
	
	return node;
}

ActionNode* Parser::ParseIntNumber(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	return factories.top()->Create<IntNumberNode>(std::stoi(lexer.Take().string));
}

ActionNode* Parser::ParseDoubleNumber(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	return factories.top()->Create<DoubleNumberNode>(std::stod(lexer.Take().string));
}

ActionNode* Parser::ParseBody(Lexer& lexer, std::stack<ActionNodeFactory*>& factories)
{
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, "def"))
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