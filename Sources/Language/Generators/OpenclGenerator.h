#pragma once
#include "IGenerator.h"

namespace Ranok
{
class OpenclGenerator: public CppGenerator
{
public:
	static const std::string sKernelProgram;

	void Predefines(std::stringstream& outCode) override;
	void Postprocess(std::stringstream& outCode) override;

	std::string ProcessHardcodedFunc(Hardcoded::FuncNames func) override;
	
	void ProcessNode(std::stringstream& outCode, const BinaryNode* node) override;
	void ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node) override;
};
}
