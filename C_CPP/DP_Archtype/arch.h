
#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <typeinfo>
#include <typeindex>

namespace Arch01
{
    // 타입을 쉽게 비교하기 위한 별칭
    using EntityID = size_t;

    // ===========================================
    // 1. 컴포넌트 (순수 데이터)
    // ===========================================

    // 컴포넌트 A: 위치 정보
    struct TransformComponent {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    // 컴포넌트 B: 상태 정보
    struct HealthComponent {
        int value = 100;
    };

    // ===========================================
    // 2. 아키타입 청크 (연속적인 메모리 블록)
    // ===========================================

    // 동일한 컴포넌트 조합을 가진 엔티티들의 데이터를 묶어 저장하는 메모리 청크
    struct ArchetypeChunk {
        // 이 아키타입이 가진 컴포넌트들의 타입 집합 (비트마스크 대신 사용)
        std::set<std::type_index> componentTypes;

        // 해당 청크에 속한 엔티티들의 ID (컴포넌트 배열과 1:1 매칭)
        std::vector<EntityID> entityIDs;

        // 실제 컴포넌트 데이터 배열 (메모리 연속성 확보)
        std::vector<TransformComponent> transforms;
        std::vector<HealthComponent> healths;

        // 청크 초기화: 어떤 타입 조합으로 구성되었는지 저장
        ArchetypeChunk(std::set<std::type_index> types) : componentTypes(std::move(types)) {}

        // 청크에 새 엔티티와 컴포넌트를 추가합니다.
        void AddEntity(EntityID id, const TransformComponent& t, const HealthComponent& h) {
            entityIDs.push_back(id);
            transforms.push_back(t); // T1, T2, T3... 가 연속적으로 저장
            healths.push_back(h);     // H1, H2, H3... 가 T 배열 바로 뒤에 저장 (높은 지역성)
        }

        // 이 청크가 특정 컴포넌트 조합을 가지고 있는지 확인합니다.
        bool HasComponents(const std::set<std::type_index>& required) const {
            for (const auto& req : required) {
                if (componentTypes.find(req) == componentTypes.end()) {
                    return false;
                }
            }
            return true;
        }
    };

    // ===========================================
    // 3. ECS 매니저 (아키타입 관리)
    // ===========================================

    class SimpleECS {
    private:
        std::map<std::set<std::type_index>, ArchetypeChunk> m_Archetypes;
        EntityID m_NextEntityID = 1;

        // Transform + Health 조합을 나타내는 타입 집합
        const std::set<std::type_index> TransformHealthType = {
            std::type_index(typeid(TransformComponent)),
            std::type_index(typeid(HealthComponent))
        };

    public:
        SimpleECS() {
            // 필수 아키타입을 미리 생성
            m_Archetypes.emplace(TransformHealthType, TransformHealthType);
            std::cout << "ECS 초기화: Transform+Health 아키타입 생성 완료.\n";
        }

        // 엔티티를 생성하고 아키타입 청크에 컴포넌트를 추가합니다.
        EntityID CreateEntityWithComponents(const TransformComponent& t, const HealthComponent& h) {
            EntityID id = m_NextEntityID++;

            // Transform + Health 아키타입을 찾아 데이터 삽입
            if (m_Archetypes.count(TransformHealthType)) {
                m_Archetypes.at(TransformHealthType).AddEntity(id, t, h);
                std::cout << " -> 엔티티 " << id << " 생성 및 'Transform+Health' 청크에 배치.\n";
            }
            return id;
        }

        // 시스템 쿼리를 위한 아키타입 목록을 반환합니다.
        // (실제 ECS에서는 내부적으로 캐싱되어 매우 빠름)
        std::vector<ArchetypeChunk*> QueryArchetypes(const std::set<std::type_index>& required) {
            std::vector<ArchetypeChunk*> results;
            for (auto& pair : m_Archetypes) {
                if (pair.second.HasComponents(required)) {
                    results.push_back(&pair.second);
                }
            }
            return results;
        }
    };

    // ===========================================
    // 4. 시스템 (로직 처리)
    // ===========================================

