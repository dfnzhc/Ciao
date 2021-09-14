#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
    float tesselationScale;
};

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout (location=0) out vec4 out_FragColor;

layout (binding = 1) uniform sampler2D tex_Diffuse;
layout (binding = 2) uniform sampler2D tex_Displacement;
layout (binding = 3) uniform sampler2D tex_Normal;

mat3 cotangentFrame( vec3 N, vec3 p, vec2 tuv) 
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( tuv );
    vec2 duv2 = dFdy( tuv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );

    // calculate handedness of the resulting cotangent frame
    float w = (dot(cross(N, T), B) < 0.0) ? -1.0 : 1.0;

    // adjust tangent if needed
    T = T * w;

    return mat3( T * invmax, B * invmax, N );
}

vec3 GetNormal(vec3 n, vec3 v, vec3 normalSample, vec2 tuv)
{
    vec3 map = normalize( 2.0 * normalSample - vec3(1.0) );
    mat3 TBN = cotangentFrame(n, v, tuv);
    return normalize(TBN * map);
}

void main()
{
    vec3 n = normalize(normal);
    
    vec3 normalSample = texture(tex_Normal, uv).xyz;
    n = GetNormal(n, normalize(cameraPos.xyz - worldPos), normalSample, uv);
    out_FragColor = vec4(n, 1.0);
}