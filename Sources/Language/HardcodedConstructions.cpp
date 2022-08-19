#include "HardcodedConstructions.h"

namespace Ranok
{

Hardcoded& Hardcoded::Get()
{
	static Hardcoded self;
	return self;
}

Hardcoded::Hardcoded()
{
	auto addVar = [this](const std::string& name, VarNames type, ActionNode* value = nullptr)
	{
		VariableNames[type] = name;
		NamedVariables[name] = type;
		VariableDeclarations[name] = _factory.CreateVariable(Token(name), value);
	};
	addVar("PI", VarNames::Pi, _factory.Create<DoubleNumberNode>(Token(Token::Type::Number, "3.141592"), 3.1415926));
	
	
	auto addFunc = [this](const std::string& name, FuncNames type, const std::vector<VariableDeclarationNode*>& args = {}, ActionNode* body = nullptr)
	{
		FunctionNames[type] = name;
		NamedFunctions[name] = type;
		FunctionDeclarations[name] = _factory.CreateFunction(FunctionSignature(Token(name), args), body);
	};
	addFunc("abs", FuncNames::Abs, {_factory.CreateVariable({"v1"})});
	addFunc("sqrt", FuncNames::Sqrt, {_factory.CreateVariable({"v1"})});
	addFunc("sin", FuncNames::Sin, {_factory.CreateVariable({"v1"})});
	addFunc("cos", FuncNames::Cos, {_factory.CreateVariable({"v1"})});
	addFunc("tan", FuncNames::Tan, {_factory.CreateVariable({"v1"})});
	addFunc("arctan", FuncNames::Arctan, {_factory.CreateVariable({"v1"})});
	addFunc("arcsin", FuncNames::Arcsin, {_factory.CreateVariable({"v1"})});
	addFunc("arccos", FuncNames::Arccos, {_factory.CreateVariable({"v1"})});
	addFunc("cosh", FuncNames::Cosh, {_factory.CreateVariable({"v1"})});
	addFunc("sinh", FuncNames::Sinh, {_factory.CreateVariable({"v1"})});
	addFunc("tanh", FuncNames::Tanh, {_factory.CreateVariable({"v1"})});
	addFunc("exp", FuncNames::Exp, {_factory.CreateVariable({"v1"})});
	addFunc("ln", FuncNames::Ln, {_factory.CreateVariable({"v1"})});
	addFunc("log", FuncNames::Log, {_factory.CreateVariable({"v1"})});
	addFunc("log10", FuncNames::Log10, {_factory.CreateVariable({"v1"})});
	addFunc("log2", FuncNames::Log2, {_factory.CreateVariable({"v1"})});
	addFunc("ceil", FuncNames::Ceil, {_factory.CreateVariable({"v1"})});
	addFunc("floor", FuncNames::Floor, {_factory.CreateVariable({"v1"})});
}
}
