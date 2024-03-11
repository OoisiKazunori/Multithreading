#include "RayPipeline.h"
#include "../Raytracing/RayPipeline.h"
#include "../Raytracing/RayRootsignature.h"
#include "../Raytracing/HitGroupMgr.h"
#include "../DirectXCommon/DirectX12Device.h"
#include "../DirectXCommon/DirectX12CmdList.h"
#include "../Buffer/DescriptorHeapMgr.h"
#include "../DirectXCommon/DirectX12.h"
#include "../Raytracing/RayShaderStorage.h"
#include "../Raytracing/Tlas.h"
#include "../Buffer/GBufferMgr.h"
#include "../DirectXCommon/DirectX12.h"
#include "../PostEffect/LensFlare.h"
#include <DirectXMath.h>
#include"../PostEffect/Outline.h"

DirectX12* Raytracing::RayPipeline::m_refDirectX12 = nullptr;

namespace Raytracing {

	RayPipeline::RayPipeline(const std::vector<RayPipelineShaderData>& arg_inputData, HitGroupMgr::HITGROUP_ID arg_useHitGroup, int arg_SRVCount, int arg_CBVCount, int arg_UAVCount, int arg_payloadSize, int arg_attribSize, int arg_reflectionCount)
	{

		/*===== �Z�b�e�B���O���� =====*/

		//�q�b�g�O���[�v����ۑ��B
		m_hitGroupName = HitGroupMgr::Instance()->hitGroupNames[arg_useHitGroup];

		//���͂��ꂽ�f�[�^��ۑ�����B
		const int INPUT_COUNT = static_cast<int>(arg_inputData.size());
		for (int index = 0; index < INPUT_COUNT; ++index) {

			//���͂��ꂽ�f�[�^�\���́B
			RayPipelineShaderData buff;

			//�ۑ�����B
			buff.m_shaderPath = arg_inputData[index].m_shaderPath;

			//�ۑ�����Ă���G���g���|�C���g��ۑ��B
			const int RG_ENTRY_COUNT = static_cast<int>(arg_inputData[index].m_rayGenEntryPoint.size());
			for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

				//�ۑ�����B
				buff.m_rayGenEntryPoint.push_back(arg_inputData[index].m_rayGenEntryPoint[entryPointIndex]);

			}
			const int MS_ENTRY_COUNT = static_cast<int>(arg_inputData[index].m_missEntryPoint.size());
			for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

				//�ۑ�����B
				buff.m_missEntryPoint.push_back(arg_inputData[index].m_missEntryPoint[entryPointIndex]);

			}

			//�ۑ�����B
			const int HS_ENTRY_COUNT = static_cast<int>(arg_inputData[index].m_hitgroupEntryPoint.size());
			for (int entryPointIndex = 0; entryPointIndex < HS_ENTRY_COUNT; ++entryPointIndex) {
				buff.m_hitgroupEntryPoint.push_back(arg_inputData[index].m_hitgroupEntryPoint[entryPointIndex]);
			}

