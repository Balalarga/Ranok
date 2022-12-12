#version 420 core
/// Fixed step ray marcher

int STEPS = 600;
float EPSILON = 0.001;
float STEP_DELTA = 0.01; // EPSILON*10

uniform float uMaxRenderDistance = 1000.0;
uniform float uSmoothStep = 0.01;
uniform vec3 uCameraPosition = vec3(0.0, 0.0, 5.0);
uniform vec2 uCameraRotation = vec2(0, 0);
uniform vec2 uResolution;
uniform vec4 uBackgroundColor = vec4(0.8, 0.8, 0.8, 1.0);

struct PhongMaterial
{
/// Emissive component.
    vec4 ke;
/// Ambient component.
    vec4 ka;
/// Diffuse component.
    vec4 kd;
/// Specular component.
    vec4 ks;
/// Shiness.
    float sh;
};

struct DirectionalLight
{
/// Light direction.
    vec3 direction;
/// Light rgb color.
    vec4 color;
};

struct PointLight
{
/// Light direction.
    vec3 position;
/// Light rgb color.
    vec4 color;
/// Light color force
    float force;
};

PointLight lights[] = PointLight[](
    PointLight(vec3(10.0, 10.0, 0.0), vec4(1.0), 0.7),
    PointLight(vec3(10.0, -20.0, 0.0), vec4(1.0), 0.5)
);

PhongMaterial material = PhongMaterial(
    vec4(0.0),
    vec4(0.5, 0.2, 0.2, 1.0),
    vec4(0.5, 0.2, 0.2, 1.0),
    vec4(0.5, 0.2, 0.2, 1.0),
    10.f
);

out vec4 color;

{generatedFvmCode}

float SdfFunc(vec3 p)
{
    float pos[3] = float[](p.x, p.y, p.z);
    return -_main(pos);
}

float Matrix4x4Det(float[16] m)
{
    int n = 4;
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

vec3 FuncNormal(vec3[4] point)
{
    float wv[4];
    wv[0] = SdfFunc(point[0]);
    wv[1] = SdfFunc(point[1]);
    wv[2] = SdfFunc(point[2]);
    wv[3] = SdfFunc(point[3]);

    float a[] = float[](
    point[0].y, point[0].z, wv[0], 1.0,
    point[1].y, point[1].z, wv[1], 1.0,
    point[2].y, point[2].z, wv[2], 1.0,
    point[3].y, point[3].z, wv[3], 1.0
    );
    float b[] = float[](
    point[0].x, point[0].z, wv[0], 1.0,
    point[1].x, point[1].z, wv[1], 1.0,
    point[2].x, point[2].z, wv[2], 1.0,
    point[3].x, point[3].z, wv[3], 1.0
    );
    float c[] = float[](
    point[0].x, point[0].y, wv[0], 1.0,
    point[1].x, point[1].y, wv[1], 1.0,
    point[2].x, point[2].y, wv[2], 1.0,
    point[3].x, point[3].y, wv[3], 1.0
    );
    float d[] = float[](
    point[0].x, point[0].y, point[0].z, 1.0,
    point[1].x, point[1].y, point[1].z, 1.0,
    point[2].x, point[2].y, point[2].z, 1.0,
    point[3].x, point[3].y, point[3].z, 1.0
    );
    float f[] = float[](
    point[0].x, point[0].y, point[0].z, wv[0],
    point[1].x, point[1].y, point[1].z, wv[1],
    point[2].x, point[2].y, point[2].z, wv[2],
    point[3].x, point[3].y, point[3].z, wv[3]
    );
    float detA = Matrix4x4Det(a);
    float detB = Matrix4x4Det(b);
    float detC = Matrix4x4Det(c);
    //float detD = Matrix4x4Det(d);
    //float detF = Matrix4x4Det(f);
    //float div = sqrt(pow(detA, 2.0)+pow(detB, 2.0)+pow(detC, 2.0)+pow(detD, 2.0));
    return normalize(vec3(detA, -detB, detC));
    //return vec3(detA/div, -detB/div, detC/div);
}

vec3 GetNormal(vec3 p)
{
    float d = SdfFunc(p); // Distance
    vec2 e = vec2(EPSILON, 0); // Epsilon
    vec3 n = d - vec3(SdfFunc(p-e.xyy), SdfFunc(p-e.yxy), SdfFunc(p-e.yyx));

    return normalize(n);
}

vec3 GetLight(vec3 p, vec3 n)
{
    // Light (directional diffuse)
    vec3 l = normalize(lights[0].position - p); // Light Vector

    float dif = dot(n, l); // Diffuse light
    dif = clamp(dif, 0.0, 1.0); // Clamp so it doesnt go below 0

    return vec3(dif);
}

vec4 FuncLight(vec3 p, vec3 n)
{
    vec4 color;
    for (int i = 0; i < 2; ++i)
    {
        vec3 lightDirection = normalize(lights[i].position - p);
        vec3 viewDirection = normalize(uCameraPosition - p);

        float cosTheta = max(0.0, dot(n, normalize(lightDirection)));

        vec4 emissive = material.ke * lights[i].color;
        vec4 ambient = material.ka * lights[i].color;
        vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);

        //Only if light is visible from the surface point.
        if(cosTheta > 0.0)
        {
            //Reflection vector around normal.
            vec3 reflectionDirection = reflect(-lightDirection, n);

            //Diffuse component.
            diffuse = material.kd * lights[i].color * cosTheta;

            //Specular component.
            specular = material.ks * lights[i].color * pow(max(0.0, dot(reflectionDirection, viewDirection)), material.sh);
        }
        color += (emissive + ambient + diffuse + specular) * lights[i].force;
    }

    return color;
}

