#include "RaytracingNoiseHeader.hlsli"

//�~����
static const float PI = 3.141592653589f;

//�~�X�V�F�[�_�[�̃C���f�b�N�X
static const int MISS_DEFAULT = 0;
static const int MISS_LIGHTING = 1;
static const int MISS_CHECKHIT = 2;

//�}�e���A���̎��
static const int MATERIAL_NONE = 0;
static const int MATERIAL_REFLECT = 1;
static const int MATERIAL_REFRACT = 2;
static const int MATERIAL_SEA = 3;

//���_���
struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

//�y�C���[�h
struct Payload
{
    float3 m_color; //�F���
    float3 m_emissive; //�P�x
    uint m_rayID; //���C��ID
};

struct MyAttribute
{
    float2 barys;
};

//���C�g�֌W�̃f�[�^
struct DirLight
{
    float3 m_dir;
    int m_isActive;
};
struct PointLight
{
    float3 m_pos;
    float m_power;
    float3 m_pad;
    int m_isActive;
};
struct LightData
{
    DirLight m_dirLight;
    PointLight m_pointLight[8];
};

//�J�����p�萔�o�b�t�@
struct CameraEyePosConstData
{
    matrix m_viewMat;
    matrix m_projMat;
    float3 m_eye;
    float m_timer;
};

//�{�����[���t�H�O�p�萔�o�b�t�@
struct RaymarchingParam
{
    float3 m_pos; //�{�����[���e�N�X�`���̃T�C�Y
    float m_gridSize; //�T���v�����O����O���b�h�̃T�C�Y
    float3 m_color; //�t�H�O�̐F
    float m_wrapCount; //�T���v�����O���W���͂ݏo�����ۂɉ���܂�Wrap���邩
    float m_sampleLength; //�T���v�����O����
    float m_density; //�Z�x�W��
    int m_isSimpleFog;
    int m_isActive;
};

//OnOff�f�o�b�O
struct DebugRaytracingParam
{
    int m_debugReflection;
    int m_debugShadow;
    float m_sliderRate;
    float m_skyFacter;
};

//OnOff�f�o�b�O
struct DebugSeaParam
{
    float m_freq;
    float m_amp;
    float m_choppy;
    float m_seaSpeed;
};

//�Ռ��g
struct ShockWave
{
    float3 m_pos;
    float m_radius;
    float m_power;
    int m_isActive;
    float m_facter;
    float m_pad;
};
struct ShockWaveParam
{
    ShockWave m_shockWave[10];
};



//�e���\�[�X��
StructuredBuffer<uint> indexBuffer : register(t1, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t2, space1);
Texture2D<float4> objectTexture : register(t0, space1);
//�T���v���[
SamplerState smp : register(s0, space1);

//TLAS
RaytracingAccelerationStructure gRtScene : register(t0);

//�J�������W�p�萔�o�b�t�@
ConstantBuffer<CameraEyePosConstData> cameraEyePos : register(b0);
ConstantBuffer<LightData> lightData : register(b1);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);
Texture2D<float4> emissiveMap : register(t5);

//�o�͐�UAV
RWTexture2D<float4> finalColor : register(u0);
RWTexture3D<float4> volumeNoiseTexture : register(u1);
RWTexture2D<float4> lensFlareTexture : register(u2);
RWTexture2D<float4> emissiveTexture : register(u3);


//barys���v�Z
inline float3 CalcBarycentrics(float2 Barys)
{
    return float3(1.0 - Barys.x - Barys.y, Barys.x, Barys.y);
}

//���������ʒu�̏����擾����֐�
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer)
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; //Triangle List �̂���.

    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };

    for (int index = 0; index < 3; ++index)
    {
        uint vtxIndex = indexBuffer[vertexId + index];
        float w = weights[index];
        v.pos += vertexBuffer[vtxIndex].pos * w;
        v.normal += vertexBuffer[vtxIndex].normal * w;
        v.uv += vertexBuffer[vtxIndex].uv * w;
    }

    return v;
}

