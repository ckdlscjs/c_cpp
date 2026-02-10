#pragma once
#include "Resource.h"
class Geometry : public BaseResource<Geometry>
{
	friend class BaseResource<Geometry>;
public:
	Geometry(size_t hash, const std::wstring& szFilePath);
	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;
	Geometry(Geometry&&) = delete;
	Geometry& operator=(Geometry&&) = delete;

public:
	const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& GetVertices() { return m_verticesByMaterial; }
	const std::map<UINT, std::vector<UINT>>& GetIndices() { return m_indicesByMaterial; }
	const std::vector<std::vector<Vector3>>& GetPoints() { return m_pointsByMeshs; }
	std::map<UINT, MTL_TEXTURES>& GetTextures() { return m_texturesByMaterial; }
	std::vector<AnimationClip>& GetAnimationClip() { return m_AnimationClip; }
	std::unordered_map<std::string, Bone>& GetBones() { return m_BoneMap; }
	std::vector<std::tuple<int, std::string, std::string>>& GetBonesHierarchy() { return m_BoneHierarchy; }
		
private:
	void ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::map<UINT, std::unordered_map<Vertex_PTNTB_Skinned, UINT, Vertex_PTNTB_Skinned_Hash>>& verticesIdentical);
	void ProcessNode(const aiScene* scene, const aiNode* node, std::map<UINT, std::unordered_map<Vertex_PTNTB_Skinned, UINT, Vertex_PTNTB_Skinned_Hash>>& verticesIdentical);
	void ProcessAnim(const aiScene* scene);

private:
	std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> m_verticesByMaterial;
	std::map<UINT, std::vector<UINT>> m_indicesByMaterial;
	std::vector<std::vector<Vector3>> m_pointsByMeshs;
	std::map<UINT, MTL_TEXTURES> m_texturesByMaterial;
	std::vector<AnimationClip> m_AnimationClip;
	std::unordered_map<std::string, Bone> m_BoneMap;
	std::vector<std::tuple<int, std::string, std::string>> m_BoneHierarchy;
};

inline Geometry::Geometry(size_t hash, const std::wstring& szFilePath) : BaseResource(hash, szFilePath)
{
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
	std::string szFullPath = _towm(szFilePath);
	unsigned int readFlag =
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		//aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded;
	const aiScene* scene = importer.ReadFile(szFullPath, readFlag);
	_ASEERTION_NULCHK(scene, "aiScene nullptr");
	std::map<UINT, std::unordered_map<Vertex_PTNTB_Skinned, UINT, Vertex_PTNTB_Skinned_Hash>> verticesIdentical;
	ProcessNode(scene, scene->mRootNode, verticesIdentical);
	if (m_BoneMap.size())
	{
		m_BoneHierarchy.resize(m_BoneMap.size());
		std::function<void(const aiNode*, int)> traverse = [&](const aiNode* node, int idx)
			{
				std::string curName = node->mName.C_Str();
				if (m_BoneMap.find(curName) != m_BoneMap.end())
				{
					std::get<0>(m_BoneHierarchy[m_BoneMap[curName].idx]) = idx;
					std::get<1>(m_BoneHierarchy[m_BoneMap[curName].idx]) = idx == -1 ? "root" : std::get<2>(m_BoneHierarchy[idx]);
					std::get<2>(m_BoneHierarchy[m_BoneMap[curName].idx]) = curName;
					idx = m_BoneMap[curName].idx;
				}
				for (UINT i = 0; i < node->mNumChildren; i++)
				{
					traverse(node->mChildren[i], idx);
				}
			};
		traverse(scene->mRootNode, -1);
	}

	ProcessAnim(scene);
}


