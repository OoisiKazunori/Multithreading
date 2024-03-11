#pragma once
#include <Helper/Compute.h>
#include "Helper/KazBufferHelper.h"
#include "Math/KazMath.h"
#include <array>

//�K�E�V�A���u���[��������N���X
namespace PostEffect {

	class GaussianBlur {

	private:

		/*===== �ϐ� =====*/

		ComputeShader m_blurXShader;		//X�����̃u���[�̃V�F�[�_�[
		ComputeShader m_blurYShader;		//Y�����̃u���[�̃V�F�[�_�[
		ComputeShader m_composeShader;		//�u���[����������V�F�[�_�[

		KazBufferHelper::BufferData m_blurXResultTexture;	//X�����̃u���[�̏o�͉摜
		KazBufferHelper::BufferData m_blurYResultTexture;	//Y�����̃u���[�̏o�͉摜
		KazBufferHelper::BufferData m_blurTargetTexture;	//�R���X�g���N�^�Őݒ肳���A�u���[��������ΏہB

		KazBufferHelper::BufferData m_blurPowerConstBuffer;	//�K�E�V�A���u���[�̋���

		static const int BLOOM_GAUSSIAN_WEIGHTS_COUNT = 8;	//�K�E�V�A���u���[�̋����Ɋւ���萔�o�b�t�@�̃p�����[�^�[�̐��B
		std::array<float, BLOOM_GAUSSIAN_WEIGHTS_COUNT> m_blurWeight;

		//�K�E�V�A���u���[�̋���
		const float BLUR_POWER = 1000000.0f;

		//�u���[�̏o�͐�e�N�X�`���̃T�C�Y
		const KazMath::Vec2<UINT> BLURX_TEXSIZE = KazMath::Vec2<UINT>(1280 / 2, 720);
		const KazMath::Vec2<UINT> BLURY_TEXSIZE = KazMath::Vec2<UINT>(1280 / 2, 720 / 2);

	public:

		/*===== �֐� =====*/

		GaussianBlur(KazBufferHelper::BufferData arg_blurTargetTexture);

		//�u���[�����s
		void ApplyBlur();

	private:

		//�u���[�̏d�݂��v�Z�B
		void CalcBloomWeightsTableFromGaussian();

	};

}