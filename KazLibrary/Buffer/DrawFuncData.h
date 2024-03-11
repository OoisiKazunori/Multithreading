#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Buffer/VertexBufferMgr.h"
#include <source_location>
#include"GBufferMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

namespace Raytracing
{
	class Blas;
}

namespace ExecuteIndirectData
{
	struct DrawIndexedIndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS m_view;
		D3D12_DRAW_INDEXED_ARGUMENTS m_drawArguments;
	};
	struct DrawIndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS m_view;
		D3D12_DRAW_ARGUMENTS m_drawArguments;
	};

	static void GenerateCommandSignature(
		Microsoft::WRL::ComPtr<ID3D12CommandSignature>& arg_commandSignature,
		const Microsoft::WRL::ComPtr<ID3D12RootSignature>& arg_rootSignature,
		const std::vector<D3D12_INDIRECT_ARGUMENT_DESC>& arg_indirectDescArray)
	{
		//�R�}���h�V�O�l�`��---------------------------
		D3D12_COMMAND_SIGNATURE_DESC desc{};
		desc.pArgumentDescs = arg_indirectDescArray.data();
		desc.NumArgumentDescs = static_cast<UINT>(arg_indirectDescArray.size());
		desc.ByteStride = sizeof(DrawIndexedIndirectCommand);

		HRESULT lResult =
			DirectX12Device::Instance()->dev->CreateCommandSignature(&desc, arg_rootSignature.Get(), IID_PPV_ARGS(&arg_commandSignature));
		//�R�}���h�V�O�l�`��---------------------------
		if (lResult != S_OK)
		{
			assert(0);
		}
	}
}

namespace DrawFuncPipelineData
{
	enum class PipelineBlendModeEnum
	{
		NONE = -1,
		ALPHA,
		ADD,
		SUB,
		MAX
	};