inline void Geometry::ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::map<UINT, std::unordered_map<Vertex_PTNTB_Skinned, UINT, Vertex_PTNTB_Skinned_Hash>>& verticesIdentical)
{
	if (mesh->mNumVertices <= 0) return;
	UINT matIdx = mesh->mMaterialIndex;
	std::vector<unsigned int> vertexLookup(mesh->mNumVertices);
	std::vector<Vector3> points;
	
	//Vertices, Points, Indicies
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		float vx, vy, vz, uvx, uvy, nx, ny, nz, tx, ty, tz, bx, by, bz;
		vx = vy = vz = uvx = uvy = nx = ny = nz = tx = ty = tz = bx = by = bz = 0.0f;
		vx = mesh->mVertices[i].x;
		vy = mesh->mVertices[i].y;
		vz = mesh->mVertices[i].z;

		if (mesh->HasTextureCoords(0))
		{
			uvx = (float)mesh->mTextureCoords[0][i].x;
			uvy = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			nx = mesh->mNormals[i].x;
			ny = mesh->mNormals[i].y;
			nz = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			tx = mesh->mTangents[i].x;
			ty = mesh->mTangents[i].y;
			tz = mesh->mTangents[i].z;
			bx = mesh->mBitangents[i].x;
			by = mesh->mBitangents[i].y;
			bz = mesh->mBitangents[i].z;
		}
		Vertex_PTNTB_Skinned vertex{ { vx, vy, vz }, { uvx, uvy }, { nx, ny, nz }, {tx, ty, tz}, {bx, by, bz } };
		
		auto it = verticesIdentical[matIdx].find(vertex);
		if (it == verticesIdentical[matIdx].end())
		{
			vertexLookup[i] = (UINT)m_verticesByMaterial[matIdx].size();
			verticesIdentical[matIdx][vertex] = (UINT)m_verticesByMaterial[matIdx].size();
			m_verticesByMaterial[matIdx].push_back(vertex);
			points.push_back({ vx, vy, vz });
		}
		else
		{
			vertexLookup[i] = it->second;
		}
	}
	
	m_pointsByMeshs.push_back(points);

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			m_indicesByMaterial[matIdx].push_back(vertexLookup[face.mIndices[j]]);
	}

	//Bones
	if (mesh->mNumBones > 0)
	{
		//본을 순회하면서 정점별로 사용될 본의인덱싱, 가중치값 초기화
		std::vector<std::vector<IWInfo>> influenceIWs(m_verticesByMaterial[matIdx].size(), std::vector<IWInfo>());
		for (UINT i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* pBone = mesh->mBones[i];
			std::string boneName = pBone->mName.C_Str();
			UINT boneIndex = 0;

			// 뼈가 처음 발견된 것이라면 목록에 추가
			if (m_BoneMap.find(boneName) == m_BoneMap.end())
			{
				boneIndex = (UINT)m_BoneMap.size();
				m_BoneMap[boneName].idx = boneIndex;
				m_BoneMap[boneName].matOffset = Matrix4x4(pBone->mOffsetMatrix);
			}
			else
			{
				boneIndex = m_BoneMap[boneName].idx;
			}

			// 해당 뼈가 영향을 주는 정점들을 순회하며 가중치 기입
			for (UINT j = 0; j < pBone->mNumWeights; j++)
			{
				UINT vertexID = pBone->mWeights[j].mVertexId;
				float weight = pBone->mWeights[j].mWeight;
				influenceIWs[vertexLookup[vertexID]].push_back({ boneIndex, weight });
			}
		}

		//수집된 데이터를 정규화(최대4, 가중치합1.0)
		for (UINT i = 0; i < m_verticesByMaterial[matIdx].size(); i++)
		{
			auto& weightList = influenceIWs[i];

			//가중치 크기순으로 내림차순 정렬
			std::sort(weightList.begin(), weightList.end(),
				[](const IWInfo& a, const IWInfo& b) {
					return a.weight > b.weight;
				});

			float totalWeight = 0.0f;

			//최대 4개의 뼈 영향력만 선택하여 임시 합계 계산
			UINT activeBones = (UINT)std::min(weightList.size(), (size_t)4);

			for (UINT j = 0; j < activeBones; j++)
			{
				totalWeight += weightList[j].weight;
			}

			//정규화 및 데이터 기입
			// 각 가중치를 totalWeight로 나누어 합이 1.0이 되도록 보정
			// 남는 슬롯은 0으로 채움
			if (totalWeight > 0.0f)
			{
				for (UINT j = 0; j < 4; j++)
				{
					if (j < activeBones)
					{
						m_verticesByMaterial[matIdx][i].bones[j] = weightList[j].boneIdx;
						m_verticesByMaterial[matIdx][i].weights[j] = weightList[j].weight / totalWeight;
					}
					else
					{
						m_verticesByMaterial[matIdx][i].bones[j] = 0;
						m_verticesByMaterial[matIdx][i].weights[j] = 0.0f;
					}
				}
			}
			else
			{
				// 만약 가중치 정보가 아예 없는 정점이라면 기본값 설정 (첫 번째 뼈에 100%)
				m_verticesByMaterial[matIdx][i].bones[0] = 0;
				m_verticesByMaterial[matIdx][i].weights[0] = 1.0f;
				for (int j = 1; j < 4; ++j) 
				{
					m_verticesByMaterial[matIdx][i].bones[j] = 0;
					m_verticesByMaterial[matIdx][i].weights[j] = 0.0f;
				}
			}
		}
		
	}

	if (matIdx >= 0)
	{
		aiMaterial* pMaterial = scene->mMaterials[mesh->mMaterialIndex];
		m_texturesByMaterial[mesh->mMaterialIndex].szMatName = pMaterial->GetName().C_Str();
		//aiTextureType_GLTF_METALLIC_ROUGHNESS is last(27)
		for (UINT type = 0; type <= aiTextureType_GLTF_METALLIC_ROUGHNESS; type++)
		{
			aiTextureType texType = (aiTextureType)type;
			for (UINT idx = 0; idx < pMaterial->GetTextureCount(texType); idx++)
			{
				aiString texPath;
				if (pMaterial->GetTexture(texType, idx, &texPath) == aiReturn_SUCCESS)
				{
					const aiTexture* pEmbeddedTexture = scene->GetEmbeddedTexture(texPath.C_Str());
					if (pEmbeddedTexture)
					{
						/*
						* mHeight가 0이면 데이터가 압축된 상태(png, jpg 등)임을 의미
						* mHeight == 0일 때: mWidth = Byte Size (파일 용량)
						* mHeight > 0일 때 : mWidth = Pixel Width(이미지 너비)
						*/
						if (pEmbeddedTexture->mHeight == 0)
						{
							ScratchImage srcatchImage;
							LoadFromWICMemory(
								reinterpret_cast<const uint8_t*>(pEmbeddedTexture->pcData), 
								static_cast<size_t>(pEmbeddedTexture->mWidth), 
								DirectX::WIC_FLAGS_NONE,
								nullptr,
								srcatchImage);
							m_texturesByMaterial[mesh->mMaterialIndex].type_textures_image[texType].push_back(std::move(srcatchImage));
						}
					}
					//외부경로저장
					m_texturesByMaterial[mesh->mMaterialIndex].type_textures_path[texType].push_back(texPath.C_Str());
				}
			}
		}
	}
}

