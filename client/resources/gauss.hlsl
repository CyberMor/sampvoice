/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

static const float offset[5] = { 0.0, 1.4347826, 3.3478260, 5.2608695, 7.1739130 };
static const float weight[5] = { 0.16818994, 0.27276957, 0.11690125, 0.024067905, 0.0021112196 };
static const float lerpBound[2] = { 0.0, 1.0 };

uniform texture2D iFrameTexture;
uniform texture2D iBackBuffer;
uniform float2 iResolution;
uniform float iLevel;

sampler2D frameSampler
{
	Texture = <iFrameTexture>;
	AddressU = Clamp;
	AddressV = Clamp;
	MipFilter = None;
	MinFilter = Linear;
	MagFilter = Linear;
	SRGBTexture = false;
};

sampler2D backBufferSampler
{
	Texture = <iBackBuffer>;
	AddressU = Clamp;
	AddressV = Clamp;
	MipFilter = None;
	MinFilter = Linear;
	MagFilter = Linear;
	SRGBTexture = false;
};

struct VsInfo
{
	float4 position : POSITION;
	float2 coord : TEXCOORD;
};

struct PsInfo
{
	float4 color : COLOR;
	float2 coord : TEXCOORD;
};

VsInfo VsFrame(VsInfo input)
{
	VsInfo output;

	output.position = input.position;
	output.coord = input.coord;

	return output;
}

float4 PsFrameHGaussianBlur(PsInfo input) : COLOR
{
	float4 color = tex2D(frameSampler, input.coord) * weight[0];

	for (int i = 1; i < 5; ++i)
	{
		color += tex2D(frameSampler, input.coord + float2(offset[i] * 4.0 / iResolution.x, 0.0)) * weight[i];
		color += tex2D(frameSampler, input.coord - float2(offset[i] * 4.0 / iResolution.x, 0.0)) * weight[i];
	}

	return color;
}

float4 PsFrameVGaussianBlur(PsInfo input) : COLOR
{
	float4 color = tex2D(frameSampler, input.coord) * weight[0];

	for (int i = 1; i < 5; ++i)
	{
		color += tex2D(frameSampler, input.coord + float2(0.0, offset[i] * 4 / iResolution.y)) * weight[i];
		color += tex2D(frameSampler, input.coord - float2(0.0, offset[i] * 4 / iResolution.y)) * weight[i];
	}

	return color;
}

float4 PsFrame(PsInfo input) : COLOR
{
	float4 orig = tex2D(backBufferSampler, input.coord);
	float4 blur = tex2D(frameSampler, input.coord);

	orig = lerp(orig, blur, (lerpBound[0] + (iLevel / 100.0) * lerpBound[1]));

	float dist = distance(input.coord, float2(0.5, 0.5)) * 0.7 * (iLevel / 100.0);
	orig.rgb *= smoothstep(1.0, 0.1, dist);

	return orig;
}

technique t0
{
	pass P0
	{
		VertexShader = compile vs_3_0 VsFrame();
		PixelShader = compile ps_3_0 PsFrameHGaussianBlur();
		AlphaBlendEnable = false;
		SRGBWriteEnable = false;
	}

	pass P1
	{
		VertexShader = compile vs_3_0 VsFrame();
		PixelShader = compile ps_3_0 PsFrameVGaussianBlur();
		AlphaBlendEnable = false;
		SRGBWriteEnable = false;
	}

	pass P2
	{
		VertexShader = compile vs_3_0 VsFrame();
		PixelShader = compile ps_3_0 PsFrame();
		AlphaBlendEnable = false;
		SRGBWriteEnable = false;
	}
}
