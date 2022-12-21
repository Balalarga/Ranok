#include "IGenerator.h"

#include <sstream>
#include "fmt/format.h"
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
	Predefines(code);
	
	std::queue<const std::vector<ActionNode*>*> declarationOrder({&tree.GlobalFactory().DeclarationOrder()});
	while (!declarationOrder.empty())
	{
		for (ActionNode* const& node : *declarationOrder.front())
			Process(code, node);
		declarationOrder.pop();
	}

	Postprocess(code);
	return code.str();
}

void IGenerator::Predefines(std::stringstream& outCode)
{
}

void IGenerator::Postprocess(std::stringstream& outCode)
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
		PrintIndent(outCode);
		ProcessNode(outCode, varDecl);
	}
	else if (auto funcCall = dynamic_cast<const FunctionCallNode*>(node))
	{
		ProcessNode(outCode, funcCall);
	}
	else if (auto func = dynamic_cast<const FunctionDeclarationNode*>(node))
	{
		++indentWidth;
		ProcessNode(outCode, func);
		--indentWidth;
	}
}

void IGenerator::PrintIndent(std::stringstream& outCode)
{
	for (int i = 0; i < indentWidth; ++i)
		outCode << "\t";
}

std::string IGenerator::ProcessHardcodedFunc(Hardcoded::FuncNames name)
{
	switch (name)
	{
	case Hardcoded::FuncNames::Arccos:
		return "acos";
	case Hardcoded::FuncNames::Arcsin:
		return "asin";
	case Hardcoded::FuncNames::Arctan:
		return "atan";
	default:
		return Hardcoded::Get().FunctionNames.at(name);
	}
}

void CppGenerator::Predefines(std::stringstream& outCode)
{
	IGenerator::Predefines(outCode);
	outCode <<
R"(double __rv__Or(double a, double b)
{
	return a + b + std::sqrt(std::pow(a, 2) + std::pow(b, 2));
}
double __rv__And(double a, double b)
{
	return a + b - std::sqrt(std::pow(a, 2) + std::pow(b, 2));
}
)";
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
	Token::Type type = node->GetToken().type;
	int currPriority = Parser::GetOperationPriority(type);
	int lPriority = Parser::GetOperationPriority(node->Left()->GetToken().type);
	int rPriority = Parser::GetOperationPriority(node->Right()->GetToken().type);
	bool needLeftParent = lPriority != -1 && lPriority < currPriority;
	bool needRightParent = rPriority != -1 && rPriority < currPriority;
	
	if (type == Token::Type::Hat)
	{
		outCode << "std::pow(";
	}
	else if (type == Token::Type::Ampersand)
	{
		outCode << "__rv__And(";
	}
	else if (type == Token::Type::Pipe)
	{
		outCode << "__rv__Or(";
	}
	
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
	
	if (type == Token::Type::Hat || type == Token::Type::Ampersand || type == Token::Type::Pipe)
	{
		outCode << ", ";
	}
	else
	{
		outCode << " " << node->Name() << " ";
	}
	
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
	if (type == Token::Type::Hat || type == Token::Type::Ampersand || type == Token::Type::Pipe)
		outCode << ")";
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
		outCode << fmt::format("float {}[{}]", node->Name(), arr->Values().size());
	else
		outCode << fmt::format("float {}", node->Name());

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
	auto it = Hardcoded::Get().NamedFunctions.find(node->Name());
	if (it != Hardcoded::Get().NamedFunctions.end())
		outCode << ProcessHardcodedFunc(it->second);
	else
		outCode << node->Name();
	
	outCode << "(";
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
		PrintIndent(outCode);
		constexpr const char* outVarName = "__out__name";
		outCode << fmt::format("void {}(", node->Name());
		for (size_t i = 0; i < sigArgs.size(); ++i)
		{
			if (const ArrayNode* arr = ActionNode::IsArray(sigArgs[i]))
				outCode << fmt::format("float {}[{}]", sigArgs[i]->Name(), arr->Values().size());
			else
				outCode << fmt::format("float {}", sigArgs[i]->Name());
			
			outCode << ", ";
		}
		outCode << "float* " << outVarName;
		outCode << ")\n{\n";
		for (size_t i = sigArgs.size(); i < node->Factory().DeclarationOrder().size(); ++i)
			Process(outCode, node->Factory().DeclarationOrder()[i]);
		
		std::string outResName = "__out__res";
		if (auto varRes = dynamic_cast<const VariableNode*>(node->Body()))
		{
			outResName = varRes->Name();
		}
		else
		{
			PrintIndent(outCode);
			outCode << "float " << outResName << "[" << arrBody->Values().size() << "] = ";
			Process(outCode, node->Body());
			outCode << ";\n";
		}
		PrintIndent(outCode);
		outCode << "memcpy(" << outVarName << ", " << outResName << ", sizeof(float) * "<< arrBody->Values().size();
		outCode << ");\n}\n";
			
	}
	else
	{
		outCode << fmt::format("float {}(", node->Name());
		for (size_t i = 0; i < sigArgs.size(); ++i)
		{
			if (const ArrayNode* asArr = ActionNode::IsArray(sigArgs[i]->Value()))
				outCode << fmt::format("float {}[{}]", sigArgs[i]->Name(), asArr->Values().size());
			else
				outCode << fmt::format("float {}", sigArgs[i]->Name());
			
			if (i+1 != sigArgs.size())
				outCode << ", ";
		}
		outCode << ")\n{\n";
		for (size_t i = sigArgs.size(); i < node->Factory().DeclarationOrder().size(); ++i)
			Process(outCode, node->Factory().DeclarationOrder()[i]);
		
		PrintIndent(outCode);
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
