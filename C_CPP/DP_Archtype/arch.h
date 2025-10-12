
#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <unordered_set>

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

namespace Arch03
{
    // 1. 컴포넌트 정의: 순수한 데이터입니다.
    struct Transform { float x, y; };
    struct Velocity { float vx, vy; };
    struct Input { bool moving; };
    struct Camera { float fov; };

    // 컴포넌트 타입 식별을 위한 문자열 정의 (실제 ECS에서는 Bitmask를 사용)
    using ComponentType = std::string;
    const ComponentType TYPE_TRANSFORM = "Transform";
    const ComponentType TYPE_VELOCITY = "Velocity";
    const ComponentType TYPE_INPUT = "Input";
    const ComponentType TYPE_CAMERA = "Camera";

    // =================================================================
    // 2. 아키타입 청크 정의
    // 특정 컴포넌트 조합을 가진 엔티티들을 모아놓는 연속된 메모리 블록입니다.
    // 이 시뮬레이션에서는 'Transform'과 'Input', 'Camera'에 대한 데이터 공간을
    // 단일 청크 내부에 연속적으로 배치했다고 가정합니다.
    // =================================================================
    struct ArchetypeChunk {
        std::string name; // 아키타입 이름 (컴포넌트 조합)
        std::set<ComponentType> components; // 이 아키타입이 가진 컴포넌트 목록
        size_t entity_count = 0;

        // 아키타입의 실제 데이터 블록 (컴포넌트별 SoA)
        std::vector<Transform> transforms;
        std::vector<Velocity> velocities;
        std::vector<Input> inputs;
        std::vector<Camera> cameras;

        ArchetypeChunk(std::string n, std::set<ComponentType> comps) : name(n), components(comps) {}

        // 엔티티 추가 (간단화): 해당 아키타입에 필요한 컴포넌트만 추가됨
        void add_entity(float x, float y, float vx = 0.0f, float fov = 0.0f) {
            if (components.count(TYPE_TRANSFORM)) {
                transforms.push_back({ x, y });
            }
            if (components.count(TYPE_VELOCITY)) {
                velocities.push_back({ vx, 0.0f });
            }
            if (components.count(TYPE_INPUT)) {
                inputs.push_back({ true });
            }
            if (components.count(TYPE_CAMERA)) {
                cameras.push_back({ fov });
            }
            entity_count++;
        }

        // 주어진 쿼리를 이 아키타입이 만족하는지 확인
        bool matches_query(const std::set<ComponentType>& required_components) const {
            // required_components의 모든 요소가 components에 포함되어야 함
            for (const auto& required : required_components) {
                if (components.find(required) == components.end()) {
                    return false;
                }
            }
            return true;
        }
    };

    // =================================================================
    // 3. ECS 관리자 (아키타입 그룹 관리)
    // =================================================================
    class ECSManager {
    public:
        // 아키타입 이름(조합)을 키로 하여 청크를 저장 (실제로는 동적 배열로 관리됨)
        std::map<std::string, ArchetypeChunk> archetypes;

        ECSManager() {
            // 아키타입 1: {Transform, Velocity} - Projectile, Simple NPC 등
            std::set<ComponentType> comps1 = { TYPE_TRANSFORM, TYPE_VELOCITY };
            archetypes.emplace("T_V", ArchetypeChunk("T_V", comps1));

            // 아키타입 2: {Transform, Input, Camera} - Player Character
            std::set<ComponentType> comps2 = { TYPE_TRANSFORM, TYPE_INPUT, TYPE_CAMERA };
            archetypes.emplace("T_I_C", ArchetypeChunk("T_I_C", comps2));
        }