			//�ۑ�����B
			m_shaderData.push_back(buff);

		}

		//�O���[�o�����[�g�V�O�l�`����ݒ�B
		m_globalRootSig = std::make_shared<RayRootsignature>();
		//�p�����[�^�[t0��TLAS(SRV)��ݒ�B
		for (int index = 0; index < arg_SRVCount; ++index)m_globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index);
		//�p�����[�^�[b0�ɃJ�����p�o�b�t�@��ݒ�B
		for (int index = 0; index < arg_CBVCount; ++index) {
			m_globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index);
		}
		//�p�����[�^�[u0�ɏo�͗p�o�b�t�@��ݒ�B
		for (int index = 0; index < arg_UAVCount; ++index)m_globalRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index);

		//���[�g�V�O�l�`���𐶐��B
		m_globalRootSig->Build(false, L"GlobalRootSig");

		//�X�e�[�g�I�u�W�F�N�g�̐ݒ��ۑ����Ă����悤�̍\���́B
		CD3DX12_STATE_OBJECT_DESC subobjects;
		//�X�e�[�g�I�u�W�F�N�g�̏�Ԃ�ݒ�B
		subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

		//�V�F�[�_�[��ǂݍ��ށB
		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());
		for (int index = 0; index < SHADER_COUNT; ++index) {

			m_shaderCode.emplace_back();

			//�V�F�[�_�[���R���p�C������B
			RayShaderStorage::Instance()->LoadShaderForDXC(m_shaderData[index].m_shaderPath, "lib_6_4", "");

			//�V�F�[�_�[��ǂݍ��ށB
			m_shaderCode[index].BytecodeLength = static_cast<SIZE_T>(RayShaderStorage::Instance()->GetShaderBin(m_shaderData[index].m_shaderPath).size());
			m_shaderCode[index].pShaderBytecode = RayShaderStorage::Instance()->GetShaderBin(m_shaderData[index].m_shaderPath).data();

			//�V�F�[�_�[�̊e�֐����R�[�h�̓o�^�B
			auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
			dxilLib->SetDXILLibrary(&m_shaderCode[index]);

			//�V�F�[�_�[�̃G���g���|�C���g��o�^�B
			const int RG_ENTRY_COUNT = static_cast<int>(arg_inputData[index].m_rayGenEntryPoint.size());
			for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

				//�ۑ�����B
				dxilLib->DefineExport(m_shaderData[index].m_rayGenEntryPoint[entryPointIndex]);

			}
			const int MS_ENTRY_COUNT = static_cast<int>(arg_inputData[index].m_missEntryPoint.size());
			for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

				//�ۑ�����B
				dxilLib->DefineExport(m_shaderData[index].m_missEntryPoint[entryPointIndex]);

			}
			const int HG_ENTRY_COUNT = static_cast<int>(arg_inputData[index].m_hitgroupEntryPoint.size());
			for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

				//�ۑ�����B
				dxilLib->DefineExport(m_shaderData[index].m_hitgroupEntryPoint[entryPointIndex]);

			}

		}

		//�q�b�g�O���[�v�̐ݒ�B
		auto hitGroup_ = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup_->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		//ClosestHitShader���G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetCHFlag(arg_useHitGroup)) {
			hitGroup_->SetClosestHitShaderImport(HitGroupMgr::Instance()->GetCH(arg_useHitGroup));
		}
		//AnyHitShader�̃G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetAHFlag(arg_useHitGroup)) {
			hitGroup_->SetAnyHitShaderImport(HitGroupMgr::Instance()->GetAH(arg_useHitGroup));
		}
		//IntersectShader�̃G���g���|�C���g��ۑ��B
		if (HitGroupMgr::Instance()->GetISFlag(arg_useHitGroup)) {
			hitGroup_->SetIntersectionShaderImport(HitGroupMgr::Instance()->GetIS(arg_useHitGroup));
		}
		//�q�b�g�O���[�v����ۑ��B
		hitGroup_->SetHitGroupExport(HitGroupMgr::Instance()->hitGroupNames[arg_useHitGroup]);

		//�O���[�o�����[�g�V�O�l�`���̐ݒ�B
		auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		rootSig->SetRootSignature(m_globalRootSig->GetRootSig().Get());

		//HitGroup�̃��[�J�����[�g�V�O�l�`����ݒ�B
		auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		chLocalRootSig->SetRootSignature(HitGroupMgr::Instance()->GetLocalRootSig(arg_useHitGroup)->GetRootSig().Get());
		auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		chAssocModel->AddExport(HitGroupMgr::Instance()->hitGroupNames[arg_useHitGroup]);
		chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

		//�V�F�[�_�[�̐ݒ�B
		auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		shaderConfig->Config(arg_payloadSize, arg_attribSize);

		//�p�C�v���C���̐ݒ�B
		auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		pipelineConfig->Config(arg_reflectionCount);

		//��������B
		DirectX12Device::Instance()->dev->CreateStateObject(
			subobjects, IID_PPV_ARGS(&m_stateObject)
		);
		m_stateObject->SetName(L"StateObject");

		//Blas�̕ێ���
		m_numBlas = 0;

		//�����Y�t���A
		m_lensFlare = std::make_shared<PostEffect::LensFlare>(GBufferMgr::Instance()->GetLensFlareBuffer(), GBufferMgr::Instance()->GetEmissiveGBuffer());

		//�A�E�g���C�������p�V�F�[�_�[
		{
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 GBufferMgr::Instance()->GetRayTracingBuffer(),
				 GBufferMgr::Instance()->GetLensFlareBuffer(),
				 GBufferMgr::Instance()->m_outline->GetOutputAlbedoTexture(),
				 GBufferMgr::Instance()->m_outline->GetOutputEmissiveTexture(),
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
			extraBuffer[3].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[3].rootParamType = GRAPHICS_PRAMTYPE_TEX4;
			m_outlineComposeShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/Outline/" + "ComposeOutline.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}
		{
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 GBufferMgr::Instance()->m_outlineBuffer,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			m_outlineCleanShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/Outline/" + "BlackOut.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}

	}

	void RayPipeline::BuildShaderTable(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY)
	{

		/*===== �V�F�[�_�[�e�[�u�����\�z =====*/

		//�R���e�i�ɓo�^����Ă���Blas�̐��B
		int blasRefCount = arg_blacVector.GetBlasRefCount();

		//Blas�̐���0�������牽�����Ȃ��B
		if (blasRefCount == 0) return;

		//Blas�̎Q�Ɛ����p�C�v���C���Ɋ܂܂�Ă���Blas�̐�����������V�F�[�_�[�e�[�u�����č\�z����B
		//if (m_numBlas < blasRefCount) {

			//�č\�z�B
			ConstructionShaderTable(arg_blacVector, arg_dispatchX, arg_dispatchY);

			m_numBlas = blasRefCount;

		//}
		//else {

		//	//�č\�z������Blas�̏����X�V�B
		//	MapHitGroupInfo(arg_blacVector);

		//}

	}

	void RayPipeline::ConstructionShaderTable(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY)
	{

		/*===== �V�F�[�_�[�e�[�u�����\�z =====*/

		//�V�F�[�_�[�e�[�u���̃T�C�Y���v�Z�B
		const auto ShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

		//RayGeneration�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
		m_raygenRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		m_raygenRecordSize = RoundUp(m_raygenRecordSize, ShaderRecordAlignment);

		//Miss�V�F�[�_�[�ł̓��[�J�����[�g�V�O�l�`�����g�p�B
		m_missRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		m_missRecordSize = RoundUp(m_missRecordSize, ShaderRecordAlignment);

		//�q�b�g�O���[�v�ł́A�ۑ�����Ă���q�b�g�O���[�v�̒�����ő�̃T�C�Y�̂��̂Ńf�[�^���m�ۂ���B
		m_hitgroupRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		m_hitgroupRecordSize += GetLargestDataSizeInHitGroup();
		m_hitgroupRecordSize = RoundUp(m_hitgroupRecordSize, ShaderRecordAlignment);

		//�g�p����e�V�F�[�_�[�̌����A�V�F�[�_�[�e�[�u���̃T�C�Y�����߂�B
		m_hitgroupCount = HitGroupMgr::Instance()->GetHitGroupCount();
		m_raygenSize = GetRayGenerationCount() * m_raygenRecordSize;
		m_missSize = GetMissCount() * m_missRecordSize;
		m_hitGroupSize = m_hitgroupCount * m_hitgroupRecordSize;

		//�e�e�[�u���̊J�n�ʒu�ɃA���C�����g���񂪂���̂Œ�������B
		m_tableAlign = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
		m_raygenRegion = RoundUp(m_raygenRecordSize, m_tableAlign);
		m_missRegion = RoundUp(m_missSize, m_tableAlign);
		m_hitgroupRegion = RoundUp(m_hitGroupSize, m_tableAlign);

		//�������ꂽBLAS�̐��B
		const int BLAS_COUNT = arg_blacVector.GetBlasRefCount();

		//�V�F�[�_�[�e�[�u���̃T�C�Y�B
		m_tableSize = m_raygenRegion + m_missRegion + m_hitgroupRegion * BLAS_COUNT;

		/*========== �V�F�[�_�[�e�[�u���̍\�z ==========*/

		//�V�F�[�_�[�e�[�u���m�ہB
		m_shaderTable = CreateBuffer(
			m_tableSize, D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD,
			L"ShaderTable0");
		m_shaderTalbeMapAddress = nullptr;
		HRESULT result = m_shaderTable->Map(0, nullptr, &m_shaderTalbeMapAddress);

		result = m_stateObject.As(&m_rtsoProps);

		//�V�F�[�_�[�e�[�u�����������݁A���C��ݒ肷��B
		WriteShadetTalbeAndSettingRay(arg_blacVector, arg_dispatchX, arg_dispatchY);

	}

	void RayPipeline::MapHitGroupInfo(BlasVector arg_blacVector)
	{

		/*===== HitGroup�̏���]�� =====*/

		uint8_t* pStart = static_cast<uint8_t*>(m_shaderTalbeMapAddress);

		//Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
		uint8_t* hitgroupStart = pStart + m_raygenRegion + m_missRegion;
		{

			uint8_t* pRecord = hitgroupStart;

			pRecord = arg_blacVector.WriteShaderRecord(pRecord, m_hitgroupRecordSize, m_stateObject, m_hitGroupName);

		}

	}

	void RayPipeline::TraceRay(Tlas arg_tlas)
	{

		/*===== ���C�g���[�V���O�����s =====*/

		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Raytracing");

		//���C�g���Ŏg�p���郊�\�[�X�ނ��Z�b�g�B
		SetRaytracingResource(arg_tlas);

		//���C�g���[�V���O�����s�B
		DirectX12CmdList::Instance()->cmdList->DispatchRays(&m_dispatchRayDesc);

		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

		/*===== �������񂾃A�E�g���C�������� =====*/

		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Outline");

		//�A�E�g���C�����v�Z
		GBufferMgr::Instance()->m_outline->Apply();

		DispatchData dispatchData;
		dispatchData.x = static_cast<UINT>(1280 / 16) + 1;
		dispatchData.y = static_cast<UINT>(720 / 16) + 1;
		dispatchData.z = static_cast<UINT>(1);
		m_outlineComposeShader.Compute(dispatchData);

		dispatchData.x = static_cast<UINT>(1280 / 16) + 1;
		dispatchData.y = static_cast<UINT>(720 / 16) + 1;
		dispatchData.z = static_cast<UINT>(1);
		m_outlineCleanShader.Compute(dispatchData);

		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

		/*===== �R�s�[�R�}���h��ς� =====*/

		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Lensflare");

		//UAV�̃o���A��\��B
		UAVBarrier({ GBufferMgr::Instance()->GetLensFlareBuffer() , GBufferMgr::Instance()->GetRayTracingBuffer() });

		////�����Y�t���A�p�̃e�N�X�`���ɃK�E�V�A���u���[�������ăt���A��\������B
		//GBufferMgr::Instance()->ApplyLensFlareBlur();

		////�����Y�t���A��������B
		//m_lensFlare->Apply();

		//�V�[�����Ƀ����Y�t���A����������B
		GBufferMgr::Instance()->ComposeLensFlareAndScene();

		//UAV�̃o���A��\��B
		UAVBarrier({ GBufferMgr::Instance()->GetLensFlareBuffer() , GBufferMgr::Instance()->GetRayTracingBuffer() });

		//�o�b�N�o�b�t�@�̏�Ԃ�J�ځB
		auto backBufferIndex = m_refDirectX12->swapchain->GetCurrentBackBufferIndex();
		BufferStatesTransition(m_refDirectX12->GetBackBuffer()[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);

		//�o�b�N�o�b�t�@�ꎞ�ۑ��p�e�N�X�`���̃X�e�[�^�X���R�s�[�p�ɕύX�B
		BufferStatesTransition(GBufferMgr::Instance()->GetBackBufferCopyBuffer().bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		//�R�s�[�����s�B
		DirectX12CmdList::Instance()->cmdList->CopyResource(GBufferMgr::Instance()->GetBackBufferCopyBuffer().bufferWrapper->GetBuffer().Get(), m_refDirectX12->GetBackBuffer()[backBufferIndex].Get());

		//�o�b�N�o�b�t�@�ꎞ�ۑ��p�e�N�X�`���̃X�e�[�^�X�����ɖ߂��B
		BufferStatesTransition(GBufferMgr::Instance()->GetBackBufferCopyBuffer().bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		//�o�b�N�o�b�t�@�̏�Ԃ����ɖ߂��B
		BufferStatesTransition(m_refDirectX12->GetBackBuffer()[backBufferIndex].Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

		/*===== �o�b�N�o�b�t�@���������� =====*/

		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "ComposeBackBuffer");

		//���������s�B
		GBufferMgr::Instance()->ComposeBackBuffer();

		//�o�b�N�o�b�t�@�ꎞ�ۑ��p�e�N�X�`���̃X�e�[�^�X���R�s�[�p�ɕύX�B
		BufferStatesTransition(GBufferMgr::Instance()->GetBackBufferCompositeBuffer().bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		//�R�s�[�����s�B
		DirectX12CmdList::Instance()->cmdList->CopyResource(m_refDirectX12->GetBackBuffer()[backBufferIndex].Get(), GBufferMgr::Instance()->GetBackBufferCompositeBuffer().bufferWrapper->GetBuffer().Get());

		//�o�b�N�o�b�t�@�ꎞ�ۑ��p�e�N�X�`���̃X�e�[�^�X�����ɖ߂��B
		BufferStatesTransition(GBufferMgr::Instance()->GetBackBufferCompositeBuffer().bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		//�o�b�N�o�b�t�@�̏�Ԃ����ɖ߂��B
		BufferStatesTransition(m_refDirectX12->GetBackBuffer()[backBufferIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);

		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

	}

	UINT RayPipeline::GetLargestDataSizeInHitGroup()
	{

		/*===== �S�Ẵq�b�g�O���[�v�̒��ōł��f�[�^�T�C�Y���傫�����̂��擾���� =====*/

		UINT largestDataSize = 0;

		const int HIT_GROUP_COUNT = HitGroupMgr::Instance()->GetHitGroupCount();
		for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

			//�f�[�^�T�C�Y���擾�B
			UINT dataSize = 0;
			dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Instance()->GetSRVCount(static_cast<HitGroupMgr::HITGROUP_ID>(index));
			dataSize += sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * HitGroupMgr::Instance()->GetCBVCount(static_cast<HitGroupMgr::HITGROUP_ID>(index));
			dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Instance()->GetUAVCount(static_cast<HitGroupMgr::HITGROUP_ID>(index));

			//�擾�����f�[�^�T�C�Y���ۑ�����Ă���f�[�^�T�C�Y��菬���������珈�����΂��B
			if (dataSize < largestDataSize) continue;

			//�擾�����f�[�^�T�C�Y��ۑ�����B
			largestDataSize = dataSize;

		}

		return largestDataSize;

	}

	void RayPipeline::WriteShadetTalbeAndSettingRay(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY)
	{

		/*===== �V�F�[�_�[�e�[�u�����������݁A���C��ݒ肷�� =====*/

		//�e�V�F�[�_�[���R�[�h����������ł����B
		uint8_t* pStart = static_cast<uint8_t*>(m_shaderTalbeMapAddress);

		//RayGeneration �p�̃V�F�[�_�[���R�[�h���������݁B
		uint8_t* rgsStart = pStart;
		{
			uint8_t* p = rgsStart;

			const int SHADER_DATA_COUNT = static_cast<int>(m_shaderData.size());
			for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

				const int RG_COUNT = static_cast<int>(m_shaderData[index].m_rayGenEntryPoint.size());
				for (int rgIndex = 0; rgIndex < RG_COUNT; ++rgIndex) {

					void* id = m_rtsoProps->GetShaderIdentifier(m_shaderData[index].m_rayGenEntryPoint[rgIndex]);
					p += WriteShaderIdentifier(p, id);

				}

			}

		}

		//Miss Shader �p�̃V�F�[�_�[���R�[�h���������݁B
		uint8_t* missStart = pStart + m_raygenRegion;
		{
			uint8_t* p = missStart;

			const int SHADER_DATA_COUNT = static_cast<int>(m_shaderData.size());
			for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

				const int MS_COUNT = static_cast<int>(m_shaderData[index].m_missEntryPoint.size());
				for (int msIndex = 0; msIndex < MS_COUNT; ++msIndex) {

					void* id = m_rtsoProps->GetShaderIdentifier(m_shaderData[index].m_missEntryPoint[msIndex]);
					p += WriteShaderIdentifier(p, id);

				}

			}

		}

		//Hit Group �p�̃V�F�[�_�[���R�[�h���������݁B
		uint8_t* hitgroupStart = pStart + m_raygenRegion + m_missRegion;
		{

			uint8_t* pRecord = hitgroupStart;

			pRecord = arg_blacVector.WriteShaderRecord(pRecord, m_hitgroupRecordSize, m_stateObject, m_hitGroupName);

		}

		//���C���ˎ��̐ݒ��ݒ�B

		//DispatchRays �̂��߂ɏ����Z�b�g���Ă���.
		auto startAddress = m_shaderTable->GetGPUVirtualAddress();
		//RayGeneration�V�F�[�_�[�̏��
		auto& shaderRecordRG = m_dispatchRayDesc.RayGenerationShaderRecord;
		shaderRecordRG.StartAddress = startAddress;
		shaderRecordRG.SizeInBytes = m_raygenSize;
		startAddress += m_raygenRegion;
		//Miss�V�F�[�_�[�̏��
		auto& shaderRecordMS = m_dispatchRayDesc.MissShaderTable;
		shaderRecordMS.StartAddress = startAddress;
		shaderRecordMS.SizeInBytes = m_missSize;
		shaderRecordMS.StrideInBytes = m_missRecordSize;
		startAddress += m_missRegion;
		//HitGroup�̏��
		auto& shaderRecordHG = m_dispatchRayDesc.HitGroupTable;
		shaderRecordHG.StartAddress = startAddress;
		shaderRecordHG.SizeInBytes = m_hitGroupSize;
		shaderRecordHG.StrideInBytes = m_hitgroupRecordSize;
		startAddress += m_hitgroupRegion;
		//���C�̏��
		m_dispatchRayDesc.Width = arg_dispatchX;
		m_dispatchRayDesc.Height = arg_dispatchY;
		m_dispatchRayDesc.Depth = 1;

	}

	UINT RayPipeline::WriteShaderIdentifier(void* arg_dst, const void* arg_shaderId)
	{
		memcpy(arg_dst, arg_shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	}

	int RayPipeline::GetRayGenerationCount()
	{
		int count = 0;

		const int SHADER_DATA_COUNT = static_cast<int>(m_shaderData.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			count += static_cast<int>(m_shaderData[index].m_rayGenEntryPoint.size());

		}

		return count;

	}

	int RayPipeline::GetMissCount()
	{
		int count = 0;

		const int SHADER_DATA_COUNT = static_cast<int>(m_shaderData.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			count += static_cast<int>(m_shaderData[index].m_missEntryPoint.size());

		}

		return count;

	}

	Microsoft::WRL::ComPtr<ID3D12Resource> RayPipeline::CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flag, D3D12_RESOURCE_STATES arg_initialState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_name)
	{

		/*===== �o�b�t�@�𐶐� =====*/

		D3D12_HEAP_PROPERTIES heapProps{};
		if (arg_heapType == D3D12_HEAP_TYPE_DEFAULT) {
			heapProps = D3D12_HEAP_PROPERTIES{
			D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
			};
		}
		if (arg_heapType == D3D12_HEAP_TYPE_UPLOAD) {
			heapProps = D3D12_HEAP_PROPERTIES{
			D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
			};
		}
		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = arg_size;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc = { 1, 0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = arg_flag;

		hr = DirectX12Device::Instance()->dev->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			arg_initialState,
			nullptr,
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
		);
		if (FAILED(hr)) {
			OutputDebugStringA("CreateBuffer failed.\n");
		}
		if (resource != nullptr && arg_name != nullptr) {
			resource->SetName(arg_name);
		}
		return resource;

	}

	void RayPipeline::UAVBarrier(const std::vector<KazBufferHelper::BufferData> &arg_bufferArray)
	{
		std::vector<D3D12_RESOURCE_BARRIER> barrier;

		for (const auto& index : arg_bufferArray) {
			barrier.emplace_back(CD3DX12_RESOURCE_BARRIER::UAV(index.bufferWrapper->GetBuffer().Get()));
		}

		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(static_cast<UINT>(barrier.size()), barrier.data());
	}

	void RayPipeline::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
	{
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			arg_resource,
			arg_before,
			arg_after),
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
	}

	void RayPipeline::SetRaytracingResource(Tlas arg_tlas)
	{

		//�O���[�o�����[�g�V�O�l�`���Ŏg���Ɛ錾���Ă��郊�\�[�X����Z�b�g�B
		DescriptorHeapMgr::Instance()->SetDescriptorHeap();
		DirectX12CmdList::Instance()->cmdList->SetComputeRootSignature(GetGlobalRootSig()->GetRootSig().Get());

		//TLAS��ݒ�B
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(arg_tlas.GetDescHeapHandle()));

		//GBuffer���\�z�B
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, GBufferMgr::Instance()->GetGPUHandle(GBufferMgr::ALBEDO));	//�A���x�h
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(2, GBufferMgr::Instance()->GetGPUHandle(GBufferMgr::NORMAL));	//�@��
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(3, GBufferMgr::Instance()->GetGPUHandle(GBufferMgr::R_M_S_ID));//�}�e���A��
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(4, GBufferMgr::Instance()->GetGPUHandle(GBufferMgr::WORLD));	//���[���h���W
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(5, GBufferMgr::Instance()->GetGPUHandle(GBufferMgr::EMISSIVE));	//���[���h���W

		//�J�����p�萔�o�b�t�@���Z�b�g�B
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(6, GBufferMgr::Instance()->GetEyePosBuffer().bufferWrapper->GetGpuAddress());
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(7, GBufferMgr::Instance()->m_lightBuffer.bufferWrapper->GetGpuAddress());

		//�������ݗpUAV
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(8, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(GBufferMgr::Instance()->GetRayTracingBuffer().bufferWrapper->GetViewHandle()));	//���C�g���o�͗p
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(9, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(m_refVolumeNoiseTexture->bufferWrapper->GetViewHandle()));	//�{�����[���t�H�O�p�e�N�X�`��
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(10, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(GBufferMgr::Instance()->GetLensFlareBuffer().bufferWrapper->GetViewHandle()));	//�����Y�t���A�p�e�N�X�`��
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(11, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(GBufferMgr::Instance()->GetEmissiveGBuffer().bufferWrapper->GetViewHandle()));	//�u���[���p�e�N�X�`��

		//�p�C�v���C����ݒ�B
		DirectX12CmdList::Instance()->cmdList->SetPipelineState1(m_stateObject.Get());

	}

}