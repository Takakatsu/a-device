Texture2D		g_texture0 : register(t0);
SamplerState	g_sampler0 : register(s0);

namespace s3d
{
	struct PSInput
	{
		float4 position	: SV_POSITION;
		float4 color	: COLOR0;
		float2 uv		: TEXCOORD0;
	};
}

cbuffer PSConstants2D : register(b0)
{
	float4 g_colorAdd;
	float4 g_sdfParam;
	float4 g_sdfOutlineColor;
	float4 g_sdfShadowColor;
	float4 g_internal;
}

cbuffer MyPCBreakFilter : register(b1)
{
	float time;
	float level;
}

float4 hash42(float2 p)
{
	float4 p4 = frac(float4(p.x, p.y, p.x, p.y) * float4(443.8975, 397.2973, 491.1871, 470.7827));
	p4 += dot(p4.wzxy, p4 + 19.19);
	return frac(float4(p4.x * p4.y, p4.x * p4.z, p4.y * p4.w, p4.x * p4.w));
}

float hash(float n)
{
	return frac(sin(n) * 43758.5453123);
}

float calc_n(in float3 x) {
	float3 p = floor(x);
	float3 f = frac(x);
	f = f * f * (3.0 - 2.0 * f);
	float n = p.x + p.y * 57.0 + 113.0 * p.z;
	float res = lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
		lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
		lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
			lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
	return res;
}

float calc_nn(float t, float lv, float2 p)
{
	float y = p.y;
	float s = t * 2.0;

	float v = (calc_n(float3(y * .01 + s, 1.0, 1.0)) + 0.0)
		* (calc_n(float3(y * 0.011 + 1000.0 + s, 1.0, 1.0)) + 0.0)
		* (calc_n(float3(y * 0.51 + 421.0 + s, 1.0, 1.0)) + 0.0);
	v *= hash42(float2(p.x + t * 0.01, p.y)).x + 0.3;
	v = pow(v + 0.3, 1.0);
	if (v < lv * 1.3) v = 0.0;
	return v;
}

float4 PS(s3d::PSInput input) : SV_TARGET
{
	float4 texColor = g_texture0.Sample(g_sampler0, input.uv);
	float one_y = 0.1;
	float2 uv = floor(input.uv * 100 / one_y) * one_y;
	float lev = calc_nn(time,level,uv);
	texColor.rgb *= float3(lev, lev, lev) * (1 - pow(level,5));
	return (texColor * input.color) + g_colorAdd;
}
