#include "ModelLoader.h"
#include"Helper/OutPutDebugStringAndCheckResult.h"
#include"../KazLibrary/Buffer/VertexBufferMgr.h"
#include"../KazLibrary/Helper/KazHelper.h"
#include"../KazLibrary/Singlton/ProgressBar.h"

ModelLoader::ModelLoader()
{
}

std::shared_ptr<ModelInfomation> ModelLoader::LoadModel(std::string arg_fileDir, std::string arg_fileName)
{
	//���d���[�h�h�~
	bool isAnimationFlag = false;
	for (int i = 0; i < m_modelCacheArray.size(); ++i)
	{
		const bool IS_SAME_FLAG = m_modelCacheArray[i].m_fileName == arg_fileDir + arg_fileName;
		//�A�j���[�V�����������Ă��郂�f���̓��C�g���̕`��̓s����d���͋���
		if (IS_SAME_FLAG)
		{
			return m_modelArray[i];
		}
		//�ǂݍ��ݑΏۂ����łɓǂݍ��܂�Ă��ăA�j���[�V�����t���Ȃ�A���̌�̃L���b�V�����ǉ����Ȃ��悤�ɂ���
		if (IS_SAME_FLAG && m_modelCacheArray[i].m_hasAnimationFlag)
		{
			isAnimationFlag = true;
		}
	}


	std::shared_ptr<Skeleton> skelton = std::make_shared<Skeleton>();
	std::vector<ModelMeshData> modelData;
	modelData = glTFLoad.Load(arg_fileName, arg_fileDir, skelton.get());

	//��������Ă��邩�m�F
	if (modelData.back().vertexData.verticesArray.size() <= 0 || modelData.back().vertexData.indexArray.size() <= 0)
	{
		ErrorCheck(arg_fileDir + arg_fileName + "�̓ǂݍ��݂Ɏ��s���܂���\n");
		assert(1);
	}
	SucceedCheck(arg_fileDir + arg_fileName + "�̓ǂݍ��݂ɐ������܂���\n");

	//���d���[�h�h�~�p�Ƀ��f���̖��O�o�^
	if (!isAnimationFlag)
	{
		m_modelCacheArray.emplace_back(ModelCacheData(arg_fileDir + arg_fileName, 4 <= skelton->bones.size()));
	}

	std::vector<VertexAndIndexGenerateData> vertArray;
	m_modelVertexDataArray.emplace_back();
	int dex = 0;
	bool vramFlag = true;
	for (const auto& meshData : modelData)
	{
		//�A�j���[�V�����Ȃ�
		if (skelton->animations.size() == 0)
		{
			std::vector<VertexBufferData>vertexData = GetVertexDataArray(meshData.vertexData, meshData.vertexData.indexArray);
			m_modelVertexDataArray.back().m_vertexDataArray.emplace_back(vertexData);
			//���_�o�b�t�@�����p�̏����X�^�b�N����B
			VertexAndIndexGenerateData vertData(m_modelVertexDataArray.back().m_vertexDataArray[dex].data(), sizeof(VertexBufferData), vertexData.size(), sizeof(vertexData[0]), meshData.vertexData.indexArray);
			vertArray.emplace_back(vertData);
			++dex;
		}
		//�A�j���[�V��������
		else
		{
			std::vector<VertexBufferAnimationData>vertexData = GetVertexAnimationDataArray(meshData.vertexData, meshData.vertexData.indexArray);
			m_modelVertexDataArray.back().m_vertexAnimationDataArray.emplace_back(vertexData);
			//���_�o�b�t�@�����p�̏����X�^�b�N����B
			VertexAndIndexGenerateData vertData(m_modelVertexDataArray.back().m_vertexAnimationDataArray[dex].data(), sizeof(VertexBufferAnimationData), vertexData.size(), sizeof(vertexData[0]), meshData.vertexData.indexArray);
			vertArray.emplace_back(vertData);
			++dex;
			vramFlag = true;
		}
	}
	//���_�o�b�t�@����
	RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GenerateBuffer(vertArray, vramFlag);
	m_modelArray.emplace_back(std::make_shared<ModelInfomation>(modelData, handle, skelton));

	return m_modelArray.back();
}

std::vector<VertexBufferData> ModelLoader::GetVertexDataArray(const VertexData& data)
{
	std::vector<VertexBufferData>result(data.indexArray.size());

	for (int i = 0; i < result.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
		result[i].tangent = data.tangentArray[i].ConvertXMFLOAT3();
		result[i].binormal = data.binormalArray[i].ConvertXMFLOAT3();
	}
	return result;
}

std::vector<VertexBufferData> ModelLoader::GetVertexDataArray(const VertexData& data, const std::vector<UINT>& indexArray)
{
	std::vector<VertexBufferData>result(data.verticesArray.size());

	bool skipFlag = 0 < data.tangentArray.size();

	for (int i = 0; i < data.verticesArray.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
		if (skipFlag)
		{
			result[i].tangent = data.tangentArray[i].ConvertXMFLOAT3();
			result[i].binormal = data.binormalArray[i].ConvertXMFLOAT3();
		}
		else
		{
			result[i].tangent = {};
			result[i].binormal = {};
		}
	}
	return result;
}

