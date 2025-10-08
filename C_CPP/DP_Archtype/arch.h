
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
    // Ÿ���� ���� ���ϱ� ���� ��Ī
    using EntityID = size_t;

    // ===========================================
    // 1. ������Ʈ (���� ������)
    // ===========================================

    // ������Ʈ A: ��ġ ����
    struct TransformComponent {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    // ������Ʈ B: ���� ����
    struct HealthComponent {
        int value = 100;
    };

    // ===========================================
    // 2. ��ŰŸ�� ûũ (�������� �޸� ���)
    // ===========================================

    // ������ ������Ʈ ������ ���� ��ƼƼ���� �����͸� ���� �����ϴ� �޸� ûũ
    struct ArchetypeChunk {
        // �� ��ŰŸ���� ���� ������Ʈ���� Ÿ�� ���� (��Ʈ����ũ ��� ���)
        std::set<std::type_index> componentTypes;

        // �ش� ûũ�� ���� ��ƼƼ���� ID (������Ʈ �迭�� 1:1 ��Ī)
        std::vector<EntityID> entityIDs;

        // ���� ������Ʈ ������ �迭 (�޸� ���Ӽ� Ȯ��)
        std::vector<TransformComponent> transforms;
        std::vector<HealthComponent> healths;

        // ûũ �ʱ�ȭ: � Ÿ�� �������� �����Ǿ����� ����
        ArchetypeChunk(std::set<std::type_index> types) : componentTypes(std::move(types)) {}

        // ûũ�� �� ��ƼƼ�� ������Ʈ�� �߰��մϴ�.
        void AddEntity(EntityID id, const TransformComponent& t, const HealthComponent& h) {
            entityIDs.push_back(id);
            transforms.push_back(t); // T1, T2, T3... �� ���������� ����
            healths.push_back(h);     // H1, H2, H3... �� T �迭 �ٷ� �ڿ� ���� (���� ������)
        }

        // �� ûũ�� Ư�� ������Ʈ ������ ������ �ִ��� Ȯ���մϴ�.
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
    // 3. ECS �Ŵ��� (��ŰŸ�� ����)
    // ===========================================

    class SimpleECS {
    private:
        std::map<std::set<std::type_index>, ArchetypeChunk> m_Archetypes;
        EntityID m_NextEntityID = 1;

        // Transform + Health ������ ��Ÿ���� Ÿ�� ����
        const std::set<std::type_index> TransformHealthType = {
            std::type_index(typeid(TransformComponent)),
            std::type_index(typeid(HealthComponent))
        };

    public:
        SimpleECS() {
            // �ʼ� ��ŰŸ���� �̸� ����
            m_Archetypes.emplace(TransformHealthType, TransformHealthType);
            std::cout << "ECS �ʱ�ȭ: Transform+Health ��ŰŸ�� ���� �Ϸ�.\n";
        }

        // ��ƼƼ�� �����ϰ� ��ŰŸ�� ûũ�� ������Ʈ�� �߰��մϴ�.
        EntityID CreateEntityWithComponents(const TransformComponent& t, const HealthComponent& h) {
            EntityID id = m_NextEntityID++;

            // Transform + Health ��ŰŸ���� ã�� ������ ����
            if (m_Archetypes.count(TransformHealthType)) {
                m_Archetypes.at(TransformHealthType).AddEntity(id, t, h);
                std::cout << " -> ��ƼƼ " << id << " ���� �� 'Transform+Health' ûũ�� ��ġ.\n";
            }
            return id;
        }

        // �ý��� ������ ���� ��ŰŸ�� ����� ��ȯ�մϴ�.
        // (���� ECS������ ���������� ĳ�̵Ǿ� �ſ� ����)
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
    // 4. �ý��� (���� ó��)
    // ===========================================

    // TransformComponent�� HealthComponent�� ���� ��ƼƼ�� ��ȸ�ϸ� ü���� ��� �ý���
    class DamageSystem {
    public:
        void Update(SimpleECS& ecs, float deltaTime) {
            // �� �ý����� �ʿ�� �ϴ� ������Ʈ ���� ����
            const std::set<std::type_index> required = {
                std::type_index(typeid(TransformComponent)),
                std::type_index(typeid(HealthComponent))
            };

            // 1. �ʿ��� ��ŰŸ�� ûũ ����� ����
            std::vector<ArchetypeChunk*> chunks = ecs.QueryArchetypes(required);

            std::cout << "\n[DamageSystem ������Ʈ ����] - " << chunks.size() << "�� ûũ ��ȸ.\n";

            // 2. �� ûũ�� ��ȸ�ϸ� �����͸� ���������� ó�� (ĳ�� ȿ�� �ش�ȭ ����!)
            for (ArchetypeChunk* chunk : chunks) {

                // ûũ�� ��� �������� ũ��� ��� �����մϴ�.
                size_t count = chunk->entityIDs.size();

                std::cout << "  - ûũ(" << count << "�� ��ƼƼ) ó�� ��...\n";

                // �޸𸮻� ���������� �پ��ִ� �� �迭(transforms, healths)�� ���ÿ� ��ȸ
                for (size_t i = 0; i < count; ++i) {
                    // ��ƼƼ ID�� ��������� ��� ����
                    EntityID currentID = chunk->entityIDs[i];

                    // ���������� ��ġ�� ������Ʈ �����Ϳ� ���� ����
                    TransformComponent& transform = chunk->transforms[i];
                    HealthComponent& health = chunk->healths[i];

                    // 3. ���� ���� (Ʈ�������� Ư�� ��ġ ���ϸ� �������� ���ظ� �Դ´ٰ� ����)
                    if (transform.y < 0.0f) {
                        health.value -= static_cast<int>(10.0f * deltaTime);
                        std::cout << "    * Entity " << currentID
                            << " (Y:" << transform.y << ") ���� ���� -> Health: "
                            << health.value << '\n';
                    }
                }
            }
            std::cout << "[DamageSystem ������Ʈ ����]\n";
        }
    };

    int Run() {
        // 1. ECS �� �ý��� �ʱ�ȭ
        SimpleECS ecs;
        DamageSystem damageSystem;
        float deltaTime = 0.5f;

        // 2. ��ƼƼ ���� (Transform + Health ����)
        ecs.CreateEntityWithComponents({ 0.0f, 5.0f, 0.0f }, { 100 });   // ��ƼƼ 1 (����)
        ecs.CreateEntityWithComponents({ 10.0f, -2.0f, 0.0f }, { 80 });  // ��ƼƼ 2 (���� ����)
        ecs.CreateEntityWithComponents({ 5.0f, 8.0f, 0.0f }, { 100 });   // ��ƼƼ 3 (����)
        ecs.CreateEntityWithComponents({ -5.0f, -0.5f, 0.0f }, { 95 }); // ��ƼƼ 4 (���� ����)

        // 3. �ý��� ������Ʈ (�� ������)
        std::cout << "\n--- 1��° ������ ---\n";
        damageSystem.Update(ecs, deltaTime);

        std::cout << "\n--- 2��° ������ ---\n";
        damageSystem.Update(ecs, deltaTime);

        return 0;
    }
}

namespace Arch02
{
    // Ÿ���� ���� ���ϱ� ���� ��Ī
    using EntityID = size_t;
    using ArchetypeKey = std::set<std::type_index>;

    // ===========================================
    // 1. ������Ʈ (���� ������)
    // ===========================================

    struct TransformComponent {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        // ... (Rotation, Scale ��)
    };

    struct HealthComponent {
        int value = 100;
    };

    // ===========================================
    // 2. ��ƼƼ ��ġ ����ü (��ƼƼ�� ����ϴ� ����)
    // ===========================================

    // ��ƼƼ ID�� �ڽ��� ������Ʈ �����͸� ã�� �� �ʿ��� '�ε��� ī��' ����
    struct EntityLocation {
        // 1. ���� ���� ��ŰŸ���� ���� Ű (�������� ����)
        ArchetypeKey archetypeKey;

        // 2. ���� ���� ûũ ������ ���� �� ��° �ε����� �ִ��� (�������� ��ġ)
        size_t chunkIndex;
    };

    // ===========================================
    // 3. ��ŰŸ�� ûũ (���� ������ �����)
    // ===========================================

    struct ArchetypeChunk {
        // �� ûũ�� ���� ��ƼƼ���� ID ���
        std::vector<EntityID> entityIDs;

        // ���� ������Ʈ ������ �迭 (�޸� ���Ӽ� �ٽ�)
        std::vector<TransformComponent> transforms;
        std::vector<HealthComponent> healths;

        // ���� ûũ�� ��ƼƼ�� ������Ʈ�� �߰��մϴ�.
        size_t AddEntity(EntityID id, const TransformComponent& t, const HealthComponent& h) {
            size_t index = entityIDs.size(); // ���� �߰��� �ε���
            entityIDs.push_back(id);
            transforms.push_back(t);
            healths.push_back(h);
            return index; // ��ƼƼ�� ûũ ������ �� ��° �ε����� �ִ��� ��ȯ
        }

        // ��ƼƼ�� ûũ ������ ���� �ε����� �̿��Ͽ� ������Ʈ �����͸� ����ϴ�.
        TransformComponent& GetTransform(size_t index) {
            return transforms[index];
        }
        HealthComponent& GetHealth(size_t index) {
            return healths[index];
        }
    };

    // ===========================================
    // 4. ECS �Ŵ��� (������ ���� �� ���)
    // ===========================================

    class SimpleECS {
    private:
        // ��ƼƼ ID -> ��ġ ���� �� (��ƼƼ ID�� ������ ��ġ�� ��� �� �� ����)
        std::map<EntityID, EntityLocation> m_EntityLocations;

        // ��ŰŸ�� Ű -> ���� ûũ ������ �� (��ŰŸ�� Ű�� ������ ûũ�� ã�� �� ����)
        // ���� ���������� ûũ�� ���� ���� �� ������, �ܼ�ȭ�� ���� 1:1�� ����
        std::map<ArchetypeKey, std::shared_ptr<ArchetypeChunk>> m_ArchetypeChunks;

        EntityID m_NextEntityID = 1;

        // ���� ��ŰŸ�� Ű (Transform + Health ����)
        const ArchetypeKey ARCHETYPE_TRANSFORM_HEALTH
        {
             std::type_index(typeid(TransformComponent)),
             std::type_index(typeid(HealthComponent))
        };

    public:
        SimpleECS() {
            // Transform + Health ��ŰŸ���� ���� ûũ ���� �� ���
            m_ArchetypeChunks[ARCHETYPE_TRANSFORM_HEALTH] =
                std::make_shared<ArchetypeChunk>();
        }

        EntityID CreateEntity(const TransformComponent& t, const HealthComponent& h) {
            EntityID id = m_NextEntityID++;

            // 1. �ش� ��ŰŸ�� ûũ�� ã���ϴ�.
            auto& chunk = m_ArchetypeChunks.at(ARCHETYPE_TRANSFORM_HEALTH);

            // 2. ûũ�� ���� ������Ʈ �����͸� �����ϰ�, ûũ �� �ε����� ����ϴ�.
            size_t chunkIndex = chunk->AddEntity(id, t, h);

            // 3. ��ƼƼ ID�� ��ġ ������ ����մϴ�. (ID�� �ε����� ����ŵ�ϴ�)
            m_EntityLocations[id] = { ARCHETYPE_TRANSFORM_HEALTH, chunkIndex };

            std::cout << "��ƼƼ " << id << " ����: [��ŰŸ�� Ű: " << ARCHETYPE_TRANSFORM_HEALTH.size() << ", ûũ �ε���: " << chunkIndex << "]\n";
            return id;
        }

        // �ܺο��� ��ƼƼ ID�� ���� ������Ʈ�� ��� �Լ�
        TransformComponent& GetTransformComponent(EntityID id) {
            // 1. ��ƼƼ ID�κ��� ��ġ ����(ArchetypeKey, ChunkIndex)�� ����ϴ�.
            const EntityLocation& location = m_EntityLocations.at(id);

            // 2. ArchetypeKey�� ���� ���� ������ �����(ûũ)�� ã���ϴ�.
            auto& chunk = m_ArchetypeChunks.at(location.archetypeKey);

            // 3. ChunkIndex�� ���� ûũ ���� �������� �迭���� �����͸� �����մϴ�.
            std::cout << " -> ��ƼƼ " << id << ": ûũ �ε��� " << location.chunkIndex << "�� ����� ������ ����\n";
            return chunk->GetTransform(location.chunkIndex);
        }
    };

    int Run() {
        SimpleECS ecs;

        // 1. ��ƼƼ ���� (�����ʹ� ûũ�� ���������� ����)
        EntityID playerID = ecs.CreateEntity({ 10.0f, 5.0f, 0.0f }, { 100 });
        EntityID enemyID = ecs.CreateEntity({ -5.0f, 0.0f, 0.0f }, { 80 });
        EntityID allyID = ecs.CreateEntity({ 20.0f, 1.0f, 0.0f }, { 90 });

        std::cout << "\n--- ��ƼƼ ������ ���� ���� ---\n";

        // 2. ��ƼƼ ID�� ���� Transform �����Ϳ� ����
        TransformComponent& playerTransform = ecs.GetTransformComponent(playerID);
        TransformComponent& enemyTransform = ecs.GetTransformComponent(enemyID);

        std::cout << "Player X ��ġ: " << playerTransform.x << '\n';
        std::cout << "Enemy X ��ġ: " << enemyTransform.x << '\n';

        // 3. ������ ����
        playerTransform.x += 5.0f;
        std::cout << "Player X ��ġ (���� ��): " << playerTransform.x << '\n';

        std::cout << "\n����: �ý����� ��ƼƼ ID�� ��ġ�� �ʰ�, ûũ�� �ٷ� ��ȸ�մϴ�.\n";

        return 0;
    }
}
