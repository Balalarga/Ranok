#include "HardcodedConstructions.h"

namespace Ranok
{
const std::map<Hardcoded::FuncNames, std::string> Hardcoded::FunctionNames
{
	{ FuncNames::Abs, "abs" },
	{ FuncNames::Sqrt, "sqrt" },
	{ FuncNames::Sin, "sin" },
	{ FuncNames::Cos, "cos" },
	{ FuncNames::Tan, "tan" },
	{ FuncNames::Arctan, "arctan" },
	{ FuncNames::Arcsin, "arcsin" },
	{ FuncNames::Arccos, "arccos" },
	{ FuncNames::Cosh, "cosh" },
	{ FuncNames::Sinh, "sinh" },
	{ FuncNames::Tanh, "tanh" },
	{ FuncNames::Exp, "exp" },
	{ FuncNames::Ln, "ln" },
	{ FuncNames::Log, "log" },
	{ FuncNames::Log10, "log10" },
	{ FuncNames::Log2, "log2" },
	{ FuncNames::Ceil, "ceil" },
	{ FuncNames::Floor, "floor" },
};

const std::map<Hardcoded::VarNames, std::string> Hardcoded::VariableNames
{
	{ VarNames::Pi, "PI" },
};

Hardcoded& Hardcoded::Get()
{
	static Hardcoded self;
	return self;
}

Hardcoded::Hardcoded()
{
	auto addVar = [this](VarNames name, ActionNode* value = nullptr)
	{
		VariableDeclarations[VariableNames.at(name)] =
			_factory.CreateVariable(Token(Token::Type::Word, VariableNames.at(name)), value);
	};
	addVar(VarNames::Pi);
	
	auto addFunc = [this](FuncNames name, const std::vector<VariableDeclarationNode*>& args = {}, ActionNode* body = nullptr)
	{
		FunctionDeclarations[FunctionNames.at(name)] = _factory.CreateFunction(FunctionSignature(Token(Token::Type::Word,
			FunctionNames.at(name)), args), body);
	};
	
	addFunc(FuncNames::Abs, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Sqrt, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Sin, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Cos, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Tan, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Arctan, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Arcsin, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Arccos, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Cosh, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Sinh, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Tanh, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Exp, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Ln, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Log, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Log10, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Log2, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Ceil, {_factory.CreateVariable({Token::Type::Word, "v1"})});
	addFunc(FuncNames::Floor, {_factory.CreateVariable({Token::Type::Word, "v1"})});
}
}