std::vector<VertexBufferAnimationData> ModelLoader::GetVertexAnimationDataArray(const VertexData& data, const std::vector<UINT>& indexArray)
{
	std::vector<VertexBufferAnimationData>result(data.verticesArray.size());

	bool skipFlag = 0 < data.tangentArray.size();

	for (int i = 0; i < data.verticesArray.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
		if (skipFlag)
		{
			result[i].tangent = data.tangentArray[i].ConvertXMFLOAT3();
			result[i].binormal = data.binormalArray[i].ConvertXMFLOAT3();
		}
		else
		{
			result[i].tangent = {};
			result[i].binormal = {};
		}
		result[i].boneNo = { data.boneIdx[i].x,data.boneIdx[i].y,data.boneIdx[i].z,data.boneIdx[i].a };
		result[i].weight = data.boneWeight[i].ConvertXMFLOAT4();
	}
	return result;
}

std::shared_ptr<ModelInfomation> ModelLoader::Load(std::string arg_fileDir, std::string arg_fileName)
{
	//���d���[�h�h�~
	bool isAnimationFlag = false;
	for (int i = 0; i < m_modelCacheArray.size(); ++i)
	{
		const bool IS_SAME_FLAG = m_modelCacheArray[i].m_fileName == arg_fileDir + arg_fileName;
		//�A�j���[�V�����������Ă��郂�f���̓��C�g���̕`��̓s����d���͋���
		if (IS_SAME_FLAG)
		{
			return m_modelArray[i];
		}
		//�ǂݍ��ݑΏۂ����łɓǂݍ��܂�Ă��ăA�j���[�V�����t���Ȃ�A���̌�̃L���b�V�����ǉ����Ȃ��悤�ɂ���
		if (IS_SAME_FLAG && m_modelCacheArray[i].m_hasAnimationFlag)
		{
			isAnimationFlag = true;
		}
	}
	m_modelCacheArray.emplace_back(ModelCacheData(arg_fileDir + arg_fileName, false));

	m_loadNameArray.emplace_back(arg_fileDir, arg_fileName);
	m_modelArray.emplace_back(std::make_shared<ModelInfomation>(VertexBufferMgr::Instance()->RegisterGenerateHandle()));
	return m_modelArray.back();
}

void ModelLoader::LoadAllModel()
{
	int handle = 0;
	for (auto& obj : m_loadNameArray)
	{
		std::shared_ptr<Skeleton> skelton = std::make_shared<Skeleton>();
		std::vector<ModelMeshData> modelData;
		modelData = glTFLoad.Load(obj.m_fileName, obj.m_fileDir, skelton.get());

		//��������Ă��邩�m�F
		if (modelData.back().vertexData.verticesArray.size() <= 0 || modelData.back().vertexData.indexArray.size() <= 0)
		{
			ErrorCheck(obj.m_fileDir + obj.m_fileName + "�̓ǂݍ��݂Ɏ��s���܂���\n");
			assert(1);
		}
		SucceedCheck(obj.m_fileDir + obj.m_fileName + "�̓ǂݍ��݂ɐ������܂���\n");

		std::vector<VertexAndIndexGenerateData> vertArray;
		m_modelVertexDataArray.emplace_back();
		int dex = 0;
		bool vramFlag = true;
		for (const auto& meshData : modelData)
		{
			//�A�j���[�V�����Ȃ�
			if (skelton->animations.size() == 0)
			{
				std::vector<VertexBufferData>vertexData = GetVertexDataArray(meshData.vertexData, meshData.vertexData.indexArray);
				m_modelVertexDataArray.back().m_vertexDataArray.emplace_back(vertexData);
				//���_�o�b�t�@�����p�̏����X�^�b�N����B
				VertexAndIndexGenerateData vertData(m_modelVertexDataArray.back().m_vertexDataArray[dex].data(), sizeof(VertexBufferData), vertexData.size(), sizeof(vertexData[0]), meshData.vertexData.indexArray);
				vertArray.emplace_back(vertData);
				++dex;
			}
			//�A�j���[�V��������
			else
			{
				std::vector<VertexBufferAnimationData>vertexData = GetVertexAnimationDataArray(meshData.vertexData, meshData.vertexData.indexArray);
				m_modelVertexDataArray.back().m_vertexAnimationDataArray.emplace_back(vertexData);
				//���_�o�b�t�@�����p�̏����X�^�b�N����B
				VertexAndIndexGenerateData vertData(m_modelVertexDataArray.back().m_vertexAnimationDataArray[dex].data(), sizeof(VertexBufferAnimationData), vertexData.size(), sizeof(vertexData[0]), meshData.vertexData.indexArray);
				vertArray.emplace_back(vertData);
				++dex;
				vramFlag = true;
			}
		}
		//���_�o�b�t�@����
		VertexBufferMgr::Instance()->RegisterGenerateData(vertArray, vramFlag, m_modelArray[handle]->modelVertDataHandle);
		m_modelArray[handle]->modelData = modelData;
		m_modelArray[handle]->skelton = skelton;
		++handle;
		ProgressBar::Instance()->IncreNowNum(1);
	}
}

