#include "ShaderGenerator.h"
#include <sstream>

#include "fmt/format.h"
#include "Language/Parser.h"

namespace Ranok
{
std::string ShaderGenerator::ProcessHardcodedFunc(Hardcoded::FuncNames func)
{
	if (func == Hardcoded::FuncNames::Abs)
		return "abs";
	return CppGenerator::ProcessHardcodedFunc(func);
}

void ShaderGenerator::Predefines(std::stringstream& outCode)
{
	outCode <<
R"(float __rv__Or(float a, float b)
{
	return a + b + sqrt(pow(a, 2) + pow(b, 2));
}
float __rv__And(float a, float b)
{
	return a + b - sqrt(pow(a, 2) + pow(b, 2));
}
)";
}

void ShaderGenerator::ProcessNode(std::stringstream& outCode, const BinaryNode* node)
{
	Token::Type type = node->GetToken().type;
	int currPriority = Parser::GetOperationPriority(type);
	int lPriority = Parser::GetOperationPriority(node->Left()->GetToken().type);
	int rPriority = Parser::GetOperationPriority(node->Right()->GetToken().type);
	bool needLeftParent = lPriority != -1 && lPriority < currPriority;
	bool needRightParent = rPriority != -1 && rPriority < currPriority;
	
	if (type == Token::Type::Hat)
	{
		outCode << "pow(";
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

void ShaderGenerator::ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node)
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
		outCode << outVarName << " = " << outResName;
		outCode << ";\n}\n";
			
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
}
