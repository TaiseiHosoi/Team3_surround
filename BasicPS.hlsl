#include "Basic.hlsli"

//float4 main(VSOutput input) : SV_TARGET
//{
//    return float4(input.uv, 0, 1);
//}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET{
	
	//return float4(1,1,1,1);
	//return float4(tex.Sample(smp,input.uv)) * color;
	// 
	//return float4(input.normal,0.5f);

	float3 light = normalize(float3(1, -1, 1));//左下億　向きのライト
	float diffuse = saturate(dot(-light, input.normal));	//diffuseを0.1のはんいにClamp
	float brightness = diffuse + 0.3f;		//アンビエントこうを0.3として計算
	float4 texcolor = float4(tex.Sample(smp, input.uv));

	return float4(texcolor.rgb*brightness, texcolor.a) * color;//輝度をRGBに代入して出力
}
