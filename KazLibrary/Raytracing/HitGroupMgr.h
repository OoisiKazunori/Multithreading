#pragma once
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <wtypes.h>
#include "../Helper/ISinglton.h"

namespace Raytracing {

	class HitGroup;
	class RayRootsignature;

	/// <summary>
	/// HitGroup���Ǘ��A�ۑ�����N���X�B���݂�1��HitGroup�����������A���₷�\��ł��B
	/// </summary>
	class HitGroupMgr : public ISingleton<HitGroupMgr>{

	private:

		/*===== �����o�ϐ� =====*/

		std::vector<std::shared_ptr<HitGroup>> m_hitGroup;	//�q�b�g�O���[�v


	public:

		/// <summary>
		/// �q�b�g�O���[�v����ID
		/// </summary>
		enum HITGROUP_ID {

			DEF,
			MAX_HIT_GROUP,

		};

		/// <summary>
		/// �q�b�g�O���[�v����ID�A������
		/// </summary>
		::std::vector<LPCWSTR> hitGroupNames;


	public:

		/*===== �����o�֐� =====*/

		/// <summary>
		/// �Z�b�e�B���O����
		/// </summary>
		void Setting();

		/// <summary>
		/// �z�񐔂̃Q�b�^
		/// </summary>
		/// <returns> �q�b�g�O���[�v�̐� </returns>
		int GetHitGroupCount() { return static_cast<int>(m_hitGroup.size()); }

		//CH�֘A�̃Q�b�^
		const LPCWSTR& GetCH(HITGROUP_ID arg_index);
		bool GetCHFlag(HITGROUP_ID arg_index);
		//AH�֘A�̃Q�b�^
		const LPCWSTR& GetAH(HITGROUP_ID arg_index);
		bool GetAHFlag(HITGROUP_ID arg_index);
		//IS�֘A�̃Q�b�^
		const LPCWSTR& GetIS(HITGROUP_ID arg_index);
		bool GetISFlag(HITGROUP_ID arg_index);
		//�e��v�f���̃Q�b�^
		int GetSRVCount(HITGROUP_ID arg_index);
		int GetCBVCount(HITGROUP_ID arg_index);
		int GetUAVCount(HITGROUP_ID arg_index);
		//���[�J�����[�g�V�O�l�`���̃Q�b�^
		const ::std::shared_ptr<RayRootsignature> GetLocalRootSig(HITGROUP_ID arg_index);
		//�q�b�g�O���[�v������q�b�g�O���[�vID���擾����B
		int GetHitGroupID(const LPCWSTR& arg_hitGroupName);

		//�q�b�g�O���[�v��SRV�̐����擾����B
		int GetHitGroupSRVCount(HITGROUP_ID arg_hitGroupID);
		//�q�b�g�O���[�v��UAV�̐����擾����B
		int GetHitGroupUAVCount(HITGROUP_ID arg_hitGroupID);

	};

}