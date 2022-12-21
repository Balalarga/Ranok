#include "OpenclGenerator.h"
#include <sstream>

#include "fmt/format.h"
#include "Language/Parser.h"

namespace Ranok
{

const std::string OpenclGenerator::sKernelProgram = "ComputeFullMimage";

std::string OpenclGenerator::ProcessHardcodedFunc(Hardcoded::FuncNames func)
{
	if (func == Hardcoded::FuncNames::Abs)
		return "fabs";
	return CppGenerator::ProcessHardcodedFunc(func);
}

void OpenclGenerator::Predefines(std::stringstream& outCode)
{
	outCode <<
R"(struct __attribute__ ((packed)) MImage3D
{
    double c[5];
    char zone;
};
double __matrix4x4Det(double* m, int n)
{
      return
         m[3] * m[1*n+2] * m[2*n+1] * m[3*n+0] - m[0*n+2] * m[1*n+3] * m[2*n+1] * m[3*n+0] -
         m[3] * m[1*n+1] * m[2*n+2] * m[3*n+0] + m[0*n+1] * m[1*n+3] * m[2*n+2] * m[3*n+0] +
         m[2] * m[1*n+1] * m[2*n+3] * m[3*n+0] - m[0*n+1] * m[1*n+2] * m[2*n+3] * m[3*n+0] -
         m[3] * m[1*n+2] * m[2*n+0] * m[3*n+1] + m[0*n+2] * m[1*n+3] * m[2*n+0] * m[3*n+1] +
         m[3] * m[1*n+0] * m[2*n+2] * m[3*n+1] - m[0*n+0] * m[1*n+3] * m[2*n+2] * m[3*n+1] -
         m[2] * m[1*n+0] * m[2*n+3] * m[3*n+1] + m[0*n+0] * m[1*n+2] * m[2*n+3] * m[3*n+1] +
         m[3] * m[1*n+1] * m[2*n+0] * m[3*n+2] - m[0*n+1] * m[1*n+3] * m[2*n+0] * m[3*n+2] -
         m[3] * m[1*n+0] * m[2*n+1] * m[3*n+2] + m[0*n+0] * m[1*n+3] * m[2*n+1] * m[3*n+2] +
         m[1] * m[1*n+0] * m[2*n+3] * m[3*n+2] - m[0*n+0] * m[1*n+1] * m[2*n+3] * m[3*n+2] -
         m[2] * m[1*n+1] * m[2*n+0] * m[3*n+3] + m[0*n+1] * m[1*n+2] * m[2*n+0] * m[3*n+3] +
         m[2] * m[1*n+0] * m[2*n+1] * m[3*n+3] - m[0*n+0] * m[1*n+2] * m[2*n+1] * m[3*n+3] -
         m[1] * m[1*n+0] * m[2*n+2] * m[3*n+3] + m[0*n+0] * m[1*n+1] * m[2*n+2] * m[3*n+3];
}
char __checkZone8(double *values)
{
    bool plus = false;
    bool zero = false;
    bool minus = false;
    for(int i = 0; i < 8; i++)
    {
        if(values[i] == 0.0)
            zero = true;
        else if(values[i] < 0)
            minus = true;
        else if(values[i] > 0)
            plus = true;
    }
    if(zero || (plus && minus))
        return 0;
    if(plus)
        return 1;
    return -1;
}

double __rv__Or(double a, double b)
{
	return a + b + sqrt(pow(a, 2) + pow(b, 2));
}
double __rv__And(double a, double b)
{
	return a + b - sqrt(pow(a, 2) + pow(b, 2));
}
)";
}

