#pragma once
#include "CommonHeader.h"

struct ComponentChunk
{
	void* data = nullptr;
	size_t size = 0;
	size_t capacity = 0;
	size_t count = 0;
	std::function<void(void*)> destructor;
};

class Archetype
{
public:
	Archetype();
	~Archetype();
	Archetype(const Archetype&) = delete;
	Archetype& operator=(const Archetype&) = delete;
	Archetype(Archetype&&) = delete;
	Archetype& operator=(Archetype&&) = delete;
public:
	template<typename T>
	void RegisterComponent();
	template<typename T>
	void AddComponent(T&& component);
	template<typename T>
	void DeleteComponent(size_t idx);
	template<typename T>
	T* GetComponents();

private:
	std::unordered_map<std::type_index, ComponentChunk> m_Components;
};

inline Archetype::Archetype()
{

}
inline Archetype::~Archetype()
{
	for (auto& iter : m_Components)
		iter.second.destructor(iter.second.data);
}

template<typename T>
inline void Archetype::RegisterComponent()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) == m_Components.end(), "Already Chunk exist");
	m_Components[type] = ComponentChunk();
	ComponentChunk& chunk = m_Components[type];
	size_t size = sizeof(T);
	if (chunk.data == nullptr)
	{
		chunk.capacity = 16;
		chunk.data = new T[chunk.capacity];
		chunk.size = size * chunk.capacity;
		chunk.count = 0;
	}
	chunk.destructor = [](void* data)
		{
			delete[] static_cast<T*>(data);
		};
}

template<typename T>
inline void Archetype::AddComponent(T&& component)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
	ComponentChunk& chunk = m_Components[type];
	size_t size = sizeof(T);
	if (chunk.count + 1 >= chunk.capacity)
	{
		chunk.capacity *= 2;
		T* newChunk = new T[chunk.capacity];

		std::memcpy(newChunk, chunk.data, chunk.size);
		delete[] static_cast<T*>(chunk.data);

		chunk.data = newChunk;
		chunk.size = size * chunk.capacity;
	}

	T* data_ptr = static_cast<T*>(chunk.data);
	data_ptr[chunk.count] = std::forward<T>(component);
	chunk.count++;
}

template<typename T>
inline void Archetype::DeleteComponent(size_t idx)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component chunk not initialized");
	ComponentChunk& chunk = m_Components[type];
	_ASEERTION_NULCHK(idx < chunk.count, "out of bound");
	if (idx < chunk.count)
	{
		T* data_ptr = static_cast<T*>(chunk.data);
		data_ptr[idx] = std::move(data_ptr[chunk.count - 1]);
	}
	chunk.count--;
}

template<typename T>
inline T* Archetype::GetComponents()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component nullptr");
	ComponentChunk& chunk = m_Components[type];
	_ASEERTION_NULCHK(chunk.data, "Chunk is nullptr");
	return static_cast<T*>(chunk.data);
}
	