    // TransformComponent와 HealthComponent를 가진 엔티티만 순회하며 체력을 깎는 시스템
    class DamageSystem {
    public:
        void Update(SimpleECS& ecs, float deltaTime) {
            // 이 시스템이 필요로 하는 컴포넌트 조합 정의
            const std::set<std::type_index> required = {
                std::type_index(typeid(TransformComponent)),
                std::type_index(typeid(HealthComponent))
            };

            // 1. 필요한 아키타입 청크 목록을 쿼리
            std::vector<ArchetypeChunk*> chunks = ecs.QueryArchetypes(required);

            std::cout << "\n[DamageSystem 업데이트 시작] - " << chunks.size() << "개 청크 순회.\n";

            // 2. 각 청크를 순회하며 데이터를 연속적으로 처리 (캐시 효율 극대화 지점!)
            for (ArchetypeChunk* chunk : chunks) {

                // 청크에 담긴 데이터의 크기는 모두 동일합니다.
                size_t count = chunk->entityIDs.size();

                std::cout << "  - 청크(" << count << "개 엔티티) 처리 중...\n";

                // 메모리상 연속적으로 붙어있는 두 배열(transforms, healths)을 동시에 순회
                for (size_t i = 0; i < count; ++i) {
                    // 엔티티 ID는 참고용으로 사용 가능
                    EntityID currentID = chunk->entityIDs[i];

                    // 연속적으로 배치된 컴포넌트 데이터에 직접 접근
                    TransformComponent& transform = chunk->transforms[i];
                    HealthComponent& health = chunk->healths[i];

                    // 3. 로직 실행 (트랜스폼이 특정 위치 이하면 지속적인 피해를 입는다고 가정)
                    if (transform.y < 0.0f) {
                        health.value -= static_cast<int>(10.0f * deltaTime);
                        std::cout << "    * Entity " << currentID
                            << " (Y:" << transform.y << ") 피해 입음 -> Health: "
                            << health.value << '\n';
                    }
                }
            }
            std::cout << "[DamageSystem 업데이트 종료]\n";
        }
    };

    int Run() {
        // 1. ECS 및 시스템 초기화
        SimpleECS ecs;
        DamageSystem damageSystem;
        float deltaTime = 0.5f;

        // 2. 엔티티 생성 (Transform + Health 조합)
        ecs.CreateEntityWithComponents({ 0.0f, 5.0f, 0.0f }, { 100 });   // 엔티티 1 (안전)
        ecs.CreateEntityWithComponents({ 10.0f, -2.0f, 0.0f }, { 80 });  // 엔티티 2 (피해 구역)
        ecs.CreateEntityWithComponents({ 5.0f, 8.0f, 0.0f }, { 100 });   // 엔티티 3 (안전)
        ecs.CreateEntityWithComponents({ -5.0f, -0.5f, 0.0f }, { 95 }); // 엔티티 4 (피해 구역)

        // 3. 시스템 업데이트 (매 프레임)
        std::cout << "\n--- 1번째 프레임 ---\n";
        damageSystem.Update(ecs, deltaTime);

        std::cout << "\n--- 2번째 프레임 ---\n";
        damageSystem.Update(ecs, deltaTime);

        return 0;
    }
}

namespace Arch02
{
    // 타입을 쉽게 비교하기 위한 별칭
    using EntityID = size_t;
    using ArchetypeKey = std::set<std::type_index>;

    // ===========================================
    // 1. 컴포넌트 (순수 데이터)
    // ===========================================

    struct TransformComponent {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        // ... (Rotation, Scale 등)
    };

    struct HealthComponent {
        int value = 100;
    };

    // ===========================================
    // 2. 엔티티 위치 구조체 (엔티티가 기억하는 정보)
    // ===========================================

    // 엔티티 ID가 자신의 컴포넌트 데이터를 찾는 데 필요한 '인덱스 카드' 역할
    struct EntityLocation {
        // 1. 내가 속한 아키타입의 고유 키 (데이터의 종류)
        ArchetypeKey archetypeKey;

        // 2. 내가 속한 청크 내에서 내가 몇 번째 인덱스에 있는지 (데이터의 위치)
        size_t chunkIndex;
    };

    // ===========================================
    // 3. 아키타입 청크 (실제 데이터 저장소)
    // ===========================================

    struct ArchetypeChunk {
        // 이 청크에 속한 엔티티들의 ID 목록
        std::vector<EntityID> entityIDs;

        // 실제 컴포넌트 데이터 배열 (메모리 연속성 핵심)
        std::vector<TransformComponent> transforms;
        std::vector<HealthComponent> healths;

        // 현재 청크에 엔티티와 컴포넌트를 추가합니다.
        size_t AddEntity(EntityID id, const TransformComponent& t, const HealthComponent& h) {
            size_t index = entityIDs.size(); // 새로 추가될 인덱스
            entityIDs.push_back(id);
            transforms.push_back(t);
            healths.push_back(h);
            return index; // 엔티티가 청크 내에서 몇 번째 인덱스에 있는지 반환
        }

