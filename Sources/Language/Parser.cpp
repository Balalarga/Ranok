#include "Parser.h"

#include "../../ThirdParty/fmt/include/fmt/compile.h"


ActionTree Parser::Parse(Lexer lexer)
{
	_errors.clear();
	
	ActionTree tree;
	
	while (!lexer.IsEmpty())
	{
		if (!IsParentheses(lexer) && !IsExponents(lexer) &&
			!IsMulOrDiv(lexer) && !IsAddOrSub(lexer) &&
			!IsCrossOrUnion(lexer))
		{
			_errors.push_back("Token " + lexer.Peek().string+" (" + std::to_string(lexer.Peek().line)+": " +
				std::to_string(lexer.Peek().column) + ") not processed");
			lexer.Pop();
		}
	}
	
	return tree;
}

bool Parser::IsParentheses(Lexer& lexer)
{
	const Token& top = lexer.Peek();
	if (top.type == Token::Type::OpenParenthesis)
	{
		
	}
	return false;
}

bool Parser::IsExponents(Lexer& lexer)
{
	return false;
}

bool Parser::IsMulOrDiv(Lexer& lexer)
{
	return false;
}

bool Parser::IsAddOrSub(Lexer& lexer)
{
	return false;
}

bool Parser::IsCrossOrUnion(Lexer& lexer)
{
	return false;
}
