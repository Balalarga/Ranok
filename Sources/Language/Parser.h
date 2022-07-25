#pragma once
#include "ActionTree.h"
#include "Lexer.h"

/**
 * \brief  Convert lexer queue into AST
 */
class Parser
{
public:
	Parser() = default;
	ActionTree Parse(Lexer lexer);
	
	const std::vector<std::string>& Errors() const { return _errors; }
	bool HasErrors() const { return !_errors.empty(); }
	
	
protected:
	bool IsParentheses(Lexer& lexer);
	bool IsExponents(Lexer& lexer);
	bool IsMulOrDiv(Lexer& lexer);
	bool IsAddOrSub(Lexer& lexer);
	bool IsCrossOrUnion(Lexer& lexer);
	
	
private:
	std::vector<std::string> _errors;
};
