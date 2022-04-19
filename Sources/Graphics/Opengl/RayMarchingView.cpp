#include "RayMarchingView.h"

#include <sstream>
#include "Graphics/Gui/Scene.h"


glm::fvec2 RayMarchingView::vertices[6] = {
//   x    y
    {-1, -1},
    {1, 1},
    {1, -1},
    {-1, -1},
    {1, 1},
    {-1, 1},
};

BufferInfo RayMarchingView::bufferInfo(vertices, 6, GL_TRIANGLES, BufferLayout({LayoutItemData(GL_FLOAT, 2)}));

std::string RayMarchingView::shaderHeader = R"(
#version 330

out vec4 color;

uniform float grad_step = 0.02;
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
    return -__resultFunc(params);
}

// get gradient in the world
vec3 gradient( vec3 pos ) {
    vec3 dx = vec3( grad_step, 0.0, 0.0 );
    vec3 dy = vec3( 0.0, grad_step, 0.0 );
    vec3 dz = vec3( 0.0, 0.0, grad_step );
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

    float shininess = 16.0;

    vec3 final = vec3( 0.0 );

    vec3 ref = reflect( dir, n );

    vec3 Ks = vec3( 0.5 );
    vec3 Kd = vec3( 1.0 );

    // light 0
    {
        vec3 light_pos   = vec3( 20.0, 20.0, 20.0 );
        vec3 light_color = vec3( 1.0, 0.7, 0.7 );

        vec3 vl = normalize( light_pos - v );

        vec3 diffuse  = Kd * vec3( max( 0.0, dot( vl, n ) ) );
        vec3 specular = vec3( max( 0.0, dot( vl, ref ) ) );

        vec3 F = fresnel( Ks, normalize( vl - dir ), vl );
        specular = pow( specular, vec3( shininess ) );

        final += light_color * mix( diffuse, specular, F );
    }

    // light 1
    {
        vec3 light_pos   = vec3( -20.0, -20.0, -30.0 );
        vec3 light_color = vec3( 0.5, 0.7, 1.0 );

        vec3 vl = normalize( light_pos - v );

        vec3 diffuse  = Kd * vec3( max( 0.0, dot( vl, n ) ) );
        vec3 specular = vec3( max( 0.0, dot( vl, ref ) ) );

        vec3 F = fresnel( Ks, normalize( vl - dir ), vl );
        specular = pow( specular, vec3( shininess ) );

        final += light_color * mix( diffuse, specular, F );
    }

    final += vec3(0.2, 0.1, 0.1) * fresnel( Ks, n, -dir );

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
        color = vec4(0.8, 0.8, 0.8, 1.0);
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
float __resultFunc(float s[3])
{
    return 1 - s[0]*s[0] - s[1]*s[1] - s[2]*s[2];
}
)" + shaderFooter;


CodeGenerator::LanguageDefinition RayMarchingView::_languageDefenition =
        CodeGenerator::LanguageDefinition().MainFuncName("__resultFunc")
        .Functions({{"abs", "abs"}})
        .NumberType("float")
        .NumberArrayType("float")
        .ArrayParamSignature("{0} {1}[{2}]");

CodeGenerator RayMarchingView::_codeGenerator(_languageDefenition);


RayMarchingView::RayMarchingView(Scene *parent):
    Renderable(parent, new Shader(defaultVertexShader, defaultFragmentShader), bufferInfo)
{
    GetShader()->AddUniforms({"grad_step", "resolution", "cameraPosition", "cameraRotation"});
}

bool RayMarchingView::SetModel(Program &program)
{
    std::stringstream stream;
    stream << shaderHeader;
    std::string code = _codeGenerator.Generate(program);
    stream << code;
    stream << shaderFooter;

    return GetShader()->UpdateFragmentShader(stream.str());
}

void RayMarchingView::Render()
{
    const auto& size = Parent()->GetRenderSize();
    GetShader()->Bind();
    GetShader()->SetUniform("resolution", glm::vec2(size.x, size.y));
    GetShader()->SetUniform("cameraPosition", glm::vec3(0, 0, Parent()->GetCamera().Zoom));
    GetShader()->SetUniform("cameraRotation", glm::vec2(Parent()->GetCamera().Pitch/5.0f, -Parent()->GetCamera().Yaw/5.0f));
    Renderable::Render();
}
