#version 430 core

in vec2 TexCoord;

layout (location = 0) out float FragAO;

uniform sampler2D sceneDepth;
uniform sampler2D texNoise;

uniform mat4 projection;
uniform mat4 inverseProjection;

uniform vec3 samples[64];
uniform float radius;
uniform float bias;
uniform float noiseScaleX;
uniform float noiseScaleY;
uniform vec3 texelSize;

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

vec3 ReconstructViewNormal(
    vec2 uv,
    vec3 centerPosition
)
{
    vec2 dx = vec2(texelSize.x, 0.0);
    vec2 dy = vec2(0.0, texelSize.y);

    float depthLeft = texture(
        sceneDepth,
        uv - dx
    ).r;

    float depthRight = texture(
        sceneDepth,
        uv + dx
    ).r;

    float depthDown = texture(
        sceneDepth,
        uv - dy
    ).r;

    float depthUp = texture(
        sceneDepth,
        uv + dy
    ).r;

    bool rightIsBetter =
        depthRight < 0.999999 &&
        (
            depthLeft >= 0.999999 ||
            abs(depthRight - texture(sceneDepth, uv).r) <=
            abs(depthLeft - texture(sceneDepth, uv).r)
        );

    bool upIsBetter =
        depthUp < 0.999999 &&
        (
            depthDown >= 0.999999 ||
            abs(depthUp - texture(sceneDepth, uv).r) <=
            abs(depthDown - texture(sceneDepth, uv).r)
        );

    vec3 horizontal;

    if (rightIsBetter)
    {
        horizontal =
            ReconstructViewPosition(
                uv + dx,
                depthRight
            ) - centerPosition;
    }
    else if (depthLeft < 0.999999)
    {
        horizontal =
            centerPosition -
            ReconstructViewPosition(
                uv - dx,
                depthLeft
            );
    }
    else
    {
        horizontal = dFdx(centerPosition);
    }

    vec3 vertical;

    if (upIsBetter)
    {
        vertical =
            ReconstructViewPosition(
                uv + dy,
                depthUp
            ) - centerPosition;
    }
    else if (depthDown < 0.999999)
    {
        vertical =
            centerPosition -
            ReconstructViewPosition(
                uv - dy,
                depthDown
            );
    }
    else
    {
        vertical = dFdy(centerPosition);
    }

    vec3 normal = normalize(
        cross(horizontal, vertical)
    );


    return normal;
}

void main()
{
    float centerDepth = texture(
        sceneDepth,
        TexCoord
    ).r;

    if (centerDepth >= 0.999999)
    {
        FragAO = 1.0;
        return;
    }

    vec3 viewPosition =
        ReconstructViewPosition(
            TexCoord,
            centerDepth
        );

    vec3 normal = ReconstructViewNormal(
        TexCoord,
        viewPosition
    );

    if (dot(normal, normal) < 0.25)
    {
        FragAO = 1.0;
        return;
    }

    vec3 randomVector = texture(
        texNoise,
        TexCoord * vec2(noiseScaleX, noiseScaleY)
    ).xyz;

    randomVector = normalize(randomVector);

    vec3 tangent =
        randomVector -
        normal * dot(randomVector, normal);

    float tangentLengthSquared = dot(
        tangent,
        tangent
    );

    if (tangentLengthSquared < 0.0001)
    {
        tangent =
            abs(normal.z) < 0.999
                ? cross(normal, vec3(0.0, 0.0, 1.0))
                : cross(normal, vec3(0.0, 1.0, 0.0));
    }
    else
    {
        tangent *= inversesqrt(tangentLengthSquared);
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
    int validSamples = 0;

    for (int i = 0; i < 64; ++i)
    {
        vec3 samplePosition =
            viewPosition +
            (TBN * samples[i]) * radius;

        vec4 projected =
            projection *
            vec4(samplePosition, 1.0);

        if (projected.w <= 0.000001)
            continue;

        projected.xyz /= projected.w;

        vec2 sampleUV =
            projected.xy * 0.5 + 0.5;

        if (
            sampleUV.x <= 0.0 ||
            sampleUV.x >= 1.0 ||
            sampleUV.y <= 0.0 ||
            sampleUV.y >= 1.0
        )
        {
            continue;
        }

        float sampledDepth = texture(
            sceneDepth,
            sampleUV
        ).r;

        if (sampledDepth >= 0.999999)
            continue;

        vec3 sampledPosition =
            ReconstructViewPosition(
                sampleUV,
                sampledDepth
            );

        float depthDelta =
            sampledPosition.z - samplePosition.z;

        // Surface must be in front of the sample point to occlude it.
        if (depthDelta >= bias)
        {
            float distanceToSurface = abs(
                viewPosition.z -
                sampledPosition.z
            );

            // Reject discontinuities that are too far away to be a real
            // local occluder. This is especially important on terrain edges
            // when viewed at a shallow angle.
            if (distanceToSurface <= radius)
            {
                float rangeCheck =
                    1.0 - smoothstep(
                        0.0,
                        radius,
                        distanceToSurface
                    );

                occlusion += rangeCheck;
            }
        }

        ++validSamples;
    }

    if (validSamples == 0)
    {
        FragAO = 1.0;
        return;
    }

    float ao =
        1.0 -
        (occlusion / float(validSamples));

    FragAO = clamp(
        ao,
        0.0,
        1.0
    );
}