void OpenclGenerator::Postprocess(std::stringstream& outCode)
{
	outCode << R"(
kernel void ComputeFullMimage(global struct MImage3D *result,
                           const int startId,
                           const uint3 spaceSize,
                           const double3 startPoint,
                           const double3 pointSize,
                           const double3 halfSize)
{
    int id = get_global_id(0);
    int spaceId = startId + id;
    double3 point;
    point.x = startPoint.x + pointSize.x * (spaceId / ( spaceSize.z * spaceSize.y ));
    point.y = startPoint.y + pointSize.y * ((spaceId / spaceSize.z ) % spaceSize.y);
    point.z = startPoint.z + pointSize.z * (spaceId % spaceSize.z);
    double wv[4];
    double params1[3] = {point.x, point.y, point.z };
    wv[0] = _main(params1);
    double params2[3] = {point.x+pointSize.x, point.y, point.z };
    wv[1] = _main(params2);
    double params3[3] = {point.x, point.y + pointSize.y, point.z };
    wv[2] = _main(params3);
    double params4[3] = {point.x, point.y, point.z + pointSize.z};
    wv[3] = _main(params4);
    double a[] = {
        point.y,             point.z,             wv[0], 1,
        point.y,             point.z,             wv[1], 1,
        point.y+pointSize.y, point.z,             wv[2], 1,
        point.y,             point.z+pointSize.z, wv[3], 1
    };
    double b[] = {
        point.x,             point.z,             wv[0], 1,
        point.x+pointSize.x, point.z,             wv[1], 1,
        point.x,             point.z,             wv[2], 1,
        point.x,             point.z+pointSize.z, wv[3], 1
    };
    double c[] = {
        point.x,             point.y,             wv[0], 1,
        point.x+pointSize.x, point.y,             wv[1], 1,
        point.x,             point.y+pointSize.y, wv[2], 1,
        point.x,             point.y,             wv[3], 1
    };
    double d[] = {
        point.x,             point.y,             point.z,              1,
        point.x+pointSize.x, point.y,             point.z,              1,
        point.x,             point.y+pointSize.y, point.z,              1,
        point.x,             point.y,             point.z+pointSize.z, 1
    };
    double f[] = {
        point.x,             point.y,             point.z,             wv[0],
        point.x+pointSize.x, point.y,             point.z,             wv[1],
        point.x,             point.y+pointSize.y, point.z,             wv[2],
        point.x,             point.y,             point.z+pointSize.z, wv[3]
    };
    double detA = __matrix4x4Det(a, 4);
    double detB = __matrix4x4Det(b, 4);
    double detC = __matrix4x4Det(c, 4);
    double detD = __matrix4x4Det(d, 4);
    double detF = __matrix4x4Det(f, 4);
    double div = sqrt(pow(detA, 2)+pow(detB, 2)+pow(detC, 2)+pow(detD, 2)+pow(detF, 2));
    result[id].c[0] = detA/div;
    result[id].c[1] = -detB/div;
    result[id].c[2] = -detC/div;
    result[id].c[3] = detD/div;
    result[id].c[4] = detF/div;
    double values[8];
    double zoneParams1[3] = {point.x+halfSize.x, point.y+halfSize.y, point.z+halfSize.z};
    values[0] = _main(zoneParams1);
    double zoneParams2[3] = {point.x+halfSize.x, point.y+halfSize.y, point.z-halfSize.z};
    values[1] = _main(zoneParams2);
    double zoneParams3[3] = {point.x+halfSize.x, point.y-halfSize.y, point.z+halfSize.z};
    values[2] = _main(zoneParams3);
    double zoneParams4[3] = {point.x+halfSize.x, point.y-halfSize.y, point.z-halfSize.z};
    values[3] = _main(zoneParams4);
    double zoneParams5[3] = {point.x-halfSize.x, point.y+halfSize.y, point.z+halfSize.z};
    values[4] = _main(zoneParams5);
    double zoneParams6[3] = {point.x-halfSize.x, point.y+halfSize.y, point.z-halfSize.z};
    values[5] = _main(zoneParams6);
    double zoneParams7[3] = {point.x-halfSize.x, point.y-halfSize.y, point.z+halfSize.z};
    values[6] = _main(zoneParams7);
    double zoneParams8[3] = {point.x-halfSize.x, point.y-halfSize.y, point.z-halfSize.z};
    values[7] = _main(zoneParams8);
    result[id].zone = __checkZone8(values);
}
)";
}

void OpenclGenerator::ProcessNode(std::stringstream& outCode, const BinaryNode* node)
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

void OpenclGenerator::ProcessNode(std::stringstream& outCode, const FunctionDeclarationNode* node)
{
	std::string nodeName;
	if (node->Name() != "main")
		nodeName = node->Name();
	else
		nodeName = "_main";
	const std::vector<VariableDeclarationNode*>& sigArgs = node->Signature().Args();
	if (const ArrayNode* arrBody = ActionNode::IsArray(node->Body()))
	{
		PrintIndent(outCode);
		constexpr const char* outVarName = "__out__name";
		outCode << fmt::format("void {}(", nodeName);
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
		
		std::string outResName = "__out__res";
		if (auto varRes = dynamic_cast<const VariableNode*>(node->Body()))
		{
			outResName = varRes->Name();
		}
		else
		{
			PrintIndent(outCode);
			outCode << "double " << outResName << "[" << arrBody->Values().size() << "] = ";
			Process(outCode, node->Body());
			outCode << ";\n";
		}
		PrintIndent(outCode);
		outCode << outVarName << " = " << outResName;
		outCode << ";\n}\n";
			
	}
	else
	{
		outCode << fmt::format("double {}(", nodeName);
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
		
		PrintIndent(outCode);
		outCode << "return ";
		Process(outCode, node->Body());
		outCode << ";\n}\n";
	}
}
}
