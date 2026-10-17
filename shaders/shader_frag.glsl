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

uniform int displayMode; // 0 - Normal, 1 - Material, 2 - Texture, 3 - PBR

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 normal = normalize(fragNormal);

    if (displayMode == 0)        { fragColor = vec4(normal, 1); }
    else if (displayMode == 1)   { fragColor = vec4(kd, 1);}
    else if (displayMode == 2)   { fragColor = vec4(texture(color, fragTexCoord).rgb, 1);}
    else if (displayMode == 3)   { fragColor = vec4(texture(color, fragPosition.xz * 0.5).rgb, 1); } // Output color value, change from (1, 0, 0) to something else
    else                         { fragColor = vec4(normal, 1); } // Output color value, change from (1, 0, 0) to something else
}