float March(vec3 origin, vec3 dir, float dStep, out vec3 point)
{
    float depth = 0.0;
    for(int i = 0; i < STEPS; ++i)
    {
        point = origin + depth * dir;
        if (-SdfFunc(point) < EPSILON)
            return depth;

        depth += dStep;
    }
    return uMaxRenderDistance;
}

vec4 FvmRayMarching(vec3 origin, vec3 dir)
{
    float dStep = uMaxRenderDistance / float(STEPS);
    vec3 basePoint;
    if (March(origin, dir, dStep, basePoint) == uMaxRenderDistance)
        return uBackgroundColor;

    vec3 newPoint;
    for (int i = 0; i < 4; ++i)
    {
        if (March(origin, dir, uSmoothStep, newPoint) != uMaxRenderDistance)
            basePoint = newPoint;
    }

    vec3 npoints[4] = vec3[](
        basePoint,
        basePoint + vec3(STEP_DELTA, 0.0, 0.0),
        basePoint + vec3(0.0, STEP_DELTA, 0.0),
        basePoint - vec3(0.0, 0.0, STEP_DELTA)
    );
    return FuncLight(basePoint, FuncNormal(npoints));
}

vec4 RayMarching(vec3 origin, vec3 dir)
{
    float dStep = uMaxRenderDistance / float(STEPS);
    vec3 point;
    if (March(origin, dir, dStep, point) != uMaxRenderDistance)
    {
        vec3 newPoint;
        for (int i = 0; i < 4; ++i)
        {
            float depth = March(origin, dir, uSmoothStep, newPoint);
            if (depth != uMaxRenderDistance)
            point = newPoint;
        }
        return vec4(GetLight(point, GetNormal(point)), 1.0);
    }
    
    return uBackgroundColor;
}

// camera rotation : pitch, yaw
mat3 RotationXY(vec2 angle)
{
    vec2 c = cos(angle);
    vec2 s = sin(angle);

    return mat3(
        c.y      ,  0.0, -s.y,
        s.y * s.x,  c.x,  c.y * s.x,
        s.y * c.x, -s.x,  c.y * c.x
    );
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = (fragCoord - 0.5 * uResolution)/uResolution.y;

    vec3 rd = normalize(vec3(uv, -1.0));

    // rotate camera
    mat3 rot = RotationXY(uCameraRotation);
    rd = rot * rd;
    vec3 eye = rot * uCameraPosition;
    
    fragColor = FvmRayMarching(eye, rd);
}