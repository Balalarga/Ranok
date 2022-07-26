#include "Parser.h"

const std::vector<ActionNode*(Parser::*)(Lexer&, ActionTree&)> Parser::PriorityFunctions{
	&Parser::GetParentheses,
	&Parser::GetExponents,
	&Parser::GetMulOrDiv,
	&Parser::GetAddOrSub,
	&Parser::GetCrossOrUnion,
};

ActionTree Parser::Parse(Lexer lexer)
{
	_errors.clear();
	
	ActionTree tree;
	while (!lexer.IsEmpty())
	{
		if (!RecursiveParsing(lexer, tree))
		{
			_errors.push_back("Token " + lexer.Peek().string+" (" + std::to_string(lexer.Peek().line)+": " +
				std::to_string(lexer.Peek().column) + ") not processed");
			lexer.Pop();
		}
	}
	
	return tree;
}

ActionNode* Parser::RecursiveParsing(Lexer& lexer, ActionTree& tree)
{
	for (auto& func : PriorityFunctions)
	{
		if (ActionNode* node = (this->*func)(lexer, tree))
			return node;
	}
	return nullptr;
}

ActionNode* Parser::GetParentheses(Lexer& lexer, ActionTree& tree)
{
	const Token& top = lexer.Peek();
	if (top.type == Token::Type::OpenParenthesis)
	{
		lexer.Pop();
		RecursiveParsing(lexer, tree);
	}
	return nullptr;
}

ActionNode* Parser::GetExponents(Lexer& lexer, ActionTree& tree)
{
	return nullptr;
}

ActionNode* Parser::GetMulOrDiv(Lexer& lexer, ActionTree& tree)
{
	return nullptr;
}

ActionNode* Parser::GetAddOrSub(Lexer& lexer, ActionTree& tree)
{
	return nullptr;
}

ActionNode* Parser::GetCrossOrUnion(Lexer& lexer, ActionTree& tree)
{
	return nullptr;
}
