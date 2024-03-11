#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>
#include <string>
#include <wrl/client.h>
#include "../Helper/ISinglton.h"
#include "dxcapi.h"

namespace Raytracing {

	// �V�F�[�_�[�f�[�^�N���X�̑O���錾
	class RayShaderData;

	/// <summary>
	/// �V�F�[�_�[�f�[�^�ۑ��N���X ���C�g���p�Ɏ����Ă��܂����B
	/// </summary>
	class RayShaderStorage : public ISingleton<RayShaderStorage> {

	private:

		/*-- �����o�ϐ� --*/

		std::vector<std::shared_ptr<RayShaderData>> m_shaderData;


	public:

		/*-- �����o�֐� --*/

		//�V�F�[�_�[�����[�h����B
		Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint);
		Microsoft::WRL::ComPtr<ID3DBlob> LoadShaderForDXC(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint);

		//�V�F�[�_�[�f�[�^��Ԃ��B
		Microsoft::WRL::ComPtr<ID3DBlob> GetShaderData(const std::string& arg_shaderPath);
		Microsoft::WRL::ComPtr<IDxcBlob> GetShaderDataForDXC(const std::string& arg_shaderPath);
		std::vector<char>& GetShaderBin(const std::string& arg_shaderPath);

	};


}