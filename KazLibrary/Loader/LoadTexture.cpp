#include "LoadTexture.h"
#include"../Loader/TextureResourceMgr.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Helper/KazHelper.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Helper/KazRenderHelper.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Singlton/ProgressBar.h"
#include<DirectXTex.h>
#include<cassert>
#include<algorithm>

const int texWidth = 256;
const int texDataCount = texWidth * texWidth;

const std::shared_ptr<KazBufferHelper::BufferData>& LoadTexture::LoadGraphBuffer(std::string RESOURCE, RESOURCE_HANDLE DESCRIPTOR_HANDLE)
{
	for (int i = 0; i < handleName.size(); i++)
	{
		if (handleName[i] == RESOURCE)
		{
			return bufferArray[i];
		}
	}

	wchar_t wfilepat[128];
	wchar_t wfilepat2[128];
	MultiByteToWideChar(CP_ACP, 0, RESOURCE.c_str(), -1, wfilepat, _countof(wfilepat));


	KazHelper::ConvertStringToWchar_t(RESOURCE, wfilepat2, 128);

	bool isDDSFile = true;
	HRESULT re = LoadFromDDSFile(wfilepat, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(re)) {
		re = LoadFromWICFile(
			wfilepat,
			DirectX::WIC_FLAGS_NONE,
			&metadata,
			scratchImg
		);
		isDDSFile = false;
	}

	img = scratchImg.GetImage(0, 0, 0);


	if (CheckResult(re, RESOURCE + "のファイル読み込みに成功しました\n", RESOURCE + "のファイル読み込みに失敗しました\n") != S_OK)
	{
		//return KazBufferHelper::BufferData();
	}


	handleName.push_back(RESOURCE);

	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);


	RESOURCE_HANDLE elementNum = handle.GetHandle();
	bufferArray.emplace_back(std::make_shared<KazBufferHelper::BufferData>());
	cpuBufferArray.emplace_back();
	mipmapUploadBufferArray.emplace_back();

	cpuBufferArray[elementNum] = KazBufferHelper::SetShaderResourceBufferData(textureDesc, RESOURCE + "-RAM");

	*bufferArray[elementNum] =
		KazBufferHelper::BufferResourceData
		(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			textureDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			RESOURCE + "-VRAM"
		);

	DescriptorHeapMgr::Instance()->CreateBufferView(DESCRIPTOR_HANDLE, srvDesc, bufferArray[elementNum]->bufferWrapper->GetBuffer().Get());

	if (isDDSFile) {

		LoadDDSFile(textureDesc, elementNum);

	}
	else {
		cpuBufferArray[elementNum].bufferWrapper->GetBuffer()->WriteToSubresource
		(
			0,
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch
		);


		bufferArray[elementNum]->bufferWrapper->CopyBuffer(
			cpuBufferArray[elementNum].bufferWrapper,
			DirectX12CmdList::Instance()->m_loadAssertThreadCmdList.cmdList
		);
	}


	bufferArray[elementNum]->rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	bufferArray[elementNum]->bufferWrapper->CreateViewHandle(std::vector<RESOURCE_HANDLE>({ DESCRIPTOR_HANDLE }));

	return bufferArray[elementNum];
}

void LoadTexture::Release()
{
	bufferArray.clear();
	cpuBufferArray.clear();
	handleName.clear();
	handle.DeleteAllHandle();
}

void LoadTexture::LoadDDSFile(CD3DX12_RESOURCE_DESC arg_textureDesc, int arg_elementNum)

{
	//MipMapを取得。
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	HRESULT re = DirectX::PrepareUpload(
		DirectX12Device::Instance()->dev.Get(), img, scratchImg.GetImageCount(), metadata, subresources);

	//Footprint(コピー可能なリソースのレイアウト)を取得。
	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 16> footprint;
	UINT64 totalBytes = 0;
	std::array<UINT64, 16> rowSizeInBytes = { 0 };
	std::array<UINT, 16> numRow = { 0 };
	DirectX12Device::Instance()->dev->GetCopyableFootprints(&arg_textureDesc, 0, static_cast<UINT>(metadata.mipLevels), 0, footprint.data(), numRow.data(), rowSizeInBytes.data(), &totalBytes);

	//Upload用のバッファを作成する。
	D3D12_RESOURCE_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = totalBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	D3D12_HEAP_PROPERTIES heap = D3D12_HEAP_PROPERTIES();
	heap.Type = D3D12_HEAP_TYPE_UPLOAD;

	DirectX12Device::Instance()->dev->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&mipmapUploadBufferArray[arg_elementNum]));
	mipmapUploadBufferArray[arg_elementNum]->SetName(L"MipMapUploadBuffer");

	//UploadBufferへの書き込み。
	void* ptr = nullptr;
	mipmapUploadBufferArray[arg_elementNum]->Map(0, nullptr, &ptr);

	for (uint32_t mip = 0; mip < metadata.mipLevels; ++mip) {

		assert(subresources[mip].RowPitch == static_cast<LONG_PTR>(rowSizeInBytes[mip]));
		assert(subresources[mip].RowPitch <= footprint[mip].Footprint.RowPitch);

		//1ピクセルのサイズ
		size_t pixelSize = rowSizeInBytes[mip] / metadata.width;

		uint8_t* uploadStart = reinterpret_cast<uint8_t*>(ptr) + footprint[mip].Offset;

		for (uint32_t height = 0; height < numRow[mip]; ++height) {

			memcpy(uploadStart + height * footprint[mip].Footprint.RowPitch, reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(subresources[mip].pData) + height * subresources[mip].RowPitch), subresources[mip].RowPitch);

		}

	}

	BufferStatesTransition(cpuBufferArray[arg_elementNum].bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
	BufferStatesTransition(mipmapUploadBufferArray[arg_elementNum].Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//コピーする。
	for (uint32_t mip = 0; mip < metadata.mipLevels; ++mip) {

		D3D12_TEXTURE_COPY_LOCATION copyDestLocation;
		copyDestLocation.pResource = cpuBufferArray[arg_elementNum].bufferWrapper->GetBuffer().Get();
		copyDestLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		copyDestLocation.SubresourceIndex = mip;

		D3D12_TEXTURE_COPY_LOCATION copySrcLocation;
		copySrcLocation.pResource = mipmapUploadBufferArray[arg_elementNum].Get();
		copySrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		copySrcLocation.PlacedFootprint = footprint[mip];

		DirectX12CmdList::Instance()->cmdList->CopyTextureRegion(
			&copyDestLocation,
			0, 0, 0,
			&copySrcLocation,
			nullptr
		);

	}

	//バッファの状態を遷移
	BufferStatesTransition(cpuBufferArray[arg_elementNum].bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);
	BufferStatesTransition(bufferArray[arg_elementNum]->bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

	//コピー
	DirectX12CmdList::Instance()->cmdList->CopyResource(bufferArray[arg_elementNum]->bufferWrapper->GetBuffer().Get(), cpuBufferArray[arg_elementNum].bufferWrapper->GetBuffer().Get());

	BufferStatesTransition(cpuBufferArray[arg_elementNum].bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ);
	BufferStatesTransition(bufferArray[arg_elementNum]->bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
}

void LoadTexture::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
{
	D3D12_RESOURCE_BARRIER barriers[] = {
	CD3DX12_RESOURCE_BARRIER::Transition(
	arg_resource,
	arg_before,
	arg_after),
	};
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
}