#include "Parser.h"

std::map<Token::Type, int> Parser::_OperationPriorities
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

	auto insensitiveCompare = [](const std::string& a, const std::string& b)
	{
		return std::ranges::equal(a, b, [](char a, char b) { return tolower(a) == tolower(b); });
	};

	while (!lexer.IsEmpty())
	{
		if (lexer.Peek().type == Token::Type::Word)
		{
			if (insensitiveCompare(lexer.Peek().string, "def"))
			{
				ParseFunction(lexer, tree);
			}
			else if ()
		}
	}


	
	tree.SetRoot(ParseExpression(lexer, tree));
	
	return tree;
}

int Parser::GetOperationPriority(Token::Type type)
{
	auto it = _OperationPriorities.find(type);
	if (it == _OperationPriorities.end())
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

ActionNode* Parser::ParseExpression(Lexer& lexer, ActionTree& tree)
{
	if (ActionNode* node = ParsePrimary(lexer, tree))
		return ParseBinary(node, lexer, tree);
	return nullptr;
}

ActionNode* Parser::ParseBinary(ActionNode* lhs, Lexer& lexer, ActionTree& tree, int priority)
{
	while (true)
	{
		int currPriority = GetOperationPriority(lexer.Peek().type);
		if (currPriority < priority)
			return lhs;
		
		Token operation = lexer.Take();
		ActionNode* rhs = ParsePrimary(lexer, tree);
		
		if (!rhs)
			return nullptr;
		
		int newOperatorPriority = GetOperationPriority(lexer.Peek().type);
		if (currPriority < newOperatorPriority)
		{
			rhs = ParseBinary(rhs, lexer, tree, newOperatorPriority);
			if (!rhs)
				return nullptr;
		}
		
		lhs = tree.Create<BinaryNode>(operation.string, lhs, rhs);
	}
}

std::optional<FunctionSignature> Parser::ParseFunctionSignature(Lexer& lexer, ActionTree& tree)
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
		if (!CheckToken(lexer.Take(), Token::Type::Comma))
			return {};
	}
	if (!CheckToken(lexer.Take(), Token::Type::ParenthesisClose))
		return {};
	
	return FunctionSignature{name.string, args};
}

FunctionDeclarationNode* Parser::ParseFunction(Lexer& lexer, ActionTree& tree)
{
	lexer.Pop();
	if (!CheckToken(lexer.Take(), Token::Type::Word))
		return nullptr;
	
	std::optional<FunctionSignature> signature = ParseFunctionSignature(lexer, tree);
	if (!signature || !CheckToken(lexer.Take(), Token::Type::BraceOpen))
		return nullptr;

	ActionNode* body = ParseExpression(lexer, tree);
	if (!body || !CheckToken(lexer.Take(), Token::Type::BraceClose))
		return nullptr;
	
	return tree.Create<FunctionDeclarationNode>(signature.value(), body);
}

ActionNode* Parser::ParsePrimary(Lexer& lexer, ActionTree& tree)
{
	if (lexer.Peek().type == Token::Type::Word)
		return ParseWord(lexer, tree);
	
	if (lexer.Peek().type == Token::Type::IntNumber)
		return ParseIntNumber(lexer, tree);
	
	if (lexer.Peek().type == Token::Type::DoubleNumber)
		return ParseDoubleNumber(lexer, tree);
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
		return ParseParentheses(lexer, tree);
	
	return nullptr;
}

ActionNode* Parser::ParseWord(Lexer& lexer, ActionTree& tree)
{
	Token name = lexer.Take();
	
	if (lexer.Peek().type == Token::Type::ParenthesisOpen)
	{
		lexer.Pop();
		std::vector<ActionNode*> args;
		while (lexer.Peek().type != Token::Type::ParenthesisClose)
		{
			ActionNode* arg = ParseExpression(lexer, tree);
			if (!arg)
				return nullptr;
			
			args.push_back(arg);
			if (lexer.Peek().type == Token::Type::ParenthesisClose)
				break;

			CheckToken(lexer.Take(), Token::Type::Comma);
		}
		lexer.Pop();
		return tree.Create<FunctionCallNode>(name.string, args);
	}
	
	if (lexer.Peek().type == Token::Type::BracketOpen)
	{
		lexer.Pop();
		auto node = ParseExpression(lexer, tree);
		
		if (!node)
			return nullptr;
		
		CheckToken(lexer.Take(), Token::Type::BracketClose);
		
		return tree.Create<ArrayGetterNode>(name.string, node);
	}
	
	return tree.Create<VariableNode>(name.string);
}

ActionNode* Parser::ParseParentheses(Lexer& lexer, ActionTree& tree)
{
	lexer.Pop();
	Token top = lexer.Peek();
	ActionNode* node = ParseExpression(lexer, tree);
	if (!node)
		return nullptr;
	
	CheckToken(lexer.Take(), Token::Type::ParenthesisClose);
	
	return node;
}

ActionNode* Parser::ParseIntNumber(Lexer& lexer, ActionTree& tree)
{
	return tree.Create<IntNumberNode>(std::stoi(lexer.Take().string));
}

ActionNode* Parser::ParseDoubleNumber(Lexer& lexer, ActionTree& tree)
{
	return tree.Create<DoubleNumberNode>(std::stod(lexer.Take().string));
}

