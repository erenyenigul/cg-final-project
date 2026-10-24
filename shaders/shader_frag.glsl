#version 410

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
	vec3 ks;
	float shininess;
	float transparency;
};

uniform sampler2D color;
uniform sampler2D displacement;
uniform sampler2D roughness;
uniform sampler2D normalMap;
uniform samplerCube environmentMap;
uniform bool hasPBRMaterial;

uniform int displayMode; // 0 - Normal, 1 - Material, 2 - Texture, 3 - PBR

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragTangent;


uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform float lightIntensity;

layout(location = 0) out vec4 fragColor;

vec3 getNormalFromMap(vec2 uv)
{
    vec3 tangentNormal = texture(normalMap, uv).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    vec3 N = normalize(fragNormal);

    vec3 T = normalize(fragTangent.xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = fragTangent.w * cross(N, T);

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


void main()
{
    vec3 normal = normalize(fragNormal);

    if (displayMode == 0)        { fragColor = vec4(normal, 1); }
    else if (displayMode == 1)   { fragColor = vec4(kd, 1);}
    else if (displayMode == 2)   { fragColor = vec4(texture(color, fragTexCoord).rgb * dot(lightDirection, normal), 1);}
    else if (displayMode == 3) {
        vec2 uv = fragTexCoord;

        vec3 N = hasPBRMaterial ? getNormalFromMap(uv) : normal;
        vec3 albedo = hasPBRMaterial ? texture(color, uv).rgb : kd;

        float diffuse = max(dot(normalize(-lightDirection), N), 0.0);
        vec3 diffuseColor = albedo * diffuse * lightColor * lightIntensity;

        vec3 I = normalize(fragPosition - cameraPosition);
        vec3 R = reflect(I, N);
        vec3 reflectionColor = texture(environmentMap, R).rgb;

        vec3 finalColor = mix(diffuseColor, reflectionColor, clamp(ks, 0.0, 1.0)); // subject tp change
        
        fragColor = vec4(finalColor, 1.0);


    }// Output color value, change from (1, 0, 0) to something else
    
    else                         { fragColor = vec4(normal, 1); } // Output color value, change from (1, 0, 0) to something else
}
