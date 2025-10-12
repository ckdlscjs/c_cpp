#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <typeindex>

// -----------------------------------------------------------------------------
// 1. ���� �� Ÿ�� ����
// -----------------------------------------------------------------------------

// ECS �ý��ۿ��� ����� �� �ִ� �ִ� ������Ʈ ���� (256��Ʈ)
constexpr size_t MAX_COMPONENTS = 256;

// ��ŰŸ�� Ű�� 256��Ʈ�� ��Ʈ������ ���ǵ˴ϴ�.
using ArchetypeKey = std::bitset<MAX_COMPONENTS>;

// -----------------------------------------------------------------------------
// 2. ������Ʈ Ÿ�� ID ����
// -----------------------------------------------------------------------------

// ��� ������Ʈ Ÿ�Կ� ���� 0���� ���������� ���� ID (��Ʈ �ε���)�� �Ҵ��ϴ� Ŭ����
class ComponentTypeID
{
private:
    static size_t nextID;

    // ������Ʈ Ÿ�� T�� ���� ���� ID�� �������� �Ҵ�
    template<typename T>
    static size_t getNextID() {
        // �����Ϸ��� Ÿ�� T�� ���� ó�� �� �Լ��� ȣ���� ���� nextID�� �������� ID�� �Ҵ��մϴ�.
        static size_t currentID = nextID++;
        // 256�� �ʰ� �� ��Ÿ�� ������ �����մϴ�.
        if (currentID >= MAX_COMPONENTS) {
            throw std::runtime_error("Error: Component limit exceeded.");
        }
        return currentID;
    }

public:
    // Ÿ�� T�� ���� ID(��Ʈ �ε���)�� ��ȯ�ϴ� ���� �������̽�
    template<typename T>
    static size_t getID() {
        // ID�� T Ÿ�Կ� ���� �ѹ��� �ʱ�ȭ�˴ϴ�.
        static size_t ID = getNextID<T>();
        return ID;
    }
};

// ���� ��� �ʱ�ȭ
size_t ComponentTypeID::nextID = 0;

// -----------------------------------------------------------------------------
// 3. ���� ������Ʈ ���� (���� ����ü)
// -----------------------------------------------------------------------------

struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health { int value; };
struct Input { bool active; };
struct Renderable {};

// -----------------------------------------------------------------------------
// 4. ��ŰŸ�� Ŭ���� (����ȭ)
// -----------------------------------------------------------------------------

// ���� ������Ʈ ûũ �����͸� �����ؾ� ������, ���⼭�� Ű�� ����
struct Archetype {
    ArchetypeKey key;
    std::string name;
    // ... ���� ������Ʈ ������ ûũ, ��ƼƼ ����Ʈ �� ...
};


// -----------------------------------------------------------------------------
// 5. ECS �ý��� ����
// -----------------------------------------------------------------------------

class ECSSystemManager {
public:
    // ArchetypeKey�� Ű�� ����Ͽ� Archetype ��ü�� �����մϴ�.
    // std::bitset�� �⺻������ std::hash�� ���ǵǾ� �־� ������ �ؽ� �Լ��� �ʿ� �����ϴ�.
    std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;

    // ��ŰŸ���� ����ϰ� ArchetypeKey�� �����մϴ�.
    template<typename... Comps>
    ArchetypeKey createArchetype(const std::string& name) {
        ArchetypeKey key;
        // ���ø� ���� ��ȸ�ϸ� �� ������Ʈ Ÿ���� ��Ʈ�� 1�� �����մϴ�.
        (key.set(ComponentTypeID::getID<Comps>()), ...);

        m_Archetypes[key] = Archetype{ key, name };
        std::cout << "-> ��ŰŸ�� ���� [" << name << "]: " << key.to_string().substr(MAX_COMPONENTS - 10) << "..." << std::endl;
        return key;
    }

    // �ý����� �ñ״�ó�� Ư�� ��ŰŸ�� Ű�� ��ġ�ϴ��� Ȯ���մϴ�.
    // �ñ״�ó: �ý����� ó���ϱ� ���� '�ݵ�� �ʿ���' ������Ʈ ���� (��Ʈ��)
    static bool MatchesSignature(const ArchetypeKey& archetypeKey, const ArchetypeKey& systemSignature) {
        // (archetypeKey & systemSignature) == systemSignature
        // ��ŰŸ�� Ű�� �ñ״�ó Ű�� ��� ��Ʈ�� �����ϰ� �ִ��� �˻��մϴ�.
        return (archetypeKey & systemSignature) == systemSignature;
    }
};