inline void Geometry::ProcessNode(const aiScene* scene, const aiNode* node, std::map<UINT, std::unordered_map<Vertex_PTNTB_Skinned, UINT, Vertex_PTNTB_Skinned_Hash>>& verticesIdentical)
{
	Matrix4x4 matRelative(node->mTransformation);
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		ProcessMesh(scene, scene->mMeshes[node->mMeshes[i]], verticesIdentical);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(scene, node->mChildren[i], verticesIdentical);
	}
}

//AnimationClip(애니메이션별)에 뼈대정보별로 각 갯수(프레임갯수)에맞게끔 S,R,T를 구성한다
inline void Geometry::ProcessAnim(const aiScene* scene)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++) 
	{
		aiAnimation* aiAnim = scene->mAnimations[i];
		AnimationClip animClip;
		animClip.szName = aiAnim->mName.C_Str();
		animClip.fDuration = (float)aiAnim->mDuration;
		animClip.fTicksPerSecond = (float)aiAnim->mTicksPerSecond != 0 ? (float)aiAnim->mTicksPerSecond : 25.0f;

		for (UINT j = 0; j < aiAnim->mNumChannels; j++) 
		{
			aiNodeAnim* channel = aiAnim->mChannels[j];
			std::string boneName = channel->mNodeName.C_Str();

			for (UINT k = 0; k < channel->mNumScalingKeys; k++)
			{
				KeyFrame_Scale kfScale;
				kfScale.fTime = (float)channel->mScalingKeys[k].mTime;
				kfScale.vScale = Vector3(channel->mScalingKeys[k].mValue.x, channel->mScalingKeys[k].mValue.y, channel->mScalingKeys[k].mValue.z);
				animClip.boneFrames_Scale[boneName].push_back(kfScale);
			}

			for (UINT k = 0; k < channel->mNumRotationKeys; k++)
			{
				KeyFrame_Rotate kfRotate;
				kfRotate.fTime = (float)channel->mRotationKeys[k].mTime;
				kfRotate.qRotate = Quarternion(channel->mRotationKeys[k].mValue.w, channel->mRotationKeys[k].mValue.x, channel->mRotationKeys[k].mValue.y, channel->mRotationKeys[k].mValue.z);
				animClip.boneFrames_Rotate[boneName].push_back(kfRotate);
			}
			
			for (UINT k = 0; k < channel->mNumPositionKeys; k++)
			{
				KeyFrame_Position kfPosition;
				kfPosition.fTime = (float)channel->mPositionKeys[k].mTime;
				kfPosition.vPosition = Vector3(channel->mPositionKeys[k].mValue.x, channel->mPositionKeys[k].mValue.y, channel->mPositionKeys[k].mValue.z);
				animClip.boneFrames_Translation[boneName].push_back(kfPosition);
			}
		}
		m_AnimationClip.push_back(animClip);
	}
}

