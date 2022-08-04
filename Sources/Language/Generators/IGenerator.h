#pragma once
#include <optional>
#include <set>
#include <sstream>
#include <stack>
#include <fmt/format.h>

#include "Language/ActionTree.h"

namespace Ranok
{
class IGenerator
{
public:
	virtual ~IGenerator() = default;
	virtual std::optional<std::string> Generate(const ActionTree& tree)
	{
		if (!tree.Root())
		{
			AddError("No root node");
			return {};
		}
        
		std::stringstream code;
		std::queue<const std::vector<ActionNode*>*> declarationOrder({&tree.GlobalFactory().DeclarationOrder()});
		while (!declarationOrder.empty())
		{
			for (ActionNode* const& node : *declarationOrder.front())
				ProcessNode(code, node);
			declarationOrder.pop();
		}
		return code.str();
	}
	
	const std::vector<std::string>& Errors() const { return _errors; }
	bool HasErrors() const { return !_errors.empty(); }
	
	virtual void ProcessNode(std::stringstream& outCode, const ActionNode* node) = 0;
	
	
protected:
	void AddError(const std::string& error) { _errors.push_back(error); }
	
	
private:
	std::vector<std::string> _errors;
};


class CppGenerator: public IGenerator
{
public:
	void ProcessNode(std::stringstream& outCode, const ActionNode* node) override
	{
		if (!node)
		{
			outCode << "0";
			return;
		}
		else if (auto varDecl = dynamic_cast<const VariableDeclarationNode*>(node))
		{
			if (varDecl->Type() == VariableType::Int)
			{
				outCode << fmt::format("int {}", varDecl->Name());
			}
			else if (varDecl->Type() == VariableType::Double)
			{
				outCode << fmt::format("double {}", varDecl->Name());
			}
			else if (auto asArr = varDecl->As<ArrayNode>())
			{
				outCode << fmt::format("double {}[{}]", asArr->Name(), asArr->Values().size());
			}

			if (varDecl->Value() != nullptr)
			{
				outCode << " = ";
				ProcessNode(outCode, varDecl->Value());
				outCode << ";\n";
			}
		}
		else if (auto arr = dynamic_cast<const ArrayNode*>(node))
		{
			outCode << "{";
			for (size_t i = 0; i < arr->Values().size(); ++i)
			{
				ProcessNode(outCode, arr->Values()[i]);
				if (i+1 != arr->Values().size())
					outCode << ", ";
			}
			outCode << "}";
		}
		else if (auto func = dynamic_cast<const FunctionDeclarationNode*>(node))
		{
			outCode << fmt::format("double {}(", func->Name());
			for (size_t i = 0; i < func->Signature().Args().size(); ++i)
			{
				if (func->Signature().Args()[i]->Type() == VariableType::Int)
				{
					outCode << fmt::format("int {}", func->Signature().Args()[i]->Name());
				}
				else if (func->Signature().Args()[i]->Type() == VariableType::Double)
				{
					outCode << fmt::format("double {}", func->Signature().Args()[i]->Name());
				}
				else if (auto asArr = func->Signature().Args()[i]->As<ArrayNode>())
				{
					outCode << fmt::format("double {}[{}]", asArr->Name(), asArr->Values().size());
				}
				if (i+1 != func->Signature().Args().size())
					outCode << ", ";
			}
			outCode << ")\n{\n";
			for (size_t i = func->Signature().Args().size(); i < func->Factory().DeclarationOrder().size(); ++i)
				ProcessNode(outCode, func->Factory().DeclarationOrder()[i]);
			outCode << "return ";
			ProcessNode(outCode, func->Body());
			outCode << ";\n}\n";
		}
		else
		{
			outCode << node->Name();
			std::queue<ActionNode*> queue = node->WalkDown();
			while (!queue.empty())
			{
				ProcessNode(outCode, queue.front());
				queue.pop();
			}
		}
	}
};
}
