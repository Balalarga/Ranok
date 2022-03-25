#pragma once

#include <glm.hpp>
#include <Ranok/LanguageCore/CodeGenerator.h>

#include "Renderable.h"

class Scene;

class RayMarchingView: public Renderable
{
public:
    RayMarchingView(Scene* parent);
    bool SetModel(Program& function);

    void Render() override;


private:
    static glm::fvec2 vertices[6];
    static BufferInfo bufferInfo;
    static std::string shaderHeader;
    static std::string shaderFooter;
    static std::string defaultFragmentShader;
    static std::string defaultVertexShader;

    static CodeGenerator::LanguageDefinition _languageDefenition;
    static CodeGenerator _codeGenerator;
};