void ModelLoader::Release()
{
	ReleaseArray(m_modelArray);
	ReleaseArray(m_loadNameArray);
	ReleaseArray(m_modelCacheArray);
}

std::vector<ModelMeshData> GLTFLoader::Load(std::string fileName, std::string fileDir, Skeleton* skelton)
{
	//std::string filepass("Resource/Test/Plane/plane.glb");
	//std::string Ext(".glb");
	std::string FileDir(fileDir);
	std::string filepass(FileDir + fileName);
	std::string Ext("." + KazHelper::GetExtension(fileName));

	//GLTFSDK������p---------------------------------------
	//https://github.com/microsoft/glTF-SDK/blob/master/GLTFSDK.Samples/Deserialize/Source/main.cpp
	auto modelFilePath = std::experimental::filesystem::path(filepass);
	if (modelFilePath.is_relative())
	{
		auto current = std::experimental::filesystem::current_path();
		current /= modelFilePath;
		current.swap(modelFilePath);
	}
	auto reader = std::make_unique<StreamReader>(modelFilePath.parent_path());
	auto stream = reader->GetInputStream(modelFilePath.filename().string());

	std::string manifest;

	auto MakePathExt = [](const std::string& ext)
		{
			return "." + ext;
		};

	std::unique_ptr<Microsoft::glTF::GLTFResourceReader> resourceReader;

	if (Ext == MakePathExt(Microsoft::glTF::GLTF_EXTENSION))
	{
		auto gltfResourceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(reader));

		std::stringstream manifestStream;

		// Read the contents of the glTF file into a string using a std::stringstream
		manifestStream << stream->rdbuf();
		manifest = manifestStream.str();

		resourceReader = std::move(gltfResourceReader);
	}
	else if (Ext == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
	{
		auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(reader), std::move(stream));

		manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

		resourceReader = std::move(glbResourceReader);
	}

	assert(resourceReader);

	Microsoft::glTF::Document doc;
	try
	{
		doc = Microsoft::glTF::Deserialize(manifest);
	}
	catch (const Microsoft::glTF::GLTFException& ex)
	{
		std::stringstream ss;
		ss << "Microsoft::glTF::Deserialize failed: ";
		ss << ex.what();
		throw std::runtime_error(ss.str());
	}

	//PrintDocumentInfo(doc);
	//PrintResourceInfo(doc, *resourceReader);

	//https://github.com/microsoft/glTF-SDK/blob/master/GLTFSDK.Samples/Deserialize/Source/main.cpp
	//GLTFSDK������p---------------------------------------


	std::vector<Microsoft::glTF::Node>node;
	std::vector<DirectX::XMMATRIX> worldMat;
	//�m�[�h�̓ǂݍ���
	for (const auto& gltfNode : doc.nodes.Elements())
	{
		//���[�J���s��v�Z
		DirectX::XMVECTOR rotation = { gltfNode.rotation.x,gltfNode.rotation.y,gltfNode.rotation.z,gltfNode.rotation.w };
		DirectX::XMVECTOR scaling = { gltfNode.scale.x,gltfNode.scale.y,gltfNode.scale.z,1.0f };
		DirectX::XMVECTOR translation = { gltfNode.translation.x,gltfNode.translation.y,gltfNode.translation.z, 1.0f };

		DirectX::XMMATRIX matScaling, matRotaion, matTranslation;
		matScaling = DirectX::XMMatrixScalingFromVector(scaling);
		matRotaion = DirectX::XMMatrixRotationQuaternion(rotation);
		matTranslation = DirectX::XMMatrixTranslationFromVector(translation);

		auto nodeTransform = DirectX::XMMatrixIdentity();
		nodeTransform *= matScaling * matRotaion * matTranslation;

		//�X�L�����
		if (!gltfNode.skinId.empty())
		{
			node.emplace_back(gltfNode);
			continue;
		}

		if (!node.empty())break;

		skelton->bones.emplace_back();
		auto& bone = skelton->bones.back();
		bone.name = gltfNode.name;

		if (bone.name == "")
		{
			break;
		}
		for (auto& child : gltfNode.children)
		{
			auto childIdx = doc.nodes.GetIndex(child);
			skelton->bones[childIdx].parent = static_cast<char>(skelton->bones.size() - 1);
		}

		bone.invBindMat = XMMatrixInverse(nullptr, nodeTransform);
	}

	//�X�L���ǂݍ���
	for (const auto& gltfSkin : doc.skins.Elements())
	{
		auto invMatAcc = doc.accessors.Get(gltfSkin.inverseBindMatricesAccessorId);

		auto data = resourceReader->ReadFloatData(doc, invMatAcc);
		for (int matIdx = 0; matIdx < invMatAcc.count; ++matIdx)
		{
			int offset = matIdx * 16;
			DirectX::XMMATRIX invBindMat = DirectX::XMMatrixSet(
				data[offset], data[offset + 1], data[offset + 2], data[offset + 3],
				data[offset + 4], data[offset + 5], data[offset + 6], data[offset + 7],
				data[offset + 8], data[offset + 9], data[offset + 10], data[offset + 11],
				data[offset + 12], data[offset + 13], data[offset + 14], data[offset + 15]);

			int boneIdx = std::atoi(gltfSkin.jointIds[matIdx].c_str());
			skelton->bones[boneIdx].invBindMat = invBindMat;
		}
	}

	//�A�j���[�V����
	for (const auto& gltfAnimNode : doc.animations.Elements())
	{
		std::string animName = gltfAnimNode.name;
		auto& modelAnim = skelton->animations[animName];

		//�A�j���[�V�����̏ڍׂȏ������T���v���[�i�L�[�t���[���������X�g�A��Ԍ`���A�L�[�t���[���ɑΉ�����A�j���[�V�����f�[�^�j
		auto animSamplers = gltfAnimNode.samplers.Elements();

		//�S�Ẵ{�[���A�j���[�V�����I������
		int animEndFrame = 0;

		//�{�[���A�T���v���[�A�p�X�itranslation,rotation,scale�j�����蓖�Ă��Ă���
		for (const auto& animChannel : gltfAnimNode.channels.Elements())
		{
			const auto& sampler = animSamplers[gltfAnimNode.samplers.GetIndex(animChannel.samplerId)];
			//�Ή�����{�[���m�[�h�擾
			const auto& boneNode = doc.nodes.Get(animChannel.target.nodeId);
			//�{�[���P�ʂ̃A�j���[�V����
			auto& boneAnim = modelAnim.boneAnim[boneNode.name];

			//�A�j���[�V�������̃^�[�Q�b�g�iPOS�AROTATE�ASCALE�j
			auto& path = animChannel.target.path;

			//��ԕ��@�i����ALINER�ɂ����Ή����Ȃ��̂œ��ɏ����i�[���Ȃ��j
			auto interpolation = sampler.interpolation;

			//�J�n / �I���t���[��
			const auto& input = doc.accessors.Get(sampler.inputAccessorId);
			const int startFrame = static_cast<int>(input.min[0] * 60);	//�P�ʂ��b�Ȃ̂� 60f / 1sec �Ƃ��ăt���[���ɕϊ�
			const int endFrame = static_cast<int>(input.max[0] * 60);	//�P�ʂ��b�Ȃ̂� 60f / 1sec �Ƃ��ăt���[���ɕϊ�
			if (animEndFrame < endFrame)animEndFrame = endFrame;

			//�L�[�t���[�����
			auto keyFrames = resourceReader->ReadBinaryData<float>(doc, input);
			for (auto& keyFrame : keyFrames)keyFrame *= 60;	//�P�ʂ��b�Ȃ̂� 60f / 1sec �Ƃ��ăt���[���ɕϊ�

			//��̓I�Ȓl
			const auto& output = doc.accessors.Get(sampler.outputAccessorId);
			auto values = resourceReader->ReadBinaryData<float>(doc, output);

			//ErrorMessage("LoadGLTFModel()",
			//	path == Microsoft::glTF::TARGET_WEIGHTS || path == Microsoft::glTF::TARGET_UNKNOWN, "���̃A�j���[�V�����̃^�[�Q�b�g�̓T�|�[�g����ĂȂ���B");

			//Rotation(Quaternion)
			if (path == Microsoft::glTF::TARGET_ROTATION)
			{
				boneAnim.rotateAnim.startFrame = startFrame;
				boneAnim.rotateAnim.endFrame = endFrame;

				for (int keyFrameIdx = 0; keyFrameIdx < keyFrames.size(); ++keyFrameIdx)
				{
					boneAnim.rotateAnim.keyFrames.emplace_back();
					auto& keyFrame = boneAnim.rotateAnim.keyFrames.back();
					keyFrame.frame = static_cast<int>(keyFrames[keyFrameIdx]);
					int offset = keyFrameIdx * 4;	//�C���f�b�N�X�̃I�t�Z�b�g
					//keyFrame.value = XMVectorSet(values[offset], -values[offset + 1], -values[offset + 2], values[offset + 3]);
					keyFrame.value = DirectX::XMVectorSet(values[offset], values[offset + 1], values[offset + 2], values[offset + 3]);
				}
			}
			//Translation
			else if (path == Microsoft::glTF::TARGET_TRANSLATION)
			{
				boneAnim.posAnim.startFrame = startFrame;
				boneAnim.posAnim.endFrame = endFrame;

				for (int keyFrameIdx = 0; keyFrameIdx < keyFrames.size(); ++keyFrameIdx)
				{
					boneAnim.posAnim.keyFrames.emplace_back();
					auto& keyFrame = boneAnim.posAnim.keyFrames.back();
					keyFrame.frame = static_cast<int>(keyFrames[keyFrameIdx]);
					int offset = keyFrameIdx * 3;	//�C���f�b�N�X�̃I�t�Z�b�g
					keyFrame.value = { values[offset], values[offset + 1], values[offset + 2] };
				}
			}
			//Scale
			else
			{
				boneAnim.scaleAnim.startFrame = startFrame;
				boneAnim.scaleAnim.endFrame = endFrame;

				for (int keyFrameIdx = 0; keyFrameIdx < keyFrames.size(); ++keyFrameIdx)
				{
					boneAnim.scaleAnim.keyFrames.emplace_back();
					auto& keyFrame = boneAnim.scaleAnim.keyFrames.back();
					keyFrame.frame = static_cast<int>(keyFrames[keyFrameIdx]);
					int offset = keyFrameIdx * 3;	//�C���f�b�N�X�̃I�t�Z�b�g
					keyFrame.value = { values[offset], values[offset + 1], values[offset + 2] };
				}
			}
		}

		//�S�Ẵ{�[���̃A�j���[�V�����I�����ԋL�^
		modelAnim.finishTime = animEndFrame;
	}

	skelton->CreateBoneTree(DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f));

	std::vector<MaterialData> modelMaterialDataArray;
	//�}�e���A�����̓ǂݍ���
	for (const auto& material : doc.materials.Elements())
	{
		modelMaterialDataArray.emplace_back();

		auto texID = material.metallicRoughness.baseColorTexture.textureId;
		//Albedo�̎擾
		LoadMaterialTexture(&modelMaterialDataArray.back(), FileDir, texID, doc, GRAPHICS_PRAMTYPE_DATA);

		//�@���}�b�v�̎擾
		texID = material.normalTexture.textureId;
		LoadMaterialTexture(&modelMaterialDataArray.back(), FileDir, texID, doc, GRAPHICS_PRAMTYPE_DATA2);

		//���^���l�X�̎擾
		texID = material.metallicRoughness.metallicRoughnessTexture.textureId;
		LoadMaterialTexture(&modelMaterialDataArray.back(), FileDir, texID, doc, GRAPHICS_PRAMTYPE_DATA3);

		//�G�~�b�V�u�̎擾
		texID = material.emissiveTexture.textureId;
		LoadMaterialTexture(&modelMaterialDataArray.back(), FileDir, texID, doc, GRAPHICS_PRAMTYPE_DATA4);
	}
	//�}�e���A�����Ȃ��ꍇ�͑���̂��̂���������
	if (doc.materials.Elements().size() == 0)
	{
		modelMaterialDataArray.emplace_back();
		for (int i = 0; i < MATERIAL_TEXTURE_MAX; ++i)
		{
			modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + i)));
		}
	}

	//���f������̃��b�V���̉�
	std::vector<ModelMeshData> meshData;
	//���b�V���̓ǂݍ���
	for (const auto& meshes : doc.meshes.Elements())
	{
		//���b�V���̖��O��ۑ�
		const auto meshName = meshes.name;

		//���b�V��ID���r���đΉ�����X�L���m�[�h���擾
		const Microsoft::glTF::Skin* gltfSkin = nullptr;
		const auto& meshId = meshes.id;
		const auto& skinNode = std::find_if(node.begin(), node.end(), [meshId](Microsoft::glTF::Node& node)
			{
				return node.meshId == meshId;
			});
		if (skinNode != node.end())gltfSkin = &doc.skins.Get(skinNode->skinId);

		//�A�N�Z�T�[�̏���ۑ�(�A�N�Z�T�[��bufferView���ǂ̂悤�ȐU�镑�������邩��`���ꂽ���́A�o�C�i���[�̏���ǂݎ��ׂɕK�v)
		Microsoft::glTF::IndexedContainer<const Microsoft::glTF::Accessor> accsessor(doc.accessors);

		//�v���~�e�B�u�ɂ�mesh�̃����_�����O�ɕK�v�ȃW�I���g����������
		for (const auto& primitive : meshes.primitives)
		{
			//���b�V������̏��
			VertexData vertexInfo;
			/*
			1...Attribute(���_���)��������Ɏw�肵�����O�����ɃA�N�Z�T�[�����o���ׂ�ID����ɓ����B
			2...ID����A�N�Z�T�[�����o��
			3...�t�@�C���̏��ƃA�N�Z�T�[����o�C�i���[�ɓ����Ă���w��̏������o���B
			���_�A�@���AUV���A��̂����Ńf�[�^�����o���B
			�A�N�Z�T�[��bufferview�Ƃ��̉��̊K�w�ɂ���buffer�̐U�镑�������鎖���o����̂ŁA�o�C�i���[���̎w��̏���T�����鎖���o����B
			*/
			//���_���̓���---------------------------------------
			auto& idPos = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION);
			auto& accPos = accsessor.Get(idPos);
			auto& vertPos = resourceReader->ReadBinaryData<float>(doc, accPos);
			//���_���̓���---------------------------------------

			//�@�����̓���---------------------------------------
			auto& idNormal = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_NORMAL);
			auto& accNormal = accsessor.Get(idNormal);
			auto& normal = resourceReader->ReadBinaryData<float>(doc, accNormal);
			//�@�����̓���---------------------------------------

			//UV���W���̓���---------------------------------------
			auto& idUV = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TEXCOORD_0);
			auto& accUV = accsessor.Get(idUV);
			auto& uv = resourceReader->ReadBinaryData<float>(doc, accUV);
			//UV���W���̓���---------------------------------------


			std::vector<uint8_t>vertJoint;
			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_JOINTS_0))
			{
				auto& idJoint = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_JOINTS_0);
				auto& accJoint = doc.accessors.Get(idJoint);
				vertJoint = resourceReader->ReadBinaryData<uint8_t>(doc, accJoint);
			}

			std::vector<float>vertWeight;
			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_WEIGHTS_0))
			{
				auto& idWeight = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_WEIGHTS_0);
				auto& accWeight = doc.accessors.Get(idWeight);
				vertWeight = resourceReader->ReadBinaryData<float>(doc, accWeight);
			}


			//��Ŏ�ɓ��ꂽ�������Ɉ�̃��b�V������ǉ�---------------------------------------

			const auto VERT_MAX_COUNT = accPos.count;
			for (int vertCount = 0; vertCount < VERT_MAX_COUNT; ++vertCount)
			{
				const int TRIANGLE_VERT_NUM = 3;
				const int UV_VERT_NUM = 2;
				int jid0 = 4 * vertCount, jid1 = 4 * vertCount + 1, jid2 = 4 * vertCount + 2, jid3 = 4 * vertCount + 3;

				//�O�p�ʂɂȂ�悤�ɃC���f�b�N�X�����߂�
				KazMath::Vec3<int>vertIndex(GetVertIndex(vertCount, TRIANGLE_VERT_NUM));
				KazMath::Vec3<int>uvIndex(GetVertIndex(vertCount, UV_VERT_NUM));

				vertexInfo.verticesArray.emplace_back(KazMath::Vec3<float>(vertPos[vertIndex.x], vertPos[vertIndex.y], vertPos[vertIndex.z]));
				vertexInfo.normalArray.emplace_back(KazMath::Vec3<float>(normal[vertIndex.x], normal[vertIndex.y], normal[vertIndex.z]));
				vertexInfo.uvArray.emplace_back(KazMath::Vec2<float>(uv[uvIndex.x], uv[uvIndex.y]));

				if (!vertWeight.empty())
				{
					vertexInfo.boneWeight.emplace_back(KazMath::Vec4<float>(vertWeight[jid0], vertWeight[jid1], vertWeight[jid2], vertWeight[jid3]));
				}
				if (!vertJoint.empty())
				{
					//�{�[���������Ȃ��ꍇ -1 �̂܂܂ɂ���K�v������
					auto joint0 = atoi(gltfSkin->jointIds[vertJoint[jid0]].c_str());
					auto joint1 = atoi(gltfSkin->jointIds[vertJoint[jid1]].c_str());
					auto joint2 = atoi(gltfSkin->jointIds[vertJoint[jid2]].c_str());
					auto joint3 = atoi(gltfSkin->jointIds[vertJoint[jid3]].c_str());

					vertexInfo.boneIdx.emplace_back();
					if (vertexInfo.boneWeight.back().x)vertexInfo.boneIdx.back().x = static_cast<int>(joint0);
					if (vertexInfo.boneWeight.back().y)vertexInfo.boneIdx.back().y = static_cast<int>(joint1);
					if (vertexInfo.boneWeight.back().z)vertexInfo.boneIdx.back().z = static_cast<int>(joint2);
					if (vertexInfo.boneWeight.back().a)vertexInfo.boneIdx.back().a = static_cast<int>(joint3);
				}
			}


			//�^���W�F���g��Ԃ̌v�Z...�|���S���̕\�ʂ𒆐S�ɁAUV.x�̌���=+X���AUV.y�̌���=+Y���A�@���̌�����+Z���Ƃ������
			//(�Q�l�T�C�g)https://coposuke.hateblo.jp/entry/2020/12/21/144327)
			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_TANGENT))
			{
				auto& tangentID = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TANGENT);
				auto& accTangent = accsessor.Get(tangentID);
				auto& vertTangent = resourceReader->ReadBinaryData<float>(doc, accTangent);

				for (int i = 0; i < vertexInfo.verticesArray.size(); ++i)
				{
					const int TRIANGLE_VERT_NUM = 3;
					KazMath::Vec3<int>vertIndex(GetVertIndex(i, TRIANGLE_VERT_NUM));
					//�^���W�F���g��Ԃ�U(tangent)V(binormal)���W�𓾂�B
					KazMath::Vec3<float>tangent(vertTangent[vertIndex.x], vertTangent[vertIndex.y], vertTangent[vertIndex.z]);
					//�@���Ɛڐ�̊O�ς��瓾����]�@��(binormal)�𓾂�
					KazMath::Vec3<float>binNormal(vertexInfo.normalArray[i].Cross(tangent));
					vertexInfo.tangentArray.emplace_back(tangent);
					vertexInfo.binormalArray.emplace_back(binNormal);
				}
			}



			//�A�N�Z�T�[����C���f�b�N�X����(???)���擾
			auto accIndex(accsessor.Get(primitive.indicesAccessorId));
			//�C���f�b�N�X�f�[�^��ǂݎ��
			std::vector<uint16_t>indices = resourceReader->ReadBinaryData<uint16_t>(doc, accIndex);
			const auto INDEX_MAX_COUNT = accIndex.count;
			for (int indexCount = 0; indexCount < INDEX_MAX_COUNT; indexCount += 3)
			{
				vertexInfo.indexArray.emplace_back(static_cast<UINT>(indices[indexCount + 1]));
				vertexInfo.indexArray.emplace_back(static_cast<UINT>(indices[indexCount + 0]));
				vertexInfo.indexArray.emplace_back(static_cast<UINT>(indices[indexCount + 2]));
			}

			//���b�V�����̒ǉ�
			vertexInfo.name = meshName;

			meshData.emplace_back();
			meshData.back().vertexData = vertexInfo;
			//�}�e���A��������̂��ǂ���(���݂�Albedo�̂ݑΉ�)
			if (doc.materials.Has(primitive.materialId))
			{
				int materialIndex = static_cast<int>(doc.materials.GetIndex(primitive.materialId));
				meshData.back().materialData = modelMaterialDataArray[materialIndex];
			}
			else
			{
				meshData.back().materialData = modelMaterialDataArray[0];
			}

			//��Ŏ�ɓ��ꂽ�������Ɉ�̃��b�V������ǉ�---------------------------------------
		}
	}

	return meshData;
}

