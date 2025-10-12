#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <typeindex>

// -----------------------------------------------------------------------------
// 1. 설정 및 타입 정의
// -----------------------------------------------------------------------------

// ECS 시스템에서 사용할 수 있는 최대 컴포넌트 개수 (256비트)
constexpr size_t MAX_COMPONENTS = 256;

// 아키타입 키는 256비트의 비트셋으로 정의됩니다.
using ArchetypeKey = std::bitset<MAX_COMPONENTS>;

// -----------------------------------------------------------------------------
// 2. 컴포넌트 타입 ID 매퍼
// -----------------------------------------------------------------------------

// 모든 컴포넌트 타입에 대해 0부터 순차적으로 고유 ID (비트 인덱스)를 할당하는 클래스
class ComponentTypeID
{
private:
    static size_t nextID;

    // 컴포넌트 타입 T에 대한 고유 ID를 정적으로 할당
    template<typename T>
    static size_t getNextID() {
        // 컴파일러가 타입 T에 대해 처음 이 함수를 호출할 때만 nextID를 증가시켜 ID를 할당합니다.
        static size_t currentID = nextID++;
        // 256개 초과 시 런타임 에러를 방지합니다.
        if (currentID >= MAX_COMPONENTS) {
            throw std::runtime_error("Error: Component limit exceeded.");
        }
        return currentID;
    }

public:
    // 타입 T의 고유 ID(비트 인덱스)를 반환하는 공용 인터페이스
    template<typename T>
    static size_t getID() {
        // ID는 T 타입에 대해 한번만 초기화됩니다.
        static size_t ID = getNextID<T>();
        return ID;
    }
};

// 정적 멤버 초기화
size_t ComponentTypeID::nextID = 0;

// -----------------------------------------------------------------------------
// 3. 예시 컴포넌트 정의 (더미 구조체)
// -----------------------------------------------------------------------------

struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Health { int value; };
struct Input { bool active; };
struct Renderable {};

// -----------------------------------------------------------------------------
// 4. 아키타입 클래스 (간소화)
// -----------------------------------------------------------------------------

// 실제 컴포넌트 청크 데이터를 포함해야 하지만, 여기서는 키만 정의
struct Archetype {
    ArchetypeKey key;
    std::string name;
    // ... 실제 컴포넌트 데이터 청크, 엔티티 리스트 등 ...
};


// -----------------------------------------------------------------------------
// 5. ECS 시스템 관리
// -----------------------------------------------------------------------------

class ECSSystemManager {
public:
    // ArchetypeKey를 키로 사용하여 Archetype 객체를 관리합니다.
    // std::bitset은 기본적으로 std::hash가 정의되어 있어 별도의 해시 함수가 필요 없습니다.
    std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;

    // 아키타입을 등록하고 ArchetypeKey를 생성합니다.
    template<typename... Comps>
    ArchetypeKey createArchetype(const std::string& name) {
        ArchetypeKey key;
        // 템플릿 팩을 순회하며 각 컴포넌트 타입의 비트를 1로 설정합니다.
        (key.set(ComponentTypeID::getID<Comps>()), ...);

        m_Archetypes[key] = Archetype{ key, name };
        std::cout << "-> 아키타입 생성 [" << name << "]: " << key.to_string().substr(MAX_COMPONENTS - 10) << "..." << std::endl;
        return key;
    }

    // 시스템의 시그니처와 특정 아키타입 키가 일치하는지 확인합니다.
    // 시그니처: 시스템이 처리하기 위해 '반드시 필요한' 컴포넌트 조합 (비트셋)
    static bool MatchesSignature(const ArchetypeKey& archetypeKey, const ArchetypeKey& systemSignature) {
        // (archetypeKey & systemSignature) == systemSignature
        // 아키타입 키가 시그니처 키의 모든 비트를 포함하고 있는지 검사합니다.
        return (archetypeKey & systemSignature) == systemSignature;
    }
};

// -----------------------------------------------------------------------------
// 6. 시스템 시그니처 생성 예제
// -----------------------------------------------------------------------------

// 시스템이 요구하는 컴포넌트 시그니처(ArchetypeKey)를 생성합니다.
template<typename... Comps>
ArchetypeKey createSystemSignature() {
    ArchetypeKey signature;
    // 템플릿 팩을 순회하며 필요한 컴포넌트의 비트를 설정합니다.
    (signature.set(ComponentTypeID::getID<Comps>()), ...);
    return signature;
}


int Run() {
    // ID 할당 테스트 (0부터 4까지 순차적으로 할당됨)
    std::cout << "--- 컴포넌트 ID 할당 ---" << std::endl;
    std::cout << "Position ID: " << ComponentTypeID::getID<Position>() << std::endl;  // 0
    std::cout << "Velocity ID: " << ComponentTypeID::getID<Velocity>() << std::endl;  // 1
    std::cout << "Health ID:   " << ComponentTypeID::getID<Health>() << std::endl;    // 2
    std::cout << "Input ID:    " << ComponentTypeID::getID<Input>() << std::endl;     // 3
    std::cout << "Renderable ID: " << ComponentTypeID::getID<Renderable>() << std::endl; // 4
    std::cout << "----------------------" << std::endl << std::endl;

    ECSSystemManager manager;

    // 1. 다양한 아키타입 생성 및 등록
    manager.createArchetype<Position, Velocity, Health>("Player_Archetype");
    manager.createArchetype<Position, Renderable>("Static_Object_Archetype");
    manager.createArchetype<Position, Velocity>("Moving_Object_Archetype");
    manager.createArchetype<Position, Velocity, Input>("Input_Archetype");

    std::cout << "\n--- 시스템 시그니처 생성 ---" << std::endl;

    // 2. 시스템이 요구하는 시그니처 정의
    // (A) PhysicsSystem: Position, Velocity가 필요
    ArchetypeKey physicsSignature = createSystemSignature<Position, Velocity>();
    std::cout << "(A) PhysicsSystem 시그니처 (Pos, Vel): "
        << physicsSignature.to_string().substr(MAX_COMPONENTS - 10) << "..." << std::endl;

    // (B) HealthSystem: Health가 필요
    ArchetypeKey healthSignature = createSystemSignature<Health>();
    std::cout << "(B) HealthSystem 시그니처 (Health): "
        << healthSignature.to_string().substr(MAX_COMPONENTS - 10) << "..." << std::endl;

    std::cout << "\n--- 시그니처 매칭 테스트 (O(1) 연산) ---" << std::endl;

    // 3. 모든 아키타입을 순회하며 매칭 테스트
    for (const auto& pair : manager.m_Archetypes) {
        const Archetype& arch = pair.second;
        std::cout << "[" << arch.name << " (" << arch.key.to_string().substr(MAX_COMPONENTS - 10) << "...) ]" << std::endl;

        // (A) PhysicsSystem 매칭 체크
        if (ECSSystemManager::MatchesSignature(arch.key, physicsSignature)) {
            std::cout << "  -> (A) PhysicsSystem: 매칭됨 (필요한 컴포넌트 모두 포함)" << std::endl;
        }
        else {
            std::cout << "  -> (A) PhysicsSystem: 매칭 안 됨" << std::endl;
        }

        // (B) HealthSystem 매칭 체크
        if (ECSSystemManager::MatchesSignature(arch.key, healthSignature)) {
            std::cout << "  -> (B) HealthSystem: 매칭됨 (필요한 컴포넌트 모두 포함)" << std::endl;
        }
        else {
            std::cout << "  -> (B) HealthSystem: 매칭 안 됨" << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
