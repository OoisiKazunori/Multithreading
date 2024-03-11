#pragma once
#include <array>
#include <wrl.h>
#include "../DirectXCommon/d3dx12.h"

namespace Raytracing {

	/// <summary>
	/// ���C�g���[�V���O�Ŏg�p����O���[�o�����[�g�V�O�l�`���A���[�J�����[�g�V�O�l�`���N���X
	/// </summary>
	class RayRootsignature {

	private:

		/*===== �萔 =====*/

		static const int MAX_ROOTPARAM = 16;
		static const int MAX_SAMPLER = 26;


	private:

		/*===== �����o�ϐ� =====*/

		::std::array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> m_rootparam;	//���[�g�p�����[�^�[
		::std::array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> m_descRange;//�f�B�X�N���v�^�e�[�u��
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootsignature;	//���[�g�V�O�l�`��
		::std::array<CD3DX12_STATIC_SAMPLER_DESC, MAX_SAMPLER> m_sampler;	//�X�^�e�B�b�N�T���v���[
		UINT m_rootparamCount;											//���[�g�p�����[�^�[��
		UINT m_samplerCount;											//�T���v���[�̐�


	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		RayRootsignature() {
			m_rootparamCount = 0;
			m_samplerCount = 0;
		}

		/// <summary>
		/// ���[�g�p�����[�^�[�ǉ�����
		/// </summary>
		/// <param name="arg_type"> �f�[�^�̎�� SRV�Ƃ�UAV�Ƃ� </param>
		/// <param name="arg_shaderRegister"> ���W�X�^�ԍ� t0�Ƃ���0�̕� </param>
		/// <param name="arg_registerSpace"> ���W�X�^�X�y�[�X t0�̌�ɂ���space�̐��� </param>
		void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE arg_type, UINT arg_shaderRegister, UINT arg_registerSpace = 0);

		/// <summary>
		/// �X�^�e�B�b�N�T���v���[�ǉ�����
		/// </summary>
		/// <param name="RegisterSpace"> ���W�X�^�X�y�[�X t0�̌�ɂ���space�̐��� </param>
		void AddStaticSampler(int arg_registerSpace = 0);

		/// <summary>
		/// ���[�g�V�O�l�`���̐���
		/// </summary>
		/// <param name="IsLocal"> ���[�J�����[�g�V�O�l�`�����ǂ��� </param>
		/// <param name="Name"> �o�b�t�@�̖��O </param>
		void Build(bool arg_isLocal, const wchar_t* arg_name = nullptr);

		//�Q�b�^
		inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSig() { return m_rootsignature; }

	};

}