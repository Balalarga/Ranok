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
		std::stack<const ActionNodeFactory*> scopeStack{ {&tree.GlobalFactory()} };
		PreprocessNode(tree.Root(), scopeStack);
		while (!_declarationStack.empty())
		{
			ProcessNode(code, _declarationStack.top(), scopeStack);
			_declarationStack.pop();
		}
		return code.str();
		ProcessNode(code, tree.Root(), scopeStack);
		scopeStack.pop();
		
		if (HasErrors() || !scopeStack.empty())
			return {};
		
		return code.str();
	}
	
	const std::vector<std::string>& Errors() const { return _errors; }
	bool HasErrors() const { return !_errors.empty(); }
    
	
	void PreprocessNode(const ActionNode* node, std::stack<const ActionNodeFactory*>& factories)
	{
		if (auto var = dynamic_cast<const VariableNode*>(node))
		{
			PreprocessNode(var->Declaration(), factories);
		}
		else if (auto varDecl = dynamic_cast<const VariableDeclarationNode*>(node))
		{
			if (!_declarationSet.contains(varDecl))
			{
				_declarationSet.emplace(varDecl);
				_declarationStack.push(varDecl);
			}
		}
		else if (auto func = dynamic_cast<const FunctionCallNode*>(node))
		{
			factories.push(&func->Root()->Factory());
			PreprocessNode(func->Root(), factories);
			for (ActionNode* const& arg : func->Arguments())
				PreprocessNode(arg, factories);
			factories.pop();
		}
		else if (auto funcDecl = dynamic_cast<const FunctionDeclarationNode*>(node))
		{
			if (!_declarationSet.contains(funcDecl))
			{
				_declarationSet.emplace(funcDecl);
				_declarationStack.push(funcDecl);
				factories.push(&funcDecl->Factory());
				PreprocessNode(funcDecl->Body(), factories);
				factories.pop();
			}
		}
		else
		{
			auto children = node->WalkDown();
			while (!children.empty())
			{
				PreprocessNode(children.front(), factories);
				children.pop();
			}
		}
	}
	
	virtual void ProcessNode(std::stringstream& outCode, const ActionNode* node, std::stack<const ActionNodeFactory*>& factories) = 0;
	
	
protected:
	void AddError(const std::string& error) { _errors.push_back(error); }
	
	
private:
	std::vector<std::string> _errors;
	std::set<std::string> _declaredNodes;
	std::stack<const ActionNode*> _declarationStack;
	std::set<const ActionNode*> _declarationSet;
};


class CppGenerator: public IGenerator
{
public:
	void ProcessNode(std::stringstream& outCode, const ActionNode* node, std::stack<const ActionNodeFactory*>& factories) override
	{
		if (auto var = dynamic_cast<const VariableDeclarationNode*>(node))
		{
			if (var->Type() == VariableType::Int)
			{
				outCode << fmt::format("int {} = ", var->Name());
			}
			else if (var->Type() == VariableType::Double)
			{
				outCode << fmt::format("double {} = ", var->Name());
			}
			else if (auto asArr = var->As<ArrayNode>())
			{
				outCode << fmt::format("double {}[{}] = ", asArr->Name(), asArr->Values().size());
			}
		}
		else if (auto func = dynamic_cast<const FunctionDeclarationNode*>(node))
		{
			outCode << fmt::format("double {}(", func->Name());
			for (size_t i = 0; i < func->Signature().Args().size(); ++i)
			{
				outCode << func->Signature().Args()[i]->Name();
				if (i+1 != func->Signature().Args().size())
					outCode << ", ";
			}
			outCode << ")\n{\n";
			factories.push(&func->Factory());
			ProcessNode(outCode, func->Body(), factories);
			factories.pop();
			outCode << "}";
		}
	}
};
}
