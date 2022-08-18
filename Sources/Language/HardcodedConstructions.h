#pragma once
#include <map>
#include <string>

#include "ActionNode.h"


namespace Ranok
{
class Hardcoded
{
public:
	static Hardcoded& Get();
	
	
	enum class FuncNames
	{
		Abs,
		Sqrt,
		Sin,
		Cos,
		Tan,
		Arctan,
		Arcsin,
		Arccos,
		Cosh,
		Sinh,
		Tanh,
		Exp,
		Ln,
		Log,
		Log10,
		Log2,
		Ceil,
		Floor,
	};
	enum class VarNames
	{
		Pi,
	};
	
	const static std::map<FuncNames, std::string> FunctionNames;
	std::map<std::string, FunctionDeclarationNode*> FunctionDeclarations;
	
	const static std::map<VarNames, std::string> VariableNames;
	std::map<std::string, VariableDeclarationNode*> VariableDeclarations;
	
	
private:
	Hardcoded();
	ActionNodeFactory _factory;
};
}
