#include "AnimationSystem.h"
#include "ResourceSystem.h"
#include "ECSSystem.h"
#include "Animation.h"

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::Init()
{
}

void AnimationSystem::Frame(float deltatime)
{
	std::cout << "Frame : " << "AnimationSystem" << " Class" << '\n';
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, T_Render_Geometry_Skeletal>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& animations = archetype->GetComponents<C_Animation>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					
					Animation* pAnimation = _ResourceSystem.GetResource<Animation>(animations[col].hash_ai);
					if(pAnimation->GetAnimations().empty()) continue;
						
					if (animations[col].szCurClip.empty())
					{
						animations[col].szCurClip = pAnimation->GetAnimations().begin()->first;
						animations[col].elapsedTime = 0.0f;
					}

					animations[col].elapsedTime += pAnimation->GetAnimations().find(animations[col].szCurClip)->second.fTicksPerSecond * deltatime;
					if (animations[col].elapsedTime > pAnimation->GetAnimations().find(animations[col].szCurClip)->second.fDuration)
						animations[col].elapsedTime = 0.0f;

					std::vector<Matrix4x4> curAnim;
					pAnimation->GetFinalTransform(animations[col].szCurClip, animations[col].elapsedTime, curAnim);
					std::memcpy(animations[col].matAnims, curAnim.data(), curAnim.size() * sizeof(Matrix4x4));
				}
				st_col = 0;
			}
		}
	}
}
