#pragma once
#include <Helper/Compute.h>
#include "../DirectXCommon/DirectX12.h"

namespace PostEffect {

	class GaussianBlur;
	class Bloom;

	//���s���邱�Ƃ�GBuffer����V�[�����Ɩ��邳���������Ă��ă����Y�t���A�������Ă����N���X�B
	class LensFlare {

	public:			//���Private�ɂ���I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I

		/*===== �ϐ� =====*/

		//�����Y�t���A�p�X�֘A
		ComputeShader m_lensFlareShader;					//�����Y�t���A��������V�F�[�_�[
		KazBufferHelper::BufferData m_lensFlareTexture;		//�����Y�t���A���������e�N�X�`��
		KazBufferHelper::BufferData m_lensColorTexture;		//�����Y�̐F�e�N�X�`��
		KazMath::Vec2<UINT> LENSFLARE_TEXSIZE = KazMath::Vec2<UINT>(1280, 720);

		//�u���[��(�t���A)�p�X
		KazBufferHelper::BufferData m_bloomTexture;
		std::shared_ptr<Bloom> m_bloom;

		//�ŏI���H�p�X�֘A
		ComputeShader m_finalProcessingShader;				//�ŏI���H and �����p�V�F�[�_�[
		KazBufferHelper::BufferData m_lendDirtTexture;		//�����Y�̉���e�N�X�`��
		KazBufferHelper::BufferData m_lensStarTexture;		//�����Y�̃X�^�[�o�[�X�g�e�N�X�`��
		KazBufferHelper::BufferData m_cametaVecConstBuffer;	//�����Y�̃X�^�[�o�[�X�g����]������萔�o�b�t�@
		struct CameraVec {
			KazMath::Vec3<float> m_cameraXVec;
			float m_pad;
			KazMath::Vec3<float> m_cameraZVec;
			float m_pad2;
		}m_cameraVec;
		KazMath::Vec2<UINT> BACKBUFFER_SIZE = KazMath::Vec2<UINT>(1280, 720);

		//�u���[�p�X�֘A
		std::shared_ptr<GaussianBlur> m_blurPath;

		//���̑�
		KazBufferHelper::BufferData m_lensFlareTargetTexture;	//�R���X�g���N�^�Őݒ肳���A�����Y�t���A��������ΏہB
		DirectX12* m_refDirectX12;
		KazBufferHelper::BufferData m_lensFlareTargetCopyTexture;	//�����Y�t���A��������Ώۂ��R�s�[�����e�N�X�`��
		KazMath::Vec2<UINT> COPY_TEXSIZE = KazMath::Vec2<UINT>(1280, 720);


	public:

		/*===== �֐� =====*/

		//�R���X�g���N�^
		LensFlare(KazBufferHelper::BufferData arg_lnesflareTargetTexture, KazBufferHelper::BufferData arg_emissiveTexture);

		//�����Y�t���A��������B
		void Apply();

	private:

		//�����Y�t���A��������e�N�X�`���̃R�s�[���쐬����B
		void GenerateCopyOfLensFlareTexture();

		//�o�b�t�@�̏�Ԃ�J�ڂ�����B
		void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

		//�e�N�X�`�����R�s�[����B
		void CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture);

	};

}