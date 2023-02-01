#include "RimLight.hlsli"

VertexShaderOutput main(float4 pos: POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VertexShaderOutput output = (VertexShaderOutput)0;

    output.Position = mul(mat, pos);

    // TODO: add your vertex shader code here.
<<<<<<< HEAD
    output.Normal = normal;
=======
    //output.Normal = normal;
    output.Normal = normalize(mul(mat, normal));

>>>>>>> rim
    output.uv = uv;

    return output;
}