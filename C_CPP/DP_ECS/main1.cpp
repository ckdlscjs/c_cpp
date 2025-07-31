//1. ��ƼƼ(Entity)
//��ƼƼ�� ���� ������ ��� '��'�� ��Ÿ���ϴ�.������ ID�� ������, �� ��ü�δ� �ƹ��� ��ɵ� �����ϴ�.����� ������Ʈ�� ���� �ο��˴ϴ�.
//
//C++

#include <iostream>
#include <vector>
#include <string>
#include <memory> // std::shared_ptr, std::unique_ptr
#include <typeindex> // std::type_index
#include <unordered_map> // std::unordered_map
#include <algorithm> // std::remove_if

// ������ ����/��� ���̺귯�� (�����δ� DirectXMath, GLM �� ���)
struct Vector3 {
    float x, y, z;
    Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    // ... ��Ÿ ������
};

struct Color {
    float r, g, b, a;
    Color(float _r = 1, float _g = 1, float _b = 1, float _a = 1) : r(_r), g(_g), b(_b), a(_a) {}
};

// --- Component (�⺻ Ŭ����) ---
class Component {
public:
    virtual ~Component() = default;
    // ��� ������Ʈ�� �ڽ��� ���� ��ƼƼ�� ID�� �� �� �ֽ��ϴ�.
    // ������ ���� �����ϱ⺸�� �ý����� ���� ��ȣ�ۿ��ϴ� ���� �����ϴ�.
    size_t ownerEntityId = 0;
};

// --- Entity ---
class Entity {
private:
    static size_t nextId;
    size_t id;
    std::string name;
    // �� ������Ʈ Ÿ���� Ű��, ������Ʈ �ν��Ͻ��� ������ ����
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public:
    Entity(const std::string& n = "UnnamedEntity") : name(n) {
        id = nextId++;
        std::cout << "Entity '" << name << "' (ID: " << id << ") created.\n";
    }

    size_t GetID() const { return id; }
    const std::string& GetName() const { return name; }

    // ������Ʈ �߰�
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        std::shared_ptr<T> component = std::make_shared<T>(std::forward<Args>(args)...);
        component->ownerEntityId = id; // ������ ID ����
        components[typeid(T)] = component;
        std::cout << "  Component " << typeid(T).name() << " added to Entity '" << name << "'.\n";
        return component.get();
    }

    // ������Ʈ ��������
    template<typename T>
    T* GetComponent() const {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    // ������Ʈ ����
    template<typename T>
    void RemoveComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            components.erase(it);
            std::cout << "  Component " << typeid(T).name() << " removed from Entity '" << name << "'.\n";
        }
    }

    // Ư�� Ÿ���� ������Ʈ�� �ִ��� Ȯ��
    template<typename T>
    bool HasComponent() const {
        return components.count(typeid(T)) > 0;
    }
};
size_t Entity::nextId = 0; // ���� ��� �ʱ�ȭ
//2. ������Ʈ(Component)
//������Ʈ�� Ư�� �����Ϳ� �� �����͸� ǥ���ϴ� �� �ʿ��� ���� �ּ����� ������ �����ϴ�.Ư�� ����� "������ ��ġ"��� ������ �� �ֽ��ϴ�.
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
    // ... Rotate, Scale ���� �޼���
};

// --- Light Component ---
enum class LightType { DIRECTIONAL, POINT, SPOT };

class LightComponent : public Component {
public:
    LightType type;
    Color color;
    float intensity; // ���� ����

    // PointLight/SpotLight ����
    Vector3 position;
    float range;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;

    // DirectionalLight/SpotLight ����
    Vector3 direction;

    // SpotLight ����
    float innerConeAngle; // �ڻ��� ������ �����ϴ� ���� �Ϲ���
    float outerConeAngle; // �ڻ��� ������ �����ϴ� ���� �Ϲ���
    float spotExponent;

    LightComponent(LightType t, const Color& c = { 1,1,1,1 }, float i = 1.0f)
        : type(t), color(c), intensity(i),
        position({ 0,0,0 }), range(10.0f),
        attenuationConstant(1.0f), attenuationLinear(0.09f), attenuationQuadratic(0.032f),
        direction({ 0,-1,0 }), innerConeAngle(0.9f), outerConeAngle(0.8f), spotExponent(8.0f) // �⺻��
    {
    }

    // ���� �޼��� (����)
    void SetDirectionalLight(const Color& c, const Vector3& dir, float i = 1.0f) {
        type = LightType::DIRECTIONAL;
        color = c;
        intensity = i;
        direction = dir;
    }
    // ... �ٸ� Ÿ���� ���� ������ ���� Set �޼����
};

// --- Mesh Component ---
class MeshComponent : public Component {
public:
    std::string meshId; // ���� �޽� �����ʹ� ResourceSystem���� �����Ǵ� ID
    std::string materialId; // ���� ���� �����ʹ� MaterialSystem���� �����Ǵ� ID

