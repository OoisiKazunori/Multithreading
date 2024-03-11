#include "BufferDuplicateBlocking.h"
#include"DirectXCommon/DirectX12Device.h"
#include"DirectXCommon/DirectX12CmdList.h"

RESOURCE_HANDLE PipelineDuplicateBlocking::GeneratePipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &DATA, const PipelineDuplicateData &CHECK_DATA)
{
	//�d���`�F�b�N
	for (int i = 0; i < duplicateDataArray.size(); ++i)
	{
		//��r����ۂɏd�v�ɂȂ�v�f
		/*
		���[�g�V�O�l�`���[�̐ݒ�A�V�F�[�_�[�̐ݒ�A�u�����h���[�h
		*/
		if (!IsSameRootSignatureGenerated(CHECK_DATA.rootsignature, duplicateDataArray[i].rootsignature))
		{
			continue;
		}
		if (CHECK_DATA.shaderOption != duplicateDataArray[i].shaderOption)
		{
			continue;
		}
		if (CHECK_DATA.blendMode != duplicateDataArray[i].blendMode)
		{
			continue;
		}
		//�d�����m�F������Ԃ�
		return i;
	}


	//�d�����Ȃ���ΐ���
	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	HRESULT lResult = S_FALSE;
	//�����ǉ�
	if (bufferArray.size() <= outputHandle)
	{
		bufferArray.emplace_back();
		duplicateDataArray.emplace_back(CHECK_DATA);
		pipelineCount.emplace_back(outputHandle);
		lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&DATA, IID_PPV_ARGS(&bufferArray[outputHandle]));
	}
	//�K�x���[�W�R���N�V����
	else
	{
		duplicateDataArray[outputHandle] = CHECK_DATA;
		pipelineCount[outputHandle].shaderHandle = outputHandle;
		lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&DATA, IID_PPV_ARGS(&bufferArray[outputHandle]));
	}

	if (lResult != S_OK)
	{
		return -1;
	}
	return outputHandle;
}

RESOURCE_HANDLE PipelineDuplicateBlocking::GeneratePipeline(const D3D12_COMPUTE_PIPELINE_STATE_DESC &DATA)
{
	//�d���`�F�b�N
	for (int i = 0; i < 0; ++i)
	{
		bool lCheckFlag = false;
		//bufferGenerateDataArray[i].VS.pShaderBytecode == DATA.VS.pShaderBytecode;

		//�d�����m�F������Ԃ�
		if (lCheckFlag)
		{
			return i;
		}
	}

	//�d�����Ȃ���ΐ���
	RESOURCE_HANDLE lHandle = handle.GetHandle();
	bufferArray.emplace_back();
	pipelineCount.emplace_back(lHandle);
	HRESULT lResult = DirectX12Device::Instance()->dev->CreateComputePipelineState(&DATA, IID_PPV_ARGS(&bufferArray[lHandle]));
	if (lResult != S_OK)
	{
		return -1;
	}
	return lHandle;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	pipelineCount[HANDLE].Call();
	return bufferArray[HANDLE];
}

void PipelineDuplicateBlocking::Update()
{
	for (auto &obj : pipelineCount)
	{
		//��x���g���Ă��Ȃ��f�[�^�͏�������B
		if (obj.IsNotCall())
		{
			RESOURCE_HANDLE releaseHandle = obj.shaderHandle;
			handle.DeleteHandle(releaseHandle);
			bufferArray[releaseHandle].Reset();
			pipelineCount[releaseHandle].Finalize();
			duplicateDataArray[releaseHandle].Finalize();
		}
		obj.Reset();
	}
}

void PipelineDuplicateBlocking::Release()
{
	handle.DeleteAllHandle();
	pipelineCount.clear();
	pipelineCount.shrink_to_fit();
	bufferArray.clear();
	bufferArray.shrink_to_fit();
	duplicateDataArray.clear();
	duplicateDataArray.shrink_to_fit();
}

RESOURCE_HANDLE ShaderDuplicateBlocking::GenerateShader(const ShaderOptionData &DATA)
{
	//�d���m�F
	for (int i = 0; i < generateDataArray.size(); ++i)
	{
		if (generateDataArray[i] == DATA)
		{
			shaderCount[i].Call();
			return i;
		}
	}

	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	//�����ǉ�
	if (bufferArray.size() <= outputHandle)
	{
		//ID�ǉ�
		generateDataArray.emplace_back(DATA);
		//�V�F�[�_�[����
		Shader lShader;
		bufferArray.emplace_back(lShader.CompileShader(DATA));
		//�Ăяo���m�F�p
		shaderCount.emplace_back(outputHandle);
	}
	//�K�x���[�W�R���N�V����
	else
	{
		//ID�ǉ�
		generateDataArray[outputHandle] = DATA;
		//�V�F�[�_�[����
		Shader lShader;
		bufferArray[outputHandle] = lShader.CompileShader(DATA);
		//�Ăяo���m�F�p
		shaderCount[outputHandle].shaderHandle = outputHandle;
	}
	return outputHandle;
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return bufferArray[HANDLE];
}