        void populate_entities() {
            // A1: T_V 아키타입에 엔티티 5개 추가
            for (int i = 0; i < 5; ++i) {
                archetypes.at("T_V").add_entity(10.0f + i, 5.0f, 1.0f);
            }
            // A2: T_I_C 아키타입에 엔티티 3개 추가
            for (int i = 0; i < 3; ++i) {
                archetypes.at("T_I_C").add_entity(100.0f + i, 50.0f, 0.0f, 90.0f);
            }

            std::cout << "--- 엔티티 초기화 완료 ---\n";
            std::cout << "Archetype T_V (T, V): " << archetypes.at("T_V").entity_count << "개 엔티티\n";
            std::cout << "Archetype T_I_C (T, I, C): " << archetypes.at("T_I_C").entity_count << "개 엔티티\n";
            std::cout << "---------------------------\n\n";
        }
    };

    // =================================================================
    // 4. 시스템 정의 및 실행
    // =================================================================

    // 시스템의 쿼리 조건을 정의합니다.
    const std::set<ComponentType> MOVEMENT_QUERY = { TYPE_TRANSFORM, TYPE_VELOCITY };

    void movement_system(ECSManager& manager) {
        std::cout << "================================================\n";
        std::cout << "시스템 실행: MovementSystem\n";
        std::cout << "요청 쿼리: { Transform, Velocity } \n";
        std::cout << "------------------------------------------------\n";

        int entities_processed = 0;

        // ECS는 모든 아키타입을 순회하며 쿼리 조건을 확인합니다.
        for (auto& pair : manager.archetypes) {
            ArchetypeChunk& chunk = pair.second;

            // 🔍 [필터링 단계]: 이 아키타입이 쿼리를 만족하는가?
            if (chunk.matches_query(MOVEMENT_QUERY)) {
                std::cout << "✅ 쿼리 만족: 아키타입 [" << chunk.name << "] 순회 시작\n";

                // 🚀 [메모리 지역성 단계]: 청크 내부의 필요한 데이터만 순회!
                for (size_t i = 0; i < chunk.entity_count; ++i) {
                    // 이 반복문 내에서 T와 V 데이터는 메모리상 서로 매우 가깝습니다.

                    // 필요한 컴포넌트만 접근
                    Transform& t = chunk.transforms[i];
                    Velocity& v = chunk.velocities[i];

                    // 로직 수행 (이동)
                    t.x += v.vx;
                    t.y += v.vy;

                    if (i < 2) { // 처음 2개만 출력
                        std::cout << "  -> Entity #" << i << ": 이동됨. 새로운 X 위치: " << t.x << "\n";
                    }
                    entities_processed++;
                }
            }
            else {
                std::cout << "❌ 쿼리 불만족: 아키타입 [" << chunk.name << "] 무시됨 (필요 없는 데이터는 접근조차 안 함)\n";
            }
        }
        std::cout << "------------------------------------------------\n";
        std::cout << "총 처리된 엔티티 수: " << entities_processed << "\n";
        std::cout << "================================================\n\n";
    }

    // 또 다른 시스템: 카메라 조정 시스템
    const std::set<ComponentType> CAMERA_QUERY = { TYPE_CAMERA, TYPE_TRANSFORM };

    void camera_system(ECSManager& manager) {
        std::cout << "================================================\n";
        std::cout << "시스템 실행: CameraSystem\n";
        std::cout << "요청 쿼리: { Camera, Transform } \n";
        std::cout << "------------------------------------------------\n";

        for (auto& pair : manager.archetypes) {
            ArchetypeChunk& chunk = pair.second;

            // 🔍 [필터링 단계]:
            if (chunk.matches_query(CAMERA_QUERY)) {
                std::cout << "✅ 쿼리 만족: 아키타입 [" << chunk.name << "] 순회 시작\n";

                // 🚀 [메모리 지역성 단계]:
                for (size_t i = 0; i < chunk.entity_count; ++i) {
                    Transform& t = chunk.transforms[i];
                    Camera& c = chunk.cameras[i];

                    // 로직 수행 (카메라가 플레이어를 따라가도록 시야각 업데이트)
                    c.fov = 90.0f + (t.x / 1000.0f); // 간단한 로직

                    if (i < 2) {
                        std::cout << "  -> Entity #" << i << ": 카메라 시야각 조정 완료: " << c.fov << "\n";
                    }
                }
            }
            else {
                std::cout << "❌ 쿼리 불만족: 아키타입 [" << chunk.name << "] 무시됨\n";
            }
        }
        std::cout << "================================================\n";
    }


