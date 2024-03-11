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
/// �摜�̕���������ۂɂǂ̍��W����ǂꂭ�炢�̃T�C�Y��UV�؂�������邩�L�^��������
/// </summary>
struct DivGraphData
{
	std::vector<KazMath::Vec2<int>> divLeftUp;
	KazMath::Vec2<int> divSize;
	RESOURCE_HANDLE handle;
};

/// <summary>
/// �摜�̓ǂݍ��݁A�摜�̊Ǘ�
/// </summary>
class TextureResourceMgr :public ISingleton<TextureResourceMgr> {
public:

	TextureResourceMgr();
	~TextureResourceMgr();

	/// <summary>
	/// �t�@�C���p�X�������ĉ摜�̓ǂݍ��݂��s���܂�
	/// </summary>
	/// <param name="RESOURCE">�ǂݍ��݂����摜�̃t�@�C���p�X</param>
	/// <returns>�n���h��</returns>
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
	///  �t�@�C���p�X�������ĉ摜��ǂݍ��݁A�������܂�
	/// </summary>
	/// <param name="RESOURCE">�ǂݍ��݂����摜�̃t�@�C���p�X</param>
	/// <param name="DIV_WIDTH_NUM">X���ɉ摜�𕪊����鐔</param>
	/// <param name="DIV_HEIGHT_NUM">Y���ɉ摜�𕪊����鐔</param>
	/// <param name="TEXSIZE_WIDTH">��������摜��X�T�C�Y</param>
	/// <param name="TEXSIZE_HEIGHT">��������摜��Y�T�C�Y</param>
	/// <returns>�n���h��</returns>
	RESOURCE_HANDLE LoadDivGraph(string RESOURCE,
		int DIV_WIDTH_NUM,
		int DIV_HEIGHT_NUM,
		int TEXSIZE_WIDTH,
		int TEXSIZE_HEIGHT
	);

	/// <summary>
	/// �ǂݍ��񂾉摜���폜���܂�
	/// </summary>
	/// <param name="HANDLE">�폜�������摜�̃n���h��</param>
	void Release(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// �n���h������V�F�[�_�[���\�[�X���R�}���h���X�g�ɐς܂��܂�
	/// </summary>
	/// <param name="GRAPH_HANDLE">�n���h��</param>
	/// <param name="PARAM">���[�g�p����</param>
	/// <param name="TYPE">���[�g�p�����̎��</param>
	void SetSRV(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE);
	void SetSRView(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE);


	/// <summary>
	/// �摜�̏���n���܂�
	/// �o�b�t�@�̎擾�Ɏ��s�����ꍇ Width��Height��0���Ԃ���܂�
	/// </summary>
	/// <param name="HANDLE"></param>
	/// <returns></returns>
	D3D12_RESOURCE_DESC GetTextureSize(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// ��������摜�̏���n���܂�
	/// �o�b�t�@�̎擾�Ɏ��s�����ꍇ texSize��X��Y��-1������܂�
	/// </summary>
	/// <param name="HANDLE">�n���h��</param>
	/// <returns>��������摜�̏��</returns>
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

	//�ʏ�̃e�N�X�`���ǂݍ���
	HandleMaker handle;
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>>bufferArray;
	std::vector<KazBufferHelper::BufferData>cpuBufferArray;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>mipmapUploadBufferArray;

	//���f���p�̃e�N�X�`���ǂݍ���
	LoadTexture m_loadModelTexture;

	//static�p�̏���
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

	//DDS�p�ǂݍ��ݏ���
	void LoadDDSFile(CD3DX12_RESOURCE_DESC arg_textureDesc, int arg_elementNum);

	//�o�b�t�@�̃X�e�[�^�X��ύX
	void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

};

