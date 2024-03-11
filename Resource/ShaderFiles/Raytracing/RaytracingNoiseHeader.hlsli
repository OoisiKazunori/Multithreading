//�������擾�B
float3 Random3D(float3 arg_st)
{
    float3 seed = float3(dot(arg_st, float3(127.1f, 311.7f, 523.3f)), dot(arg_st, float3(269.5f, 183.3f, 497.5f)), dot(arg_st, float3(419.2f, 371.9f, 251.6f)));
    return -1.0f + 2.0f * frac(sin(seed) * 43758.5453123f);
}

//3D�O���f�B�G���g�m�C�Y�֐�
float GradientNoise(float3 arg_st)
{
    float3 i = floor(arg_st);
    float3 f = frac(arg_st);

    //���̗אړ_�̍��W�����߂�
    float3 u = f * f * (3.0f - 2.0f * f);

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
float3 PerlinNoiseWithWind(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float arg_windStrength, float arg_windSpeed, float arg_timer, float3 arg_worldPos, float arg_threshold)
{
    float amplitude = 1.0f;

    //���̉e�����v�Z
    float3 windDirection = normalize(float3(1, 0, 0)); //���̕�����ݒ�i���̏ꍇ�� (1, 0, 0) �̕����j
    float3 windEffect = windDirection * arg_windStrength * (arg_timer * arg_windSpeed);

    //�v���C���[�̃��[���h���W�Ɋ�Â��m�C�Y����
    float3 worldSpaceCoords = arg_st + arg_worldPos / 100.0f;
    //float3 worldSpaceCoords = arg_st;

    float3 noiseValue = float3(0, 0, 0);

    for (int j = 0; j < 3; ++j)
    {
        float frequency = pow(2.0f, float(j));
        float localAmplitude = amplitude;
        float sum = 0.0f;
        float maxValue = 0.0f;
        
        for (int i = 0; i < arg_octaves; ++i)
        {
            sum += localAmplitude * GradientNoise((worldSpaceCoords + windEffect) * frequency + (arg_timer + windEffect.x)); //�X���[�Y�Ȏ��ԕϐ��ƕ��̉e�����m�C�Y�֐��ɓK�p
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

//�t���N�^���m�C�Y
float FBM(float2 arg_st)
{
    float result = 0.0f;
    float amplitude = 1.0f; //�U��

    for (int counter = 0; counter < 5; counter++)
    {
        result += amplitude * GradientNoise(float3(arg_st, 1.0f));
        amplitude *= 0.5f; //�U�������炷�B��������ƃm�C�Y���ׂ����Ȃ��Ă����B
        arg_st *= 2.0f; //���g���������Ă����B    
    }

    return result;
}

//�h���C�����[�s���O
float3 DomainWarping(float2 arg_st, float arg_time)
{

    float3 color = float3(1.0f, 1.0f, 1.0f);

    //�ŏ��̈���
    float2 q = float2(0.0f, 0.0f);
    q.x = FBM(arg_st + float2(0.0f, 0.0f));
    q.y = FBM(arg_st + float2(1.0f, 1.0f));

    //�ŏ��̈���������ɉ��H�B
    float2 r = float2(0.0f, 0.0f);
    r.x = FBM(arg_st + (4.0f * q) + float2(1.7f, 9.2f) + (0.15f * arg_time));
    r.y = FBM(arg_st + (4.0f * q) + float2(8.3f, 2.8f) + (0.12f * arg_time));
    
    //�F�����߂�B
    float3 mixColor1 = float3(0.8f, 0.35f, 0.12f);
    float3 mixColor2 = float3(0.3f, 0.75f, 0.69f);
    color = lerp(color, mixColor1, clamp(length(q), 0.0f, 1.0f));
    color = lerp(color, mixColor2, clamp(length(r), 0.0f, 1.0f));

    //�O�i�K�ڂ̃m�C�Y���擾�B
    float f = FBM(arg_st + 4.0f * r);

    //���ʂ�g�ݍ��킹��B
    float coef = (f * f * f + (0.6f * f * f) + (0.5f * f)) * 10.0f;
    return color * coef;
    
}



//�������擾�B
float2 Random2D(float2 arg_st)
{
    float2 seed = float2(dot(arg_st, float2(127.1f, 311.7f)), dot(arg_st, float2(269.5f, 183.3f)));
    return -1.0f + 2.0f * frac(sin(seed) * 43758.5453123f);
}

//2D�O���f�B�G���g�m�C�Y�֐�
float GradientNoise2D(float2 arg_st)
{
    float2 i = floor(arg_st);
    float2 f = frac(arg_st);

    //�l�̗אړ_�̍��W�����߂�
    float2 u = f * f * (3.0f - 2.0f * f);

    float a = dot(Random2D(i), f - float2(0, 0));
    float b = dot(Random2D(i + float2(1, 0)), f - float2(1, 0));
    float c = dot(Random2D(i + float2(0, 1)), f - float2(0, 1));
    float d = dot(Random2D(i + float2(1, 1)), f - float2(1, 1));

    //�m�C�Y�l���Ԃ���
    float x1 = lerp(a, b, u.x);
    float x2 = lerp(c, d, u.x);

    return lerp(x1, x2, u.y);
}

float PerlinNoise2D(float2 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float arg_threshold)
{
    float noiseValue = 0;
    
    float frequency = 0.9f;
    float localAmplitude = 5.0f;
    float sum = 0.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < arg_octaves; ++i)
    {
        sum += localAmplitude * GradientNoise2D(arg_st * frequency);
        maxValue += localAmplitude;

        localAmplitude *= arg_persistence;
        frequency *= arg_lacunarity;
    }

    noiseValue = (sum / maxValue + 1.0f) * 0.5f; //�m�C�Y�l���ă}�b�s���O

    if (noiseValue <= arg_threshold)
    {
        noiseValue = 0.0f;
    }
    return noiseValue;
}


//�C�p

//�����𓾂�B
float Hash(float2 arg_point)
{
    float h = dot(arg_point, float2(127.1f, 311.7f));
    return frac(sin(h) * 43758.5453123f);
}

//�o�����[�m�C�Y
float ValueNoise(float2 arg_point, float arg_st)
{
    
    return GradientNoise(float3(arg_point, arg_st));
    
    float2 i = floor(arg_point);
    float2 f = frac(arg_point);

    //u = -2.0f^3 + 3.0f^2
    float2 uid = f * float2(-2.0f, -2.0f);
    uid += float2(3.0f, 3.0f);
    float2 u = f * f * uid;

    //�O���b�h��ɗ��������߂ĕ�Ԃ���B
    float dim = 1.0f;
    float a = Hash(i + float2(0.0f, 0.0f));
    float b = Hash(i + float2(dim, 0.0f));
    float c = Hash(i + float2(0.0f, dim));
    float d = Hash(i + float2(dim, dim));
    float result = lerp(lerp(a, b, u.x),
                        lerp(c, d, u.x), u.y);
    return (2.0f * result) - 1.0f;
}