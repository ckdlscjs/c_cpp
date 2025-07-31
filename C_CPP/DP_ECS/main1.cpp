//1. 엔티티(Entity)
//엔티티는 게임 월드의 모든 '것'을 나타냅니다.고유한 ID를 가지며, 그 자체로는 아무런 기능도 없습니다.기능은 컴포넌트를 통해 부여됩니다.
//
//C++

#include <iostream>
#include <vector>
#include <string>
#include <memory> // std::shared_ptr, std::unique_ptr
#include <typeindex> // std::type_index
#include <unordered_map> // std::unordered_map
#include <algorithm> // std::remove_if

// 간단한 벡터/행렬 라이브러리 (실제로는 DirectXMath, GLM 등 사용)
struct Vector3 {
    float x, y, z;
    Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    // ... 기타 연산자
};

struct Color {
    float r, g, b, a;
    Color(float _r = 1, float _g = 1, float _b = 1, float _a = 1) : r(_r), g(_g), b(_b), a(_a) {}
};

// --- Component (기본 클래스) ---
class Component {
public:
    virtual ~Component() = default;
    // 모든 컴포넌트는 자신이 속한 엔티티의 ID를 알 수 있습니다.
    // 하지만 직접 접근하기보다 시스템을 통해 상호작용하는 것이 좋습니다.
    size_t ownerEntityId = 0;
};

// --- Entity ---
class Entity {
private:
    static size_t nextId;
    size_t id;
    std::string name;
    // 각 컴포넌트 타입을 키로, 컴포넌트 인스턴스를 값으로 저장
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public:
    Entity(const std::string& n = "UnnamedEntity") : name(n) {
        id = nextId++;
        std::cout << "Entity '" << name << "' (ID: " << id << ") created.\n";
    }

    size_t GetID() const { return id; }
    const std::string& GetName() const { return name; }

    // 컴포넌트 추가
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        std::shared_ptr<T> component = std::make_shared<T>(std::forward<Args>(args)...);
        component->ownerEntityId = id; // 소유자 ID 설정
        components[typeid(T)] = component;
        std::cout << "  Component " << typeid(T).name() << " added to Entity '" << name << "'.\n";
        return component.get();
    }

    // 컴포넌트 가져오기
    template<typename T>
    T* GetComponent() const {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    // 컴포넌트 제거
    template<typename T>
    void RemoveComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            components.erase(it);
            std::cout << "  Component " << typeid(T).name() << " removed from Entity '" << name << "'.\n";
        }
    }

    // 특정 타입의 컴포넌트가 있는지 확인
    template<typename T>
    bool HasComponent() const {
        return components.count(typeid(T)) > 0;
    }
};
size_t Entity::nextId = 0; // 정적 멤버 초기화
//2. 컴포넌트(Component)
//컴포넌트는 특정 데이터와 그 데이터를 표현하는 데 필요한 아주 최소한의 로직을 가집니다.특정 기능의 "데이터 뭉치"라고 생각할 수 있습니다.
//
//C++

// --- Transform Component ---
class TransformComponent : public Component {
public:
    Vector3 position;
    Vector3 rotation; // Euler angles for simplicity
    Vector3 scale;

    TransformComponent(const Vector3& pos = { 0,0,0 },
        const Vector3& rot = { 0,0,0 },
        const Vector3& sca = { 1,1,1 })
        : position(pos), rotation(rot), scale(sca) {
    }

    void Translate(const Vector3& delta) { position = position + delta; }
    // ... Rotate, Scale 등의 메서드
};

// --- Light Component ---
enum class LightType { DIRECTIONAL, POINT, SPOT };

class LightComponent : public Component {
public:
    LightType type;
    Color color;
    float intensity; // 빛의 강도

    // PointLight/SpotLight 전용
    Vector3 position;
    float range;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;

    // DirectionalLight/SpotLight 전용
    Vector3 direction;

    // SpotLight 전용
    float innerConeAngle; // 코사인 값으로 저장하는 것이 일반적
    float outerConeAngle; // 코사인 값으로 저장하는 것이 일반적
    float spotExponent;

    LightComponent(LightType t, const Color& c = { 1,1,1,1 }, float i = 1.0f)
        : type(t), color(c), intensity(i),
        position({ 0,0,0 }), range(10.0f),
        attenuationConstant(1.0f), attenuationLinear(0.09f), attenuationQuadratic(0.032f),
        direction({ 0,-1,0 }), innerConeAngle(0.9f), outerConeAngle(0.8f), spotExponent(8.0f) // 기본값
    {
    }

    // 편의 메서드 (예시)
    void SetDirectionalLight(const Color& c, const Vector3& dir, float i = 1.0f) {
        type = LightType::DIRECTIONAL;
        color = c;
        intensity = i;
        direction = dir;
    }
    // ... 다른 타입의 조명 설정을 위한 Set 메서드들
};

// --- Mesh Component ---
class MeshComponent : public Component {
public:
    std::string meshId; // 실제 메쉬 데이터는 ResourceSystem에서 관리되는 ID
    std::string materialId; // 실제 재질 데이터는 MaterialSystem에서 관리되는 ID

