#pragma once
#include <deque>
#include <set>

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
	ActionNode* ParseExpression(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParsePrimary(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParseWord(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParseParentheses(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParseNumber(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParseBody(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParseArrayValues(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	ActionNode* ParseBinary(ActionNode* lhs, Lexer& lexer, std::deque<ActionNodeFactory*>& factories, int priority = 0);
	
	VariableDeclarationNode* ParseVariableDeclaration(Lexer& lexer, std::deque<ActionNodeFactory*>& factory);
	FunctionDeclarationNode* ParseFunction(Lexer& lexer, std::deque<ActionNodeFactory*>& factories);
	
	bool CheckToken(const Token& token, Token::Type expected);
	
	/**
	 * \brief Create error message from token with optional format args:\n
	 * <b>{name}</b> - token name\n
	 * <b>{line}</b> - token line\n
	 * <b>{column}</b> - token column
	 * \param errFormat Error string with formatter args
	 * \param token Where was the error founded
	 */
	void DumpTokenError(const std::string& errFormat, const Token& token);
	
	
private:
	const std::string _unnamePrefix = "__unnamedVar__";
	long _unnamedCounter = 0;
	std::vector<std::string> _errors;
	static const std::map<Token::Type, int> _operationPriorities;
	static const std::set<std::string> _reservedWords;
};
}
