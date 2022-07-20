#pragma once

#include <string_view>
#include <queue>
#include <map>

#include "Token.h"

/**
 * Split input text into queue of basic tokens
 * don't produce errors, only Unexpected symbols
 */
class Lexer
{
public:
	Lexer(std::string_view code);
	
	const std::string_view& BasicText() const { return _basicText; }
	
	Token Take();
	void Pop() { return _lexemes.pop(); }
	bool IsEmpty() const { return _lexemes.empty(); }
	const Token& Peek() const { return _lexemes.front(); }
	
	
	static const std::map<char, Token::Type> SymbolTypes;
	
	
private:
	void FillQueue(std::string_view code);
	
	std::string_view _basicText;
	std::queue<Token> _lexemes;
};
