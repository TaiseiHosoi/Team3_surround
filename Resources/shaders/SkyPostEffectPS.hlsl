#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);  	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      	// 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float uPixel = 1.0f / 1280.0f;
	float vPixel = 1.0f / 720.0f;

	int blarX = 4;
	int blarY = 4;
	int blerBlend = blarX * blarY;

	float4 texcolor = { 0,0,0,0 };

	for (int i = 0; i < blarX; i++) {
		for (int j = 0; j < blarY; j++) {
			float2 uvv = { -uPixel * i,-vPixel * j };
			texcolor += tex.Sample(smp, input.uv + uvv);
			uvv = float2(0.0f, -vPixel * j);
			texcolor += tex.Sample(smp, input.uv + uvv);
			uvv = float2 (uPixel * i, -vPixel * j);
			texcolor += tex.Sample(smp, input.uv + uvv);

			uvv = float2(-uPixel * i, 0.0f);
			texcolor += tex.Sample(smp, input.uv + uvv);
			uvv = float2(0.0f, 0.0f);
			texcolor += tex.Sample(smp, input.uv + uvv);
			uvv = float2(uPixel * i, 0.0f);
			texcolor += tex.Sample(smp, input.uv + uvv);

			uvv = float2(-uPixel * i, vPixel * j);
			texcolor += tex.Sample(smp, input.uv + uvv);
			uvv = float2(0.0f, vPixel * j);
			texcolor += tex.Sample(smp, input.uv + uvv);
			uvv = float2(uPixel * i, vPixel * j);
			texcolor += tex.Sample(smp, input.uv + uvv);
		}
	}

	return float4(texcolor.rgb / (blerBlend * 2), texcolor.a);
}