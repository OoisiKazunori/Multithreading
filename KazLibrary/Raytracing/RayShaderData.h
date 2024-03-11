#pragma once
#include <d3d12.h>
#include "../dxcapi.h"
#include <string>
#include <wrl.h>
#include <vector>

namespace Raytracing {

	//�V�F�[�_�[�f�[�^��ۑ����邽�߂̃f�[�^�N���X
	class RayShaderData {

	private:

		/*-- �����o�ϐ� --*/

		Microsoft::WRL::ComPtr<ID3DBlob> m_shaderBlob;	//�V�F�[�_�[�̃f�[�^
		Microsoft::WRL::ComPtr<IDxcBlob> m_shaderBlobDxc;//�V�F�[�_�[�̃f�[�^ dxc�p
		std::vector<char> m_shaderBin;					//dxc�ŃR���p�C�������ۂɏo�͂�����̕�����
		std::string m_entryPoint;						//�V�F�[�_�[�̃G���g���[�|�C���g
		std::string m_shaderModel;						//�V�F�[�_�[���f��
		std::string m_shaderPath;						//�V�F�[�_�[�̃t�@�C���p�X ������g���ăV�F�[�_�[�𔻒f����B


	public:

		/*-- �����o�֐� --*/

		//�R���X�g���N�^
		RayShaderData(const std::string& arg_shaderPath, const std::string& arg_entryPoint, const std::string& arg_shaderModel, bool arg_isDXC = false);

		//���[�h����
		void LoadShader();
		void LoadShaderDXC();

		//�V�F�[�_�[�̃t�@�C���p�X���擾���鏈��
		const std::string& GetShaderPath() { return m_shaderPath; }
		//�V�F�[�_�[�̃f�[�^���擾���鏈��
		const Microsoft::WRL::ComPtr<ID3DBlob>& GetShaderBlob() { return m_shaderBlob; }
		const Microsoft::WRL::ComPtr<IDxcBlob>& GetShaderBlobDXC() { return m_shaderBlobDxc; }
		std::vector<char>& GetShaderBin() { return m_shaderBin; }

	private:

		std::wstring StringToWString(std::string arg_OString);

	};

}