        // 엔티티가 청크 내에서 가진 인덱스를 이용하여 컴포넌트 데이터를 얻습니다.
        TransformComponent& GetTransform(size_t index) {
            return transforms[index];
        }
        HealthComponent& GetHealth(size_t index) {
            return healths[index];
        }
    };

    // ===========================================
    // 4. ECS 매니저 (데이터 관리 및 룩업)
    // ===========================================

    class SimpleECS {
    private:
        // 엔티티 ID -> 위치 정보 맵 (엔티티 ID를 받으면 위치를 즉시 알 수 있음)
        std::map<EntityID, EntityLocation> m_EntityLocations;

        // 아키타입 키 -> 실제 청크 데이터 맵 (아키타입 키를 받으면 청크를 찾을 수 있음)
        // 실제 구현에서는 청크가 여러 개일 수 있으나, 단순화를 위해 1:1로 가정
        std::map<ArchetypeKey, std::shared_ptr<ArchetypeChunk>> m_ArchetypeChunks;

        EntityID m_NextEntityID = 1;

        // 예제 아키타입 키 (Transform + Health 조합)
        const ArchetypeKey ARCHETYPE_TRANSFORM_HEALTH
        {
             std::type_index(typeid(TransformComponent)),
             std::type_index(typeid(HealthComponent))
        };

    public:
        SimpleECS() {
            // Transform + Health 아키타입을 위한 청크 생성 및 등록
            m_ArchetypeChunks[ARCHETYPE_TRANSFORM_HEALTH] =
                std::make_shared<ArchetypeChunk>();
        }

        EntityID CreateEntity(const TransformComponent& t, const HealthComponent& h) {
            EntityID id = m_NextEntityID++;

            // 1. 해당 아키타입 청크를 찾습니다.
            auto& chunk = m_ArchetypeChunks.at(ARCHETYPE_TRANSFORM_HEALTH);

            // 2. 청크에 실제 컴포넌트 데이터를 삽입하고, 청크 내 인덱스를 얻습니다.
            size_t chunkIndex = chunk->AddEntity(id, t, h);

            // 3. 엔티티 ID에 위치 정보를 등록합니다. (ID는 인덱스를 가리킵니다)
            m_EntityLocations[id] = { ARCHETYPE_TRANSFORM_HEALTH, chunkIndex };

            std::cout << "엔티티 " << id << " 생성: [아키타입 키: " << ARCHETYPE_TRANSFORM_HEALTH.size() << ", 청크 인덱스: " << chunkIndex << "]\n";
            return id;
        }

        // 외부에서 엔티티 ID를 통해 컴포넌트를 얻는 함수
        TransformComponent& GetTransformComponent(EntityID id) {
            // 1. 엔티티 ID로부터 위치 정보(ArchetypeKey, ChunkIndex)를 얻습니다.
            const EntityLocation& location = m_EntityLocations.at(id);

            // 2. ArchetypeKey를 통해 실제 데이터 저장소(청크)를 찾습니다.
            auto& chunk = m_ArchetypeChunks.at(location.archetypeKey);

            // 3. ChunkIndex를 통해 청크 내의 연속적인 배열에서 데이터를 참조합니다.
            std::cout << " -> 엔티티 " << id << ": 청크 인덱스 " << location.chunkIndex << "를 사용해 데이터 참조\n";
            return chunk->GetTransform(location.chunkIndex);
        }
    };

    int Run() {
        SimpleECS ecs;

        // 1. 엔티티 생성 (데이터는 청크에 연속적으로 쌓임)
        EntityID playerID = ecs.CreateEntity({ 10.0f, 5.0f, 0.0f }, { 100 });
        EntityID enemyID = ecs.CreateEntity({ -5.0f, 0.0f, 0.0f }, { 80 });
        EntityID allyID = ecs.CreateEntity({ 20.0f, 1.0f, 0.0f }, { 90 });

        std::cout << "\n--- 엔티티 데이터 직접 접근 ---\n";

        // 2. 엔티티 ID를 통해 Transform 데이터에 접근
        TransformComponent& playerTransform = ecs.GetTransformComponent(playerID);
        TransformComponent& enemyTransform = ecs.GetTransformComponent(enemyID);

        std::cout << "Player X 위치: " << playerTransform.x << '\n';
        std::cout << "Enemy X 위치: " << enemyTransform.x << '\n';

        // 3. 데이터 수정
        playerTransform.x += 5.0f;
        std::cout << "Player X 위치 (수정 후): " << playerTransform.x << '\n';

        std::cout << "\n참고: 시스템은 엔티티 ID를 거치지 않고, 청크를 바로 순회합니다.\n";

        return 0;
    }
}