void ShaderDuplicateBlocking::Update()
{
	for (auto &obj : shaderCount)
	{
		//��x���g���Ă��Ȃ��f�[�^�͏�������B
		if (obj.IsNotCall())
		{
			RESOURCE_HANDLE releaseHandle = obj.shaderHandle;
			handle.DeleteHandle(releaseHandle);
			bufferArray[releaseHandle].Reset();
			shaderCount[releaseHandle].Finalize();
			generateDataArray[releaseHandle].fileName = "";
		}
		obj.Reset();
	}
}

void ShaderDuplicateBlocking::Release()
{
	handle.DeleteAllHandle();
	generateDataArray.clear();
	generateDataArray.shrink_to_fit();
	shaderCount.clear();
	shaderCount.shrink_to_fit();
	bufferArray.clear();
	bufferArray.shrink_to_fit();
}

RESOURCE_HANDLE RootSignatureDuplicateBlocking::GenerateRootSignature(const RootSignatureDataTest &DATA)
{
	//�d���`�F�b�N
	for (int i = 0; i < dataForDuplicateBlocking.size(); ++i)
	{
		if (IsSameRootSignatureGenerated(DATA, dataForDuplicateBlocking[i]))
		{
			callDataArray[i].Call();
			return i;
		}
	}

	std::vector<CD3DX12_ROOT_PARAMETER> lRootparamArray(DATA.rangeArray.size());

	int lCountDescriptorNum = 0;
	//�f�X�N���v�^�q�[�v�̐ݒ肪�K�v�Ȑ����v�Z����
	for (int i = 0; i < DATA.rangeArray.size(); ++i)
	{
		const bool L_THIS_TYPE_IS_DESCRIPTOR_FLAG = DATA.rangeArray[i].rangeType % 2 == 0;
		if (L_THIS_TYPE_IS_DESCRIPTOR_FLAG)
		{
			++lCountDescriptorNum;
		}
	}
	std::vector<CD3DX12_DESCRIPTOR_RANGE> lDescRangeRangeArray(lCountDescriptorNum);

#pragma region ���[�g�p�����[�^�[��ݒ肷��
	GraphicsRootSignature::ParamData lViewData;
	UINT lDescriptorArrayNum = 0;

	std::vector<RootSignatureParameter> lParamArrayData;
	for (int i = 0; i < DATA.rangeArray.size(); i++)
	{
		switch (DATA.rangeArray[i].rangeType)
		{
		case GRAPHICS_RANGE_TYPE_CBV_VIEW:
			//���[�g�p�����̐ݒ�
			lRootparamArray[i].InitAsConstantBufferView(lViewData.cbv, 0, D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.cbv;
			break;

		case GRAPHICS_RANGE_TYPE_CBV_DESC:
			//���[�g�p�����̐ݒ�
			lDescRangeRangeArray[lDescriptorArrayNum].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, lViewData.cbv);
			lRootparamArray[i].InitAsDescriptorTable(1, &lDescRangeRangeArray[lDescriptorArrayNum], D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.cbv;
			break;

		case GRAPHICS_RANGE_TYPE_SRV_VIEW:
			//���[�g�p�����̐ݒ�
			lRootparamArray[i].InitAsShaderResourceView(lViewData.srv, 0, D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.srv;
			break;

		case GRAPHICS_RANGE_TYPE_SRV_DESC:
			//���[�g�p�����̐ݒ�
			lDescRangeRangeArray[lDescriptorArrayNum].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, lViewData.srv);
			lRootparamArray[i].InitAsDescriptorTable(1, &lDescRangeRangeArray[lDescriptorArrayNum], D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.srv;
			break;


		case GRAPHICS_RANGE_TYPE_UAV_VIEW:
			//���[�g�p�����̐ݒ�
			lRootparamArray[i].InitAsUnorderedAccessView(lViewData.uav, 0, D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.uav;
			break;

		case GRAPHICS_RANGE_TYPE_UAV_DESC:
			//���[�g�p�����̐ݒ�
			lDescRangeRangeArray[lDescriptorArrayNum].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, lViewData.uav);
			lRootparamArray[i].InitAsDescriptorTable(1, &lDescRangeRangeArray[lDescriptorArrayNum], D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.uav;
			break;
		case -1:
			break;
		}

#pragma region ���[�g�p�����[�^�[�����ݒ���s���ׂ̐ݒ�
		RootSignatureParameter lData;
		lData.range = DATA.rangeArray[i].rangeType;
		lData.paramData.type = DATA.rangeArray[i].dataType;

		const bool L_THIS_TYPE_IS_DESCRIPTOR_FLAG = DATA.rangeArray[i].rangeType % 2 == 0;
		if (L_THIS_TYPE_IS_DESCRIPTOR_FLAG)
		{
			lData.type = GRAPHICS_ROOTSIGNATURE_TYPE_DESCRIPTORTABLE;
			++lDescriptorArrayNum;
		}
		else
		{
			lData.type = GRAPHICS_ROOTSIGNATURE_TYPE_VIEW;
		}
		lData.paramData.param = lViewData.TotalNum() - 1;
		lParamArrayData.push_back(lData);
#pragma endregion
	}

#pragma endregion

#pragma region �o�[�W������������ł̃V���A���C�Y
	D3D12_ROOT_SIGNATURE_FLAGS lRootSignatureType = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC lRootSignatureDesc;
	lRootSignatureDesc.Init_1_0(
		static_cast<UINT>(lRootparamArray.size()),
		lRootparamArray.data(),
		static_cast<UINT>(DATA.samplerArray.size()),
		DATA.samplerArray.data(),
		lRootSignatureType);

	Microsoft::WRL::ComPtr<ID3DBlob> lRootSigBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> lErrorBlob = nullptr;
	HRESULT lResult = D3DX12SerializeVersionedRootSignature(
		&lRootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&lRootSigBlob,
		&lErrorBlob);

	if (lResult != S_OK)
	{
		assert(0);
	}
#pragma endregion


	//���[�g�V�O�l�`���̐���
	Microsoft::WRL::ComPtr<ID3D12RootSignature> lRootSignature;
	lResult = DirectX12Device::Instance()->dev->CreateRootSignature(
		0,
		lRootSigBlob->GetBufferPointer(),
		lRootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&lRootSignature)
	);
	if (lResult != S_OK)
	{
		return -1;
	}

	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	//���[�g�V�O�l�`���[�ƃp�����f�[�^�̕ۊ�
	if (rootSignatureArray.size() <= outputHandle)
	{
		rootSignatureArray.emplace_back(lRootSignature, lParamArrayData);
		callDataArray.emplace_back(outputHandle);
		dataForDuplicateBlocking.emplace_back(DATA);
	}
	else
	{
		rootSignatureArray[outputHandle] = RootSignatureData(lRootSignature, lParamArrayData);
		callDataArray[outputHandle].shaderHandle = outputHandle;
		dataForDuplicateBlocking[outputHandle] = DATA;
	}

	return outputHandle;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignatureDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	callDataArray[HANDLE].Call();
	return rootSignatureArray[HANDLE].buffer;
}

const std::vector<RootSignatureParameter> &RootSignatureDuplicateBlocking::GetRootParam(RESOURCE_HANDLE ROOTSIGNATURE_HANDLE)
{
	return rootSignatureArray[ROOTSIGNATURE_HANDLE].rootParamDataArray;
}

void RootSignatureDuplicateBlocking::Update()
{
	for (auto &obj : callDataArray)
	{
		//��x���g���Ă��Ȃ��f�[�^�͏�������B
		if (obj.IsNotCall())
		{
			RESOURCE_HANDLE releaseHandle = obj.shaderHandle;
			handle.DeleteHandle(releaseHandle);
			rootSignatureArray[releaseHandle].buffer.Reset();
			rootSignatureArray[releaseHandle].rootParamDataArray.clear();
			dataForDuplicateBlocking[releaseHandle].rangeArray.clear();
			dataForDuplicateBlocking[releaseHandle].samplerArray.clear();
			callDataArray[releaseHandle].Finalize();
		}
		obj.Reset();
	}
}

void RootSignatureDuplicateBlocking::Release()
{
	handle.DeleteAllHandle();
	rootSignatureArray.clear();
	rootSignatureArray.shrink_to_fit();
	dataForDuplicateBlocking.clear();
	dataForDuplicateBlocking.shrink_to_fit();
	callDataArray.clear();
	callDataArray.shrink_to_fit();
}
