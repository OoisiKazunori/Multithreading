#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../DirectXCommon/DirectX12Device.h"

struct CmdListHelper
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;

	CmdListHelper()
	{
		DirectX12Device::Instance()->dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
		DirectX12Device::Instance()->dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
		cmdList->SetName(L"assetLoad_cmdList");
		cmdAllocator->SetName(L"assetLoad_allocator");
	}
};

/// <summary>
/// コマンドリストの生成し、管理します
/// </summary>
class DirectX12CmdList :public ISingleton<DirectX12CmdList>
{
public:
	DirectX12CmdList();
	~DirectX12CmdList();

	/// <summary>
	/// コマンドリストの生成
	/// </summary>
	/// <param name="cmdAllocator">コマンドアロケーター</param>
	void CreateCmdList(ID3D12CommandAllocator* cmdAllocator);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> cmdList;
	CmdListHelper m_loadAssertThreadCmdList;
private:
	HRESULT result;
	CmdListHelper m_loadTextureThreadCmdList;
	friend ISingleton<DirectX12CmdList>;
};