    MeshComponent(const std::string& mId = "defaultMesh", const std::string& matId = "defaultMaterial")
        : meshId(mId), materialId(matId) {
    }
};
//3. �ý���(System)
//�ý����� Ư�� Ÿ���� ������Ʈ���� ó���ϴ� ������ �����ϴ�.��� ��ƼƼ�� ��ȸ�ϸ� Ư�� ������Ʈ�� ���� ��ƼƼ�� ���ؼ��� �۾��� �����մϴ�.
//
//C++

// --- System (�⺻ Ŭ����) ---
class System {
public:
    virtual ~System() = default;
    virtual void Update(float deltaTime) = 0;
    // �ý����� ó���� ��ƼƼ���� ���/�����ϴ� �޼��� (�ܺο��� ȣ��)
    void RegisterEntity(Entity* entity) { m_entities.push_back(entity); }
    void UnregisterEntity(Entity* entity) {
        m_entities.erase(std::remove(m_entities.begin(), m_entities.at(0), entity), m_entities.end());
    }

protected:
    std::vector<Entity*> m_entities; // �� �ý����� ���� �ִ� ��ƼƼ��
};

// --- Transform System ---
class TransformSystem : public System {
public:
    void Update(float deltaTime) override {
        // ��� ��ƼƼ�� ��ȸ�ϸ� TransformComponent�� ���� ��ƼƼ�� ó��
        // �����δ� �� ����ȭ�� ������� Ư�� ������Ʈ�� ���� ��ƼƼ ����� ����
        std::cout << "--- TransformSystem Update ---\n";
        for (Entity* entity : m_entities) {
            TransformComponent* transform = entity->GetComponent<TransformComponent>();
            if (transform) {
                // transform->position.x += 1.0f * deltaTime; // ����: ������ ����
                std::cout << "  Entity '" << entity->GetName()
                    << "' Position: (" << transform->position.x << ", "
                    << transform->position.y << ", " << transform->position.z << ")\n";
            }
        }
    }
};

// --- Light System ---
// �� �ý����� �� ������Ʈ�� ���� ��ƼƼ�� ã�� ���̴��� ���� �غ� �մϴ�.
class LightSystem : public System {
public:
    void Update(float deltaTime) override {
        std::cout << "--- LightSystem Update ---\n";
        // ���̴��� ���� LightInfo ����ü �迭 (���� ��� ���ۿ� ����)
        // �ִ� ���� ������ �����ؾ� �մϴ�.
        // LightInfo shaderLights[MAX_LIGHTS]; 
        int lightCount = 0;

        for (Entity* entity : m_entities) {
            LightComponent* lightComp = entity->GetComponent<LightComponent>();
            if (lightComp) {
                // �� ���� ������Ʈ�� Transform ������Ʈ�� ������ �ִٸ� ��ġ/���� ���� ��������
                TransformComponent* transformComp = entity->GetComponent<TransformComponent>();

                std::cout << "  Processing Light for Entity '" << entity->GetName() << "' (Type: ";
                switch (lightComp->type) {
                    case LightType::DIRECTIONAL: std::cout << "Directional"; break;
                    case LightType::POINT: std::cout << "Point"; break;
                    case LightType::SPOT: std::cout << "Spot"; break;
                }
                std::cout << ")\n";

                // LightInfo ����ü�� �����͸� ä��� ���̴��� ���� �غ�
                // (�� �κ��� GPU API�� ���� �ٸ��ϴ�)
                // shaderLights[lightCount].color = lightComp->color;
                // if (transformComp) shaderLights[lightCount].position = transformComp->position;
                // ... ���

                lightCount++;
                // MAX_LIGHTS�� �ʰ��ϸ� ó�� �ߴ� �Ǵ� ���
            }
        }
        // ���������� lightCount�� shaderLights �迭�� GPU ��� ���ۿ� ���ε�
        std::cout << "  Total " << lightCount << " lights prepared for shader.\n";
    }
};

// --- Render System (����ȭ) ---
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
                // �����δ� ���⼭ ������ API ȣ�� (SetMesh, SetMaterial, SetTransform, DrawIndexed ��)
            }
        }
    }
};
//4. ���� ���� / �� �Ŵ���
//��ƼƼ, ������Ʈ, �ý����� �����ϰ� �����ϴ� �ֻ��� �����Դϴ�.
//
//C++

