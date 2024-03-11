
//�o�͐�UAV
RWTexture3D<float4> NoiseTexture : register(u0);

cbuffer NoiseParam : register(b0)
{
    float3 m_worldPos;
    float m_timer;
    float m_windSpeed;
    float m_windStrength;
    float m_threshold;
    float m_scale;
    int m_octaves;
    float m_persistence;
    float m_lacunarity;
    float m_pad;
};



//3D�̃����_���n�b�V���֐�
float3 Random3D(float3 arg_st)
{
    float3 seed = float3(dot(arg_st, float3(127.1f, 311.7f, 523.3f)), dot(arg_st, float3(269.5f, 183.3f, 497.5f)), dot(arg_st, float3(419.2f, 371.9f, 251.6f)));
    return -1.0f + 2.0f * frac(sin(seed) * 43758.5453123f);
}

//3D�O���f�B�G���g�m�C�Y�֐�
float Noise(float3 arg_st)
{
    float3 i = floor(arg_st);
    float3 f = frac(arg_st);

    //���̗אړ_�̍��W�����߂�
    float3 u = f * f * (3.0 - 2.0 * f);

    float a = dot(Random3D(i), f - float3(0, 0, 0));
    float b = dot(Random3D(i + float3(1, 0, 0)), f - float3(1, 0, 0));
    float c = dot(Random3D(i + float3(0, 1, 0)), f - float3(0, 1, 0));
    float d = dot(Random3D(i + float3(1, 1, 0)), f - float3(1, 1, 0));
    float e = dot(Random3D(i + float3(0, 0, 1)), f - float3(0, 0, 1));
    float f1 = dot(Random3D(i + float3(1, 0, 1)), f - float3(1, 0, 1));
    float g = dot(Random3D(i + float3(0, 1, 1)), f - float3(0, 1, 1));
    float h = dot(Random3D(i + float3(1, 1, 1)), f - float3(1, 1, 1));

    //�m�C�Y�l���Ԃ���
    float x1 = lerp(a, b, u.x);
    float x2 = lerp(c, d, u.x);
    float y1 = lerp(e, f1, u.x);
    float y2 = lerp(g, h, u.x);

    float xy1 = lerp(x1, x2, u.y);
    float xy2 = lerp(y1, y2, u.y);

    return lerp(xy1, xy2, u.z);
}

//3D�p�[�����m�C�Y�֐��i���̕\���t���j
float3 PerlinNoiseWithWind(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float arg_windStrength, float arg_windSpeed, float arg_time, float3 arg_worldPos, float arg_threshold)
{
    
    //���̉e�����v�Z
    float3 windDirection = normalize(float3(1, 0, 0)); //���̕�����ݒ�
    float3 windEffect = windDirection * arg_windStrength * (arg_time * arg_windSpeed);

    //�v���C���[�̃��[���h���W�Ɋ�Â��m�C�Y����
    //float3 worldSpaceCoords = arg_st + arg_worldPos / 100.0f;
    float3 worldSpaceCoords = arg_st;

    float3 noiseValue = float3(0, 0, 0);

    for (int j = 0; j < 3; ++j)
    {
        float frequency = pow(2.0f, float(j));
        float localAmplitude = 1.0f;
        float sum = 0.0f;
        float maxValue = 0.0f;
        
        for (int i = 0; i < arg_octaves; ++i)
        {
            sum += localAmplitude * Noise((worldSpaceCoords + windEffect) * frequency + (arg_time + windEffect.x)); //�X���[�Y�Ȏ��ԕϐ��ƕ��̉e�����m�C�Y�֐��ɓK�p
            maxValue += localAmplitude;

            localAmplitude *= arg_persistence;
            frequency *= arg_lacunarity;
        }

        noiseValue[j] = (sum / maxValue + 1.0f) * 0.5f; //�m�C�Y�l��0.0����1.0�͈̔͂ɍă}�b�s���O

        if (noiseValue[j] <= arg_threshold)
        {
            noiseValue[j] = 0.0f;
        }
    }

    return noiseValue;
}

[numthreads(8, 8, 4)]
void CSmain(uint3 threadIdx : SV_DispatchThreadID)
{
    
    float3 st = threadIdx.xyz / m_scale * 10.0f; //�X�P�[������
    

    //�p�[�����m�C�Y��K�p
    float3 perlinValue = PerlinNoiseWithWind(st, m_octaves, m_persistence, m_lacunarity, m_windStrength, m_windSpeed, m_timer, m_worldPos, m_threshold);
    
    //�F��ۑ��B
    NoiseTexture[threadIdx] = float4(perlinValue.x, perlinValue.y, perlinValue.z, 0.1f);
    
}