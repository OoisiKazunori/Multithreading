#pragma once
#include"Polygon.h"
#include"../KazLibrary/Helper/ISinglton.h"

struct IAPolygonData
{
	std::shared_ptr<KazBufferHelper::BufferData> m_buffer;
	IAPolygonData()
	{};
	IAPolygonData(const KazBufferHelper::BufferData& arg_vertexBufferGennerateData) :
		m_buffer(std::make_shared<KazBufferHelper::BufferData>(arg_vertexBufferGennerateData))
	{};

	void Generate(const KazBufferHelper::BufferData& arg_vertexBufferGennerateData)
	{
		m_buffer = std::make_shared<KazBufferHelper::BufferData>(arg_vertexBufferGennerateData);
	};
};

struct PolygonGenerateData
{
	void* m_ptr;
	int m_structureSize;
	size_t m_arraySize;
	std::string m_name;
};

struct IAPolygonBufferData
{
	IAPolygonData m_cpuBuffer, m_gpuBuffer;

	IAPolygonBufferData(const PolygonGenerateData& arg_vertex) :
		m_cpuBuffer(
			KazBufferHelper::SetVertexBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize))
		),
		m_gpuBuffer(
			KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize))
		)
	{
		m_cpuBuffer.m_buffer->bufferWrapper->TransData(arg_vertex.m_ptr, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize));
		m_gpuBuffer.m_buffer->bufferWrapper->CopyBuffer(m_cpuBuffer.m_buffer->bufferWrapper->GetBuffer());
	};
	IAPolygonBufferData()
	{};

	void GenerateBuffer(const PolygonGenerateData& arg_vertex)
	{
		m_gpuBuffer.Generate(KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize), arg_vertex.m_name + "-VRAM"));
		m_cpuBuffer.Generate(KazBufferHelper::SetVertexBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize), arg_vertex.m_name + "-RAM"));

		m_cpuBuffer.m_buffer->bufferWrapper->TransData(arg_vertex.m_ptr, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize));

		m_gpuBuffer.m_buffer->bufferWrapper->CopyBuffer(
			m_cpuBuffer.m_buffer->bufferWrapper,
			DirectX12CmdList::Instance()->m_loadAssertThreadCmdList.cmdList
		);
	};
};

/// <summary>
/// 頂点情報
/// </summary>
struct VertexAndIndexGenerateData
{
	void* verticesPos;
	int structureSize;
	size_t arraySize;
	int oneArraySize;
	std::vector<UINT>indices;

	VertexAndIndexGenerateData(void* VERT_PTR, int STRUCTURE_SIZE, size_t ARRAY_SIZE, int ONE_ARRAY_SIZE, std::vector<UINT>INDICES) :
		verticesPos(VERT_PTR), structureSize(STRUCTURE_SIZE), arraySize(ARRAY_SIZE), oneArraySize(ONE_ARRAY_SIZE), indices(INDICES)
	{};
};

struct VertexGenerateData
{
	void* verticesPos;
	int structureSize;
	size_t arraySize;
	int oneArraySize;

	VertexGenerateData(void* VERT_PTR, int STRUCTURE_SIZE, size_t ARRAY_SIZE, int ONE_ARRAY_SIZE) :
		verticesPos(VERT_PTR), structureSize(STRUCTURE_SIZE), arraySize(ARRAY_SIZE), oneArraySize(ONE_ARRAY_SIZE)
	{};
};

/// <summary>
/// ラスタライザ描画用の頂点バッファ管理
/// 基本使い回し
/// </summary>
class VertexBufferMgr :public ISingleton<VertexBufferMgr>
{
public:
	RESOURCE_HANDLE GenerateBuffer(const VertexGenerateData& arg_vertexData, bool arg_generateInVRAMFlag);


	RESOURCE_HANDLE GenerateBuffer(const std::vector<VertexAndIndexGenerateData>& vertexData, bool arg_generateInVRAMFlag = true);