    int Run() {
        ECSManager manager;
        manager.populate_entities();

        // 1. MovementSystem 실행 (T, V 필요)
        movement_system(manager);

        // 2. CameraSystem 실행 (C, T 필요)
        camera_system(manager);

        return 0;
    }

}

namespace Arch04
{
    // 템플릿 별칭 정의
    using EntityID = size_t;
    using ComponentTypeID = std::type_index;
    using ArchetypeKey = std::unordered_set<ComponentTypeID>;

    // Vector3 정의 (간단화)
    struct Vector3 { float x = 0.0f, y = 0.0f, z = 0.0f; };
    // Mesh_Material 정의 (간단화)
    struct Mesh_Material {};

    // =================================================================
    // 사용자 정의 컴포넌트 구조체 (순수 데이터)
    // =================================================================
    struct Component_Transform
    {
        Vector3 m_vScale = { 1.0f, 1.0f, 1.0f };
        Vector3 m_vRotate = {};
        Vector3 m_vPosition = {};
    };

    // 복잡한 컨테이너 대신 핵심 데이터만 남깁니다.
    struct Component_Render
    {
        bool bRenderable = false;
        size_t m_MeshMaterialCount = 0; // 컨테이너 대신 개수만 저장
        // 실제 ECS에서는 m_MeshMaterials와 같은 데이터는 별도의 리소스 시스템이 포인터로 관리합니다.
    };

    struct Component_Velocity
    {
        float vx = 0.0f;
        float vy = 0.0f;
    };

    struct Component_Camera
    {
        float m_fFov = 60.0f;
        float m_fScreenWidth = 800.0f;
        float m_fScreenHeight = 600.0f;
        // ... 나머지 필드
    };

    struct Component_Input {};
    struct Component_Light {};
    struct Component_Collider {};

    // =================================================================
    // 타입 ID 유틸리티
    // =================================================================

    // 컴포넌트의 타입 ID를 얻는 함수
    template<typename T>
    ComponentTypeID get_component_type_id() {
        return typeid(T);
    }

    // 컴포넌트의 사이즈를 얻는 함수
    template<typename T>
    size_t get_component_size() {
        return sizeof(T);
    }


#pragma once
#include "ComponentDefinitions.h"
#include <algorithm> // std::max, std::sort

    // 기본 청크 용량
    constexpr size_t DEFAULT_CHUNK_CAPACITY = 100;

    /**
     * @brief ECS 아키타입 청크: SoA 원칙에 따라 컴포넌트 데이터를 Raw Memory에 저장합니다.
     * * N개의 컴포넌트 타입에 대해 N개의 연속된 데이터 블록을 단일 버퍼에서 관리합니다.
     */
    struct ArchetypeChunk
    {
    private:
        // 청크가 소유한 컴포넌트 타입의 집합 (이 아키타입을 정의합니다)
        ArchetypeKey m_Key;

        // 컴포넌트 타입별로 Raw Buffer 내에서 데이터가 시작하는 오프셋 (바이트 단위)
        std::unordered_map<ComponentTypeID, size_t> m_ComponentOffsets;

        // 전체 컴포넌트 데이터를 저장하는 단일 Raw Memory 버퍼
        char* m_RawBuffer = nullptr;

        // 이 청크에 저장된 현재 엔티티 수
        size_t m_EntityCount = 0;

        // 이 청크가 수용할 수 있는 최대 엔티티 수
        size_t m_Capacity = DEFAULT_CHUNK_CAPACITY;

