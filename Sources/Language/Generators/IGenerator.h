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
	
	bool IsArray(const ActionNode* node);
	virtual std::optional<std::string> Generate(const ActionTree& tree);

	void Process(std::stringstream& outCode, const ActionNode* node);
	
	
	virtual void ProcessNode(std::stringstream& outCode, const DoubleNumberNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const ArrayNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const ArrayGetterNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const VariableDeclarationNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const VariableNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const UnaryNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const BinaryNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const FunctionCallNode* node, int priority = 0) = 0;
	virtual void ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node, int priority = 0) = 0;
	
	
protected:
	void AddError(const std::string& error) { _errors.push_back(error); }
	
	
private:
	std::vector<std::string> _errors;
};


class CppGenerator: public IGenerator
{
public:
	void ProcessNode(std::stringstream& outCode, const DoubleNumberNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const ArrayNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const VariableNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const BinaryNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const ArrayGetterNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const VariableDeclarationNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const FunctionCallNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node, int priority = 0) override;
	void ProcessNode(std::stringstream& outCode, const UnaryNode* node, int priority = 0) override;
};
}