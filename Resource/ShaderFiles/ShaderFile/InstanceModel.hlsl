#include"ModelBuffer.hlsli"

struct MatBuffer
{
    matrix worldMat;
    matrix viewMat;
    matrix projectionMat;
    matrix rotaion;
};

RWStructuredBuffer<MatBuffer>matrixBuffer:register(u0);

struct PosUvNormalTangentBinormalOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT2;
    float3 binormal : BINORMAL;
    uint id :SV_InstanceID;
};

//ディファードレンダリング対応
PosUvNormalTangentBinormalOutput VSDefferdMain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, float3 tangent : TANGENT, float3 binormal : BINORMAL,uint id : SV_InstanceID)
{
    PosUvNormalTangentBinormalOutput op;
    op.svpos = mul(matrixBuffer[id].worldMat, pos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(matrixBuffer[id].viewMat, op.svpos);
    op.svpos = mul(matrixBuffer[id].projectionMat, op.svpos);
    op.uv = uv;
    op.normal = normal;
    op.binormal = binormal;
    op.tangent = tangent;
    op.id = id;
    return op;
}

cbuffer ColorBuffer : register(b0)
{
    float4 color;
}

//ディファードレンダリング対応
GBufferOutput PSDefferdMain(PosUvNormalTangentBinormalOutput input) : SV_TARGET
{
    float4 normalColor = NormalTex.Sample(smp, input.uv);
    //-1.0f ~ 1.0f
    float3 normalVec = 2 * normalColor - 1.0f;
    normalVec = normalize(normalVec);

    float3 normal = mul(matrixBuffer[input.id].rotaion, float4(input.normal, 1.0f));
    normal = normalize(normal);
    float3 tangent = mul(matrixBuffer[input.id].rotaion, float4(input.tangent, 1.0f));
    tangent = normalize(tangent);
    float3 binormal = cross(normal, tangent);

    float3 nWorld = CalucurateTangentToLocal(normalVec, normal, tangent, binormal);
    if (IsEnableToUseMaterialTex(normalColor))
    {
        nWorld = input.normal;
    }

    float4 texColor = AlbedoTex.Sample(smp, input.uv);
    float4 mrColor = MetalnessRoughnessTex.Sample(smp, input.uv);

    if (IsEnableToUseMaterialTex(mrColor))
    {
        mrColor.xyz = float3(0.0f, 0.0f, 0.0f);
    }

    GBufferOutput output;
    output.albedo = texColor;
    output.normal = float4(normal, 1.0f);
    output.metalnessRoughness = float4(mrColor.xyz, raytracingId);
    output.world = float4(input.worldPos, 1.0f);
    output.emissive = EmissiveTex.Sample(smp, input.uv);
    return output;
}