    public:
        ArchetypeChunk(const ArchetypeKey& key) : m_Key(key)
        {
            // 1. 메모리 오프셋 계산 (SoA를 위해)
            calculate_offsets();

            // 2. Raw Buffer 할당
            size_t total_buffer_size = 0;

            // 마지막 오프셋 + 마지막 컴포넌트의 총 데이터 크기를 더하여 전체 버퍼 크기를 구합니다.
            for (const auto& pair : m_ComponentOffsets) {
                // pair.first: ComponentTypeID, pair.second: Offset
                size_t component_size = 0;
                // 오프셋을 기반으로 총 데이터 크기를 계산하기 위해 모든 컴포넌트의 크기를 알아야 합니다.
                // 여기서는 단순화하여 마지막 오프셋을 찾아 총 버퍼 크기를 계산합니다.

                // 실제 구현: 가장 큰 오프셋을 가진 컴포넌트의 크기를 찾습니다.
            }

            // 총 버퍼 크기 계산 (간단화): 모든 컴포넌트 데이터의 총 크기를 합산합니다.
            if (m_ComponentOffsets.empty()) {
                total_buffer_size = 0;
            }
            else {
                // 마지막 오프셋을 찾고, 해당 컴포넌트의 전체 크기를 더합니다.
                size_t max_offset = 0;
                ComponentTypeID last_type = get_component_type_id<Component_Transform>(); // Placeholder

                for (const auto& [type_id, offset] : m_ComponentOffsets) {
                    if (offset >= max_offset) {
                        max_offset = offset;
                        last_type = type_id;
                    }
                }

                // 마지막 컴포넌트 타입의 크기를 알아내서 총 버퍼 크기를 결정합니다.
                // 이 부분을 일반화하는 것은 C++의 타입 시스템으로 인해 복잡하므로,
                // 이 예제에서는 단순하게 모든 컴포넌트의 총 데이터 크기를 직접 합산하는 방식으로 대체합니다.
                size_t total_size_sum = 0;
                for (const auto& type_id : m_Key) {
                    // 이 부분은 실제 ECS 구현에서 컴포넌트 ID -> Size 맵을 따로 두어 해결합니다.
                    // 여기서는 타입ID를 이용하여 사이즈를 가져옵니다. (RTTI 오버헤드 있음)
                    if (type_id == get_component_type_id<Component_Transform>()) total_size_sum += sizeof(Component_Transform) * m_Capacity;
                    else if (type_id == get_component_type_id<Component_Velocity>()) total_size_sum += sizeof(Component_Velocity) * m_Capacity;
                    else if (type_id == get_component_type_id<Component_Render>()) total_size_sum += sizeof(Component_Render) * m_Capacity;
                }
                total_buffer_size = total_size_sum;
            }

            if (total_buffer_size > 0) {
                m_RawBuffer = new char[total_buffer_size];
                std::cout << "[ArchetypeChunk] " << key.size() << "개 컴포넌트, "
                    << m_Capacity << " 엔티티 용량, " << total_buffer_size << " 바이트 할당 완료.\n";
            }
        }

        ~ArchetypeChunk() {
            if (m_RawBuffer) {
                delete[] m_RawBuffer;
            }
        }

