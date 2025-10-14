#pragma once
#include "CommonHeader.h"

struct ComponentChunk
{
	void* data = nullptr;
	size_t size = 0;
	size_t capacity = 0;
	size_t count = 0;
	std::function<void(void*)> destructor;
	std::function<void(void*)> destruct_only;
	std::function<void(void*, void*)> swap_and_pop;
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
	T* GetComponents();

	void DeleteComponent(size_t idx);

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
		chunk.size = size;
		chunk.count = 0;
	}
	chunk.destructor = [](void* data)
		{
			delete[] static_cast<T*>(data);
		};
	chunk.destruct_only = [](void* ptr)
		{
			// **�� �޸� �� ����:** ��ü�� �Ҹ��ڸ� ��������� ȣ���մϴ�.
			static_cast<T*>(ptr)->~T();
		};

	chunk.swap_and_pop = [](void* dest, void* src)
		{
			T* dest_ptr = static_cast<T*>(dest);
			T* src_ptr = static_cast<T*>(src);

			// 1. dest ��ġ�� �ִ� ���� ��ü �Ҹ� (������ ��ü�� �ڿ� ����)
			dest_ptr->~T();

			// 2. src ��ü�� dest ��ġ�� �̵� ���� (Placement New & Move)
			// std::move�� ���� �ڿ� �������� dest�� �����մϴ�.
			//dest_ptr = new T(std::move(*src_ptr));
			new (dest_ptr) T(std::move(*src_ptr));

			// 3. src ��ġ�� �ִ� ��ü �Ҹ� (moved-from ���� ����)
			src_ptr->~T();
			// ���� T�� Trivial�ϴٸ�, �����Ϸ��� �� ��� ������ memcpy �������� ����ȭ�մϴ�.
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
	}

	T* data_ptr = static_cast<T*>(chunk.data);
	data_ptr[chunk.count] = std::forward<T>(component);
	chunk.count++;
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

inline void Archetype::DeleteComponent(size_t idx)
{
	for (const auto& iter : m_Components)
	{
		ComponentChunk& chunk = m_Components[iter.first];
		// _ASEERTION_NULCHK(idx < chunk.count, "out of bound");

		size_t last_idx = chunk.count - 1;

		if (idx < chunk.count)
		{
			// ��� ������Ʈ�� �����ʹ� T Ÿ���� �迭 ���·� ����Ǿ� �ִٰ� ����
			BYTE* data_ptr = static_cast<BYTE*>(chunk.data);
			size_t size = chunk.size;

			if (idx != last_idx)
			{
				// 1. SWAP: ������ ��Ҹ� ������ ��ġ�� �����ϰ� �̵� �� �Ҹ�
				void* dest_ptr = data_ptr + (idx * size);
				void* src_ptr = data_ptr + (last_idx * size);

				chunk.swap_and_pop(dest_ptr, src_ptr);
			}
			else
			{
				// 2. POP: ������ ��Ҵ� �̵��� �ʿ� ���� �Ҹ길 ��ŵ�ϴ�.
				void* ptr = data_ptr + (idx * size);
				chunk.destruct_only(ptr);
			}
		}

		chunk.count--;
	}
}


	