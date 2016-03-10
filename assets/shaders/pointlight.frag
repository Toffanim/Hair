#version 410 core

in block
{
	vec2 Texcoord;
} In; 

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform samplerCube Shadow;

layout(location = 0, index = 0) out vec4 Color;

uniform mat4 InverseProjection;

uniform light
{
	vec3 Position;
	vec3 Color;
	float Intensity;
} PointLight;

vec3 pointLight( in vec3 p, in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{
	vec3 l = normalize(PointLight.Position - p);
	float ndotl = max(dot(n, l), 0.0);
	vec3 h = normalize(l+v);
	float ndoth = max(dot(n, h), 0.0);
	float d = distance(PointLight.Position, p);
	float att = 1.f / (d*d);
	return att * PointLight.Color * PointLight.Intensity * (diffuseColor * ndotl + specularColor * pow(ndoth, specularPower));
}

void main(void)
{
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord).rgba;
	vec4 normalBuffer = texture(NormalBuffer, In.Texcoord).rgba;
	float depth = texture(DepthBuffer, In.Texcoord).r;

	vec3 n = normalBuffer.rgb;
	vec3 diffuseColor = colorBuffer.rgb;
	vec3 specularColor = colorBuffer.aaa;
	float specularPower = normalBuffer.a;

	vec2 xy = In.Texcoord * 2.0 -1.0;
	vec4 wP = InverseProjection * vec4(xy, depth * 2.0 -1.0, 1.0);
	vec3 p = vec3(wP.xyz / wP.w);
	vec3 v = normalize(-p);

        vec3 dir = p - PointLight.Position;
        float closestDepth = texture( Shadow, dir ).r;
        closestDepth *= 100.f;
        float currentDepth = length(dir);
        float shadow = currentDepth - 0.05 > closestDepth ? 0.0 : 1.0;
        
	Color = vec4(pointLight(p, n, v, diffuseColor, specularColor, specularPower), 1.0);
        Color = shadow * vec4(pointLight(p, n, v, diffuseColor, specularColor, specularPower), 1.0);
}
