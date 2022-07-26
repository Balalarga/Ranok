#pragma once
#include <functional>

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
	ActionNode* RecursiveParsing(Lexer& lexer, ActionTree& tree);
	
	ActionNode* GetParentheses(Lexer& lexer, ActionTree& tree);
	ActionNode* GetExponents(Lexer& lexer, ActionTree& tree);
	ActionNode* GetMulOrDiv(Lexer& lexer, ActionTree& tree);
	ActionNode* GetAddOrSub(Lexer& lexer, ActionTree& tree);
	ActionNode* GetCrossOrUnion(Lexer& lexer, ActionTree& tree);
	
	
private:
	std::vector<std::string> _errors;
	static const std::vector<ActionNode*(Parser::*)(Lexer&, ActionTree&)> PriorityFunctions;
};