// --- Game (�Ǵ� SceneManager) ---
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
        // �� �ý��ۿ� ���� �ִ� ��ƼƼ���� ����մϴ�.
        // �����δ� �� ������ �Ŵ����� ��ƼƼ �߰�/���� �� �ý��ۿ� �˸��ϴ�.
        for (auto& sys : systems) {
            for (auto& ent : entities) {
                // ���⼭�� ��� �ý��ۿ� ��� ��ƼƼ�� ����ϰ�, �ý��� ���ο��� ������Ʈ ������ üũ�մϴ�.
                // �� ���� ���: System�� �����ִ� ������Ʈ Ÿ���� ����ϰ�,
                // Entity Manager�� ��ƼƼ ����/������Ʈ �߰�/���� �� �ش� �ý��ۿ� �˸��� ���.
                sys->RegisterEntity(ent.get());
            }
        }
    }

    void Run() {
        float deltaTime = 1.0f / 60.0f; // ���� ��Ÿ Ÿ��
        std::cout << "\n--- Game Loop Start ---\n";

        // �����Ӹ��� ��� �ý��� ������Ʈ
        for (auto& sys : systems) {
            sys->Update(deltaTime);
        }

        std::cout << "\n--- Game Loop End ---\n";
    }
};

// --- ���� �Լ� ---
int main() {
    Game game;

    // �ý��� �߰�
    TransformSystem* transformSystem = game.AddSystem<TransformSystem>();
    LightSystem* lightSystem = game.AddSystem<LightSystem>();
    RenderSystem* renderSystem = game.AddSystem<RenderSystem>(); // LightSystem �ڿ� ����ǵ���

    // ��ƼƼ ���� �� ������Ʈ �߰�
    // 1. �÷��̾� ��ƼƼ (Ʈ������, �޽�)
    Entity* player = game.CreateEntity("Player");
    player->AddComponent<TransformComponent>(Vector3(0, 0, 0));
    player->AddComponent<MeshComponent>("playerMesh", "playerMaterial");

    // 2. �� ��Ʈ ��ƼƼ (���� �����)
    // DirectionalLight�� ��ġ�� ���ӵ��� �����Ƿ�, ������ 'SceneRoot' ��ƼƼ�� ���Դϴ�.
    Entity* sceneRoot = game.CreateEntity("SceneRoot");
    LightComponent* mainDirLight = sceneRoot->AddComponent<LightComponent>(LightType::DIRECTIONAL);
    mainDirLight->SetDirectionalLight({ 1.0f, 0.95f, 0.9f, 1.0f }, { 0.5f, -1.0f, 0.5f, 0.0f }, 1.0f); // �¾籤

    // 3. ���� ��ƼƼ (Ʈ������, ����Ʈ ����Ʈ)
    Entity* lamp = game.CreateEntity("Lamp");
    lamp->AddComponent<TransformComponent>(Vector3(5, 2, 0));
    LightComponent* pointLight = lamp->AddComponent<LightComponent>(LightType::POINT, Color(1.0f, 0.5f, 0.0f, 1.0f), 0.8f);
    pointLight->position = Vector3(5, 2, 0); // TransformComponent�� �ߺ��� �� �����Ƿ�, LightSystem���� Transform�� �о���� �����ϴ� ���� ����

    // 4. ����Ʈ����Ʈ ��ƼƼ (Ʈ������, ����Ʈ ����Ʈ)
    Entity* spotlightSource = game.CreateEntity("SpotlightSource");
    spotlightSource->AddComponent<TransformComponent>(Vector3(-3, 3, -3));
    LightComponent* spotLight = spotlightSource->AddComponent<LightComponent>(LightType::SPOT, Color(0.0f, 1.0f, 0.0f, 1.0f), 0.5f);
    spotLight->position = Vector3(-3, 3, -3);
    spotLight->direction = Vector3(0, -1, 0); // �Ʒ� ����

    game.InitSystems(); // �ý��ۿ� ��ƼƼ ���

    game.Run(); // ���� ���� ����

    return 0;
}
/*ECS�� �ٽ� ��Ģ ���
��ƼƼ : ���� ������Ʈ�� ID�� ��, �� ��ü�δ� ����� �����ϴ�.

������Ʈ : Ư�� ��ɿ� �ʿ��� �����Ϳ� �ּ����� ������ �����ϴ�. (�� : TransformComponent�� ��ġ, ȸ��, ũ�� �����͸�, LightComponent�� ������ Ÿ��, ����, ���� �� �����͸� ����)

�ý��� : Ư�� Ÿ���� ������Ʈ���� ���� ��� ��ƼƼ�� ��ȸ�ϸ� �ش� ����� ó���ϴ� ������ �����ϴ�. (LightSystem�� ��� LightComponent�� ã�� GPU�� ������ ���ε�)

�̷��� �и��� ���� �ڵ��� ������, Ȯ�强, ���뼺�� ũ�� ���˴ϴ�.�����ϽŴ�� Light, Transform ���� ������Ʈȭ�Ͽ� ��ƼƼ�� ���̰� �ý����� �̸� ó���ϴ� ����� ���� ���� �������� �ſ� ȿ������ �����Դϴ�.*/