	//��
	static D3D12_RENDER_TARGET_BLEND_DESC SetAlphaBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC lAlphaBlendDesc{};
		lAlphaBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		lAlphaBlendDesc.BlendEnable = true;
		lAlphaBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		lAlphaBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		lAlphaBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		lAlphaBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		lAlphaBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		lAlphaBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		return lAlphaBlendDesc;
	};

	//���Z����
	static D3D12_RENDER_TARGET_BLEND_DESC SetAddBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC lAddBlendDesc{};
		lAddBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		lAddBlendDesc.BlendEnable = true;
		lAddBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		lAddBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		lAddBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		lAddBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		lAddBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		lAddBlendDesc.DestBlend = D3D12_BLEND_ONE;
		return lAddBlendDesc;
	}

	//���Z����
	static D3D12_RENDER_TARGET_BLEND_DESC SetSubBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC subBlendDesc{};
		subBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		subBlendDesc.BlendEnable = true;
		subBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		subBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		subBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		subBlendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		subBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		subBlendDesc.DestBlend = D3D12_BLEND_ONE;

		return subBlendDesc;
	}

	//���]����
	static D3D12_RENDER_TARGET_BLEND_DESC SetRevBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC lRevBlendDesc{};
		lRevBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;;
		lRevBlendDesc.BlendEnable = true;
		lRevBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		lRevBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		lRevBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		lRevBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		lRevBlendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		lRevBlendDesc.DestBlend = D3D12_BLEND_ZERO;

		return lRevBlendDesc;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetTest()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};



		static D3D12_INPUT_ELEMENT_DESC input3DLayOut =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		gPipeline.InputLayout.pInputElementDescs = &input3DLayOut;
		gPipeline.InputLayout.NumElements = 1;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetTex()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[2];
		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 2;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;

		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAddBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPos()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut;
		input3DLayOut =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = &input3DLayOut;
		gPipeline.InputLayout.NumElements = 1;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosNormal()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[2];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 2;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormal()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[3];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[2] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 3;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormalTangentBinormal()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[5];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[2] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[3] =
		{
			"TANGENT",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[4] =
		{
			"BINORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 5;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormalTangentBinormalBoneNoWeight()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[7];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[2] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[3] =
		{
			"TANGENT",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[4] =
		{
			"BINORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[5] =
		{
			"BONE_NO",
			0,
			DXGI_FORMAT_R32G32B32A32_SINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[6] =
		{
			"WEIGHT",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 7;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosLine()
	{
		//�p�C�v���C���̐ݒ�
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut;
		input3DLayOut =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = &input3DLayOut;
		gPipeline.InputLayout.NumElements = 1;

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		rasterrize.FillMode = D3D12_FILL_MODE_WIREFRAME;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}



};

namespace DrawFuncData
{
	class DrawingByRasterize;
	enum class VERT_TYPE
	{
		INDEX,
		INSTANCE,
		MULTI_MESHED,
		EXECUTEINDIRECT_INDEX,
		EXECUTEINDIRECT_INSTANCE,
	};

	struct ExcuteIndirectArgumentData
	{
		KazBufferHelper::BufferData m_uploadArgumentBuffer;
		KazBufferHelper::BufferData m_uavArgumentBuffer;
		UINT m_maxCommandCount;
		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> m_desc;
		ExecuteIndirectData::DrawIndexedIndirectCommand m_argumentCommandData;

		void GenerateArgumentBuffer();
	};

	//�`�施�߂̔��s���s���f�[�^
	struct DrawData
	{
		bool generateFlag = false;

		//���_���
		KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData drawMultiMeshesIndexInstanceCommandData;
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;
		DrawFuncData::VERT_TYPE drawCommandType;
		//�}�e���A�����
		std::vector<std::vector<std::shared_ptr<KazBufferHelper::BufferData>>> materialBuffer;
		RESOURCE_HANDLE m_modelVertexBufferHandle;
		std::shared_ptr<ModelInfomation>m_modelData;

		//ExcuteIndirect���s����
		ExcuteIndirectArgumentData m_executeIndirectGenerateData;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature>m_commandSignature;
		RESOURCE_HANDLE m_commandRootsignatureHandle;

		//�p�C�v���C�����
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineData;
		std::vector<ShaderOptionData> shaderDataArray;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE depthHandle;
		RESOURCE_HANDLE pipelineHandle;
		std::vector<RESOURCE_HANDLE> shaderHandleArray;
		RESOURCE_HANDLE m_rootsignatureHandle;


		//���̑��`��ɕK�v�ȃo�b�t�@���
		std::vector<KazBufferHelper::BufferData>* buffer;

		//�f�o�b�N���
		std::source_location drawCallData;

		DrawData() :generateFlag(false), renderTargetHandle(-1)
		{
		};
	};

	struct PipelineGenerateData
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
		std::vector<ShaderOptionData>shaderDataArray;
		DrawFuncPipelineData::PipelineBlendModeEnum blendMode;
	};
	struct DrawFuncBufferData
	{
		DrawFuncBufferData(const KazBufferHelper::BufferResourceData& DATA, GraphicsRootParamType ROOT_PARAM, GraphicsRangeType RANGE_TYPE) :
			resourceData(DATA), rootParam(ROOT_PARAM), rangeType(RANGE_TYPE)
		{};

		KazBufferHelper::BufferResourceData resourceData;
		GraphicsRootParamType rootParam;
		GraphicsRangeType rangeType;
	};

	//DrawCallData���Ŏg�p���郌�C�g���[�V���O�֘A�̏��
	struct RaytracingData {
		std::vector<std::shared_ptr<Raytracing::Blas>> m_blas;	//���C�g���[�V���O�Ɏg�p����Blas�B���C�g���[�V���O���s���ꍇ�A�����BlasVector�ɎQ�Ƃ�ۑ�����B
		bool m_isRaytracingInitialized;							//���C�g���[�V���O�ɕK�v�ȏ�񂪏���������Ă��邩�B
		bool m_isRaytracingEnable;								//���C�g���[�V���O���L��������Ă��邩�B m_isRaytracingInitialized(����������Ă���) and m_isRaytracingEnable(�L��������Ă���) ����TLAS�ɕۑ������B ���X�^���C�Y�̕`��͍s�����ǃ��C�g���͈�U�؂�ꍇ�Ɏg�p���邱�Ƃ��l�����č쐬���܂����B�����ɂ���ăJ�����O�����肷��ꍇ�ł��ˁI
	};

	//DrawFunc���g�p����ۂɕK�v�ȃf�[�^
	struct DrawCallData
	{
		DrawCallData(std::source_location location = std::source_location::current()) :
			callLocation(location), renderTargetHandle(-1), depthHandle(-1), m_deleteInSceneFlag(false)
		{
		};
		bool m_deleteInSceneFlag;
		/// <summary>
		/// ���C�g���[�V���O������
		/// </summary>
		/// <param name="arg_isOpaque"> ���̃I�u�W�F�N�g�͕s�������HAnyhitShader���Ă΂�邩�ǂ����������Ŕ��f����B </param>
		void SetupRaytracing(bool arg_isOpaque);
		//���_���
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;
		KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData drawMultiMeshesIndexInstanceCommandData;
		VERT_TYPE drawCommandType;
		std::shared_ptr<ModelInfomation>m_modelData;
		std::vector<std::vector<std::shared_ptr<KazBufferHelper::BufferData>>> materialBuffer;
		//���_��񂪊i�[����Ă���f�[�^�̃n���h��
		RESOURCE_HANDLE m_modelVertDataHandle;
		//ExecuteIndirect
		ExcuteIndirectArgumentData m_executeIndirectGenerateData;

		//���C�g���[�V���O�Ɏg�p������
		RaytracingData m_raytracingData;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE depthHandle;
		//�p�C�v���C�����
		PipelineGenerateData pipelineData;

		//���̑��K�v�ȃo�b�t�@�̐ݒ�
		std::vector<KazBufferHelper::BufferData>extraBufferArray;

		std::source_location callLocation;
	};

	//�P�F�̃|���S���\��(�C���f�b�N�X�Ȃ�)
	static DrawCallData SetDrawPolygonData(const KazRenderHelper::DrawInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;
		//���_���
		drawCallData.drawInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::INSTANCE;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�F���
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�p�C�v���C�����̃Z�b�g
		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//�P�F�̃|���S���\��(�C���f�b�N�X����)
	static DrawCallData SetDrawPolygonIndexData(const KazRenderHelper::DrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA, std::source_location location = std::source_location::current())
	{
		DrawCallData drawCallData;
		//���_���
		drawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::INDEX;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�F���
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�p�C�v���C�����̃Z�b�g
		drawCallData.pipelineData = PIPELINE_DATA;

		drawCallData.callLocation = location;

		return drawCallData;
	};

	static DrawCallData SetDrawGLTFAnimationIndexMaterialData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		//��
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX) * 256)
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMMATRIX);

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//OBJ���f���̃|���S���\��(�C���f�b�N�X����)
	static DrawCallData SetDrawOBJIndexData(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;
		//���_���
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�}�e���A�����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(MaterialBufferData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�p�C�v���C�����̃Z�b�g
		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//���f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������)
	static DrawCallData SetDrawGLTFIndexMaterialData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;


		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//���f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������)
	static DrawCallData SetDrawGLTFIndexMaterialLightData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g���
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT3))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT3);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//���C�g���ł̃��f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g�����ł̔��f
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//���C�g���ł̃��f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������A�C���X�^���V���O�`��)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingInstanceData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back();
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//���C�g�����ł̔��f
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};


	//���C�g���ł̃��f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������)
	static DrawCallData SetDrawInstanceGLTFIndexMaterialInRayTracingData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back();
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g�����ł̔��f
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//���C�g���ł̃��f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������A�u���[���̉����ݒ肠��)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingBloomData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g�����ł̔��f
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		//�G�~�b�V�u..xyz�F,a����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		drawCallData.SetupRaytracing(true);

		return drawCallData;
	};


	//���f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������A�u���[���̉����ݒ肠��A�A�j���[�V�����Ή�)
	static DrawCallData SetDrawGLTFAnimationIndexMaterialBloomData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g�����ł̔��f
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		//�G�~�b�V�u..xyz�F,a����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		return drawCallData;
	};

	//���C�g���ł̃��f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������A�u���[���̉����ݒ肠��A�A�j���[�V�����Ή�)
	static DrawCallData SetDrawGLTFAnimationIndexMaterialInRayTracingBloomData(const std::shared_ptr<ModelInfomation>& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		//���_���
		drawCallData.m_modelData = MODEL_DATA;
		drawCallData.m_modelVertDataHandle = MODEL_DATA->modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA->modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g�����ł̔��f
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//�F��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		//�G�~�b�V�u..xyz�F,a����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		drawCallData.SetupRaytracing(true);

		return drawCallData;
	};

	//���C�g���̂݃Z�b�g�A�b�v
	DrawCallData SetRaytracingData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA);


	//�s����̂�
	static DrawCallData SetTransformData(const KazRenderHelper::DrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;
		//���_���
		drawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::INDEX;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//�s����ƃe�N�X�`��
	static DrawCallData SetTexPlaneData(const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GetPlaneHandle();
		//���_���
		drawCallData.m_modelVertDataHandle = VertexBufferMgr::Instance()->GetPlaneHandle();
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�e�N�X�`�����
		drawCallData.extraBufferArray.emplace_back();
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};


	static DrawCallData SetSpriteAlphaData(const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		//���_���
		drawCallData.m_modelVertDataHandle = VertexBufferMgr::Instance()->GetPlaneHandle();
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//��Z
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�e�N�X�`�����
		drawCallData.extraBufferArray.emplace_back();
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	static DrawCallData SetDefferdRenderingModel(std::shared_ptr<ModelInfomation>arg_model)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}

		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdMain", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		drawCall.SetupRaytracing(true);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingModel(std::shared_ptr<ModelInfomation>arg_model, std::vector<ShaderOptionData>arg_shader)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}

		pipelineData.shaderDataArray = arg_shader;

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		drawCall.SetupRaytracing(true);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingInstanceModel(std::shared_ptr<ModelInfomation>arg_model, std::vector<ShaderOptionData>arg_shader)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}

		pipelineData.shaderDataArray = arg_shader;

		drawCall = DrawFuncData::SetDrawInstanceGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		drawCall.SetupRaytracing(true);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingModelAnimation(std::shared_ptr<ModelInfomation>arg_model, bool arg_isOpaque = true)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdAnimationMain", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.SetupRaytracing(arg_isOpaque);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingModelInstance(std::shared_ptr<ModelInfomation>arg_model, bool arg_isOpaque = true)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdAnimationMain", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingInstanceData(arg_model, pipelineData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.SetupRaytracing(arg_isOpaque);

		return drawCall;
	};


	static DrawCallData SetDefferdRenderingModelAnimationZAllways(std::shared_ptr<ModelInfomation>arg_model, bool arg_isOpaque = true)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdAnimationMain", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();
		drawCall.pipelineData.desc.DepthStencilState.DepthEnable = false;							//�[�x�e�X�g���s��
		drawCall.pipelineData.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		drawCall.pipelineData.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//���������OK
		drawCall.pipelineData.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.SetupRaytracing(arg_isOpaque);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingModelAnimationNoLighting(std::shared_ptr<ModelInfomation>arg_model, bool arg_isOpaque = true)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdAnimationMainNoLighting", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.SetupRaytracing(arg_isOpaque);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingModelAnimationDissolve(std::shared_ptr<ModelInfomation>arg_model)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "PostEffect/Dissolve/" + "DissolveModel.hlsl", "PSDefferdAnimationMainDissolve", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(arg_model, pipelineData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4) * 2)
		);
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA5;

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_TEX;

		drawCall.SetupRaytracing(true);

		return drawCall;
	};

	static DrawCallData SetLine(RESOURCE_HANDLE arg_vertexHandle)
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosLine();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawLine.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawLine.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		DrawCallData drawCallData;
		//���_���
		drawCallData.drawInstanceCommandData = VertexBufferMgr::Instance()->GetVertexBuffer(arg_vertexHandle).instanceData;
		drawCallData.drawCommandType = VERT_TYPE::INSTANCE;
		drawCallData.m_modelVertDataHandle = arg_vertexHandle;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMMATRIX);
		//�F���
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = pipelineData;
		drawCallData.drawInstanceCommandData.topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

		return drawCallData;
	}

	static DrawCallData SetLine(const DrawFuncData::PipelineGenerateData& arg_pipeline)
	{
		DrawFuncData::PipelineGenerateData pipelineData(arg_pipeline);

		DrawCallData drawCallData;
		//���_���
		drawCallData.drawCommandType = VERT_TYPE::INSTANCE;

		//�s����
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMMATRIX);
		//�F���
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = pipelineData;
		drawCallData.drawInstanceCommandData.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		return drawCallData;
	}

	static DrawCallData SetExecuteIndirect(const PipelineGenerateData& PIPELINE_DATA, const D3D12_GPU_VIRTUAL_ADDRESS& arg_address, UINT arg_maxCountNum, UINT arg_indexNum = -1)
	{
		DrawCallData drawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GetPlaneHandle();
		//���_���
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(handle).index;
		drawCallData.drawCommandType = VERT_TYPE::EXECUTEINDIRECT_INDEX;

		drawCallData.pipelineData = PIPELINE_DATA;

		//ExecuteIndirect���s
		drawCallData.m_executeIndirectGenerateData.m_maxCommandCount = arg_maxCountNum;

		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> args;
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
		args[0].UnorderedAccessView.RootParameterIndex = 0;
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
		drawCallData.m_executeIndirectGenerateData.m_desc = args;

		drawCallData.m_executeIndirectGenerateData.m_argumentCommandData.m_view = arg_address;
		drawCallData.m_executeIndirectGenerateData.m_argumentCommandData.m_drawArguments =
		{
			drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].indexCountPerInstance,
			arg_maxCountNum,
			drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].startIndexLocation,
			(int)drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].baseVertexLocation,
			drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].startInstanceLocation
		};

		if (arg_indexNum != -1)
		{
			drawCallData.m_executeIndirectGenerateData.m_argumentCommandData.m_drawArguments.IndexCountPerInstance = arg_indexNum;
		}

		drawCallData.m_executeIndirectGenerateData.m_uploadArgumentBuffer = KazBufferHelper::SetUploadBufferData(sizeof(ExecuteIndirectData::DrawIndexedIndirectCommand));
		drawCallData.m_executeIndirectGenerateData.m_uploadArgumentBuffer.bufferWrapper->TransData(&drawCallData.m_executeIndirectGenerateData.m_argumentCommandData, sizeof(ExecuteIndirectData::DrawIndexedIndirectCommand));

		drawCallData.m_executeIndirectGenerateData.m_uavArgumentBuffer = KazBufferHelper::SetGPUBufferData(sizeof(ExecuteIndirectData::DrawIndexedIndirectCommand));

		drawCallData.m_executeIndirectGenerateData.m_uavArgumentBuffer.bufferWrapper->CopyBuffer(
			drawCallData.m_executeIndirectGenerateData.m_uploadArgumentBuffer.bufferWrapper->GetBuffer());

		drawCallData.m_executeIndirectGenerateData.m_uavArgumentBuffer.bufferWrapper->ChangeBarrier(
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		return drawCallData;
	};

	static DrawCallData SetParticleInRaytracing(const std::shared_ptr<KazBufferHelper::BufferData>& arg_buffer, const std::shared_ptr<KazBufferHelper::BufferData>& arg_indexBuffer)
	{
		DrawCallData drawCallData;
		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GetPlaneHandle();
		drawCallData.m_modelVertDataHandle = VertexBufferMgr::Instance()->StackVertexBuffer(arg_buffer, arg_indexBuffer);
		drawCallData.materialBuffer.emplace_back();
		drawCallData.materialBuffer.back().emplace_back(TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::TestPath + "white1x1.png"));
		drawCallData.materialBuffer.back().back()->rootParamType = GRAPHICS_PRAMTYPE_TEX;
		drawCallData.SetupRaytracing(true);

		return drawCallData;
	};


	static DrawFuncData::PipelineGenerateData GetModelShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvmain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetModelBloomShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdBloomMain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetAnimationModelBloomShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdMain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdBloomMain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetSpriteShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetTex();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetSpriteAlphaShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetTex();
		pipelineData.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSAlphaMain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetSpriteAlphaDepthAlwaysShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetTex();
		pipelineData.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSAlphaMain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicInstanceShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicInstanceEmissiveShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "PSEmissiveMain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicInstanceShader2()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "PS", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = 1;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPos();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Basic.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Basic.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = 1;
		pipelineData.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineData.desc.SampleDesc.Count = 1;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicGBufferShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPos();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Basic.hlsl", "GBufferVSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Basic.hlsl", "GBufferPSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetDefferedModelShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSModel", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSModel", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = 1;
		pipelineData.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		return pipelineData;
	};

	static DrawFuncData::PipelineGenerateData GetAnimationModelShader()
	{
		DrawFuncData::PipelineGenerateData pipelineData;
		pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSAnimationModel", "vs_6_4", SHADER_TYPE_VERTEX);
		pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSModel", "ps_6_4", SHADER_TYPE_PIXEL);
		pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//���̑��ݒ�
		pipelineData.desc.NumRenderTargets = 1;
		pipelineData.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		return pipelineData;
	};

}