	RESOURCE_HANDLE GeneratePlaneBuffer();
	RESOURCE_HANDLE GenerateBoxBuffer();
	RESOURCE_HANDLE StackVertexBuffer(const std::shared_ptr<KazBufferHelper::BufferData>& arg_vertexBuffer,
		const std::shared_ptr<KazBufferHelper::BufferData>& arg_indexBuffer);

	void ReleaseVeretexIndexBuffer(RESOURCE_HANDLE HANDLE);
	void ReleaseVeretexBuffer(RESOURCE_HANDLE HANDLE);
	PolygonMultiMeshedIndexData GetVertexIndexBuffer(RESOURCE_HANDLE HANDLE);
	PolygonInstanceData GetVertexBuffer(RESOURCE_HANDLE HANDLE);

	RESOURCE_HANDLE GetPlaneHandle()
	{
		return m_planeHandle;
	};
	void GenerateBasicVertex()
	{
		m_planeHandle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
	}

	//頂点バッファの事前登録
	RESOURCE_HANDLE RegisterGenerateHandle();
	void RegisterGenerateData(const std::vector<VertexAndIndexGenerateData>& vertexData, bool arg_generateInVRAMFlag, RESOURCE_HANDLE arg_handle);
	void GenerateAllBuffer();
	void Release();

	size_t GetCallNum()
	{
		return m_handleMaxNum;
	}

private:
	size_t m_handleMaxNum;
	HandleMaker m_handle;
	HandleMaker m_vertexHandle;
	std::vector<std::vector<std::unique_ptr<PolygonBuffer>>>m_vertexBufferArray;
	//インデックスあり------
	std::vector<PolygonMultiMeshedIndexData>m_drawIndexDataArray;
	std::vector<std::vector<std::vector<IAPolygonBufferData>>>m_polygonIndexBufferArray;
	//インデックス無し------
	std::vector<IAPolygonBufferData>m_polygonBufferArray;
	std::vector<PolygonInstanceData>m_drawDataArray;
	int sDescHandle = 0;

	//マルチスレッド向けの処理-------------------------------------------------
	struct GenerateData
	{
		RESOURCE_HANDLE m_handle;
		std::vector<VertexAndIndexGenerateData> m_registerGenerateDataArray;
		bool m_registerGenerateVRAMFlag;

		GenerateData(const std::vector<VertexAndIndexGenerateData>& vertexData, bool arg_generateInVRAMFlag, RESOURCE_HANDLE arg_handle) :
			m_registerGenerateDataArray(vertexData), m_registerGenerateVRAMFlag(arg_generateInVRAMFlag), m_handle(arg_handle)
		{
		}
	};
	std::vector<GenerateData> m_registerGenerateDataArray;

	template<typename T>
	void ReleaseArray(std::vector<T>& arg_array);

	std::vector<DirectX::XMFLOAT3>GetPlaneVertices(const KazMath::Vec2<float>& anchorPoint, const KazMath::Vec2<float>& scale, const KazMath::Vec2<int>& texSize);


	RESOURCE_HANDLE m_planeHandle;

	void GenerateIABuffer(IAPolygonBufferData* arg_bufferData, const PolygonGenerateData& arg_generateBufferData, std::string arg_name);
	void GenerateBufferView(const D3D12_SHADER_RESOURCE_VIEW_DESC& arg_view);

	void SetInDescriptorHeap(const std::shared_ptr<KazBufferHelper::BufferData>& arg_bufferData, const D3D12_SHADER_RESOURCE_VIEW_DESC& arg_viewDesc);
	D3D12_SHADER_RESOURCE_VIEW_DESC SetView(UINT arg_structureByte, UINT arg_numElement);

	KazRenderHelper::DrawIndexedInstancedData SetIndexInstanceData(size_t arg_indexArrayNum);
};

template<typename T>
inline void VertexBufferMgr::ReleaseArray(std::vector<T>& arg_array)
{
	arg_array.clear();
	arg_array.shrink_to_fit();
}