void GLTFLoader::PrintDocumentInfo(const Microsoft::glTF::Document& document)
{
	// Asset Info
	std::cout << "Asset Version:    " << document.asset.version << "\n";
	std::cout << "Asset MinVersion: " << document.asset.minVersion << "\n";
	std::cout << "Asset Generator:  " << document.asset.generator << "\n";
	std::cout << "Asset Copyright:  " << document.asset.copyright << "\n\n";

	// Scene Info
	std::cout << "Scene Count: " << document.scenes.Size() << "\n";

	if (document.scenes.Size() > 0U)
	{
		std::cout << "Default Scene Index: " << document.GetDefaultScene().id << "\n\n";
	}
	else
	{
		std::cout << "\n";
	}

	// Entity Info
	std::cout << "Node Count:     " << document.nodes.Size() << "\n";
	std::cout << "Camera Count:   " << document.cameras.Size() << "\n";
	std::cout << "Material Count: " << document.materials.Size() << "\n\n";

	// Mesh Info
	std::cout << "Mesh Count: " << document.meshes.Size() << "\n";
	std::cout << "Skin Count: " << document.skins.Size() << "\n\n";

	// Texture Info
	std::cout << "Image Count:   " << document.images.Size() << "\n";
	std::cout << "Texture Count: " << document.textures.Size() << "\n";
	std::cout << "Sampler Count: " << document.samplers.Size() << "\n\n";

	// Buffer Info
	std::cout << "Buffer Count:     " << document.buffers.Size() << "\n";
	std::cout << "BufferView Count: " << document.bufferViews.Size() << "\n";
	std::cout << "Accessor Count:   " << document.accessors.Size() << "\n\n";

	// Animation Info
	std::cout << "Animation Count: " << document.animations.Size() << "\n\n";

	for (const auto& extension : document.extensionsUsed)
	{
		std::cout << "Extension Used: " << extension << "\n";
	}

	if (!document.extensionsUsed.empty())
	{
		std::cout << "\n";
	}

	for (const auto& extension : document.extensionsRequired)
	{
		std::cout << "Extension Required: " << extension << "\n";
	}

	if (!document.extensionsRequired.empty())
	{
		std::cout << "\n";
	}
}

