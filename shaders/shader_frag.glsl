#version 410

#define PI 3.14159265359

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
	vec3 ks;
	float shininess;
	float transparency;
};

// PBR Shading Inputs. Material texture maps
uniform sampler2D color;
uniform sampler2D displacement;
uniform sampler2D roughness;
uniform float metallic;

uniform int displayMode; // 0 - Normal, 1 - Material, 2 - Texture, 3 - PBR

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;

layout(location = 0) out vec4 fragColor;


float G1(vec3 h, vec3 v, vec3 n, float alpha) {
    float char_plus = (dot(v, h) / dot (v, n) > 0) ? 1.0 : 0.0; 

    float c = dot(n, v);
    float tan2 = (1.0 - c*c) / (c*c); 

    return char_plus * 2.0 / (1 + sqrt(1+ alpha*alpha*tan2));
}

float G2(vec3 h, vec3 v, vec3 l, vec3 n, float alpha) {
    return G1(h, v, n, alpha) * G1(h, l, n, alpha);
}

vec3 F(vec3 f_0, vec3 n, vec3 l) {
    return f_0 + (1-f_0)*pow(1-max(dot(n, l), 0.0), 5.0);
}

float D(vec3 h, vec3 n, float alpha) {
    float c = dot(n, h);
    float c_4 = c*c*c*c;
    float tan2 = max((1.0 - c*c) / (c*c), 0.001f);

    return alpha*alpha / (PI*c_4 * pow(alpha*alpha + tan2, 2.0));
}

vec4 pbr() {
    float r = texture(roughness, fragTexCoord).r;
    float alpha = r * r;
    vec3 albedo = texture(color, fragTexCoord).rgb;

    vec3 v = normalize(cameraPosition - fragPosition);
    vec3 l = normalize(lightDirection);
    vec3 h = normalize(v + l);
    vec3 n = normalize(fragNormal);
    
    vec3 f_0 = mix(vec3(0.04), albedo, metallic);
    vec3 f = F(f_0, h, v);
    float g2 = G2(h, v, l, n, alpha);
    float d = D(h, n, alpha);
    
    vec3 radiance = lightColor * lightIntensity;

    float NdotL = max(dot(n, l), 0.0);
    float NdotV = max(dot(n, v), 0.0);

    vec3 kD = (1.0 - f) * (1.0 - metallic);
    vec3 diffuse = kD * albedo / PI;
    vec3 specular = (f*g2*d / max(4.0 * NdotV * NdotL, 0.001f));

    vec3 c = radiance * (specular + diffuse) * NdotL + ambientColor * albedo;

    return vec4(pow(c, vec3(1.0 / 2.2)), 1);

}

void main()
{
    vec3 normal = normalize(fragNormal);

    if (displayMode == 0)        { fragColor = vec4(normal, 1); }
    else if (displayMode == 1)   { fragColor = vec4(kd, 1);}
    else if (displayMode == 2)   { fragColor = vec4(texture(color, fragTexCoord).rgb * (max(dot(lightDirection, normal), 0.0) + ambientColor), 1);}
    else if (displayMode == 3)   { fragColor = pbr(); }
    else { fragColor = vec4(normal, 1); } // Output color value, change from (1, 0, 0) to something else
}
