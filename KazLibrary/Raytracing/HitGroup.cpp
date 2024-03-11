#include "HitGroup.h"
#include "RayRootSignature.h"

namespace Raytracing {

	HitGroup::HitGroup()
	{
	}

	void HitGroup::Generate(const HitGroupInitData& arg_inputData, int arg_registerSpace, const LPCWSTR& arg_hitGroupName)
	{


		/*===== �������� =====*/

		//���͂��ꂽ�f�[�^�̒l��ۑ�����B
		m_CH = arg_inputData.m_CH;
		m_AH = arg_inputData.m_AH;
		m_IS = arg_inputData.m_IS;
		m_SRVcount = arg_inputData.m_SRVcount;
		m_CBVcount = arg_inputData.m_CBVcount;
		m_UAVcount = arg_inputData.m_UAVcount;
		m_registerSpace = arg_registerSpace;
		m_hitGroupName = arg_hitGroupName;

		//���[�J�����[�g�V�O�l�`���𐶐�����B
		m_localRootSig = std::make_shared<RayRootsignature>();

		//SRV�ɂ��Ēǉ��B
		for (int index = 0; index < m_SRVcount; ++index) {

			m_localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index, m_registerSpace);

		}
		//UAV�ɂ��Ēǉ��B
		for (int index = 0; index < m_UAVcount; ++index) {

			m_localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index, m_registerSpace);

		}
		//CBV�ɂ��Ēǉ��B
		for (int index = 0; index < m_CBVcount; ++index) {

			m_localRootSig->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index, m_registerSpace);

		}
		//�T���v���[��ǉ��B
		m_localRootSig->AddStaticSampler(m_registerSpace);

		//���[�J�����[�g�V�O�l�`���𐶐��B
		m_localRootSig->Build(true, m_hitGroupName);


	}

}