// -----------------------------------------------------------------------------
// 6. �ý��� �ñ״�ó ���� ����
// -----------------------------------------------------------------------------

// �ý����� �䱸�ϴ� ������Ʈ �ñ״�ó(ArchetypeKey)�� �����մϴ�.
template<typename... Comps>
ArchetypeKey createSystemSignature() {
    ArchetypeKey signature;
    // ���ø� ���� ��ȸ�ϸ� �ʿ��� ������Ʈ�� ��Ʈ�� �����մϴ�.
    (signature.set(ComponentTypeID::getID<Comps>()), ...);
    return signature;
}


int Run() {
    // ID �Ҵ� �׽�Ʈ (0���� 4���� ���������� �Ҵ��)
    std::cout << "--- ������Ʈ ID �Ҵ� ---" << std::endl;
    std::cout << "Position ID: " << ComponentTypeID::getID<Position>() << std::endl;  // 0
    std::cout << "Velocity ID: " << ComponentTypeID::getID<Velocity>() << std::endl;  // 1
    std::cout << "Health ID:   " << ComponentTypeID::getID<Health>() << std::endl;    // 2
    std::cout << "Input ID:    " << ComponentTypeID::getID<Input>() << std::endl;     // 3
    std::cout << "Renderable ID: " << ComponentTypeID::getID<Renderable>() << std::endl; // 4
    std::cout << "----------------------" << std::endl << std::endl;

    ECSSystemManager manager;

    // 1. �پ��� ��ŰŸ�� ���� �� ���
    manager.createArchetype<Position, Velocity, Health>("Player_Archetype");
    manager.createArchetype<Position, Renderable>("Static_Object_Archetype");
    manager.createArchetype<Position, Velocity>("Moving_Object_Archetype");
    manager.createArchetype<Position, Velocity, Input>("Input_Archetype");

    std::cout << "\n--- �ý��� �ñ״�ó ���� ---" << std::endl;

    // 2. �ý����� �䱸�ϴ� �ñ״�ó ����
    // (A) PhysicsSystem: Position, Velocity�� �ʿ�
    ArchetypeKey physicsSignature = createSystemSignature<Position, Velocity>();
    std::cout << "(A) PhysicsSystem �ñ״�ó (Pos, Vel): "
        << physicsSignature.to_string().substr(MAX_COMPONENTS - 10) << "..." << std::endl;

    // (B) HealthSystem: Health�� �ʿ�
    ArchetypeKey healthSignature = createSystemSignature<Health>();
    std::cout << "(B) HealthSystem �ñ״�ó (Health): "
        << healthSignature.to_string().substr(MAX_COMPONENTS - 10) << "..." << std::endl;

    std::cout << "\n--- �ñ״�ó ��Ī �׽�Ʈ (O(1) ����) ---" << std::endl;

    // 3. ��� ��ŰŸ���� ��ȸ�ϸ� ��Ī �׽�Ʈ
    for (const auto& pair : manager.m_Archetypes) {
        const Archetype& arch = pair.second;
        std::cout << "[" << arch.name << " (" << arch.key.to_string().substr(MAX_COMPONENTS - 10) << "...) ]" << std::endl;

        // (A) PhysicsSystem ��Ī üũ
        if (ECSSystemManager::MatchesSignature(arch.key, physicsSignature)) {
            std::cout << "  -> (A) PhysicsSystem: ��Ī�� (�ʿ��� ������Ʈ ��� ����)" << std::endl;
        }
        else {
            std::cout << "  -> (A) PhysicsSystem: ��Ī �� ��" << std::endl;
        }

        // (B) HealthSystem ��Ī üũ
        if (ECSSystemManager::MatchesSignature(arch.key, healthSignature)) {
            std::cout << "  -> (B) HealthSystem: ��Ī�� (�ʿ��� ������Ʈ ��� ����)" << std::endl;
        }
        else {
            std::cout << "  -> (B) HealthSystem: ��Ī �� ��" << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
