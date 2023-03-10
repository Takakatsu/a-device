# version 410

uniform sampler2D Texture0;

layout(location = 0) in vec4 Color;
layout(location = 1) in vec2 UV;

layout(location = 0) out vec4 FragColor;

layout(std140) uniform PSConstants2D
{
	vec4 g_colorAdd;
	vec4 g_sdfParam;
	vec4 g_sdfOutlineColor;
	vec4 g_sdfShadowColor;
	vec4 g_internal;
};

layout(std140) uniform MyPCFilter
{
	float time;
};

//void main()
//{
//	vec4 texColor=texture(Texture0,UV);

//	float step=4.0;

//	texColor=texColor/step;

//	FragColor = (texColor * Color) + g_colorAdd;
//}

void main()
{
	vec2 ra = texture(Texture0, UV + vec2(-0.02, 0.0)).ra;
	vec2 ga = texture(Texture0, UV).ga;
	vec2 ba = texture(Texture0, UV + vec2(+0.02, 0.0)).ba;

	float a = (ra.y + ga.y + ba.y) / 3;
	vec4 texColor = vec4(ra.x, ga.x, ba.x, a);	

	FragColor = (texColor * Color) + g_colorAdd;
}