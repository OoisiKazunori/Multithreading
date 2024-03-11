#pragma once
#include"../DirectXCommon/Base.h"
#include<memory>

namespace Raytracing {

	/// <summary>
	/// BottomLevel�̉����\���� Blas�Ƃ��čŒ���K�v�ȋ@�\���܂Ƃ߂��N���X�ŁA�S�Ẵ��f���N���X���{�N���X�����B
	/// </summary>
	class Blas {

	private:

		/*===== �����o�ϐ� =====*/

		Microsoft::WRL::ComPtr<ID3D12Resource> m_blasBuffer;		//Blas�̃o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> m_scratchBuffer;		//Blas���`������ۂɕK�v�ȃo�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> m_updateBuffer;		//Blas�̃A�b�v���[�h�p�o�b�t�@

		D3D12_RAYTRACING_GEOMETRY_DESC m_geomDesc;

		RESOURCE_HANDLE m_textureHandle;							//�g�p����e�N�X�`���̃A�h���X
		RESOURCE_HANDLE m_vertexDataHandle;							//���_�f�[�^��ۑ����Ă���f�[�^�̃n���h��
		int m_meshNumber;											//�������b�V���̕`������Ă���ۂɎg�p����B����Blas�͒��_�f�[�^�̂ǂ̃��b�V�����Q�Ƃ��邩�̏��B
		int m_hitGroupIndex;


	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <param name="arg_isOpaque"> �s�����t���O t:�s���� f:�������ɂȂ�\�������� </param>
		/// <param name="arg_vertexDataHandle"> Blas���`�����郂�f���̌`���ۑ����Ă���f�[�^�̃n���h�� </param>
		/// <param name="arg_textureHandle"> �g�p����e�N�X�`���̃A�h���X </param>
		Blas(bool arg_isOpaque, RESOURCE_HANDLE arg_vertexDataHandle, int arg_meshNumber, RESOURCE_HANDLE arg_textureHandle, int arg_hitGroupIndex);

		//�X�V����
		void Update();

		/// <summary>
		/// Blas�̃o�b�t�@��Ԃ��B
		/// </summary>
		/// <returns> Blas�̃o�b�t�@ </returns>
		inline Microsoft::WRL::ComPtr<ID3D12Resource> GetBlasBuffer() { return m_blasBuffer; }

		/// <summary> 
		/// �Q�ƌ��̃V�F�[�_�[���R�[�h�ɃA�N�Z�X���ď������ށB
		/// </summary>
		/// <param name="arg_dst"> �������ݐ�|�C���^ </param>
		/// <param name="arg_recordSize"> �������ރT�C�Y </param>
		/// <param name="arg_stageObject"> �������ރI�u�W�F�N�g </param>
		/// <param name="arg_hitGroupName"> �g�p����HitGroup�̖��O </param>
		/// <returns> �������݌�̃|�C���^ </returns>
		uint8_t* WriteShaderRecord(uint8_t* arg_dst, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stageObject, LPCWSTR arg_hitGroupName);

	private:

		/// <summary>
		/// Blas�̌`��ݒ�ɕK�v�ȍ\���̂�Ԃ��B
		/// </summary>
		/// <param name="arg_isOpaque"> �s�����t���O t:�s���� f:�������ɂȂ�\�������� </param>
		/// <returns></returns>
		D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(bool arg_isOpaque);

		/// <summary>
		/// Blas���\�z
		/// </summary>
		/// <param name="arg_geomDesc"> Blas�̌`��\���́B���N���X�̊֐�����擾����B </param>
		void BuildBlas(const D3D12_RAYTRACING_GEOMETRY_DESC& arg_geomDesc);

		/// <summary>
		/// �o�b�t�@�𐶐�����֐� �ʓ|�������̂Ŏ��̃G���W�����炻�̂܂܎����Ă��܂����B
		/// </summary>
		/// <param name="arg_size"> �o�b�t�@�T�C�Y </param>
		/// <param name="arg_flags"> �o�b�t�@�̃t���O </param>
		/// <param name="arg_initState"> �o�b�t�@�̏������ </param>
		/// <param name="arg_heapType"> �q�[�v�̎�� </param>
		/// <param name="arg_bufferName"> �o�b�t�@�ɂ��閼�O </param>
		/// <returns> �������ꂽ�o�b�t�@ </returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_bufferName = nullptr);

		// GPU�f�B�X�N���v�^���������ށB
		inline UINT WriteGPUDescriptor(void* arg_dst, const D3D12_GPU_DESCRIPTOR_HANDLE* arg_descriptor)
		{
			memcpy(arg_dst, arg_descriptor, sizeof(arg_descriptor));
			return static_cast<UINT>((sizeof(arg_descriptor)));
		}
		inline UINT WriteGPUVirtualAddress(void* arg_dst, const D3D12_GPU_VIRTUAL_ADDRESS* arg_descriptor)
		{
			memcpy(arg_dst, arg_descriptor, sizeof(arg_descriptor));
			return static_cast<UINT>((sizeof(arg_descriptor)));
		}

	public:

		int GetHitGroupIndex();

	};

}