    MeshComponent(const std::string& mId = "defaultMesh", const std::string& matId = "defaultMaterial")
        : meshId(mId), materialId(matId) {
    }
};
//3. 시스템(System)
//시스템은 특정 타입의 컴포넌트들을 처리하는 로직을 가집니다.모든 엔티티를 순회하며 특정 컴포넌트를 가진 엔티티에 대해서만 작업을 수행합니다.
//
//C++

// --- System (기본 클래스) ---
class System {
public:
    virtual ~System() = default;
    virtual void Update(float deltaTime) = 0;
    // 시스템이 처리할 엔티티들을 등록/해제하는 메서드 (외부에서 호출)
    void RegisterEntity(Entity* entity) { m_entities.push_back(entity); }
    void UnregisterEntity(Entity* entity) {
        m_entities.erase(std::remove(m_entities.begin(), m_entities.at(0), entity), m_entities.end());
    }

protected:
    std::vector<Entity*> m_entities; // 이 시스템이 관심 있는 엔티티들
};

// --- Transform System ---
class TransformSystem : public System {
public:
    void Update(float deltaTime) override {
        // 모든 엔티티를 순회하며 TransformComponent를 가진 엔티티만 처리
        // 실제로는 더 최적화된 방법으로 특정 컴포넌트를 가진 엔티티 목록을 관리
        std::cout << "--- TransformSystem Update ---\n";
        for (Entity* entity : m_entities) {
            TransformComponent* transform = entity->GetComponent<TransformComponent>();
            if (transform) {
                // transform->position.x += 1.0f * deltaTime; // 예시: 움직임 적용
                std::cout << "  Entity '" << entity->GetName()
                    << "' Position: (" << transform->position.x << ", "
                    << transform->position.y << ", " << transform->position.z << ")\n";
            }
        }
    }
};

// --- Light System ---
// 이 시스템은 빛 컴포넌트를 가진 엔티티를 찾아 셰이더로 보낼 준비를 합니다.
class LightSystem : public System {
public:
    void Update(float deltaTime) override {
        std::cout << "--- LightSystem Update ---\n";
        // 셰이더로 보낼 LightInfo 구조체 배열 (실제 상수 버퍼와 매핑)
        // 최대 조명 개수를 정의해야 합니다.
        // LightInfo shaderLights[MAX_LIGHTS]; 
        int lightCount = 0;

        for (Entity* entity : m_entities) {
            LightComponent* lightComp = entity->GetComponent<LightComponent>();
            if (lightComp) {
                // 이 조명 컴포넌트가 Transform 컴포넌트도 가지고 있다면 위치/방향 정보 가져오기
                TransformComponent* transformComp = entity->GetComponent<TransformComponent>();

                std::cout << "  Processing Light for Entity '" << entity->GetName() << "' (Type: ";
                switch (lightComp->type) {
                    case LightType::DIRECTIONAL: std::cout << "Directional"; break;
                    case LightType::POINT: std::cout << "Point"; break;
                    case LightType::SPOT: std::cout << "Spot"; break;
                }
                std::cout << ")\n";

                // LightInfo 구조체에 데이터를 채우고 셰이더로 보낼 준비
                // (이 부분은 GPU API에 따라 다릅니다)
                // shaderLights[lightCount].color = lightComp->color;
                // if (transformComp) shaderLights[lightCount].position = transformComp->position;
                // ... 등등

                lightCount++;
                // MAX_LIGHTS를 초과하면 처리 중단 또는 경고
            }
        }
        // 최종적으로 lightCount와 shaderLights 배열을 GPU 상수 버퍼에 업로드
        std::cout << "  Total " << lightCount << " lights prepared for shader.\n";
    }
};

// --- Render System (간략화) ---
class RenderSystem : public System {
public:
    void Update(float deltaTime) override {
        std::cout << "--- RenderSystem Update ---\n";
        for (Entity* entity : m_entities) {
            MeshComponent* meshComp = entity->GetComponent<MeshComponent>();
            TransformComponent* transformComp = entity->GetComponent<TransformComponent>();
            if (meshComp && transformComp) {
                std::cout << "  Drawing Entity '" << entity->GetName()
                    << "' Mesh: " << meshComp->meshId
                    << ", Material: " << meshComp->materialId
                    << ", at Position: (" << transformComp->position.x << ", "
                    << transformComp->position.y << ", " << transformComp->position.z << ")\n";
                // 실제로는 여기서 렌더링 API 호출 (SetMesh, SetMaterial, SetTransform, DrawIndexed 등)
            }
        }
    }
};
//4. 게임 루프 / 씬 매니저
//엔티티, 컴포넌트, 시스템을 통합하고 관리하는 최상위 계층입니다.
//
//C++

// --- Game (또는 SceneManager) ---
class Game {
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<System>> systems;

public:
    Entity* CreateEntity(const std::string& name = "UnnamedEntity") {
        entities.push_back(std::make_unique<Entity>(name));
        return entities.back().get();
    }

