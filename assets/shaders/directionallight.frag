#version 410 core

in block
{
	vec2 Texcoord;
} In; 

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2DShadow Shadow;

layout(location = 0, index = 0) out vec4 Color;

uniform mat4 InverseProjection;

uniform light
{
	vec3 Direction;
	vec3 Color;
	float Intensity;
        mat4 WorldToLightScreen;
} DirectionalLight;

vec3 directionalLight(in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{

#if 1
	vec3 l = normalize(-DirectionalLight.Direction);
	float ndotl = max(dot(n, l), 0.0);
	vec3 h = normalize(l+v);
	float ndoth = max(dot(n, h), 0.0);
	return DirectionalLight.Color * DirectionalLight.Intensity * (diffuseColor * ndotl + specularColor * pow(ndoth, specularPower));
#endif

#if 0
    //Kajiya-Kay lighting
    vec3 l = normalize(-DirectionalLight.Direction);
	float ndotl = sqrt( 1 - pow(max(dot(n, l), 0.0),2) );
	vec3 h = normalize(l+v);
	float ndoth = sqrt(1-pow(max(dot(n, h), 0.0),2));
	return DirectionalLight.Color * DirectionalLight.Intensity * (diffuseColor * ndotl + specularColor * pow(ndoth, specularPower));
#endif
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

	vec4 wlP = DirectionalLight.WorldToLightScreen * vec4(p, 1.0);
	vec3 lP = vec3(wlP / wlP.w) * 0.5 + 0.5;

	float shadowDepth = textureProj(Shadow, vec4(lP.xy, lP.z -0.005, 1.0), 0.0);
	Color = vec4(diffuseColor * 0.2, 1.f) + shadowDepth * vec4(directionalLight(n, v, diffuseColor, specularColor, specularPower), 1.0);
        //Color = vec4( vec3(shadowDepth), 1.0);
}
