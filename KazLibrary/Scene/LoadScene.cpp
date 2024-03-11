#include "LoadScene.h"
#include"../KazLibrary/Singlton/ProgressBar.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

LoadScene::LoadScene() :
	m_bar(m_rasterize, "Resource/Test/white1x1.png", true, true),
	m_barFlame(m_rasterize, "Resource/SceneUI/Load/Flame.png", true, true),
	m_loadText(m_rasterize, "Resource/SceneUI/Load/NowLoading.png", true, true)
{
	m_rateBuffer = KazBufferHelper::SetConstBufferData(sizeof(float));
	m_rateBuffer.rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	m_rateBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA3;

	DrawFuncData::PipelineGenerateData pipelineData;
	pipelineData.desc = DrawFuncPipelineData::SetTex();
	pipelineData.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Bar.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Bar.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
	pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
	DrawFuncData::DrawCallData drawCall = DrawFuncData::SetSpriteAlphaData(pipelineData);


	drawCall.extraBufferArray.emplace_back(m_rateBuffer);

	m_bar.m_tex.Load(m_rasterize, drawCall, true);

	//描画情報生成
	m_rasterize.GeneratePipeline();
}

void LoadScene::Update()
{
	float scaleRate = ProgressBar::Instance()->GetNowRate();
	m_rateBuffer.bufferWrapper->TransData(&scaleRate, sizeof(float));
}

void LoadScene::Draw(DirectX12* arg_directX12)
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();
	//ロード用の画面描画
	m_loadText.m_tex.Draw2D(m_rasterize, KazMath::Transform2D({ 1280.0f / 2.0f,720.0f / 2.0f - 100.0f + sinf((float)m_timer) * 30.0f }, { 1.0f,1.0f }));

	m_bar.m_tex.Draw2D(m_rasterize, KazMath::Transform2D({ 1280.0f / 2.0f,720.0f / 2.0f + 100.0f }, { 790.0f,90.0f }));
	m_barFlame.m_tex.Draw2D(m_rasterize, KazMath::Transform2D({ 1280.0f / 2.0f,720.0f / 2.0f + 100.0f }, { 1.0f,1.0f }));

	m_timer += 0.01f;

	//ラスタライザ描画
	m_rasterize.SortAndRender();
	m_rasterize.UISortAndRender();

	RenderTargetStatus::Instance()->SwapResourceBarrier();

	arg_directX12->ActCommand(
		{ DirectX12CmdList::Instance()->cmdList },
		{ arg_directX12->cmdAllocator }
	);
}
