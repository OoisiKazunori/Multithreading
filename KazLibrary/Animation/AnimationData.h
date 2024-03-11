#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include<map>

template<typename T>
struct KeyFrame
{
	int frame;
	T value;
};

template<typename T>
struct Animation
{
	int startFrame;
	int endFrame;
	std::vector<KeyFrame<T>>keyFrames;
};

class Bone
{
public:
	static size_t GetSizeWithOutName()
	{
		return sizeof(char) + sizeof(int) + sizeof(KazMath::Vec3<float>) + sizeof(DirectX::XMMATRIX);
	}

	std::string name;
	char parent = -1;	//�e�{�[��
	std::vector<char>children;	//�q�{�[��
	int transLayer = 0;	//�ό`�K�w
	KazMath::Vec3<float> pos = { 0.0f,0.0f,0.0f };
	DirectX::XMMATRIX invBindMat = DirectX::XMMatrixIdentity();
};

class Skeleton
{
public:
	static const std::string DEFINE_PARENT_BONE_NAME;
	struct BoneAnimation
	{
	public:
		Animation<KazMath::Vec3<float>>posAnim;
		Animation<DirectX::XMVECTOR>rotateAnim;
		Animation<KazMath::Vec3<float>>scaleAnim;
	};
	struct ModelAnimation
	{
		//�S�Ẵ{�[���̃A�j���[�V�������I�����鎞��
		int finishTime;
		//�{�[���P�ʂ̃A�j���[�V����
		std::map<std::string, BoneAnimation>boneAnim;
	};

	//���W�n�ϊ����C�����邽�߂̍s��
	DirectX::XMMATRIX coordinateSystemConvertMat = DirectX::XMMatrixIdentity();

	std::vector<Bone>bones;
	std::map<std::string, int>boneIdxTable;
	/*
		�A�j���[�V�������iAnimator����̎Q�Ɨp�j
		�L�[�� �A�j���[�V������
	*/
	std::map<std::string, ModelAnimation>animations;

	void CreateBoneTree(const DirectX::XMMATRIX& CoordinateSysConvertMat);
	int GetIndex(const std::string& BoneName);
};

struct VertexData
{
	std::string name;
	std::vector<KazMath::Vec3<float>> verticesArray;
	std::vector<KazMath::Vec2<float>> uvArray;
	std::vector<KazMath::Vec3<float>> normalArray;
	std::vector<KazMath::Vec3<float>> tangentArray;
	std::vector<KazMath::Vec3<float>> binormalArray;
	std::vector<KazMath::Vec4<int>> boneIdx;
	std::vector<KazMath::Vec4<float>> boneWeight;
	std::vector<UINT>indexArray;
};

//�o�b�t�@�����p�̍\����
struct MaterialBufferData
{
	DirectX::XMFLOAT3 ambient;//�A���r�G���g
	float pad;
	DirectX::XMFLOAT3 diffuse;//�f�B�t���[�Y
	float pad2;
	DirectX::XMFLOAT3 specular;//�X�y�L�����[
	float alpha;
};

struct MaterialData
{
	KazMath::Vec3<float> ambient;//�A���r�G���g
	KazMath::Vec3<float> diffuse;//�f�B�t���[�Y
	KazMath::Vec3<float> specular;//�X�y�L�����[
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>> textureBuffer;

	MaterialBufferData GetMaterialData();
};

struct ModelMeshData
{
	//���_���
	VertexData vertexData;
	//�}�e���A�����
	MaterialData materialData;
};

struct ModelInfomation
{
	std::vector<ModelMeshData> modelData;
	RESOURCE_HANDLE modelVertDataHandle;
	std::shared_ptr<Skeleton> skelton;

	ModelInfomation(const std::vector<ModelMeshData>& model, RESOURCE_HANDLE vertHandle, const std::shared_ptr<Skeleton>& skel);
	ModelInfomation(RESOURCE_HANDLE vertHandle);
};

class ModelAnimator
{
public:

	static const int MAX_BONE_NUM = 256;
	ModelAnimator() {}
	ModelAnimator(std::weak_ptr<ModelInfomation>Model);
	void Attach(std::weak_ptr<ModelInfomation>Model);


	//�P�ʍs��Ŗ��߂�
	void Reset();

	//�w�肵���A�j���[�V�����̎n�܂�̎p�����Z�b�g
	void SetStartPosture(const std::string& AnimationName);

	//�w�肵���A�j���[�V�����̏I���̎p�����Z�b�g
	void SetEndPosture(const std::string& AnimationName);

	//�A�j���[�V�����Đ�
	void Play(const std::string& AnimationName, const bool& Loop, const bool& Blend, float InitPast = 0.0f);
	//��������̃A�j���[�V���������ݍĐ�����
	bool IsPlay()const
	{
		return !playAnimations.empty();
	}

	//�w��̃A�j���[�V���������ݍĐ�����
	bool IsPlay(const std::string& AnimationName)
	{
		auto result = std::find_if(playAnimations.begin(), playAnimations.end(), [AnimationName](PlayAnimation& Anim)
			{
				return AnimationName.compare(Anim.name) == 0;
			});
		return result != playAnimations.end();
	}
	//�A�j���[�V�����X�V
	void Update(const float& arg_timeScale);

	//�{�[���g�����X�t�H�[���ɐe�ݒ�
	void SetParentTransform(KazMath::Transform3D& arg_parent);

	const KazBufferHelper::BufferData& GetBoneMatBuff() { return boneBuff; }
	KazMath::Transform3D& GetBoneTransform(const std::string& BoneName);
private:

	//�Ή�����X�P���g���̎Q��
	std::weak_ptr<Skeleton>attachSkelton;
	//�{�[���̃��[�J���s��
	KazBufferHelper::BufferData boneBuff;
	//�{�[���s��iGPU���M�p�j
	std::array<DirectX::XMMATRIX, MAX_BONE_NUM>boneMatricies;
	//�e�{�[����Transform
	std::vector<KazMath::Transform3D>m_boneTransform;

	struct PlayAnimation
	{
		std::string name;	//�A�j���[�V������
		float past = 0.0f;
		bool loop = false;
		bool finish = false;

		PlayAnimation(const std::string& Name, const bool& Loop) :name(Name), loop(Loop) {}
	};

	std::list<PlayAnimation>playAnimations;

	//ComputeShader computeAnimation;


	//�{�[��Transform�ꊇ�v�Z
	void CalculateTransform(KazMath::Transform3D& BoneTransform, const Skeleton::BoneAnimation& BoneAnim, const float& Frame, bool& FinishFlg);
	void BoneMatrixRecursive(const int& BoneIdx, const float& Past, bool* Finish, Skeleton::ModelAnimation& Anim);

};

class InitAnimationData :public ISingleton<InitAnimationData>
{
public:
	InitAnimationData()
	{
		m_initBoneBuffer = KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX) * ModelAnimator::MAX_BONE_NUM);
		m_initBoneBuffer.rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	};

	const KazBufferHelper::BufferData& GetBoneInitBuffer(GraphicsRootParamType arg_rootParam)
	{
		m_initBoneBuffer.rootParamType = arg_rootParam;
		return m_initBoneBuffer;
	}
private:
	KazBufferHelper::BufferData m_initBoneBuffer;
};