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

cbuffer MyPCFilter : register(b1)
{
	float time;
}

float4 PS(s3d::PSInput input) : SV_TARGET
{
	//posterize
	float4 texColor = g_texture0.Sample(g_sampler0, input.uv);

	float step = 4.0;

	texColor.rgb = round(texColor.rgb * step) / step;

	//noise
	float nIntensity = 0.2;
	float sIntensity = 0.2;
	float sCount = 1000.0;

	float x = input.uv.x * input.uv.y * 1000.0 * time;
	x = fmod(x, 13.0) * fmod(x, 123.0);
	float dx = fmod(x, 0.01);

	float3 cResult = texColor.rgb + texColor.rgb * (clamp(0.1 + dx * 100.0, 0.0, 1.0)-0.5);
	float2 sc = float2(sin(input.uv.y * sCount), cos(input.uv.y * sCount));
	cResult += texColor.rgb * float3(sc.x, sc.y, sc.x) * sIntensity;
	cResult = texColor.rgb + clamp(nIntensity, 0.0, 1.0) * (cResult - texColor.rgb);

	texColor.rgb = cResult;
	return (texColor * input.color) + g_colorAdd;
}
