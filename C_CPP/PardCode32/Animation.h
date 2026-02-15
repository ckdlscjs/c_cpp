#pragma once
#include "Resource.h"

class Animation : public BaseResource<Animation>
{
	friend class BaseResource<Animation>;
public:
	Animation(size_t hash, const std::wstring& szFilePath, const std::unordered_map<std::string, Bone>& boneMap, const std::vector<NodeHierarchy>& nodeHierarchy, const std::unordered_map<std::string, AnimationClip>& clips);
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation(Animation&&) = delete;
	Animation& operator=(Animation&&) = delete;

public:
	void GetFinalTransform(const std::string& szName, const float t, std::vector<Matrix4x4>& mats);
	const std::unordered_map<std::string, AnimationClip>& GetAnimations();
private:
	Vector3		FrameInterpolate_Vector(const std::vector<KeyFrame_Vector>& frames, const float t);
	Quarternion FrameInterpolate_Quarternion(const std::vector<KeyFrame_Quarternion>& frames, const float t);

private:
	std::unordered_map<std::string, AnimationClip> m_AnimationClip;
	std::vector<NodeHierarchy> m_Hierarchy;
	std::unordered_map<std::string, Bone> m_BoneMap;

};
inline Animation::Animation(size_t hash, const std::wstring& szFilePath, const std::unordered_map<std::string, Bone>& boneMap, const std::vector<NodeHierarchy>& nodeHierarchy, const std::unordered_map<std::string, AnimationClip>& clips)
	: BaseResource(hash, szFilePath), m_BoneMap(boneMap), m_Hierarchy(nodeHierarchy), m_AnimationClip(clips)
{

}

inline void Animation::GetFinalTransform(const std::string& szName, const float t, std::vector<Matrix4x4>& mats)
{
	// 1. 모든 노드(뼈 포함 모든 계층)의 Root 기준 전역 행렬을 저장할 공간
	std::vector<Matrix4x4> toRoot(m_Hierarchy.size());
	mats.resize(m_BoneMap.size());

	const auto& animClip = m_AnimationClip[szName];

	// 2. 계층 구조 순회 (m_Hierarchy가 부모가 항상 자식보다 앞에 있다는 가정 하에)
	for (UINT idx = 0; idx < m_Hierarchy.size(); idx++)
	{
		int idx_parent = m_Hierarchy[idx].idx_parent;
		std::string curName = m_Hierarchy[idx].szName_Cur;
		Matrix4x4 toParent = m_Hierarchy[idx].matRelative;
		
		
		// 해당 노드에 애니메이션 키값이 있다면 덮어쓰거나 결합
		if (animClip.boneFrames_Scale.find(curName) != animClip.boneFrames_Scale.end())
		{
			Vector3 vScale = FrameInterpolate_Vector(animClip.boneFrames_Scale.at(curName), t);
			Quarternion qRotate = FrameInterpolate_Quarternion(animClip.boneFrames_Rotate.at(curName), t);
			Vector3 vTranslation = FrameInterpolate_Vector(animClip.boneFrames_Translation.at(curName), t);

			// 보통 애니메이션이 있으면 초기 상대행렬(matRelative)을 '대체'합니다.
			toParent = GetMat_World(vScale, qRotate, vTranslation);
		}
		

		// 3. 부모의 '이미 계산된' 전역 행렬을 가져와 곱함 (누적의 핵심)
		if (idx_parent == -1) // 루트 노드인 경우
		{
			toRoot[idx] = toParent;
		}
		else
		{
			toRoot[idx] = toParent * toRoot[idx_parent];
		}
	}

	// 4. 스키닝 행렬 완성: vertex * offset * toRoot(최종)
	for (UINT idx = 0; idx < m_Hierarchy.size(); idx++)
	{
		std::string curName = m_Hierarchy[idx].szName_Cur;
		if (m_BoneMap.find(curName) != m_BoneMap.end())
		{
			// 쉐이더로 보낼 최종 행렬 계산
			// 순서: Offset(정점을 뼈 공간으로) -> toRoot(뼈 공간 정점을 애니메이션된 월드로)
			mats[m_BoneMap[curName].idx] = m_BoneMap[curName].matOffset * toRoot[idx];
		}
	}
}

inline const std::unordered_map<std::string, AnimationClip>& Animation::GetAnimations()
{
	return m_AnimationClip;
}


//특정시간 t를 기반으로 keyframe
inline Vector3 Animation::FrameInterpolate_Vector(const std::vector<KeyFrame_Vector>& frames, const float t)
{
	if (frames.size() <= 1)
		return frames[0].vValue;
	auto iter = std::upper_bound(frames.begin(), frames.end(), t, [](const float val, const KeyFrame_Vector& frame)
		{
			return val < frame.fTime;
		});
	UINT prv = 0, nxt = 0;
	if (iter == frames.begin()) { prv = 0; nxt = 0; }
	else if (iter == frames.begin() + frames.size()) { prv = frames.size() - 1; nxt = frames.size() - 1; }
	else { prv = (unsigned)(iter - frames.begin() - 1); nxt = (unsigned)(iter - frames.begin()); }

	double dt = frames[nxt].fTime - frames[prv].fTime;
	double amount = (dt > 0.0) ? (t - frames[prv].fTime) / dt : 0.0f;
	return lerp(frames[prv].vValue, frames[nxt].vValue, amount);
}

inline Quarternion Animation::FrameInterpolate_Quarternion(const std::vector<KeyFrame_Quarternion>& frames, const float t)
{
	if (frames.size() <= 1)
		return frames[0].qValue;
	auto iter = std::upper_bound(frames.begin(), frames.end(), t, [](const float val, const KeyFrame_Quarternion& frame)
		{
			return val < frame.fTime;
		});
	UINT prv = 0, nxt = 0;
	if (iter == frames.begin()) { prv = 0; nxt = 0; }
	else if (iter == frames.begin() + frames.size()) { prv = frames.size() - 1; nxt = frames.size() - 1; }
	else { prv = (unsigned)(iter - frames.begin() - 1); nxt = (unsigned)(iter - frames.begin()); }

	double dt = frames[nxt].fTime - frames[prv].fTime;
	double amount = (dt > 0.0) ? (t - frames[prv].fTime) / dt : 0.0f;
	return Slerp(frames[prv].qValue, frames[nxt].qValue, amount);
}