    template<typename T, typename... Args>
    T* AddSystem(Args&&... args) {
        static_assert(std::is_base_of<System, T>::value, "T must inherit from System");
        std::unique_ptr<T> system = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = system.get();
        systems.push_back(std::move(system));
        return rawPtr;
    }

    void InitSystems() {
        // 각 시스템에 관심 있는 엔티티들을 등록합니다.
        // 실제로는 더 복잡한 매니저가 엔티티 추가/제거 시 시스템에 알립니다.
        for (auto& sys : systems) {
            for (auto& ent : entities) {
                // 여기서는 모든 시스템에 모든 엔티티를 등록하고, 시스템 내부에서 컴포넌트 유무를 체크합니다.
                // 더 나은 방법: System이 관심있는 컴포넌트 타입을 명시하고,
                // Entity Manager가 엔티티 생성/컴포넌트 추가/제거 시 해당 시스템에 알리는 방식.
                sys->RegisterEntity(ent.get());
            }
        }
    }

    void Run() {
        float deltaTime = 1.0f / 60.0f; // 예시 델타 타임
        std::cout << "\n--- Game Loop Start ---\n";

        // 프레임마다 모든 시스템 업데이트
        for (auto& sys : systems) {
            sys->Update(deltaTime);
        }

        std::cout << "\n--- Game Loop End ---\n";
    }
};

// --- 메인 함수 ---
int main() {
    Game game;

    // 시스템 추가
    TransformSystem* transformSystem = game.AddSystem<TransformSystem>();
    LightSystem* lightSystem = game.AddSystem<LightSystem>();
    RenderSystem* renderSystem = game.AddSystem<RenderSystem>(); // LightSystem 뒤에 실행되도록

    // 엔티티 생성 및 컴포넌트 추가
    // 1. 플레이어 엔티티 (트랜스폼, 메쉬)
    Entity* player = game.CreateEntity("Player");
    player->AddComponent<TransformComponent>(Vector3(0, 0, 0));
    player->AddComponent<MeshComponent>("playerMesh", "playerMaterial");

    // 2. 씬 루트 엔티티 (전역 조명용)
    // DirectionalLight는 위치에 종속되지 않으므로, 가상의 'SceneRoot' 엔티티에 붙입니다.
    Entity* sceneRoot = game.CreateEntity("SceneRoot");
    LightComponent* mainDirLight = sceneRoot->AddComponent<LightComponent>(LightType::DIRECTIONAL);
    mainDirLight->SetDirectionalLight({ 1.0f, 0.95f, 0.9f, 1.0f }, { 0.5f, -1.0f, 0.5f, 0.0f }, 1.0f); // 태양광

    // 3. 램프 엔티티 (트랜스폼, 포인트 라이트)
    Entity* lamp = game.CreateEntity("Lamp");
    lamp->AddComponent<TransformComponent>(Vector3(5, 2, 0));
    LightComponent* pointLight = lamp->AddComponent<LightComponent>(LightType::POINT, Color(1.0f, 0.5f, 0.0f, 1.0f), 0.8f);
    pointLight->position = Vector3(5, 2, 0); // TransformComponent와 중복될 수 있으므로, LightSystem에서 Transform을 읽어가도록 구현하는 것이 좋음

    // 4. 스포트라이트 엔티티 (트랜스폼, 스포트 라이트)
    Entity* spotlightSource = game.CreateEntity("SpotlightSource");
    spotlightSource->AddComponent<TransformComponent>(Vector3(-3, 3, -3));
    LightComponent* spotLight = spotlightSource->AddComponent<LightComponent>(LightType::SPOT, Color(0.0f, 1.0f, 0.0f, 1.0f), 0.5f);
    spotLight->position = Vector3(-3, 3, -3);
    spotLight->direction = Vector3(0, -1, 0); // 아래 방향

    game.InitSystems(); // 시스템에 엔티티 등록

    game.Run(); // 게임 루프 실행

    return 0;
}
/*ECS의 핵심 원칙 요약
엔티티 : 게임 오브젝트의 ID일 뿐, 그 자체로는 기능이 없습니다.

컴포넌트 : 특정 기능에 필요한 데이터와 최소한의 로직을 가집니다. (예 : TransformComponent는 위치, 회전, 크기 데이터만, LightComponent는 조명의 타입, 색상, 강도 등 데이터만 가짐)

시스템 : 특정 타입의 컴포넌트들을 가진 모든 엔티티를 순회하며 해당 기능을 처리하는 로직을 가집니다. (LightSystem은 모든 LightComponent를 찾아 GPU로 데이터 업로드)

이러한 분리를 통해 코드의 유연성, 확장성, 재사용성이 크게 향상됩니다.말씀하신대로 Light, Transform 등을 컴포넌트화하여 엔티티에 붙이고 시스템이 이를 처리하는 방식은 현대 게임 엔진에서 매우 효과적인 설계입니다.*/