#pragma once
#include <Helper/Compute.h>
#include "Helper/KazBufferHelper.h"
#include "Math/KazMath.h"
#include <array>

//�A�E�g���C�����o�͂���N���X�B
namespace PostEffect {

	class Outline {

	private:

		/*===== �ϐ� =====*/

		ComputeShader m_outlineShader;		//�A�E�g���C�����v�Z�B

		KazBufferHelper::BufferData m_outlineTargetTexture;		//�A�E�g���C����������Ώۂ̃e�N�X�`��
		KazBufferHelper::BufferData m_outputAlbedoTexture;		//�A�E�g���C���̐F�e�N�X�`��
		KazBufferHelper::BufferData m_outputEmissiveTexture;	//�A�E�g���C���̃G�~�b�V�u�̃e�N�X�`��

		KazBufferHelper::BufferData m_outlineColorConstBuffer;	//�A�E�g���C���̐F
		KazMath::Vec4<float> m_outlineColor;

	public:

		/*===== �֐� =====*/

		Outline(KazBufferHelper::BufferData arg_outlineTargetTexture);

		//�A�E�g���C������������
		void Apply();

		KazBufferHelper::BufferData GetOutputAlbedoTexture() { return m_outputAlbedoTexture; }
		KazBufferHelper::BufferData GetOutputEmissiveTexture() { return m_outputEmissiveTexture; }

	};

}