#version 430 core

in vec2 TexCoord;

layout (location = 0) out float FragAO;

uniform sampler2D sceneDepth;
uniform sampler2D sceneNormal;
uniform sampler2D texNoise;

uniform mat4 projection;
uniform mat4 inverseProjection;

uniform vec3 samples[64];
uniform float radius;
uniform float bias;
uniform float noiseScaleX;
uniform float noiseScaleY;
uniform vec3 texelSize;

const float DEPTH_CLEAR = 0.999999;

vec3 ReconstructViewPosition(
    vec2 uv,
    float depth
)
{
    vec4 clip = vec4(
        uv * 2.0 - 1.0,
        depth * 2.0 - 1.0,
        1.0
    );

    vec4 view = inverseProjection * clip;

    if (abs(view.w) < 0.000001)
        return vec3(0.0);

    return view.xyz / view.w;
}

bool ValidDepth(float depth)
{
    return depth < DEPTH_CLEAR;
}

bool ProjectToUV(
    vec3 viewPosition,
    out vec2 uv
)
{
    vec4 projected =
        projection *
        vec4(viewPosition, 1.0);

    if (projected.w <= 0.000001)
    {
        uv = vec2(0.0);
        return false;
    }

    projected.xyz /= projected.w;

    uv = projected.xy * 0.5 + 0.5;

    return
        uv.x > 0.0 &&
        uv.x < 1.0 &&
        uv.y > 0.0 &&
        uv.y < 1.0;
}

void main()
{
    float centerDepth = texture(
        sceneDepth,
        TexCoord
    ).r;

    if (!ValidDepth(centerDepth))
    {
        FragAO = 1.0;
        return;
    }

    vec3 viewPosition =
        ReconstructViewPosition(
            TexCoord,
            centerDepth
        );

    vec3 normal = normalize(
        texture(
            sceneNormal,
            TexCoord
        ).xyz
    );

    if (dot(normal, normal) < 0.5)
    {
        FragAO = 1.0;
        return;
    }

    vec3 randomVector = texture(
        texNoise,
        TexCoord * vec2(
            noiseScaleX,
            noiseScaleY
        )
    ).xyz;

    randomVector = normalize(
        randomVector
    );

    vec3 tangent =
        randomVector -
        normal * dot(
            randomVector,
            normal
        );

    float tangentLengthSquared =
        dot(tangent, tangent);

    if (tangentLengthSquared < 0.0001)
    {
        tangent =
            abs(normal.z) < 0.999
                ? cross(
                    normal,
                    vec3(0.0, 0.0, 1.0)
                  )
                : cross(
                    normal,
                    vec3(0.0, 1.0, 0.0)
                  );

        tangent = normalize(tangent);
    }
    else
    {
        tangent *= inversesqrt(
            tangentLengthSquared
        );
    }

    vec3 bitangent = normalize(
        cross(normal, tangent)
    );

    mat3 TBN = mat3(
        tangent,
        bitangent,
        normal
    );

    float occlusion = 0.0;

    for (int i = 0; i < 64; ++i)
    {
        vec3 samplePosition =
            viewPosition +
            TBN * samples[i] * radius;

        vec2 sampleUV;

        if (!ProjectToUV(
            samplePosition,
            sampleUV
        ))
        {
            continue;
        }

        float sampledDepth = texture(
            sceneDepth,
            sampleUV
        ).r;

        if (!ValidDepth(sampledDepth))
            continue;

        vec3 sampledPosition =
            ReconstructViewPosition(
                sampleUV,
                sampledDepth
            );

        float depthDifference =
            sampledPosition.z -
            samplePosition.z;

        if (depthDifference > bias)
        {
            float surfaceDistance = length(
                sampledPosition - viewPosition
            );

            float rangeWeight = 1.0 - smoothstep(
                radius,
                radius * 1.5,
                surfaceDistance
            );

            occlusion += rangeWeight;
        }
    }

    float ao =
        1.0 -
        (occlusion / 64.0);

    FragAO = clamp(
        ao,
        0.0,
        1.0
    );
}
