#include "RaymarchingView.h"

#include <sstream>

#include "Config/ConfigManager.h"
#include "Config/IConfig.h"
#include "Graphics/Shading/ShaderPart.h"

#include "Log/Logger.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{

class RaymarchingConfig: public IConfig
{
public:
    RaymarchingConfig(): IConfig("Modules/RayMarchingViewConfig")
    {
        
    }
	
    void Serialize(JsonArchive& archive) override
    {
        archive.Serialize("resolution", resolution);
        archive.Serialize("shininess", shininess);
        archive.Serialize("gradStep", gradStep);
        archive.Serialize("light_pos1", light_pos1);
        archive.Serialize("light_color1", light_color1);
        archive.Serialize("light_pos2", light_pos2);
        archive.Serialize("light_color2", light_color2);
        archive.Serialize("targetColor", targetColor);
        archive.Serialize("backgroundColor", backgroundColor);
        archive.Serialize("cameraPosition", cameraPosition);
        archive.Serialize("cameraRotation", cameraRotation);
    }
    bool operator!=(const IConfig& oth) override
    {
        const auto casted = dynamic_cast<const RaymarchingConfig*>(&oth);
        return casted && 
            casted->light_pos1 != light_pos1 &&
            casted->light_color1 != light_color1 &&
            casted->light_pos2 != light_pos2 &&
            casted->light_color2 != light_color2 &&
            casted->targetColor != targetColor &&
            casted->backgroundColor != backgroundColor &&
            casted->resolution != resolution &&
            casted->cameraPosition != cameraPosition &&
            casted->cameraRotation != cameraRotation;
    }
    
    float shininess = 16.0f;
    float gradStep = 0.02f;
    glm::vec3 light_pos1   = glm::vec3( 20.0, 20.0, 20.0 );
    glm::vec3 light_color1 = glm::vec3( 1.0, 0.7, 0.7 );
    glm::vec3 light_pos2   = glm::vec3( -20.0, -20.0, -30.0 );
    glm::vec3 light_color2 = glm::vec3( 0.5, 0.7, 1.0 );
    glm::vec3 targetColor = glm::vec3(0.2, 0.1, 0.1);
    glm::vec3 backgroundColor = glm::vec3(0.8, 0.8, 0.8);
    glm::vec2 resolution = glm::vec2(800, 600);
    glm::vec3 cameraPosition = glm::vec3(0, 0, 5);
    glm::vec2 cameraRotation = glm::vec2(0, 0);
};
static std::shared_ptr<RaymarchingConfig> rayMarchConfig = ConfigManager::Instance().CreateConfigs<RaymarchingConfig>();

glm::fvec2 RayMarchingView::vertices[6] = {
    //   x    y
    {-1, -1},
    {1, 1},
    {1, -1},
    {-1, -1},
    {1, 1},
    {-1, 1},
};

std::string RayMarchingView::shaderHeader = R"(
#version 420 core

out vec4 color;

uniform float shininess = 16.0;
uniform vec3 light_pos1  = vec3( 20.0, 20.0, 20.0 );
uniform vec3 light_color1 = vec3( 1.0, 0.7, 0.7 );
uniform vec3 light_pos2  = vec3( -20.0, -20.0, -30.0 );
uniform vec3 light_color2 = vec3( 0.5, 0.7, 1.0 );
uniform vec3 targetColor = vec3(0.2, 0.1, 0.1);
uniform vec3 backgroundColor = vec3(0.8, 0.8, 0.8);
uniform float gradStep = 0.02;
uniform vec2 resolution = vec2(800, 600);
uniform vec3 cameraPosition = vec3(0, 0, 5);
uniform vec2 cameraRotation = vec2(0, 0);

// ray marching
const int max_iterations = 2048;
const float stop_threshold = 0.001;
const float clip_far = 1000.0;

// math
const float PI = 3.14159265359;
const float DEG_TO_RAD = PI / 180.0;

)";
std::string RayMarchingView::shaderFooter = R"(

// get distance in the world
float dist_field( vec3 p ) {
    float params[3] = float[](p.x, p.y, p.z);
    return -_main(params);
}

