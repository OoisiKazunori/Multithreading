#pragma once
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>

namespace Raytracing
{

	class Blas;

	//BottomLevelAccelerationStructure�ɕK�v�ȃf�[�^���܂Ƃ߂��N���X
	//���C�g���ŕ`�悷��I�u�W�F�N�g��BLAS�𖈃t���[������ɓo�^����B
	class BlasVector {

	private:

		/*===== �����o�ϐ� =====*/

		std::vector<std::weak_ptr<Blas>> m_refBlas;	//���C�g���ŕ`�悷��Blas�̎Q�� (���t���[���o�^����B)
		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> m_instanceDesc;	//Blas��Tlas�ɓo�^���邽�߂ɕό`������ԁB


	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		BlasVector();

		/// <summary>
		/// �X�V���� ��ɔz�����ɂ���B
		/// </summary>
		void Update();

		/// <summary>
		/// ���C�g���Ƃ��ĕ`�悷�邽�߂ɔz��ɒǉ��B
		/// </summary>
		/// <param name="arg_refBlas"></param>
		/// <param name="arg_worldMat"></param>
		void Add(std::weak_ptr<Blas> arg_refBlas, const DirectX::XMMATRIX& arg_worldMat, int arg_instanceIndex = 0);
		void AddVector(std::weak_ptr<Blas> arg_refBlas, std::vector<DirectX::XMMATRIX> arg_worldMat, int arg_instanceIndex = 0, UINT arg_instanceMask = 0xFF);

		/// <summary>
		/// �ۑ�����Ă���Q�Ƃ̐��B
		/// </summary>
		int GetBlasRefCount();
		int GetInstanceCount();

		/// <summary> 
		/// �Q�ƌ��̃V�F�[�_�[���R�[�h�ɃA�N�Z�X���ď������ށB
		/// </summary>
		/// <param name="arg_dest"> �������ݐ�|�C���^ </param>
		/// <param name="arg_recordSize"> �������ރT�C�Y </param>
		/// <param name="arg_stateObject"> �������ރI�u�W�F�N�g </param>
		/// <param name="arg_hitGroup"> �g�p����HitGroup�̖��O </param>
		/// <returns> �������݌�̃|�C���^ </returns>
		uint8_t* WriteShaderRecord(uint8_t* arg_dest, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stateObject, LPCWSTR arg_hitGroup);

		/// <summary>
		/// Instance�z��̐擪�A�h���X��Ԃ��B
		/// </summary>
		/// <returns></returns>
		inline void* GetInstanceData() { return m_instanceDesc.data(); }

	};

}