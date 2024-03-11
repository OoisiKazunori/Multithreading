#include "AnimationData.h"

void Skeleton::CreateBoneTree(const DirectX::XMMATRIX& CoordinateSysConvertMat)
{
	//�{�[�����Ȃ��Ȃ疳��
	if (bones.empty())return;

	//�S�Ă̐e�ƂȂ�{�[����ǉ�
	int parentBoneIdx = static_cast<int>(bones.size());
	bones.emplace_back();
	bones.back().name = "Defined_Parent";

	//�{�[���m�[�h�}�b�v�����
	for (int idx = 0; idx < bones.size(); ++idx)
	{
		auto& bone = bones[idx];
		boneIdxTable[bone.name] = idx;

		if (bone.parent != -1)continue;	//���ɐe������
		if (idx == parentBoneIdx)continue;	//���M���S�Ă̐e
		bone.parent = parentBoneIdx;
	}

	//�e�q�֌W�\�z
	for (int i = 0; i < bones.size(); ++i)
	{
		auto& bone = bones[i];
		//�e�C���f�b�N�X���`�F�b�N(���肦�Ȃ��ԍ��Ȃ�Ƃ΂�)
		if (bone.parent < 0 || bones.size() < bone.parent)
		{
			continue;
		}
		bones[bone.parent].children.emplace_back(i);
	}

	coordinateSystemConvertMat = CoordinateSysConvertMat;
}

int Skeleton::GetIndex(const std::string& BoneName)
{
	assert(!bones.empty());
	assert(boneIdxTable.contains(BoneName));
	return boneIdxTable[BoneName];
}

void ModelAnimator::CalculateTransform(KazMath::Transform3D& BoneTransform, const Skeleton::BoneAnimation& BoneAnim, const float& Frame, bool& FinishFlg)
{
	//Translation
	//�L�[�t���[�����Ȃ�
	if (!BoneAnim.posAnim.keyFrames.empty())
	{
		//���ʂ̊i�[��
		KazMath::Vec3<float>translation;

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.posAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.posAnim.startFrame)translation = BoneAnim.posAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.posAnim.endFrame < Frame)translation = BoneAnim.posAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<KazMath::Vec3<float>>* firstKey = nullptr;
			const KeyFrame<KazMath::Vec3<float>>* secondKey = nullptr;
			for (auto& key : BoneAnim.posAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					translation = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					translation = KazMath::Larp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					break;
				}
			}
		}
		//���ʂ�Transform�ɔ��f
		BoneTransform.pos = translation;
	}

	//Rotation
	//�L�[�t���[�����Ȃ�
	if (!BoneAnim.rotateAnim.keyFrames.empty())
	{
		//���ʂ̊i�[��
		DirectX::XMVECTOR rotation{};

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.rotateAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.rotateAnim.startFrame)rotation = BoneAnim.rotateAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.rotateAnim.endFrame < Frame)rotation = BoneAnim.rotateAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<DirectX::XMVECTOR>* firstKey = nullptr;
			const KeyFrame<DirectX::XMVECTOR>* secondKey = nullptr;
			for (auto& key : BoneAnim.rotateAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					rotation = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					float rate = (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame);
					//rotation = XMQuaternionSlerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					rotation = DirectX::XMQuaternionSlerpV(firstKey->value, secondKey->value, DirectX::XMVectorSet(rate, rate, rate, rate));
					break;
				}
			}
		}
		//���ʂ�Transform�ɔ��f
		BoneTransform.quaternion = rotation;
	}
	//Scaling
	//�L�[�t���[�����Ȃ�
	if (!BoneAnim.scaleAnim.keyFrames.empty())
	{
		//���ʂ̊i�[��
		KazMath::Vec3<float>scale;

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.scaleAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.scaleAnim.startFrame)scale = BoneAnim.scaleAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.scaleAnim.endFrame < Frame)scale = BoneAnim.scaleAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<KazMath::Vec3<float>>* firstKey = nullptr;
			const KeyFrame<KazMath::Vec3<float>>* secondKey = nullptr;
			for (auto& key : BoneAnim.scaleAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					scale = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					scale = KazMath::Larp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					break;
				}
			}
			//���ʂ�Transform�ɔ��f
			BoneTransform.scale = scale;
		}
	}
}

void ModelAnimator::BoneMatrixRecursive(const int& BoneIdx, const float& Past, bool* Finish, Skeleton::ModelAnimation& Anim)
{
	auto skel = attachSkelton.lock();
	const auto& bone = skel->bones[BoneIdx];
	const auto& boneAnim = Anim.boneAnim[bone.name];

	//�{�[���A�j���[�V�����s��̌v�Z
	bool finish = true;

	CalculateTransform(m_boneTransform[BoneIdx], boneAnim, Past, finish);

	//�A�j���[�V�����͏I�����Ă��Ȃ�
	if (Finish != nullptr && !finish)*Finish = false;

	boneMatricies[BoneIdx] = skel->coordinateSystemConvertMat
		* skel->bones[BoneIdx].invBindMat * m_boneTransform[BoneIdx].GetMat()
		* XMMatrixInverse(nullptr, skel->coordinateSystemConvertMat);

	//�q���Ăяo���čċA�I�Ɍv�Z
	for (auto& child : bone.children)
	{
		BoneMatrixRecursive(child, Past, Finish, Anim);
	}
}

ModelAnimator::ModelAnimator(std::weak_ptr<ModelInfomation> Model)
{
	Attach(Model);
}

