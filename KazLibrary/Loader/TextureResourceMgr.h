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
#include"../KazLibrary/Loader/LoadTexture.h"

/// <summary>
/// 画像の分割をする際にどの座標からどれくらいのサイズでUV切り取りをするか記録したもの
/// </summary>
struct DivGraphData
{
	std::vector<KazMath::Vec2<int>> divLeftUp;
	KazMath::Vec2<int> divSize;
	RESOURCE_HANDLE handle;
};

/// <summary>
/// 画像の読み込み、画像の管理
/// </summary>
class TextureResourceMgr :public ISingleton<TextureResourceMgr> {
public:

	TextureResourceMgr();
	~TextureResourceMgr();

	/// <summary>
	/// ファイルパスを書いて画像の読み込みを行います
	/// </summary>
	/// <param name="RESOURCE">読み込みたい画像のファイルパス</param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE LoadGraph(std::string RESOURCE);
	const std::shared_ptr<KazBufferHelper::BufferData>& LoadGraphBuffer(std::string RESOURCE);
	const std::shared_ptr<KazBufferHelper::BufferData>& LoadGraphBufferForModel(std::string RESOURCE);
	const std::shared_ptr<KazBufferHelper::BufferData>& StaticLoadGraphBuffer(std::string RESOURCE);
	void LoadAllGraphBuffer();
	void Release();
	size_t GetLoadCountNum()
	{
		return handleName.size();
	}

	/// <summary>
	///  ファイルパスを書いて画像を読み込み、分割します
	/// </summary>
	/// <param name="RESOURCE">読み込みたい画像のファイルパス</param>
	/// <param name="DIV_WIDTH_NUM">X軸に画像を分割する数</param>
	/// <param name="DIV_HEIGHT_NUM">Y軸に画像を分割する数</param>
	/// <param name="TEXSIZE_WIDTH">分割する画像のXサイズ</param>
	/// <param name="TEXSIZE_HEIGHT">分割する画像のYサイズ</param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE LoadDivGraph(string RESOURCE,
		int DIV_WIDTH_NUM,
		int DIV_HEIGHT_NUM,
		int TEXSIZE_WIDTH,
		int TEXSIZE_HEIGHT
	);

	/// <summary>
	/// 読み込んだ画像を削除します
	/// </summary>
	/// <param name="HANDLE">削除したい画像のハンドル</param>
	void Release(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// ハンドルからシェーダーリソースをコマンドリストに積ませます
	/// </summary>
	/// <param name="GRAPH_HANDLE">ハンドル</param>
	/// <param name="PARAM">ルートパラム</param>
	/// <param name="TYPE">ルートパラムの種類</param>
	void SetSRV(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE);
	void SetSRView(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE);


	/// <summary>
	/// 画像の情報を渡します
	/// バッファの取得に失敗した場合 WidthとHeightに0が返されます
	/// </summary>
	/// <param name="HANDLE"></param>
	/// <returns></returns>
	D3D12_RESOURCE_DESC GetTextureSize(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// 分割する画像の情報を渡します
	/// バッファの取得に失敗した場合 texSizeのXとYに-1が入ります
	/// </summary>
	/// <param name="HANDLE">ハンドル</param>
	/// <returns>分割する画像の情報</returns>
	DivGraphData GetDivData(RESOURCE_HANDLE HANDLE);

private:
	//std::unique_ptr<CreateGpuBuffer> buffers;
	const DirectX::Image* img;

	D3D12_STATIC_SAMPLER_DESC samplerDesc;

	static const int texWidth = 256;
	static const int texDataCount = texWidth * texWidth;
	const int DescriptorMaxNum = 5000;

	DirectX::ScratchImage scratchImg;
	UINT setHandle;
	UINT IncreSize;
	DirectX::TexMetadata metadata;

	HRESULT result;

	std::vector<std::string> handleName;

	vector<DivGraphData> divData;

	//通常のテクスチャ読み込み
	HandleMaker handle;
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>>bufferArray;
	std::vector<KazBufferHelper::BufferData>cpuBufferArray;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>mipmapUploadBufferArray;

	//モデル用のテクスチャ読み込み
	LoadTexture m_loadModelTexture;

	//static用の処理
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>>staticBufferArray;
	std::vector<KazBufferHelper::BufferData>staticCpuBufferArray;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>staticMipmapUploadBufferArray;
	int staticHandle;
	int loadTextureHandle;

	struct RegisterData
	{
		std::string m_handleName;
		RESOURCE_HANDLE m_handle;
		RegisterData(std::string arg_name, RESOURCE_HANDLE arg_handle) :m_handleName(arg_name), m_handle(arg_handle)
		{};
	};
	std::vector<RegisterData>m_registerDataArray;

	friend ISingleton<TextureResourceMgr>;

	//DDS用読み込み処理
	void LoadDDSFile(CD3DX12_RESOURCE_DESC arg_textureDesc, int arg_elementNum);

	//バッファのステータスを変更
	void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

};

