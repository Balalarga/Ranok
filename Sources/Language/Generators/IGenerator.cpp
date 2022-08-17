#include "IGenerator.h"
#include "fmt/format.h"

#include <sstream>

#include "Language/Parser.h"


namespace Ranok
{

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

void IGenerator::Predefines(std::stringstream& outCode)
{
}

void IGenerator::Process(std::stringstream& outCode, const ActionNode* node)
{
	if (!node)
	{
		outCode << "nullptr";
		return;
	}
	
	if (auto num = dynamic_cast<const DoubleNumberNode*>(node))
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
	else if (auto unary = dynamic_cast<const UnaryNode*>(node))
	{
		ProcessNode(outCode, unary);
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


void CppGenerator::Predefines(std::stringstream& outCode)
{
	IGenerator::Predefines(outCode);
}

// --------------------------------------CppGenerator-------------------------------------
void CppGenerator::ProcessNode(std::stringstream& outCode, const DoubleNumberNode* node)
{
	double intPart;
	if (modf(node->Value(), &intPart) == 0.0)
		outCode << static_cast<int>(node->Value());
	else
		outCode << node->Value();
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const ArrayNode* node)
{
	outCode << "{";
	for (size_t i = 0; i < node->Values().size(); ++i)
	{
		Process(outCode, node->Values()[i]);
		if (i < node->Values().size()-1)
			outCode << ", ";
	}
	outCode << "}";
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const VariableNode* node)
{
	outCode << node->Name();
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const BinaryNode* node)
{
	int currPriority = Parser::GetOperationPriority(node->GetToken().type);
	int lPriority = Parser::GetOperationPriority(node->Left()->GetToken().type);
	int rPriority = Parser::GetOperationPriority(node->Right()->GetToken().type);
	bool needLeftParent = lPriority != -1 && lPriority < currPriority;
	bool needRightParent = rPriority != -1 && rPriority < currPriority;
	
	if (needLeftParent)
	{
		outCode << "(";
		Process(outCode, node->Left());
		outCode << ")";
	}
	else
	{
		Process(outCode, node->Left());
	}
	
	outCode << " " << node->Name() << " ";
	if (needRightParent)
	{
		outCode << "(";
		Process(outCode, node->Right());
		outCode << ")";
	}
	else
	{
		Process(outCode, node->Right());
	}
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const ArrayGetterNode* node)
{
	outCode << node->Name() << "[";
	Process(outCode, node->Id());
	outCode << "]";
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const VariableDeclarationNode* node)
{
	if (const ArrayNode* arr = ActionNode::IsArray(node->Value()))
		outCode << fmt::format("double {}[{}]", node->Name(), arr->Values().size());
	else
		outCode << fmt::format("double {}", node->Name());

	if (auto arrNode = dynamic_cast<const ArrayNode*>(node->Value()))
	{
		outCode << " = ";
		outCode << "{";
		for (size_t i = 0; i < arrNode->Values().size(); ++i)
		{
			Process(outCode, arrNode->Values()[i]);
			if (i < arrNode->Values().size()-1)
				outCode << ", ";
		}
		outCode << "}";
	}
	else if (auto func = dynamic_cast<const FunctionCallNode*>(node->Value()))
	{
		if (ActionNode::IsArray(node->Value()))
		{
			outCode << "; ";
			ProcessNode(outCode, func, node);
		}
		else
		{
			outCode << " = ";
			Process(outCode, node->Value());
		}
	}
	else
	{
		outCode << " = ";
		Process(outCode, node->Value());
	}
	outCode << ";\n";
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const FunctionCallNode* node, const ActionNode* result)
{
	outCode << node->Name() << "(";
	const std::vector<ActionNode*>& args = node->Arguments();
	for (size_t i = 0; i < args.size(); ++i)
	{
		Process(outCode, args[i]);
		if (i < args.size()-1)
			outCode << ", ";
	}
	if (result)
	{
		if (!args.empty())
			outCode << ", ";
		outCode << result->Name();
	}
	outCode << ")";
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node)
{
	const std::vector<VariableDeclarationNode*>& sigArgs = node->Signature().Args();
	if (const ArrayNode* arrBody = ActionNode::IsArray(node->Body()))
	{
		constexpr const char* outVarName = "__out__name";
		constexpr const char* outResName = "__out__res";
		outCode << fmt::format("void {}(", node->Name());
		for (size_t i = 0; i < sigArgs.size(); ++i)
		{
			if (const ArrayNode* arr = ActionNode::IsArray(sigArgs[i]))
				outCode << fmt::format("double {}[{}]", sigArgs[i]->Name(), arr->Values().size());
			else
				outCode << fmt::format("double {}", sigArgs[i]->Name());
			
			outCode << ", ";
		}
		outCode << "double* " << outVarName;
		outCode << ")\n{\n";
		for (size_t i = sigArgs.size(); i < node->Factory().DeclarationOrder().size(); ++i)
			Process(outCode, node->Factory().DeclarationOrder()[i]);

		outCode << "double " << outResName << "[" << arrBody->Values().size() << "] = ";
		Process(outCode, node->Body());
		outCode << ";\nmemcpy(" << outVarName << ", " << outResName << ", sizeof(double) * "<< arrBody->Values().size() ;
		outCode << ";\n}\n";
	}
	else
	{
		outCode << fmt::format("double {}(", node->Name());
		for (size_t i = 0; i < sigArgs.size(); ++i)
		{
			if (const ArrayNode* asArr = ActionNode::IsArray(sigArgs[i]->Value()))
				outCode << fmt::format("double {}[{}]", sigArgs[i]->Name(), asArr->Values().size());
			else
				outCode << fmt::format("double {}", sigArgs[i]->Name());
			
			if (i+1 != sigArgs.size())
				outCode << ", ";
		}
		outCode << ")\n{\n";
		for (size_t i = sigArgs.size(); i < node->Factory().DeclarationOrder().size(); ++i)
			Process(outCode, node->Factory().DeclarationOrder()[i]);
		
		outCode << "return ";
		Process(outCode, node->Body());
		outCode << ";\n}\n";
	}
}

void CppGenerator::ProcessNode(std::stringstream& outCode, const UnaryNode* node)
{
	outCode << "(" << node->Name();
	Process(outCode, node->Child());
	outCode << ")";
}
}