void ModelAnimator::Attach(std::weak_ptr<ModelInfomation> Model)
{
	auto model = Model.lock();
	auto skel = model->skelton;
	assert(skel->bones.size() <= MAX_BONE_NUM);

	//�o�b�t�@������
	if (!boneBuff.bufferWrapper)
	{
		boneBuff = KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX) * MAX_BONE_NUM, "BoneMatricies");
	}
	Reset();

	//�X�P���g�����A�^�b�`
	attachSkelton = skel;

	m_boneTransform.resize(skel->bones.size());
	for (int boneIdx = 0; boneIdx < skel->bones.size(); ++boneIdx)
	{
		//�e������Ȃ�A�e�{�[����Transform�o�^
		if (skel->bones[boneIdx].parent != -1)
		{
			m_boneTransform[boneIdx].SetParent(&m_boneTransform[skel->bones[boneIdx].parent]);
		}
	}

}

void ModelAnimator::Reset()
{
	//�P�ʍs��Ŗ��߂ă��Z�b�g
	std::array<DirectX::XMMATRIX, MAX_BONE_NUM>initMat;
	initMat.fill(DirectX::XMMatrixIdentity());
	boneBuff.bufferWrapper->TransData(initMat.data(), sizeof(DirectX::XMMATRIX) * MAX_BONE_NUM);
	//�Đ����A�j���[�V���������Z�b�g
	playAnimations.clear();
}

void ModelAnimator::SetStartPosture(const std::string& AnimationName)
{
	auto skel = attachSkelton.lock();
	if (!skel)return;	//�X�P���g�����A�^�b�`����Ă��Ȃ�

	//�A�j���[�V�������擾
	auto& anim = skel->animations[AnimationName];

	//�{�[���s����ċA�I�Ɍv�Z
	BoneMatrixRecursive(static_cast<int>(skel->bones.size() - 1), 0, nullptr, anim);

	//�o�b�t�@�Ƀf�[�^�]��
	boneBuff.bufferWrapper->TransData(boneMatricies.data(), sizeof(DirectX::XMMATRIX) * MAX_BONE_NUM);
}

void ModelAnimator::SetEndPosture(const std::string& AnimationName)
{
	auto skel = attachSkelton.lock();
	if (!skel)return;	//�X�P���g�����A�^�b�`����Ă��Ȃ�

	//�A�j���[�V�������擾
	auto& anim = skel->animations[AnimationName];

	//�{�[���s����ċA�I�Ɍv�Z
	BoneMatrixRecursive(static_cast<int>(skel->bones.size() - 1), static_cast<float>(anim.finishTime), nullptr, anim);

	//�o�b�t�@�Ƀf�[�^�]��
	boneBuff.bufferWrapper->TransData(boneMatricies.data(), sizeof(DirectX::XMMATRIX) * MAX_BONE_NUM);
}

void ModelAnimator::Play(const std::string& AnimationName, const bool& Loop, const bool& Blend, float InitPast)
{
	if (!Blend)Reset();

	auto skel = attachSkelton.lock();
	assert(skel);
	assert(skel->animations.contains(AnimationName));

	//���ɍĐ��������ׂ�
	auto already = std::find_if(playAnimations.begin(), playAnimations.end(), [AnimationName](PlayAnimation& Anim)
		{ return !Anim.name.compare(AnimationName); });
	//�Đ����Ȃ烊�Z�b�g���Ă����
	if (already != playAnimations.end())
	{
		already->past = static_cast<float>(InitPast);
		already->loop = Loop;
		return;
	}

	//�V�K�A�j���[�V�����ǉ�
	playAnimations.emplace_back(AnimationName, Loop);
}

void ModelAnimator::Update(const float& arg_timeScale)
{
	auto skel = attachSkelton.lock();
	if (!skel)return;	//�X�P���g�����A�^�b�`����Ă��Ȃ�
	if (playAnimations.empty())return;	//�A�j���[�V�����Đ����łȂ�

	//�P�ʍs��Ŗ��߂ă��Z�b�g
	std::fill(boneMatricies.begin(), boneMatricies.end(), DirectX::XMMatrixIdentity());

	//�Đ����̃A�j���[�V����
	for (auto& playAnim : playAnimations)
	{
		//�A�j���[�V�������擾
		auto& anim = skel->animations[playAnim.name];

		//�A�j���[�V�������I�����Ă��邩�̃t���O
		bool animFinish = true;

		//�{�[���s����ċA�I�Ɍv�Z
		BoneMatrixRecursive(static_cast<int>(skel->bones.size() - 1), playAnim.past, &animFinish, anim);

		//�t���[���o��
		playAnim.past += arg_timeScale;
		//�A�j���[�V�����̏I�����L�^
		playAnim.finish = animFinish;
	}

	//�I�����Ă���A�j���[�V�����𒲂ׂ�
	for (auto itr = playAnimations.begin(); itr != playAnimations.end();)
	{
		//�A�j���[�V�����I�����Ă��Ȃ��Ȃ�X���[
		if (!itr->finish) { ++itr; continue; }
		//���[�v�t���O�������Ă���Ȃ�o�߃t���[�������Z�b�g���ăX���[
		if (itr->loop) { itr->past = 0; ++itr; continue; }
		//�I�����Ă���̂ō폜
		itr = playAnimations.erase(itr);
	}

	//�o�b�t�@�Ƀf�[�^�]��
	boneBuff.bufferWrapper->TransData(boneMatricies.data(), sizeof(DirectX::XMMATRIX) * MAX_BONE_NUM);
}

void ModelAnimator::SetParentTransform(KazMath::Transform3D& arg_parent)
{
	for (auto& boneTransform : m_boneTransform)
	{
		boneTransform.SetParent(&arg_parent);
	}
}

KazMath::Transform3D& ModelAnimator::GetBoneTransform(const std::string& BoneName)
{
	assert(attachSkelton.lock()->boneIdxTable.contains(BoneName));
	return m_boneTransform[attachSkelton.lock()->boneIdxTable[BoneName]];
}