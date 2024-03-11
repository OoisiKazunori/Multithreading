#pragma once
#include <Helper/Compute.h>
#include "../DirectXCommon/DirectX12.h"

namespace PostEffect {

	class GaussianBlur;

	//���s���邱�Ƃ�GBuffer����V�[�����Ɩ��邳���������Ă���Bloom�������Ă����N���X�B
	class Bloom {

	public:

		/*===== �ϐ� =====*/

		//���͂��ꂽ�e�N�X�`���Ƃ��̃R�s�[�e�N�X�`��
		KazBufferHelper::BufferData m_targetTexture;
		KazBufferHelper::BufferData m_targetCopyTexture;

		//�쐣���u���[����������ۂɕK�v�ȃe�N�X�`��
		static const int BLUR_TEXTURE_COUNT = 4;
		std::array<KazBufferHelper::BufferData, BLUR_TEXTURE_COUNT> m_blurTextures;

		//�u���[�������Ă�����B
		std::array<std::shared_ptr<GaussianBlur>, BLUR_TEXTURE_COUNT + 1> m_blur;

		//�ŏI�����p�V�F�[�_�[
		ComputeShader m_composeShader;

		//���̑�
		KazMath::Vec2<UINT> TEXSIZE = KazMath::Vec2<UINT>(1280, 720);


	public:

		/*===== �֐� =====*/

		//�R���X�g���N�^
		Bloom(KazBufferHelper::BufferData arg_bloomTargetTexture);

		//�u���[����������B
		void Apply();

	private:

		//�e�N�X�`�����R�s�[����B
		void CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture);

		//�o�b�t�@�̏�Ԃ�J�ڂ�����B
		void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

	};

}