// get gradient in the world
vec3 gradient( vec3 pos ) {
    vec3 dx = vec3( gradStep, 0.0, 0.0 );
    vec3 dy = vec3( 0.0, gradStep, 0.0 );
    vec3 dz = vec3( 0.0, 0.0, gradStep );
    return normalize (
                vec3(
                    dist_field( pos + dx ) - dist_field( pos - dx ),
                    dist_field( pos + dy ) - dist_field( pos - dy ),
                    dist_field( pos + dz ) - dist_field( pos - dz )
                    )
                );
}

vec3 fresnel( vec3 F0, vec3 h, vec3 l ) {
    return F0 + ( 1.0 - F0 ) * pow( clamp( 1.0 - dot( h, l ), 0.0, 1.0 ), 5.0 );
}

// phong shading
vec3 shading( vec3 v, vec3 n, vec3 dir, vec3 eye ) {
    // ...add lights here...

    vec3 final = vec3( 0.0 );

    vec3 ref = reflect( dir, n );

    vec3 Ks = vec3( 0.5 );
    vec3 Kd = vec3( 1.0 );

    // light 0
    {
        vec3 vl = normalize( light_pos1 - v );

        vec3 diffuse  = Kd * vec3( max( 0.0, dot( vl, n ) ) );
        vec3 specular = vec3( max( 0.0, dot( vl, ref ) ) );

        vec3 F = fresnel( Ks, normalize( vl - dir ), vl );
        specular = pow( specular, vec3( shininess ) );

        final += light_color1 * mix( diffuse, specular, F );
    }

    // light 1
    {
        vec3 vl = normalize( light_pos2 - v );

        vec3 diffuse  = Kd * vec3( max( 0.0, dot( vl, n ) ) );
        vec3 specular = vec3( max( 0.0, dot( vl, ref ) ) );

        vec3 F = fresnel( Ks, normalize( vl - dir ), vl );
        specular = pow( specular, vec3( shininess ) );

        final += light_color2 * mix( diffuse, specular, F );
    }

    final += targetColor * fresnel( Ks, n, -dir );

    return final;
}

bool ray_vs_aabb(vec3 o, vec3 dir, vec3 bmin, vec3 bmax, inout vec2 e ) {
    vec3 a = ( bmin - o ) / dir;
    vec3 b = ( bmax - o ) / dir;

    vec3 s = min( a, b );
    vec3 t = max( a, b );

    e.x = max( max( s.x, s.y ), max( s.z, e.x ) );
    e.y = max( min( t.x, t.y ), max( t.z, e.y ) );

    return e.x < e.y;
}

// ray marching
bool ray_marching( vec3 o, vec3 dir, inout float depth, inout vec3 n ) {
    float t = 0.0;
    float d = 10000.0;
    float dt = 0.0;
    for ( int i = 0; i < max_iterations; i++ ) {
        vec3 v = o + dir * t;
        d = dist_field( v );
        if ( d < 0.001 ) {
            break;
        }
        dt = min( abs(d), 0.1 );
        t += dt;
        if ( t > depth ) {
            break;
        }
    }

    if ( d >= 0.001 ) {
        return false;
    }

    t -= dt;
    for ( int i = 0; i < 4; i++ ) {
        dt *= 0.5;

        vec3 v = o + dir * ( t + dt );
        if ( dist_field( v ) >= 0.001 ) {
            t += dt;
        }
    }

    depth = t;
    n = normalize( gradient( o + dir * t ) );
    return true;

    return true;
}

// get ray direction
vec3 ray_dir( float fov, vec2 size, vec2 pos ) {
    vec2 xy = pos - size * 0.5;

    float cot_half_fov = tan( ( 90.0 - fov * 0.5 ) * DEG_TO_RAD );
    float z = size.y * 0.5 * cot_half_fov;

    return normalize( vec3( xy, -z ) );
}

// camera rotation : pitch, yaw
mat3 rotationXY( vec2 angle ) {
    vec2 c = cos( angle );
    vec2 s = sin( angle );

    return mat3(
                c.y      ,  0.0, -s.y,
                s.y * s.x,  c.x,  c.y * s.x,
                s.y * c.x, -s.x,  c.y * c.x
                );
}

