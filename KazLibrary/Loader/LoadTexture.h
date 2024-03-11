#pragma once
#include<DirectXTex.h>
#include<vector>
#include<memory>
#include<string>
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"

class LoadTexture
{
public:

	const std::shared_ptr<KazBufferHelper::BufferData>& LoadGraphBuffer(std::string RESOURCE,RESOURCE_HANDLE DESCRIPTOR_HANDLE);
	void Release();
	size_t GetLoadCountNum()
	{
		return handleName.size();
	}


private:
	HandleMaker handle;
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>>bufferArray;
	std::vector<KazBufferHelper::BufferData>cpuBufferArray;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>mipmapUploadBufferArray;
	std::vector<std::string> handleName;

	DirectX::ScratchImage scratchImg;
	DirectX::TexMetadata metadata;
	const DirectX::Image* img;

	//DDS用読み込み処理
	void LoadDDSFile(CD3DX12_RESOURCE_DESC arg_textureDesc, int arg_elementNum);

	//バッファのステータスを変更
	void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

};
