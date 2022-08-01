#pragma once
#include <optional>

#include "ActionTree.h"
#include "Lexer.h"

namespace Ranok
{
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
	ActionNode* ParseExpression(Lexer& lexer, ActionNodeFactory& factory);
	
	ActionNode* ParsePrimary(Lexer& lexer, ActionNodeFactory& factory);
	ActionNode* ParseWord(Lexer& lexer, ActionNodeFactory& factory);
	ActionNode* ParseParentheses(Lexer& lexer, ActionNodeFactory& factory);
	ActionNode* ParseIntNumber(Lexer& lexer, ActionNodeFactory& factory);
	ActionNode* ParseDoubleNumber(Lexer& lexer, ActionNodeFactory& factory);
	ActionNode* ParseBody(Lexer& lexer, ActionNodeFactory& factory);
	
	ActionNode* ParseBinary(ActionNode* lhs, Lexer& lexer, ActionNodeFactory& factory, int priority = 0);
	
	VariableDeclarationNode* ParseVariableDeclaration(Lexer& lexer, ActionNodeFactory& factory);
	
	std::optional<FunctionSignature> ParseFunctionSignature(Lexer& lexer);
	FunctionDeclarationNode* ParseFunction(Lexer& lexer, ActionNodeFactory& factory);
	
	
	bool CheckToken(const Token& token, Token::Type expected);
	
	
private:
	std::vector<std::string> _errors;
	static const std::map<Token::Type, int> _operationPriorities;
};
}