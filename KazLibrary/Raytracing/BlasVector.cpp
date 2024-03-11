#include "BlasVector.h"
#include "Blas.h"

namespace Raytracing
{

	BlasVector::BlasVector()
	{
	}

	void BlasVector::Update()
	{
		m_refBlas.resize(0);
		m_instanceDesc.resize(0);
	}

	void BlasVector::Add(std::weak_ptr<Blas> arg_refBlas, const DirectX::XMMATRIX& arg_worldMat, int arg_instanceIndex)
	{

		/*===== Tlas�ɓo�^���邽�߂ɔz��ɒǉ����� =====*/

		//�Q�Ƃ�ǉ����āB
		m_refBlas.emplace_back(arg_refBlas);
		int hitGroupSize = static_cast<int>(m_refBlas.size()) - 1;


		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

		//�s���ݒ�B
		XMStoreFloat3x4(
			reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
			arg_worldMat);

		//�C���X�^���X�̏ڍׂ�ݒ�B
		instanceDesc.InstanceID = arg_instanceIndex;			//���C�g���ōs�������̃t���O�������Őݒ肷��B�}�e���A�����Őݒ肵�Ă��悢�B
		instanceDesc.InstanceMask = 0xFF;
		instanceDesc.InstanceContributionToHitGroupIndex = hitGroupSize;
		instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instanceDesc.AccelerationStructure = m_refBlas.back().lock()->GetBlasBuffer()->GetGPUVirtualAddress();

		//�C���X�^���X��ǉ��B
		m_instanceDesc.emplace_back(instanceDesc);

	}

	void BlasVector::AddVector(std::weak_ptr<Blas> arg_refBlas, std::vector<DirectX::XMMATRIX> arg_worldMat, int arg_instanceIndex, UINT arg_instanceMask)
	{

		/*===== Tlas�ɓo�^���邽�߂ɔz��ɒǉ����� =====*/

		//�Q�Ƃ�ǉ����āB
		int hitGroupSize = static_cast<int>(m_refBlas.size());
		m_refBlas.emplace_back(arg_refBlas);

		int instanceCount = static_cast<int>(m_instanceDesc.size());

		m_instanceDesc.resize(instanceCount + static_cast<int>(arg_worldMat.size()));

		for (int index = 0; index < static_cast<int>(arg_worldMat.size()); ++index) {

			D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

			//�s���ݒ�B
			XMStoreFloat3x4(
				reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
				arg_worldMat[index]);

			//�C���X�^���X�̏ڍׂ�ݒ�B
			instanceDesc.InstanceID = arg_instanceIndex;			//���C�g���ōs�������̃t���O�������Őݒ肷��B�}�e���A�����Őݒ肵�Ă��悢�B
			instanceDesc.InstanceMask = arg_instanceMask;
			instanceDesc.InstanceContributionToHitGroupIndex = hitGroupSize;
			instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
			instanceDesc.AccelerationStructure = m_refBlas.back().lock()->GetBlasBuffer()->GetGPUVirtualAddress();

			//�C���X�^���X��ǉ��B
			m_instanceDesc[instanceCount + index] = instanceDesc;

		}

	}

	int BlasVector::GetBlasRefCount()
	{
		return static_cast<int>(m_refBlas.size());
	}

	int BlasVector::GetInstanceCount()
	{
		return static_cast<int>(m_instanceDesc.size());
	}

	uint8_t* BlasVector::WriteShaderRecord(uint8_t* arg_dest, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stateObject, LPCWSTR arg_hitGroup)
	{

		/*===== �V�F�[�_�[���R�[�h���������� =====*/

		for (auto& index : m_refBlas) {
			arg_dest = index.lock()->WriteShaderRecord(arg_dest, arg_recordSize, arg_stateObject, arg_hitGroup);
		}

		return arg_dest;

	}

}
