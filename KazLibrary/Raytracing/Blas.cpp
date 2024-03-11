#include "Blas.h"
#include "../Loader/FbxModelResourceMgr.h"
#include "../Render/RenderData.h"
#include "../DirectXCommon/DirectX12Device.h"
#include "../DirectXCommon/DirectX12CmdList.h"
//#include "../Raytracing/HitGroupMgr.h"
#include "../Buffer/DescriptorHeapMgr.h"
#include "../Buffer/VertexBufferMgr.h"
#include <memory>

Raytracing::Blas::Blas(bool IsOpaque, RESOURCE_HANDLE arg_vertexDataHandle, int arg_meshNumber, RESOURCE_HANDLE arg_textureHandle, int arg_hitGroupIndex)
{

	/*===== �R���X�g���N�^ =====*/

	//Blas�̍\�z�ɕK�v�Ȍ`��f�[�^���擾�B
	m_vertexDataHandle = arg_vertexDataHandle;
	m_meshNumber = arg_meshNumber;
	m_textureHandle = arg_textureHandle;
	m_hitGroupIndex = arg_hitGroupIndex;
	m_geomDesc = GetGeometryDesc(IsOpaque);

	//Blas���\�z�B
	BuildBlas(m_geomDesc);

}

void Raytracing::Blas::Update()
{

	//�X�V�̂��߂̒l��ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &m_geomDesc;
	//BLAS �̍X�V�������s�����߂̃t���O��ݒ肷��B
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	//�C���v���[�X�X�V�����s����B
	asDesc.SourceAccelerationStructureData = m_blasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = m_blasBuffer->GetGPUVirtualAddress();
	//�X�V�p�̍�ƃo�b�t�@��ݒ肷��B
	asDesc.ScratchAccelerationStructureData = m_updateBuffer->GetGPUVirtualAddress();

	//�R�}���h���X�g�ɐςށB
	DirectX12CmdList::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	//���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = m_blasBuffer.Get();
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);

}

uint8_t* Raytracing::Blas::WriteShaderRecord(uint8_t* arg_dst, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stageObject, LPCWSTR arg_hitGroupName)
{

	/*===== �V�F�[�_�[���R�[�h���������� =====*/

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
	arg_stageObject.As(&rtsoProps_);
	auto entryBegin = arg_dst;

	auto mode_ = rtsoProps_->GetShaderIdentifier(arg_hitGroupName);
	if (mode_ == nullptr) {
		throw std::logic_error("Not found ShaderIdentifier");
	}

	//�V�F�[�_�[���ʎq���������ށB
	memcpy(arg_dst, mode_, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	arg_dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	//���_�p�f�[�^
	auto vertexData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_vertexDataHandle);

	//����̃v���O�����ł͈ȉ��̏����Ńf�B�X�N���v�^���L�^�B
	//[0] : �C���f�b�N�X�o�b�t�@
	//[1] : ���_�o�b�t�@
	//�� ���[�J�����[�g�V�O�l�`���̏����ɍ��킹��K�v������B
	//�e�N�X�`�����������ށB
	arg_dst += WriteGPUDescriptor(arg_dst, &DescriptorHeapMgr::Instance()->GetGpuDescriptorView(m_textureHandle));
	arg_dst += WriteGPUDescriptor(arg_dst, &DescriptorHeapMgr::Instance()->GetGpuDescriptorView(vertexData.indexBuffer[m_meshNumber]->bufferWrapper->GetViewHandle()));
	arg_dst += WriteGPUDescriptor(arg_dst, &DescriptorHeapMgr::Instance()->GetGpuDescriptorView(vertexData.vertBuffer[m_meshNumber]->bufferWrapper->GetViewHandle()));

	arg_dst = entryBegin + arg_recordSize;
	return arg_dst;

}

D3D12_RAYTRACING_GEOMETRY_DESC Raytracing::Blas::GetGeometryDesc(bool arg_isOpaque)
{

	/*===== Blas�̌`��ݒ�ɕK�v�ȍ\���̂�Ԃ� =====*/

	//���_�p�f�[�^
	auto vertexData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_vertexDataHandle);

	//�`��f�[�^�̃t���O��ݒ�B
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	//�s�����t���O��ݒ�B GPU����AnyHitShader���ĂԂ��ǂ����������Ō���B
	if (arg_isOpaque) {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	}
	else {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
	}

	//�`��f�[�^�ׂ̍������ڂ�ݒ�B
	auto& triangles = geometryDesc.Triangles;
	triangles.VertexBuffer.StartAddress = vertexData.vertBuffer[m_meshNumber]->bufferWrapper->GetGpuAddress();
	triangles.VertexBuffer.StrideInBytes = vertexData.vertBuffer[m_meshNumber]->structureSize;
	triangles.VertexCount = vertexData.vertBuffer[m_meshNumber]->elementNum;
	triangles.IndexBuffer = vertexData.indexBuffer[m_meshNumber]->bufferWrapper->GetGpuAddress();
	triangles.IndexCount = vertexData.indexBuffer[m_meshNumber]->elementNum;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void Raytracing::Blas::BuildBlas(const D3D12_RAYTRACING_GEOMETRY_DESC& arg_geomDesc)
{

	/*===== Blas���\�z =====*/

	//AS(���C�Ƃ̔����v���ɍs�����߂ɕK�v�ȃ|���S���f�[�^�\����)�̐ݒ�ɕK�v�Ȋe���ڂ�ݒ�B
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	//D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &arg_geomDesc;

	//�֐����g���ĕK�v�ȃ������ʂ����߂�.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectX12Device::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	//�X�N���b�`�o�b�t�@�𐶐�����B
	m_scratchBuffer = CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(m_scratchBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, barrierToUAV);
	m_scratchBuffer->SetName(L"BlasScratchBuffer");

	//Blas�̃o�b�t�@�𐶐�����B
	m_blasBuffer = CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	m_blasBuffer->SetName(L"BlasBuffer");

	//�X�V�p�o�b�t�@�𐶐�����B
	m_updateBuffer = CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	barrierToUAV[0] = { CD3DX12_RESOURCE_BARRIER::Transition(m_updateBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, barrierToUAV);
	m_updateBuffer->SetName(L"BlasUpdateBuffer");

	//Blas���\�z����B
	buildASDesc.ScratchAccelerationStructureData = m_scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = m_blasBuffer->GetGPUVirtualAddress();

	//�R�}���h���X�g�ɐς�Ŏ��s����B
	DirectX12CmdList::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	//���\�[�X�o���A�̐ݒ�B
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = m_blasBuffer.Get();
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);

}

Microsoft::WRL::ComPtr<ID3D12Resource> Raytracing::Blas::CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_bufferName)
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
	resDesc.Flags = arg_flags;

	hr = DirectX12Device::Instance()->dev->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		arg_initState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);
	if (FAILED(hr)) {
		OutputDebugStringA("CreateBuffer failed.\n");
	}
	if (resource != nullptr && arg_bufferName != nullptr) {
		resource->SetName(arg_bufferName);
	}
	return resource;

}

int Raytracing::Blas::GetHitGroupIndex()
{
	return m_hitGroupIndex;
}