//���C��������
void CastRay(inout Payload arg_payload, float3 arg_origin, float3 arg_dir, float arg_far, int arg_msIndex, RAY_FLAG arg_rayFlag, RaytracingAccelerationStructure arg_scene, uint arg_instanceMask)
{
    //���C�̐ݒ�
    RayDesc rayDesc;
    rayDesc.Origin = arg_origin; //���C�̔��˒n�_��ݒ�B

    rayDesc.Direction = arg_dir; //���C�̎ˏo������ݒ�B
    rayDesc.TMin = 1.0f; //���C�̍ŏ��l
    rayDesc.TMax = arg_far; //���C�̍ő�l(�J������Far�݂����Ȋ����B)
    
    //���C�𔭎�
    TraceRay(
        arg_scene, //TLAS
        arg_rayFlag,
        arg_instanceMask,
        0, //�Œ�ł悵�B
        1, //�Œ�ł悵�B
        arg_msIndex, //MissShader�̃C���f�b�N�X�BRenderScene.cpp��m_pipelineShaders��MissShader��o�^���Ă���B
        rayDesc,
        arg_payload);
}

//���C�g�����ōs�����C�e�B���O�p�X
void LightingPass(inout float arg_bright, inout float arg_pointlightBright, float4 arg_worldPosMap, float4 arg_normalMap, LightData arg_lightData, uint2 arg_launchIndex, RaytracingAccelerationStructure arg_scene, bool arg_isFar)
{
    
    //�f�B���N�V�������C�g�B
    if (arg_lightData.m_dirLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //�u�f�B���N�V�������C�g���L����������v ���� �u���݂̃X�N���[�����W�̈ʒu�ɖ@�����������܂�Ă�����(�����������܂�Ă��Ȃ��Ƃ��납��͉e�p�̃��C���΂��Ȃ��悤�ɂ��邽�߁B)�v
    {
        
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB
        payloadData.m_rayID = 1;
        
        //���C������
        CastRay(payloadData, arg_worldPosMap.xyz, -arg_lightData.m_dirLight.m_dir, 1000.0f, MISS_LIGHTING, RAY_FLAG_NONE, arg_scene, 0x01);
                
        //���C�g�̃x�N�g���Ɩ@�����疾�邳���v�Z����B
        float bright = saturate(dot(arg_normalMap.xyz, -arg_lightData.m_dirLight.m_dir));
        
        //�g�D�[�����ۂ����邽�߂Ƀ��C�g�̖��邳���X�e�b�v
        if (bright <= 0.6f)
        {
            bright = 0.4f;
        }
        else
        {
            bright = 0.8f;
        }
        
        //���C�g���̌��ʂ̉e�����������ށB
        arg_bright += payloadData.m_color.x * bright;
        
    }
    
    //�|�C���g���C�g
    for (int index = 0; index < 10; ++index)
    {
    
        if (arg_lightData.m_pointLight[index].m_isActive && 0.1f < length(arg_normalMap.xyz))    //�u�|�C���g���C�g���L����������v ���� �u���݂̃X�N���[�����W�̈ʒu�ɖ@�����������܂�Ă�����(�����������܂�Ă��Ȃ��Ƃ��납��͉e�p�̃��C���΂��Ȃ��悤�ɂ��邽�߁B)�v
        {
        
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
            Payload payloadData;
            payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
            payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB
        
        //�|�C���g���C�g����̃x�N�g�������߂�B
            float3 lightDir = normalize(arg_lightData.m_pointLight[index].m_pos - arg_worldPosMap.xyz);
            float distance = length(arg_lightData.m_pointLight[index].m_pos - arg_worldPosMap.xyz);
        
        //���������C�g�̍ő�e���͈͂��傫�������烌�C���΂��Ȃ��B
            if (distance < arg_lightData.m_pointLight[index].m_power)
            {
            
        
            //���C������
                CastRay(payloadData, arg_worldPosMap.xyz, lightDir, distance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene, 0x01);
            
                
            //-------------------------------------------------------------------------------�����ɃW���b�N����̃��C�g�̏����������B
            
            //���C�g���邳�̊��������߂�B
                float brightRate = saturate(distance / arg_lightData.m_pointLight[index].m_power);
                brightRate = brightRate == 0.0f ? 0.0f : pow(2.0f, 10.0f * brightRate - 10.0f);
                
            //���C�g�̃x�N�g���Ɩ@�����疾�邳���v�Z����B
                float bright = saturate(dot(arg_normalMap.xyz, lightDir));
        
            //���Ŗ��邳�ɃC�[�W���O��������B
                bright *= 1.0f - brightRate;

               
            //���C�g���̌��ʂ̉e�����������ށB
                arg_pointlightBright += payloadData.m_color.x * (bright * arg_lightData.m_pointLight[index].m_pad.x);
                
            }
        
        }
        
    }
    
    arg_pointlightBright = saturate(arg_pointlightBright);

}



//��̐F���擾�B
float3 GetSkyColor(float3 arg_eyeVec)
{
    arg_eyeVec.y = max(arg_eyeVec.y, 0.0f);
    float r = pow(1.0f - arg_eyeVec.y, 2.0f);
    float g = 1.0f - arg_eyeVec.y;
    float b = 0.6f + (1.0f - arg_eyeVec.y) * 0.4f;
    return float3(r, g, b);
}

//���C�e�B���O�Ɋւ���֐�
float Diffuse(float3 arg_normal, float3 arg_light, float arg_position)
{
    return pow(dot(arg_normal, arg_light) * 0.4f + 0.6f, arg_position);
}
float Specular(float3 arg_normal, float3 arg_light, float3 arg_eye, float arg_specular)
{
    float nrm = (arg_specular + 8.0f) / (PI * 8.0f);
    return pow(max(dot(reflect(arg_eye, arg_normal), arg_light), 0.0f), arg_specular) * nrm;
}

//�C�̐F���擾
static const float3 SEA_BASE = float3(0.1f, 0.19f, 0.22f); //��B �C�����������瓮�����Ă݂ĉ����𔻒f����B
static const float3 SEA_WATER_COLOR = float3(0.8f, 0.9f, 0.6f); //���O�I�ɐ��̐F
float3 GetSeaColor(float3 arg_position, float3 arg_normal, float3 arg_light, float3 arg_rayDir, float3 arg_dist /*arg_position - ���C�̌��_*/)
{
    //�C�Ɋւ���萔 �����o�����炱����萔�o�b�t�@�ɓ���ĕς�����悤�ɂ���B
    const float SEA_HEIGHT = 0.6f; //�C�̌��E�̍����H
    
    //�t���l���̌v�Z�Ŕ��˗������߂�B http://marupeke296.com/DXPS_PS_No7_FresnelReflection.html
    float fresnel = clamp(1.0f - dot(arg_normal, -arg_rayDir), 0.0f, 1.0f);
    fresnel = pow(fresnel, 3.0f) * 0.65f;

    //���ˁA���܂����ꍇ�̐F�����߂�B
    float3 reflected = GetSkyColor(reflect(arg_rayDir, arg_normal));
    float3 refracted = SEA_BASE + Diffuse(arg_normal, arg_light, 80.0f) * SEA_WATER_COLOR * 0.12f; //�C�̐F ���̐�ɃI�u�W�F�N�g������ꍇ�A���̋����ɉ����Ă��̐F���Ԃ���B

    //�t���l���̌v�Z�œ���ꂽ���˗�����F���Ԃ���B
    float3 color = lerp(refracted, reflected, fresnel);

    //�����������߂�B
    float atten = max(1.0f - dot(arg_dist, arg_dist) * 0.001f, 0.0f);
    color += SEA_WATER_COLOR * (arg_position.y - SEA_HEIGHT) * 0.18f * atten; //�g�̍����ɂ���ĐF��ς��Ă�H�����𒲐�����Δ����ł��邩���H

    //�X�y�L���������߂Č�����o���I
    color += float3(float3(1.0f, 1.0f, 1.0f) * Specular(arg_normal, arg_light, arg_rayDir, 5.0f));

    return color;
}

//�S���̗v�f������̒l�ȓ��Ɏ��܂��Ă��邩�B
bool IsInRange(float3 arg_value, float arg_range, float arg_wrapCount)
{
    
    bool isInRange = arg_value.x / arg_range <= arg_wrapCount && arg_value.y / arg_range <= arg_wrapCount && arg_value.z / arg_range <= arg_wrapCount;
    isInRange &= 0 < arg_value.x && 0 < arg_value.y && 0 < arg_value.z;
    return isInRange;
}

void SecondaryPass(float3 arg_viewDir, inout float4 arg_emissiveColor, float4 arg_worldColor, float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //���C��ID���݂āA���C��ł��ǂ����𔻒f
    if (arg_materialInfo.w == MATERIAL_REFRACT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(1, 1, 1);
        payloadData.m_rayID = 0;
        
        //���C������
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 100.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        
        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        arg_emissiveColor.xyz += payloadData.m_emissive;
        
    }
    else if (arg_materialInfo.w == MATERIAL_REFLECT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(1, 1, 1);
        payloadData.m_rayID = 0;
        
        //���C������
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 100.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        
        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        arg_emissiveColor.xyz += payloadData.m_emissive;
        
    }
    else if (arg_materialInfo.w == MATERIAL_SEA && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload refractionColor;
        refractionColor.m_emissive = float3(0.0f, 0.0f, 0.0f);
        refractionColor.m_color = float3(1, 1, 1);
        refractionColor.m_rayID = 2;
        Payload reflectionColor;
        reflectionColor.m_emissive = float3(0.0f, 0.0f, 0.0f);
        reflectionColor.m_color = float3(1, 1, 1);
        reflectionColor.m_rayID = 2;
        
        //���C������
        float3 rayOrigin = arg_worldColor.xyz;
        CastRay(refractionColor, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 500.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        CastRay(reflectionColor, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 500.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        
        //���C�������������������Ă��Ȃ����ŐF��ς���B
        if (refractionColor.m_color.x < 0)
        {
            refractionColor.m_color = arg_albedoColor.xyz;
        }
        if (reflectionColor.m_color.x < 0)
        {
            reflectionColor.m_color = float3(0, 0, 0);
        }
        
        //�C�̐F�̊���
        float perOfSeaColor = (1.0f - arg_materialInfo.y);
        
        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((refractionColor.m_color), 1) * (perOfSeaColor / 2.0f);
        arg_finalColor += float4((reflectionColor.m_color), 1) * (perOfSeaColor / 2.0f);
        
        arg_emissiveColor.w = 1.0f;
        arg_emissiveColor.xyz += refractionColor.m_emissive;
        arg_emissiveColor.xyz += reflectionColor.m_emissive;
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
}



float3 IntersectionPos(float3 Dir, float3 A, float Radius)
{
    float b = dot(A, Dir);
    float c = dot(A, A) - Radius * Radius;
    float d = max(b * b - c, 0.0f);

    return A + Dir * (-b + sqrt(d));
}
float Scale(float FCos)
{
    float x = 1.0f - FCos;
    return 0.25f * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25f))));
}

