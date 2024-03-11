#pragma once
#include <string>
#include <memory>
#include <wtypes.h>

namespace Raytracing {

	class RayRootsignature;

	/// <summary>
	/// HitGroup�𐶐�����ۂɓn���f�[�^�\����
	/// </summary>
	struct EntryPoint {
		LPCWSTR m_entryPoint;
		bool m_isActive;
		EntryPoint() {};
		EntryPoint(LPCWSTR arg_entry, bool arg_flag) :m_entryPoint(arg_entry), m_isActive(arg_flag) {};
	};
	struct HitGroupInitData {

		EntryPoint m_CH;	//ClosestHitShader
		EntryPoint m_AH;	//AnyHitShader
		EntryPoint m_IS;	//IntersectShader
		int m_SRVcount;		//SRV�̐�
		int m_CBVcount;		//CBV�̐�
		int m_UAVcount;		//UAV�̐�
		HitGroupInitData() {};
	};

	/// <summary>
	/// �q�b�g�O���[�v�N���X
	/// </summary>
	class HitGroup {

	private:

		/*===== �����o�ϐ� =====*/

		EntryPoint m_CH;	//ClosestHitShader
		EntryPoint m_AH;	//AnyHitShader
		EntryPoint m_IS;	//IntersectShader
		int m_SRVcount;		//SRV�̐�
		int m_CBVcount;		//CBV�̐�
		int m_UAVcount;		//UAV�̐�

		int m_registerSpace;	//�V�F�[�_�[���W�X�^�[�̃��W�X�^�[�X�y�[�X�ԍ�

		LPCWSTR m_hitGroupName;	//���̃q�b�g�O���[�v�̖��O

		std::shared_ptr<RayRootsignature> m_localRootSig;	//���̃q�b�g�O���[�v�Ŏg�p���郍�[�J�����[�g�V�O�l�`��


	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		HitGroup();

		/// <summary>
		/// ��������
		/// </summary>
		/// <param name="InputData"> HitGroup�̏������p�f�[�^ </param>
		/// <param name="RegisterSpace"> ���W�X�^�X�y�[�X </param>
		/// <param name="HitGroupName"> ����HitGroup�̖��O�̕����� </param>
		void Generate(const HitGroupInitData& arg_inputData, int arg_registerSpace, const LPCWSTR& arg_hitGroupName);

		// �e��Q�b�^
		const LPCWSTR& GetCH() { return m_CH.m_entryPoint; }
		const LPCWSTR& GetAH() { return m_AH.m_entryPoint; }
		const LPCWSTR& GetIS() { return m_IS.m_entryPoint; }
		bool GetCHFlag() { return m_CH.m_isActive; }
		bool GetAHFlag() { return m_AH.m_isActive; }
		bool GetISFlag() { return m_IS.m_isActive; }
		int GetSRVCount() { return m_SRVcount; }
		int GetCBVCount() { return m_CBVcount; }
		int GetUAVCount() { return m_UAVcount; }
		const std::shared_ptr<RayRootsignature> GetLoacalRootSig() { return m_localRootSig; };

	};

}