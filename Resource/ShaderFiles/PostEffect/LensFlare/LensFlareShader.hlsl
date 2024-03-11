
//���͏��
Texture2D<float4> LensColor : register(t0);

//���͏��
RWTexture2D<float4> InputImg : register(u0);

//�o�͐�UAV  
RWTexture2D<float4> OutputImg : register(u1);

//�F����
float3 TextureDistorted(float2 texcoord, float2 direction, uint2 texSize, float3 distortion)
{
    return float3(
         InputImg[(texcoord + direction * distortion.r) * texSize].r,
         InputImg[(texcoord + direction * distortion.g) * texSize].g,
         InputImg[(texcoord + direction * distortion.b) * texSize].b
      );
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

    //�e�N�X�`���̃T�C�Y
    const float2 TEXSIZE = float2(1280.0f, 720.0f);
    const float2 LENSCOLOR_TEXSIZE = float2(256.0f, 1.0f);
    const float2 WINDOW_CENTER = float2(0.5f, 0.5f);
    
    //�T���v�����O����e�N�X�`���̏㉺���E�𔽓]������B
    float2 texpos = float2(DTid.x, DTid.y) / TEXSIZE;
    texpos = -texpos + float2(1.0f, 1.0f);
 
    //��ʒ��S�Ɍ������Ẵx�N�g�������߂�B�S�[�X�g�ɂ��΂������B
    float ghostDispersal = 0.36f;
    float2 ghostVec = (WINDOW_CENTER - texpos) * ghostDispersal;
    
    //�F�����p�p�����[�^�[���v�Z�B
    float distortionValue = 15.0f;
    float2 texelSize = 1.0f / TEXSIZE;
    float3 distortion = float3(-texelSize.x * distortionValue, 0.0f, texelSize.x * distortionValue);
    float2 direction = normalize(ghostVec);
   
    //�S�[�X�g���T���v�����O����B
    float4 sampleResult = float4(0,0,0,1);
    uint ghostCount = 8;
    for (int i = 0; i < ghostCount; ++i)
    {
        float2 offset = frac(texpos + ghostVec * float(i));
        
        float weight = length(WINDOW_CENTER - offset) / length(WINDOW_CENTER);
        weight = pow(1.0f - weight, 10.0f);
  
        //sampleResult.xyz += InputImg[(offset) * TEXSIZE] * weight;
        sampleResult.xyz += TextureDistorted((offset), direction, TEXSIZE, distortion) * weight;

    }
    
    //�����Y�̐F��K�p�B
    uint2 lensColorUV = (length(WINDOW_CENTER - texpos) / length(WINDOW_CENTER)) * LENSCOLOR_TEXSIZE;
    sampleResult *= LensColor[lensColorUV];
    
    //�n���[���T���v�����O
    float haloWidth = 0.47f;
    float2 haloVec = normalize(ghostVec) * haloWidth;
    float weight = length(WINDOW_CENTER - frac(texpos + haloVec)) / length(WINDOW_CENTER);
    weight = pow(1.0f - weight, 5.0f);
    //sampleResult.xyz += InputImg[(texpos + haloVec) * TEXSIZE] * weight;
    sampleResult.xyz += TextureDistorted(texpos + haloVec, direction, TEXSIZE, distortion) * weight;
    
    OutputImg[DTid.xy] = sampleResult;
    
}