void GLTFLoader::PrintResourceInfo(const Microsoft::glTF::Document& document, const Microsoft::glTF::GLTFResourceReader& resourceReader)
{
	// Use the resource reader to get each mesh primitive's position data
	for (const auto& mesh : document.meshes.Elements())
	{
		std::cout << "Mesh: " << mesh.id << "\n";

		for (const auto& meshPrimitive : mesh.primitives)
		{
			std::string accessorId;

			if (meshPrimitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION, accessorId))
			{
				const Microsoft::glTF::Accessor& accessor = document.accessors.Get(accessorId);

				const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
				const auto dataByteLength = data.size() * sizeof(float);

				std::cout << "MeshPrimitive: " << dataByteLength << " bytes of position data\n";
			}
		}

		std::cout << "\n";
	}

	// Use the resource reader to get each image's data
	for (const auto& image : document.images.Elements())
	{
		std::string filename;

		if (image.uri.empty())
		{
			assert(!image.bufferViewId.empty());

			auto& bufferView = document.bufferViews.Get(image.bufferViewId);
			auto& buffer = document.buffers.Get(bufferView.bufferId);

			filename += buffer.uri; //NOTE: buffer uri is empty if image is stored in GLB binary chunk
		}
		else if (Microsoft::glTF::IsUriBase64(image.uri))
		{
			filename = "Data URI";
		}
		else
		{
			filename = image.uri;
		}

		auto data = resourceReader.ReadBinaryData(document, image);

		std::cout << "Image: " << image.id << "\n";
		std::cout << "Image: " << data.size() << " bytes of image data\n";

		if (!filename.empty())
		{
			std::cout << "Image filename: " << filename << "\n\n";
		}
	}
}

