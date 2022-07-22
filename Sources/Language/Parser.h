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
	ActionTree Parse(const Lexer&  lexer);

};
