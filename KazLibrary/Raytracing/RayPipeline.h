#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <array>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "BlasVector.h"
#include "Tlas.h"
#include "HitGroupMgr.h"
#include "DirectXCommon/DirectX12.h"
#include "Helper/KazBufferHelper.h"
#include "Helper/Compute.h"

namespace PostEffect {
	class LensFlare;
}

namespace Raytracing {

	class RayRootsignature;
	//class RaytracingOutput;

	/// <summary>
	/// �V�F�[�_�[�e�[�u���ɓo�^����e��V�F�[�_�[�̃G���g���|�C���g����ۑ��������́B
	/// </summary>
	struct RayPipelineShaderData {

		std::string m_shaderPath;					//�V�F�[�_�[�p�X
		std::vector<LPCWSTR> m_rayGenEntryPoint;	//�G���g���|�C���g
		std::vector<LPCWSTR> m_missEntryPoint;		//�G���g���|�C���g
		std::vector<LPCWSTR> m_hitgroupEntryPoint;	//�G���g���|�C���g
		RayPipelineShaderData() {};
		RayPipelineShaderData(const std::string& arg_shaderPath, const std::vector<LPCWSTR>& arg_RGEntry, const std::vector<LPCWSTR>& arg_MSEntry, const std::vector<LPCWSTR>& arg_HGEntry)
			:m_shaderPath(arg_shaderPath), m_rayGenEntryPoint(arg_RGEntry), m_missEntryPoint(arg_MSEntry), m_hitgroupEntryPoint(arg_HGEntry) {};

	};

	/// <summary>
	/// ���C�g���[�V���O�Ŏg�p����p�C�v���C��
	/// </summary>
	class RayPipeline {

	private:

		/*===== �����o�ϐ� =====*/

		std::vector<RayPipelineShaderData> m_shaderData;		//�g�p����V�F�[�_�[��Z�߂��\����
		std::vector<D3D12_SHADER_BYTECODE> m_shaderCode;		//�g�p����V�F�[�_�[�̃o�C�g�R�[�h
		Microsoft::WRL::ComPtr<ID3D12StateObject> m_stateObject;//�X�e�[�g�I�u�W�F�N�g
		std::shared_ptr<RayRootsignature> m_globalRootSig;		//�O���[�o�����[�g�V�O�l�`��
		D3D12_DISPATCH_RAYS_DESC m_dispatchRayDesc;				//���C���ˎ��̐ݒ�
		Microsoft::WRL::ComPtr<ID3D12Resource> m_shaderTable;	//�V�F�[�_�[�e�[�u��
		void* m_shaderTalbeMapAddress;							//�V�F�[�_�[�e�[�u���̃f�[�^�]���pMap�A�h���X
		Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> m_rtsoProps;
		LPCWSTR m_hitGroupName;
		int m_numBlas;											//�p�C�v���C���ɑg�ݍ��܂�Ă���Blas�̐��BBlasRefecenceVector���ێ����Ă���Q�Ƃ̐������̕ϐ��̐��𒴂�����p�C�v���C������蒼���B


		//�V�F�[�_�[�e�[�u���̍\�z�ɕK�v�ȕϐ��Q
		UINT m_raygenRecordSize;
		UINT m_missRecordSize;
		UINT m_hitgroupRecordSize;
		UINT m_hitgroupCount;
		UINT m_raygenSize;
		UINT m_missSize;
		UINT m_hitGroupSize;
		UINT m_tableAlign;
		UINT m_hitgroupRegion;
		UINT m_tableSize;
		UINT m_raygenRegion;
		UINT m_missRegion;

		static DirectX12* m_refDirectX12;

		KazBufferHelper::BufferData* m_refVolumeNoiseTexture;
		KazBufferHelper::BufferData* m_refRaymarchingConstData;
		KazBufferHelper::BufferData* m_refDebugOnOffConstData;
		KazBufferHelper::BufferData* m_refDebugSeaConstData;

		//�����Y�t���A�N���X
		std::shared_ptr<PostEffect::LensFlare> m_lensFlare;