void main()
{
    // default ray dir
    vec3 dir = ray_dir( 45.0, resolution, gl_FragCoord.xy );

    // default ray origin
    vec3 eye = cameraPosition;
    //vec3 eye = vec3(0, 0, 5);

    // rotate camera
    mat3 rot = rotationXY( cameraRotation );
    dir = rot * dir;
    eye = rot * eye;

    // ray marching
    float depth = clip_far;
    vec3 n = vec3( 0.0 );
    if ( !ray_marching( eye, dir, depth, n ) ) {
        color = vec4(backgroundColor, 1.0);
        return;
    }

    // shading
    vec3 pos = eye + dir * depth;

    vec3 newColor = shading( pos, n, dir, eye );
    color = vec4( pow( newColor, vec3(1.0/1.2) ), 1.0 );
})";

std::string RayMarchingView::defaultVertexShader = R"(
#version 330

layout(location = 0) in vec2 position;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

std::string RayMarchingView::defaultFragmentShader = shaderHeader + R"(
float _main(float s[3])
{
    return 1 - s[0]*s[0] - s[1]*s[1] - s[2]*s[2];
}
)" + shaderFooter;

Buffer RayMarchingView::bufferInfo(DataPtr(vertices, std::size(vertices), sizeof(vertices[0])), BufferLayout().Float(2));

ShaderGenerator RayMarchingView::_codeGenerator;

std::shared_ptr<ShaderPart> RayMarchingView::_defaultVShader = std::make_shared<ShaderPart>(ShaderPart::Type::Vertex, defaultVertexShader);
std::shared_ptr<ShaderPart> RayMarchingView::_defaultFShader = std::make_shared<ShaderPart>(ShaderPart::Type::Fragment, defaultFragmentShader);
std::shared_ptr<Shader> RayMarchingView::_defaultShader = std::make_shared<Shader>(_defaultVShader, _defaultFShader);

RaymarchingMaterial::RaymarchingMaterial(const std::shared_ptr<Shader>& shader):
    IMaterial(shader)
{
    
}

void RaymarchingMaterial::SetupUniforms()
{
    SetUniform("shininess", rayMarchConfig->shininess);
    SetUniform("light_pos1", rayMarchConfig->light_pos1);
    SetUniform("light_color1", rayMarchConfig->light_color1);
    SetUniform("light_pos2", rayMarchConfig->light_pos2);
    SetUniform("light_color2", rayMarchConfig->light_color2);
    SetUniform("targetColor", rayMarchConfig->targetColor);
    SetUniform("backgroundColor", rayMarchConfig->backgroundColor);
    SetUniform("gradStep", rayMarchConfig->gradStep);
    SetUniform("resolution", rayMarchConfig->resolution);
    SetUniform("cameraPosition", rayMarchConfig->cameraPosition);
    SetUniform("cameraRotation", rayMarchConfig->cameraRotation);
}

RayMarchingView::RayMarchingView():
    FrameBuffer(rayMarchConfig->resolution),
    Object(bufferInfo, nullptr),
    _material(_defaultShader)
{
    SetMaterial(&_material);
}

std::optional<std::string> RayMarchingView::SetProgram(ActionTree& tree)
{
    std::optional<std::string> code = _codeGenerator.Generate(tree);
    if (!code.has_value())
        return code;
    
    std::stringstream stream;
    stream << shaderHeader;
    stream << code.value();
    stream << shaderFooter;
    std::string fullCode = stream.str();
    
    bool status = GetMaterial()->GetShader().lock()->UpdateShaderPart(std::make_shared<ShaderPart>(ShaderPart::Type::Fragment, fullCode));
    if (!status)
        Logger::Error("Shader compilation error");
    return fullCode;
}

void RayMarchingView::Bind()
{
    std::shared_ptr<Shader> shader = _material.GetShader().lock();
    if (!shader->IsCompiled())
        shader->Compile();
    FrameBuffer::Bind();
}
}