void GLTFLoader::PrintInfo(const std::experimental::filesystem::path& path)
{
	// Pass the absolute path, without the filename, to the stream reader
	auto streamReader = std::make_unique<StreamReader>(path.parent_path());

	std::experimental::filesystem::path pathFile = path.filename();
	std::experimental::filesystem::path pathFileExt = pathFile.extension();

	std::string manifest;

	auto MakePathExt = [](const std::string& ext)
		{
			return "." + ext;
		};

	std::unique_ptr<Microsoft::glTF::GLTFResourceReader> resourceReader;

	// If the file has a '.gltf' extension then create a GLTFResourceReader
	if (pathFileExt == MakePathExt(Microsoft::glTF::GLTF_EXTENSION))
	{
		auto gltfStream = streamReader->GetInputStream(pathFile.u8string()); // Pass a UTF-8 encoded filename to GetInputString
		auto gltfResourceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(streamReader));

		std::stringstream manifestStream;

		// Read the contents of the glTF file into a string using a std::stringstream
		manifestStream << gltfStream->rdbuf();
		manifest = manifestStream.str();

		resourceReader = std::move(gltfResourceReader);
	}

	// If the file has a '.glb' extension then create a GLBResourceReader. This class derives
	// from GLTFResourceReader and adds support for reading manifests from a GLB container's
	// JSON chunk and resource data from the binary chunk.
	if (pathFileExt == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
	{
		auto glbStream = streamReader->GetInputStream(pathFile.u8string()); // Pass a UTF-8 encoded filename to GetInputString
		auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(streamReader), std::move(glbStream));

		manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

		resourceReader = std::move(glbResourceReader);
	}

	if (!resourceReader)
	{
		throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
	}

	Microsoft::glTF::Document document;

	try
	{
		document = Microsoft::glTF::Deserialize(manifest);
	}
	catch (const Microsoft::glTF::GLTFException& ex)
	{
		std::stringstream ss;

		ss << "Microsoft::glTF::Deserialize failed: ";
		ss << ex.what();

		throw std::runtime_error(ss.str());
	}

	std::cout << "### glTF Info - " << pathFile << " ###\n\n";

	PrintDocumentInfo(document);
	PrintResourceInfo(document, *resourceReader);
}