		//�A�E�g���C�������p
		ComputeShader m_outlineComposeShader;
		ComputeShader m_outlineCleanShader;

	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <param name="InputData"> �g�p����V�F�[�_�[�̊e��f�[�^ </param>
		/// <param name="UseHitGroup"> �g�p����HitGroup�̖��O </param>
		/// <param name="SRVCount"> �g�p����SRV�̐� </param>
		/// <param name="CBVCount"> �g�p����CBV�̐� </param>
		/// <param name="UAVCount"> �g�p����UAV�̐� </param>
		/// <param name="PayloadSize"> �V�F�[�_�[�Ŏg�p����y�C���[�h�̃T�C�Y </param>
		/// <param name="AttribSize"> �d�S���W�Ȃǂ̌v�Z�Ɏg�p����\���̂̃T�C�Y �Œ� </param>
		/// <param name="ReflectionCount"> �ő僌�C�ˏo�� </param>
		RayPipeline(const std::vector<RayPipelineShaderData>& arg_inputData, HitGroupMgr::HITGROUP_ID arg_useHitGroup, int arg_SRVCount, int arg_CBVCount, int arg_UAVCount, int arg_payloadSize, int arg_attribSize, int arg_reflectionCount = 4);

		/// <summary>
		/// �V�F�[�_�[�e�[�u�����\�z�B ���t���[���S�Ă�Draw���I������Ƃ��ɌĂԁB
		/// </summary>
		/// <param name="arg_blacVector"> Blas�̔z�� </param>
		/// <param name="arg_dispatchX"> ���̉�ʃT�C�Y </param>
		/// <param name="arg_dispatchY"> �c�̉�ʃT�C�Y </param>
		void BuildShaderTable(BlasVector arg_blacVector, int arg_dispatchX = 1280, int arg_dispatchY = 720);

		/// <summary>
		/// �V�F�[�_�[�e�[�u�����\�z�B
		/// </summary>
		/// <param name="arg_blacVector"> Blas�̔z�� </param>
		/// <param name="arg_dispatchX"> ���̉�ʃT�C�Y </param>
		/// <param name="arg_dispatchY"> �c�̉�ʃT�C�Y </param>
		void ConstructionShaderTable(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY);

		/// <summary>
		/// HipGroup�̃f�[�^��GPU�ɓ]���B Blas�̃f�[�^�Ȃǂ𑗂�B
		/// </summary>
		/// <param name="arg_blacVector"> Blas�̔z�� </param>
		void MapHitGroupInfo(BlasVector arg_blacVector);

		/// <summary>
		/// ���C�g�����s�B
		/// </summary>
		void TraceRay(Tlas arg_tlas);

		//�e��Q�b�^
		Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return m_stateObject; }
		D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return m_dispatchRayDesc; }
		std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return m_globalRootSig; }

		static void SetDirectX12(DirectX12* arg_refDirectX12) { m_refDirectX12 = arg_refDirectX12; }
		void SetVolumeFogTexture(KazBufferHelper::BufferData* arg_volumeFogTexture) { m_refVolumeNoiseTexture = arg_volumeFogTexture; }
		void SetRaymarchingConstData(KazBufferHelper::BufferData* arg_raymarchingParamConstData) { m_refRaymarchingConstData = arg_raymarchingParamConstData; }
		void SetDebugOnOffConstData(KazBufferHelper::BufferData* arg_debugOnOffConstData) { m_refDebugOnOffConstData = arg_debugOnOffConstData; }
		void SetDebugSeaConstData(KazBufferHelper::BufferData* arg_debugSeaConstData) { m_refDebugSeaConstData = arg_debugSeaConstData; }

	private:

		/// <summary>
		/// �q�b�g�O���[�v�̒��ōł��g�p�f�[�^�T�C�Y���傫���̂��擾����B
		/// </summary>
		/// <returns> �q�b�g�O���[�v�̒��ōł��傫���T�C�Y </returns>
		UINT GetLargestDataSizeInHitGroup();

		/// <summary>
		/// �A���C�������g����B
		/// </summary>
		/// <param name="Size"> �A���C�������g�������T�C�Y�B </param>
		/// <param name="Align"> �A���C�������g </param>
		/// <returns> ���� </returns>
		UINT RoundUp(size_t arg_aize, UINT arg_align) {
			return UINT(arg_aize + arg_align - 1) & ~(arg_align - 1);
		}

		/// <summary>
		/// �V�F�[�_�[�e�[�u�����������݁A���C��ݒ肷��B--------------------------------------------------------------------------�V�F�[�_�[�e�[�u���̏������݂ƃ��C�̐ݒ�̊֐��𕪂���B
		/// </summary>
		/// <param name="arg_blacVector"> Blas�̔z�� </param>
		/// <param name="arg_dispatchX"></param>
		/// <param name="arg_dispatchY"></param>
		void WriteShadetTalbeAndSettingRay(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY);

		/// <summary>
		/// �V�F�[�_�[���ʎq���������ށB
		/// </summary>
		/// <param name="Dst"> �������ݐ� </param>
		/// <param name="ShaderId"> ��������ID </param>
		/// <returns> �������񂾌�̃n���h�� </returns>
		UINT WriteShaderIdentifier(void* arg_dst, const void* arg_shaderId);

		/// <summary>
		/// RayGeneration�V�F�[�_�[�̐����擾�B
		/// </summary>
		/// <returns></returns>
		int GetRayGenerationCount();

		/// <summary>
		///  MissShader�̐����擾�B
		/// </summary>
		/// <returns></returns>
		int GetMissCount();

		/// <summary>
		/// �o�b�t�@�𐶐�����֐� �ʓ|�������̂Ŏ��̃G���W�����炻�̂܂܎����Ă��܂����B
		/// </summary>
		/// <param name="Size"> �o�b�t�@�T�C�Y </param>
		/// <param name="Flags"> �o�b�t�@�̃t���O </param>
		/// <param name="InitialState"> �o�b�t�@�̏������ </param>
		/// <param name="HeapType"> �q�[�v�̎�� </param>
		/// <param name="Name"> �o�b�t�@�ɂ��閼�O </param>
		/// <returns> �������ꂽ�o�b�t�@ </returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initialState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_name = nullptr);

		//UAV�Ƀ��\�[�X�o���A��������B
		void UAVBarrier(const std::vector<KazBufferHelper::BufferData> &arg_bufferArray);

		//�o�b�t�@�̏�Ԃ�J�ڂ�����B
		void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

		//���C�g���Ŏg�p���郊�\�[�X�ނ��Z�b�g�B
		void SetRaytracingResource(Tlas arg_tlas);

	};

}