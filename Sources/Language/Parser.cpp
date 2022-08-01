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
	
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, "def"))
			{
				lexer.Pop();
				if (FunctionDeclarationNode* func = ParseFunction(lexer, tree.GlobalFactory()))
					tree.AddGlobalFunction(func);
				else
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "return"))
			{
				lexer.Pop();
				if (ActionNode* result = ParseExpression(lexer, tree.GlobalFactory()))
					tree.SetRoot(result);
				else
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "var"))
			{
				lexer.Pop();
				if (VariableDeclarationNode* var = ParseVariableDeclaration(lexer, tree.GlobalFactory()))
					tree.AddGlobalVariable(var);
				else
					break;
			} 
			else
			{
				if (VariableDeclarationNode* var = ParseVariableDeclaration(lexer, tree.GlobalFactory()))
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
	
	_errors.push_back("Expected " + Token::TypeString(expected) + ", but have "
		+ token.string+" (" + std::to_string(token.line) + ": "
		+ std::to_string(token.column) + ")");
	return false;
}

ActionNode* Parser::ParseExpression(Lexer& lexer, ActionNodeFactory& factory)
{
	if (ActionNode* node = ParsePrimary(lexer, factory))
		return ParseBinary(node, lexer, factory);
	return nullptr;
}

ActionNode* Parser::ParseBinary(ActionNode* lhs, Lexer& lexer, ActionNodeFactory& factory, int priority)
{
	while (true)
	{
		int currPriority = GetOperationPriority(lexer.Peek().type);
		if (currPriority < priority)
			return lhs;
		
		Token operation = lexer.Take();
		ActionNode* rhs = ParsePrimary(lexer, factory);
		
		if (!rhs)
			return nullptr;
		
		int newOperatorPriority = GetOperationPriority(lexer.Peek().type);
		if (currPriority < newOperatorPriority)
		{
			rhs = ParseBinary(rhs, lexer, factory, newOperatorPriority);
			if (!rhs)
				return nullptr;
		}
		
		lhs = factory.Create<BinaryNode>(operation.string, lhs, rhs);
	}
}

VariableDeclarationNode* Parser::ParseVariableDeclaration(Lexer& lexer, ActionNodeFactory& factory)
{
	Token var = lexer.Take();
	if (!CheckToken(lexer.Peek(), Token::Type::Assign))
		return nullptr;
	
	lexer.Pop();
	ActionNode* val = ParseExpression(lexer, factory);
	if (!val)
		return nullptr;
	
	return factory.CreateVariable(var.string, val);
}

std::optional<FunctionSignature> Parser::ParseFunctionSignature(Lexer& lexer)
{
	if (lexer.Peek().type != Token::Type::Word)
		return {};

	Token name = lexer.Take();
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisOpen))
		return {};
	
	std::vector<std::string> args;
	while (lexer.Peek().type == Token::Type::Word)
	{
		args.push_back(lexer.Take().string);
		if (lexer.Peek().type == Token::Type::Comma)
			lexer.Pop();
	}
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisClose))
		return {};
	
	return FunctionSignature{name.string, args};
}

FunctionDeclarationNode* Parser::ParseFunction(Lexer& lexer, ActionNodeFactory& factory)
{
	std::optional<FunctionSignature> signature = ParseFunctionSignature(lexer);
	if (!signature || !CheckToken(lexer.Take(), Token::Type::BraceOpen))
		return nullptr;

	FunctionDeclarationNode* func = factory.CreateFunction(signature.value());
	ActionNode* body = ParseExpression(lexer, func->Factory());
	if (!body || !CheckToken(lexer.Take(), Token::Type::BraceClose))
	{
		
		return nullptr;	
	}
	
	return 
}

ActionNode* Parser::ParsePrimary(Lexer& lexer, ActionNodeFactory& factory)
{
	if (lexer.Peek().type == Token::Type::Word)
		return ParseWord(lexer, factory);
	
	if (lexer.Peek().type == Token::Type::IntNumber)
		return ParseIntNumber(lexer, factory);
	
	if (lexer.Peek().type == Token::Type::DoubleNumber)
		return ParseDoubleNumber(lexer, factory);
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
		return ParseParentheses(lexer, factory);
	
	return nullptr;
}

ActionNode* Parser::ParseWord(Lexer& lexer, ActionNodeFactory& factory)
{
	Token name = lexer.Take();
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
	{
		lexer.Pop();
		std::vector<ActionNode*> args;
		while (lexer.Peek().type != Token::Type::ParenthesisClose)
		{
			ActionNode* arg = ParseExpression(lexer, factory);
			if (!arg)
				return nullptr;
			
			args.push_back(arg);
			if (lexer.Peek().type == Token::Type::ParenthesisClose)
				break;

			CheckToken(lexer.Take(), Token::Type::Comma);
		}
		lexer.Pop();
		return factory.Create<FunctionCallNode>(name.string, args);
	}
	
	if (lexer.Peek().type == Token::Type::BracketOpen)
	{
		lexer.Pop();
		auto node = ParseExpression(lexer, factory);
		
		if (!node)
			return nullptr;
		
		CheckToken(lexer.Take(), Token::Type::BracketClose);
		
		return factory.Create<ArrayGetterNode>(name.string, node);
	}
	
	if (VariableDeclarationNode* var = factory.FindVariable(name.string))
		return factory.Create<VariableNode>(name.string, var);
	
	return nullptr;
}

ActionNode* Parser::ParseParentheses(Lexer& lexer, ActionNodeFactory& factory)
{
	lexer.Pop();
	Token top = lexer.Peek();
	ActionNode* node = ParseExpression(lexer, factory);
	if (!node)
		return nullptr;
	
	CheckToken(lexer.Take(), Token::Type::ParenthesisClose);
	
	return node;
}

ActionNode* Parser::ParseIntNumber(Lexer& lexer, ActionNodeFactory& factory)
{
	return factory.Create<IntNumberNode>(std::stoi(lexer.Take().string));
}

ActionNode* Parser::ParseDoubleNumber(Lexer& lexer, ActionNodeFactory& factory)
{
	return factory.Create<DoubleNumberNode>(std::stod(lexer.Take().string));
}

ActionNode* Parser::ParseBody(Lexer& lexer, ActionNodeFactory& factory)
{
	while (!lexer.IsEmpty() && lexer.Peek().type != Token::Type::EndFile)
	{
		if (CheckToken(lexer.Peek(), Token::Type::Word))
		{
			if (StringUtils::Compare(lexer.Peek().string, "def"))
			{
				lexer.Pop();
				if (!ParseFunction(lexer, factory))
					break;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "return", false))
			{
				lexer.Pop();
				if (ActionNode* result = ParseExpression(lexer, factory))
					return result;
			}
			else if (StringUtils::Compare(lexer.Peek().string, "var", false))
			{
				lexer.Pop();
				if (!ParseVariableDeclaration(lexer, factory))
					break;
			}
			else
			{
				if (!ParseVariableDeclaration(lexer, factory))
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