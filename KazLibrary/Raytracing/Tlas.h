#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "BlasVector.h"

namespace Raytracing {

	/// <summary>
	/// TopLevel�̉����\���� BlasVector��o�^���邱�Ƃɂ���ă��C�g����Ԃ��\�z����B
	/// </summary>
	class Tlas {

	private:

		/*===== �����o�ϐ� =====*/

		Microsoft::WRL::ComPtr<ID3D12Resource> m_tlasBuffer;		//Tlas�̃o�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> m_scratchBuffer;		//Tlas���`������ۂɕK�v�ȃo�b�t�@
		Microsoft::WRL::ComPtr<ID3D12Resource> m_instanceDescBuffer;//�C���X�^���X�o�b�t�@
		void* m_instanceDescMapAddress_;							//�C���X�^���X�o�b�t�@�Ƀf�[�^�𑗂�ۂ̃A�h���X
		int m_instanceCapacity;										//���݂�Tlas�̃o�b�t�@��Instance���̋��e�ʁB InstanceVector�̗v�f�������̐��𒴂�����S�Ẵo�b�t�@���Đ�������B
		int m_descHeapIndex;										//Tlas��ۑ����Ă���DescriptorHeap��̃C���f�b�N�X


	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		Tlas();

		/// <summary>
		/// TLAS���\�z�B�S�Ă�Draw�R�[�����I������ɌĂ�ł��������B
		/// </summary>
		/// <param name="arg_blasVector"> �g�p����Blas�̔z�� </param>
		void Build(BlasVector arg_blasVector);

		int GetDescHeapHandle() { return m_descHeapIndex; }

	private:

		/// <summary>
		/// Tlas���\�zor�č\�z����֐��B
		/// </summary>
		/// <param name="arg_blasVector"> �g�p����Blas�̔z�� </param>
		void ReBuildTlas(BlasVector arg_blasVector);

		/// <summary>
		/// Tlas���X�V����֐��B
		/// </summary>
		/// <param name="arg_blasVector"> �g�p����Blas�̔z�� </param>
		void UpdateTlas(BlasVector arg_blasVector);

		/// <summary>
		/// �A�h���X�ɏ����������ޏ���
		/// </summary>
		/// <param name="m_mapAddress"> �f�[�^�̑���� </param>
		/// <param name="m_pData"> ����f�[�^ </param>
		/// <param name="m_dataSize"> ����f�[�^�̃T�C�Y </param>
		void WriteToMemory(void* m_mapAddress, const void* m_pData, size_t m_dataSize);

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

	};

}