#pragma once
#include <optional>

#include "Language/ActionTree.h"

namespace Ranok
{
class IGenerator
{
public:
	virtual ~IGenerator() = default;
	const std::vector<std::string>& Errors() const { return _errors; }
	bool HasErrors() const { return !_errors.empty(); }
	
	std::optional<std::string> Generate(const ActionTree& tree);
	virtual void Predefines(std::stringstream& outCode);
	
	
protected:
	void Process(std::stringstream& outCode, const ActionNode* node);
	
	virtual void ProcessNode(std::stringstream& outCode, const DoubleNumberNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const ArrayNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const ArrayGetterNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const VariableDeclarationNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const VariableNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const UnaryNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const BinaryNode* node) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const FunctionCallNode* node, const ActionNode* result = nullptr) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node) = 0;
	
	void AddError(const std::string& error) { _errors.push_back(error); }
	
	
private:
	std::vector<std::string> _errors;
};


class CppGenerator: public IGenerator
{
public:
	void Predefines(std::stringstream& outCode) override;
	
	void ProcessNode(std::stringstream& outCode, const DoubleNumberNode* node) override;
	void ProcessNode(std::stringstream& outCode, const ArrayNode* node) override;
	void ProcessNode(std::stringstream& outCode, const VariableNode* node) override;
	void ProcessNode(std::stringstream& outCode, const BinaryNode* node) override;
	void ProcessNode(std::stringstream& outCode, const ArrayGetterNode* node) override;
	void ProcessNode(std::stringstream& outCode, const VariableDeclarationNode* node) override;
	void ProcessNode(std::stringstream& outCode, const FunctionCallNode* node, const ActionNode* result = nullptr) override;
	void ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node) override;
	void ProcessNode(std::stringstream& outCode, const UnaryNode* node) override;
};
}