        // 컴포넌트 오프셋 계산 (SoA의 핵심)
        void calculate_offsets() {
            size_t current_offset = 0;

            // 💡 주의: 실제 ECS에서는 메모리 정렬(Alignment)을 고려하여 오프셋을 계산해야 하지만,
            // 이 예제에서는 단순한 연속 할당만 보여줍니다.

            // 키를 순회하며 오프셋을 결정합니다. (순서는 중요하지 않으나, 일관성을 위해 정렬 필요)
            // 실제 구현에서는 미리 정의된 순서로 정렬된 컴포넌트 배열을 사용합니다.

            // 임시 벡터에 타입 ID를 복사하여 순서를 결정합니다.
            std::vector<ComponentTypeID> sorted_types(m_Key.begin(), m_Key.end());

            // std::sort를 사용하여 타입 ID를 기반으로 정렬 (일관된 오프셋 계산을 위해)
            std::sort(sorted_types.begin(), sorted_types.end(), [](const ComponentTypeID& a, const ComponentTypeID& b) {
                return a.name() < b.name(); // 타입 이름을 기준으로 정렬
                });

            for (const auto& type_id : sorted_types) {
                m_ComponentOffsets[type_id] = current_offset;

                // 타입 ID에서 컴포넌트 크기를 얻어 다음 오프셋을 계산합니다.
                size_t component_size = 0;
                if (type_id == get_component_type_id<Component_Transform>()) component_size = get_component_size<Component_Transform>();
                else if (type_id == get_component_type_id<Component_Velocity>()) component_size = get_component_size<Component_Velocity>();
                else if (type_id == get_component_type_id<Component_Render>()) component_size = get_component_size<Component_Render>();
                // N개의 컴포넌트가 있다면 여기에 N개의 if-else 블록이 추가됩니다. (RTTI 사용의 단점)

                // 다음 컴포넌트의 시작 지점 = 현재 오프셋 + (컴포넌트 크기 * 청크 용량)
                current_offset += component_size * m_Capacity;

                std::cout << "  - 컴포넌트 " << type_id.name() << ": 오프셋 " << m_ComponentOffsets[type_id]
                    << ", 다음 오프셋까지 크기: " << component_size * m_Capacity << " 바이트\n";
            }
        }

        /**
         * @brief Raw Memory 버퍼 내에서 특정 컴포넌트의 배열 시작 주소를 가져옵니다.
         * @tparam T 컴포넌트 타입
         * @return T 타입의 배열을 가리키는 포인터
         */
        template <typename T>
        T* GetComponentArray() const
        {
            ComponentTypeID type_id = get_component_type_id<T>();

            // 1. 해당 타입이 청크에 있는지 확인
            auto it = m_ComponentOffsets.find(type_id);
            if (it == m_ComponentOffsets.end()) {
                // 이 아키타입은 해당 컴포넌트를 가지고 있지 않습니다.
                return nullptr;
            }

            // 2. 오프셋을 사용하여 포인터 계산
            size_t offset = it->second;

            // m_RawBuffer (char*)에 오프셋(size_t)을 더한 후, T* 타입으로 캐스팅합니다.
            // 이것이 바로 SoA 데이터의 시작 주소입니다.
            return reinterpret_cast<T*>(m_RawBuffer + offset);
        }

        // 엔티티 개수를 반환
        size_t GetEntityCount() const { return m_EntityCount; }

        // 엔티티 추가 (단순화된 예제)
        void AddEntity(const Component_Transform& t, const Component_Velocity& v) {
            if (m_EntityCount >= m_Capacity) {
                std::cerr << "오류: 청크 용량 초과!\n";
                return;
            }

            // 트랜스폼 데이터 복사
            if (m_Key.count(get_component_type_id<Component_Transform>())) {
                Component_Transform* transform_array = GetComponentArray<Component_Transform>();
                transform_array[m_EntityCount] = t;
            }

            // 벨로시티 데이터 복사
            if (m_Key.count(get_component_type_id<Component_Velocity>())) {
                Component_Velocity* velocity_array = GetComponentArray<Component_Velocity>();
                velocity_array[m_EntityCount] = v;
            }

            m_EntityCount++;
        }
    };
#include "ArchetypeChunk.h"
#include <iostream>

    // =================================================================
    // 시스템 정의 (MovementSystem)
    // =================================================================