//��C�U��
float3 AtmosphericScattering(float3 pos, inout float3 mieColor)
{
    
    //���C���[�U���萔
    float kr = 0.0025f;
    //�~�[�U���萔
    float km = 0.005f;

    //��C���̐������T���v�����O���鐔�B
    float fSamples = 2.0f;

    //��̐F �F�I�ɂ͔��߂̒��F
    float3 three_primary_colors = float3(0.68f, 0.55f, 0.44f);
    //���̔g���H
    float3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);

    //��C���̈�ԏ�̍����B
    float fOuterRadius = 10250.0f;
    //�n���S�̂̒n��̍����B
    float fInnerRadius = 10200.0f;

    //���z���̋����H
    float fESun = 10.0f;
    //���z���̋����Ƀ��C���[�U���萔�������ă��C���[�U���̋��������߂Ă���B
    float fKrESun = kr * fESun;
    //���z���̋����Ƀ~�[�U���萔�������ă��C���[�U���̋��������߂Ă���B
    float fKmESun = km * fESun;

    //���C���[�U���萔�ɉ~�����������Ă���̂����A����Ȃ�0�ɋ߂��l�B
    float fKr4PI = kr * 4.0f * PI;
    //�~�[�U���萔�ɉ~�����������Ă���̂����A�~�[�U���萔��0�Ȃ̂ł���̒l��0�B
    float fKm4PI = km * 4.0f * PI;

    //�n���S�̂ł̑�C�̊����B
    float fScale = 1.0f / (fOuterRadius - fInnerRadius);
    //���ϑ�C���x�����߂鍂���B
    float fScaleDepth = 0.35f;
    //�n���S�̂ł̑�C�̊����𕽋ϑ�C���x�Ŋ������l�B
    float fScaleOverScaleDepth = fScale / fScaleDepth;

    //�U���萔�����߂�ۂɎg�p����l�B
    float g = -0.999f;
    //�U���萔�����߂�ۂɎg�p����l���悵�����́B�Ȃ��B
    float g2 = g * g;

    //���������V���̃��[���h���W
    float3 worldPos = normalize(pos) * fOuterRadius;
    worldPos = IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);

    //�J�������W ���v�Z�����ƒ��S�Œ�ɂȂ��Ă��܂��Ă������B
    float3 v3CameraPos = float3(0.0, fInnerRadius + 1.0f, 0.0f);

    //�f�B���N�V���i�����C�g�̏ꏊ�����߂�B
    float3 dirLightPos = -lightData.m_dirLight.m_dir * 15000.0f;

    //�f�B���N�V���i�����C�g�ւ̕��������߂�B
    float3 v3LightDir = normalize(dirLightPos - worldPos);

    //�V���㒸�_����J�����܂ł̃x�N�g��(������C���ɓ˓������_����J�����܂ł̌��̃x�N�g��)
    float3 v3Ray = worldPos - v3CameraPos;

    //��C�ɓ˓����Ă���̓_�ƃJ�����܂ł̋����B
    float fFar = length(v3Ray);

    //���K�����ꂽ�g�U�������������B
    v3Ray /= fFar;

    //�T���v�����O����n�_���W ��������A�̒��_
    float3 v3Start = v3CameraPos;
    //�T���v���ł̓J�����̈ʒu��(0,Radius,0)�Ȃ̂ŃJ�����̍����B�ǂ̈ʒu�Ɉړ����Ă��n�����_�Ō���Ό��_(�n���̒��S)����̍����B
    float fCameraHeight = length(v3CameraPos);
    //�n�ォ��̖@��(?)�Ɗg�U��������Ă����p�x�̓��ςɂ���ċ��߂�ꂽ�p�x���J�����̍����Ŋ���B
    float fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
    //�J�n�n�_�̍����ɕ��ϑ�C���x���������l�̎w�������߂�H
    float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    //�J�n�n�_�̂Ȃɂ��̊p�x�̃I�t�Z�b�g�B
    float fStartOffset = fStartDepth * Scale(fStartAngle);

    //�T���v���|�C���g�Ԃ̒����B
    float fSampleLength = fFar / fSamples;
    //�T���v���|�C���g�Ԃ̒����ɒn���̑�C�̊�����������B
    float fScaledLength = fSampleLength * fScale;
    //�g�U�������������ɃT���v���̒����������邱�ƂŃT���v���|�C���g�Ԃ̃��C���x�N�g�������߂�B
    float3 v3SampleRay = v3Ray * fSampleLength;
    //�ŏ��̃T���v���|�C���g�����߂�B0.5�������Ă�̂͏������������߁H
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;

    //�F���
    float3 v3FrontColor = 0.0f;
    for (int n = 0; n < int(fSamples); ++n)
    {
        //�T���v���|�C���g�̍����B�ǂ���ɂ��挴�_�͒n���̒��S�Ȃ̂ŁA���̒l�����݈ʒu�̍����ɂȂ�B
        float fHeight = length(v3SamplePoint);
        //�n�ォ��T���v���|�C���g�̍����̍��ɕ��ϑ�C���x�����������́B  ���x�ɉ����đ�C���x���w���I�ɏ������Ȃ��Ă����̂�\�����Ă���H
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        //�n�ォ�猩���T���v���|�C���g�̖@���ƃf�B���N�V���i�����C�g�̕����̊p�x�����߂āA�T���v���|�C���g�̍����Ŋ���B
        float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight; //�����̒l��-1�ɂȂ遨Scale���̌v�Z��exp�̈�����43�ɂȂ�A�ƂĂ��Ȃ��ł����l������B �� -�ɂȂ�Ȃ��悤�ɂ���H
        //�n�ォ�猩���T���v���|�C���g�̖@���ƎU���������ł��Ă������̊p�x�����߂āA�T���v���|�C���g�̍����Ŋ���B
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        //�U�����H
        float fScatter = (fStartOffset + fDepth * (Scale(fLightAngle * 1) - Scale(fCameraAngle * 1)));

        //�F���Ƃ̌������H
        float3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
        //�T���v���|�C���g�̈ʒu���l�����ĎU�������F�����߂�B
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        //�T���v���|�C���g���ړ�������B
        v3SamplePoint += v3SampleRay;

    }

    //���C���[�U���Ɏg�p����F���
    float3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
    //�~�[�U���Ɏg�p����F���
    float3 c1 = v3FrontColor * fKmESun;
    //�J�������W����V���̍��W�ւ̃x�N�g���B
    float3 v3Direction = v3CameraPos - worldPos;

    //float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos2 = fcos * fcos;

    //���C���[�U���̖��邳�B
    float rayleighPhase = 0.75f * (1.0f + fcos2);
    //�~�[�U���̖��邳�B
    float miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fcos2) / pow(1.0f + g2 - 2.0f * g * fcos, 1.5f);

    //�~�[�U���̐F��ۑ��B
    mieColor = c0 * rayleighPhase;

    //�ŏI���ʂ̐F
    float3 col = 1.0f;
    col.rgb = rayleighPhase * c0 + miePhase * c1;

    //��_�܂ł̃x�N�g���Ƒ��z�܂ł̃x�N�g�����߂������甒�F�ɕ`�悷��B
    int sunWhite = step(0.999f, dot(normalize(dirLightPos - v3CameraPos), normalize(worldPos - v3CameraPos)));
    
    return col + float3(sunWhite, sunWhite, sunWhite);

}