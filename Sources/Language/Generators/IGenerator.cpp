#include "IGenerator.h"

namespace Ranok
{
bool IGenerator::IsArray(const ActionNode* node)
{
	if (auto arr = dynamic_cast<const ArrayNode*>(node))
		return true;

	if (auto var = dynamic_cast<const VariableNode*>(node))
		return IsArray(var->Declaration()->Value());

	if (auto funcCall = dynamic_cast<const FunctionCallNode*>(node))
		return IsArray(funcCall->Root()->Body());
	
	return false;
}

std::optional<std::string> IGenerator::Generate(const ActionTree& tree)
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
			Process(code, node);
		declarationOrder.pop();
	}
	return code.str();
}

void IGenerator::Process(std::stringstream& outCode, const ActionNode* node)
{
	if (!node)
	{
		outCode << "0";
		return;
	}
	else if (auto num = dynamic_cast<const DoubleNumberNode*>(node))
	{
		ProcessNode(outCode, num);
	}
	else if (auto arrVar = dynamic_cast<const ArrayNode*>(node))
	{
		ProcessNode(outCode, arrVar);
	}
	else if (auto var = dynamic_cast<const VariableNode*>(node))
	{
		ProcessNode(outCode, var);
	}
	else if (auto bin = dynamic_cast<const BinaryNode*>(node))
	{
		ProcessNode(outCode, bin);
	}
	else if (auto arrGet = dynamic_cast<const ArrayGetterNode*>(node))
	{
		ProcessNode(outCode, arrGet);
	}
	else if (auto varDecl = dynamic_cast<const VariableDeclarationNode*>(node))
	{
		ProcessNode(outCode, varDecl);
	}
	else if (auto funcCall = dynamic_cast<const FunctionCallNode*>(node))
	{
		ProcessNode(outCode, funcCall);
	}
	else if (auto func = dynamic_cast<const FunctionDeclarationNode*>(node))
	{
		ProcessNode(outCode, func);
	}
}


// --------------------------------------CppGenerator-------------------------------------
void CppGenerator::ProcessNode(std::stringstream& outCode, const DoubleNumberNode* node)
{
	double intpart;
	if (modf(node->Value(), &intpart) == 0.0)
		outCode << static_cast<int>(node->Value());
	else
		outCode << node->Value();
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const ArrayNode* node)
{
	outCode << node->Name();
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const VariableNode* node)
{
	outCode << node->Name();
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const BinaryNode* node)
{
	Process(outCode, node->Left());
	outCode << " " << node->Name() << " ";
	Process(outCode, node->Right());
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const ArrayGetterNode* node)
{
	outCode << node->Name() << "[";
	Process(outCode, node->Id());
	outCode << "]";
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const VariableDeclarationNode* node)
{
	if (node->Type() == VariableType::Array)
	{
		auto asArr = static_cast<const ArrayNode*>(node->Value());
		outCode << fmt::format("double {}[{}]", asArr->Name(), asArr->Values().size());
	}
	else
	{
		outCode << fmt::format("double {}", node->Name());
	}

	if (node->Value() != nullptr)
	{
		outCode << " = ";
		if (auto arrNode = dynamic_cast<const ArrayNode*>(node->Value()))
		{
			outCode << "{";
			for (size_t i = 0; i < arrNode->Values().size(); ++i)
			{
				Process(outCode, arrNode->Values()[i]);
				if (i < arrNode->Values().size()-1)
					outCode << ", ";
			}
			outCode << "}";
		}
		else
		{
			Process(outCode, node->Value());
		}
		outCode << ";\n";
	}
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const FunctionCallNode* node)
{
	outCode << node->Name() << "(";
	for (size_t i = 0; i < node->Arguments().size(); ++i)
	{
		Process(outCode, node->Arguments()[i]);
		if (i < node->Arguments().size()-1)
			outCode << ", ";
	}
	outCode << ")";
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node)
{
	if (IsArray(node->Body()))
	{
		constexpr const char* outVarName = "__out__name";
		outCode << fmt::format("void {}(", node->Name());
		for (size_t i = 0; i < node->Signature().Args().size(); ++i)
		{
			if (node->Signature().Args()[i]->Type() == VariableType::Array)
			{
				auto asArr = static_cast<const ArrayNode*>(node->Signature().Args()[i]->Value());
				outCode << fmt::format("double {}[{}]", asArr->Name(), asArr->Values().size());
			}
			else
			{
				outCode << fmt::format("double {}", node->Signature().Args()[i]->Name());
			}
			if (i+1 != node->Signature().Args().size())
				outCode << ", ";
		}
		outCode << ", double* " << outVarName;
		outCode << ")\n{\n";
		for (size_t i = node->Signature().Args().size(); i < node->Factory().DeclarationOrder().size(); ++i)
			Process(outCode, node->Factory().DeclarationOrder()[i]);
		
		outCode << outVarName << " = ";
		Process(outCode, node->Body());
		outCode << ";\n}\n";
	}
	else
	{
		outCode << fmt::format("double {}(", node->Name());
		for (size_t i = 0; i < node->Signature().Args().size(); ++i)
		{
			if (node->Signature().Args()[i]->Type() == VariableType::Array)
			{
				auto asArr = static_cast<const ArrayNode*>(node->Signature().Args()[i]->Value());
				outCode << fmt::format("double {}[{}]", asArr->Name(), asArr->Values().size());
			}
			else
			{
				outCode << fmt::format("double {}", node->Signature().Args()[i]->Name());
			}
			if (i+1 != node->Signature().Args().size())
				outCode << ", ";
		}
		outCode << ")\n{\n";
		for (size_t i = node->Signature().Args().size(); i < node->Factory().DeclarationOrder().size(); ++i)
			Process(outCode, node->Factory().DeclarationOrder()[i]);
		outCode << "return ";
		Process(outCode, node->Body());
		outCode << ";\n}\n";
	}
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const UnaryNode* node)
{
	outCode << node->Name();
	Process(outCode, node->Child());
}
}
