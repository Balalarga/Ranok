#pragma once
#include <optional>

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
	
	static int GetOperationPriority(Token::Type type);
	
	
protected:
	ActionNode* ParseExpression(Lexer& lexer, ActionTree& tree);

	ActionNode* ParsePrimary(Lexer& lexer, ActionTree& tree);
	ActionNode* ParseWord(Lexer& lexer, ActionTree& tree);
	ActionNode* ParseParentheses(Lexer& lexer, ActionTree& tree);
	ActionNode* ParseIntNumber(Lexer& lexer, ActionTree& tree);
	ActionNode* ParseDoubleNumber(Lexer& lexer, ActionTree& tree);
	
	ActionNode* ParseBinary(ActionNode* lhs, Lexer& lexer, ActionTree& tree, int priority = 0);

	VariableDeclarationNode* ParseVariableDeclaration(Lexer& lexer, ActionTree& tree);
	
	std::optional<FunctionSignature> ParseFunctionSignature(Lexer& lexer, ActionTree& tree);
	FunctionDeclarationNode* ParseFunction(Lexer& lexer, ActionTree& tree);
	
	
	bool CheckToken(const Token& token, Token::Type expected);
	
	
private:
	std::vector<std::string> _errors;
	static const std::map<Token::Type, int> _operationPriorities;
};