void GLTFLoader::LoadMaterialTexture(MaterialData* arg_material, std::string arg_fileDir, std::string arg_id, const Microsoft::glTF::Document& arg_doc, GraphicsRootParamType arg_rootParam)
{
	//�e�N�X�`���̎擾
	if (!arg_id.empty())
	{
		auto& texture = arg_doc.textures.Get(arg_id);
		auto& image = arg_doc.images.Get(texture.imageId);
		if (!image.uri.empty())
		{
			std::string textureFilePass(arg_fileDir + image.uri);
			std::shared_ptr<KazBufferHelper::BufferData> textureBuffer(TextureResourceMgr::Instance()->LoadGraphBufferForModel(textureFilePass));

			//�e�N�X�`���ǂݍ���
			arg_material->textureBuffer.emplace_back(textureBuffer);
			arg_material->textureBuffer.back()->rootParamType = arg_rootParam;

			//if (textureBuffer->bufferWrapper)
			//{	
			//}
			//else
			//{
			//	arg_material->textureBuffer.emplace_back(LoadErrorTex(arg_rootParam));
			//}
		}
		else
		{
			arg_material->textureBuffer.emplace_back(LoadErrorTex(arg_rootParam));
		}
	}
	else
	{
		arg_material->textureBuffer.emplace_back(LoadErrorTex(arg_rootParam));
	}
}

ModelInfomation::ModelInfomation(const std::vector<ModelMeshData>& model, RESOURCE_HANDLE vertHandle, const std::shared_ptr<Skeleton>& skel) :
	modelData(model), modelVertDataHandle(vertHandle), skelton(skel)
{
}

ModelInfomation::ModelInfomation(RESOURCE_HANDLE vertHandle) : modelVertDataHandle(vertHandle)
{
}

MaterialBufferData MaterialData::GetMaterialData()
{
	MaterialBufferData material;
	material.diffuse = { diffuse.x,diffuse.y,diffuse.z };
	material.ambient = { ambient.x,ambient.y,ambient.z };
	material.specular = { specular.x,specular.y,specular.z };
	material.alpha = 1.0f;
	return material;
}