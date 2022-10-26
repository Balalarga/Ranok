#pragma once
#include "IGenerator.h"

namespace Ranok
{
class ShaderGenerator: public CppGenerator
{
public:
	void Predefines(std::stringstream& outCode) override;
	
	void ProcessNode(std::stringstream& outCode, const BinaryNode* node) override;
	void ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node) override;
	
};
}