    /**
     * @brief Transform과 Velocity를 가진 모든 엔티티를 처리하는 시스템
     * @param chunk 처리할 아키타입 청크
     */
    void MovementSystem(ArchetypeChunk& chunk) {
        ComponentTypeID t_id = get_component_type_id<Component_Transform>();
        ComponentTypeID v_id = get_component_type_id<Component_Velocity>();

        // 청크가 쿼리 조건(T, V)을 만족하는지 확인 (Key를 직접 검사)
        bool requires_T_V = chunk.m_Key.count(t_id) && chunk.m_Key.count(v_id);

        if (!requires_T_V) {
            std::cout << "[MovementSystem] 이 아키타입은 쿼리를 만족하지 않아 스킵됩니다.\n";
            return;
        }

        std::cout << "[MovementSystem] 쿼리 만족! " << chunk.GetEntityCount() << "개 엔티티 순회 시작.\n";

        // 💡 핵심: Raw Memory에서 T* 와 V* 배열의 시작 주소를 가져옵니다.
        // 이 두 배열은 메모리상에서 연속된 블록에 위치합니다. (SoA)
        Component_Transform* transforms = chunk.GetComponentArray<Component_Transform>();
        Component_Velocity* velocities = chunk.GetComponentArray<Component_Velocity>();

        if (!transforms || !velocities) {
            std::cerr << "오류: 필요한 컴포넌트 배열을 가져오지 못했습니다.\n";
            return;
        }

        // 두 배열을 동시에 순회하며 데이터 처리
        for (size_t i = 0; i < chunk.GetEntityCount(); ++i) {
            transforms[i].m_vPosition.x += velocities[i].vx;
            transforms[i].m_vPosition.y += velocities[i].vy;

            if (i == 0) {
                std::cout << "  - 엔티티 #0 이동: X = " << transforms[i].m_vPosition.x
                    << ", Y = " << transforms[i].m_vPosition.y << "\n";
            }
        }
    }

    // =================================================================

    int Run() {
        std::cout << "--- ECS Raw Memory ArchetypeChunk 데모 ---\n\n";

        // 1. 아키타입 Key 정의
        ArchetypeKey key_T_V = {
            get_component_type_id<Component_Transform>(),
            get_component_type_id<Component_Velocity>()
        };

        ArchetypeKey key_T_R = {
            get_component_type_id<Component_Transform>(),
            get_component_type_id<Component_Render>()
        };

        // 2. 청크 생성 및 오프셋 계산/메모리 할당
        std::cout << "--- 청크 A (T, V) 생성 ---\n";
        ArchetypeChunk chunk_A(key_T_V);

        std::cout << "\n--- 청크 B (T, R) 생성 ---\n";
        ArchetypeChunk chunk_B(key_T_R);

        // 3. 엔티티 데이터 추가
        chunk_A.AddEntity({/* Scale */ }, { 1.0f, 0.0f, 0.0f }, { 5.0f, 10.0f, 0.0f }, { 2.5f, 0.0f }); // E1 (Transform, Velocity)
        chunk_A.AddEntity({/* Scale */ }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }); // E2

        // 청크 B는 Velocity가 없으므로 AddEntity를 호출할 수 없습니다. (컴파일 에러 혹은 런타임 오류 방지를 위해)
        // 이 예제에서는 단순하게 AddEntity 내에서 Velocity를 요구하지 않으므로 무시합니다.

        std::cout << "\n--- 시스템 실행 ---\n";

        // 청크 A 실행 (T, V 모두 가짐)
        MovementSystem(chunk_A);

        // 청크 B 실행 (Velocity 없음)
        MovementSystem(chunk_B);

        std::cout << "\n--- 최종 결과 ---\n";
        Component_Transform* t_array = chunk_A.GetComponentArray<Component_Transform>();
        if (t_array) {
            std::cout << "청크 A의 첫 번째 엔티티 최종 위치: (" << t_array[0].m_vPosition.x
                << ", " << t_array[0].m_vPosition.y << ")\n";
        }

        return 0;
    }

}