//#pragma once
//#include "CommonHeader.h"
//
//struct ComponentChunk
//{
//	void* data = nullptr;
//	size_t size = 0;
//	size_t capacity = 0;
//	size_t count = 0;
//	std::function<void(void*)> destructor;
//	/*
//	std::vector, string ���� �ܼ������Ͱ� �ƴ� �����Ҵ��� �������� �����ϴ� �����������̳� std����� ���̺귯���� 
//	Component��ҷ� ���� �̸� �Ҹ��ڸ� ��������� ȣ���� �ùٸ� ������ �ʿ��ϴ�
//	*/
//	std::function<void(void*)> destruct_only;
//	std::function<void(void*, void*)> swap_and_pop;
//};
//
//class Archetype
//{
//public:
//	Archetype();
//	~Archetype();
//	Archetype(const Archetype&) = delete;
//	Archetype& operator=(const Archetype&) = delete;
//	Archetype(Archetype&&) = delete;
//	Archetype& operator=(Archetype&&) = delete;
//public:
//	template<typename T>
//	void RegisterComponent();
//	template<typename T>
//	void AddComponent(T&& component);
//	template<typename T>
//	T* GetComponents();
//	void DeleteComponent(size_t chunkIdx);
//
//private:
//	std::unordered_map<std::type_index, ComponentChunk> m_Components;
//};
//
//
//inline Archetype::Archetype()
//{
//
//}
//inline Archetype::~Archetype()
//{
//	for (auto& iter : m_Components)
//		iter.second.destructor(iter.second.data);
//}
//
//template<typename T>
//inline void Archetype::RegisterComponent()
//{
//	std::type_index type = typeid(T);
//	_ASEERTION_NULCHK(m_Components.find(type) == m_Components.end(), "Already Chunk exist");
//	m_Components[type] = ComponentChunk();
//	ComponentChunk& chunk = m_Components[type];
//	size_t size = sizeof(T);
//	if (chunk.data == nullptr)
//	{
//		chunk.capacity = 16;
//		chunk.data = new T[chunk.capacity];
//		chunk.size = size;
//		chunk.count = 0;
//	}
//	chunk.destructor = [](void* data)
//		{
//			delete[] static_cast<T*>(data);
//		};
//
//	chunk.destruct_only = [](void* ptr)
//		{
//			// **�� �޸� �� ����:** ��ü�� �Ҹ��ڸ� ��������� ȣ���մϴ�.
//			static_cast<T*>(ptr)->~T();
//		};
//
//	chunk.swap_and_pop = [](void* dest, void* src)
//		{
//			T* dest_ptr = static_cast<T*>(dest);
//			T* src_ptr = static_cast<T*>(src);
//
//			// 1. dest ��ġ�� �ִ� ���� ��ü �Ҹ� (������ ��ü�� �ڿ� ����)
//			dest_ptr->~T();
//
//			// 2. src ��ü�� dest ��ġ�� �̵� ���� (Placement New & Move)
//			// std::move�� ���� �ڿ� �������� dest�� �����Ѵ�, ���ӵ� �迭������ ���ο� ��ü��ġ�� �������ش� 
//			new (dest_ptr) T(std::move(*src_ptr));
//
//			// 3. src ��ġ�� �ִ� ��ü �Ҹ� (moved-from ���� ����)
//			src_ptr->~T();
//			// ���� T�� Trivial�ϴٸ�, �����Ϸ��� �� ��� ������ memcpy �������� ����ȭ�մϴ�.
//		};
//}
//
//template<typename T>
//inline void Archetype::AddComponent(T&& component)
//{
//	std::type_index type = typeid(T);
//	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
//	ComponentChunk& chunk = m_Components[type];
//	size_t size = sizeof(T);
//	if (chunk.count + 1 >= chunk.capacity)
//	{
//		chunk.capacity *= 2;
//		T* newChunk = new T[chunk.capacity];
//		std::memcpy(newChunk, chunk.data, chunk.size);
//		delete[] static_cast<T*>(chunk.data);
//		chunk.data = newChunk;
//	}
//
//	T* data_ptr = static_cast<T*>(chunk.data);
//	data_ptr[chunk.count] = std::forward<T>(component);
//	chunk.count++;
//}
//
//template<typename T>
//inline T* Archetype::GetComponents()
//{
//	std::type_index type = typeid(T);
//	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component nullptr");
//	ComponentChunk& chunk = m_Components[type];
//	_ASEERTION_NULCHK(chunk.data, "Chunk is nullptr");
//	return static_cast<T*>(chunk.data);
//}
//
//inline void Archetype::DeleteComponent(size_t chunkIdx)
//{
//	for (const auto& iter : m_Components)
//	{
//		ComponentChunk& chunk = m_Components[iter.first];
//		_ASEERTION_NULCHK(chunkIdx < chunk.count, "out of bound");
//
//		// ��� ������Ʈ�� �����ʹ� T Ÿ���� �迭 ���·� ����Ǿ� �ִٰ� ����
//		BYTE* data_ptr = static_cast<BYTE*>(chunk.data);
//		size_t size = chunk.size;
//		size_t last_idx = chunk.count - 1;
//		if (chunkIdx >= last_idx)
//		{
//			//destroy only, ��������Ҹ� ����
//			void* ptr = data_ptr + (chunkIdx * size);
//			chunk.destruct_only(ptr);
//		}
//		else
//		{
//			//swap and pop, ��Ҹ� �����ϰ� ������Ҹ� ����ִ´�
//			void* dest_ptr = data_ptr + (chunkIdx * size);
//			void* src_ptr = data_ptr + (last_idx * size);
//			chunk.swap_and_pop(dest_ptr, src_ptr);
//		}
//		chunk.count--;
//	}
//}

#pragma once
#include "CommonHeader.h"
#define CHUNK_BYTE_LIMIT 16384		//16KB

class ComponentChunk
{
public:
	virtual ~ComponentChunk() = default;
	virtual void SwapAndPop(size_t chunkIdx, ComponentChunk* srcChunk) = 0;
	virtual void AddChunk() = 0;
	virtual size_t GetCount() const = 0;
	virtual size_t GetCapacity() const = 0;
};
template<typename T>
class ChunkData : public ComponentChunk
{
public:
	ChunkData() : m_capacity(std::max((size_t)1, CHUNK_BYTE_LIMIT / sizeof(T)))
	{
		m_data.reserve(m_capacity);
	}
	void SwapAndPop(size_t chunkIdx, ComponentChunk* srcChunk) override
	{
		_ASEERTION_NULCHK(GetCount() && chunkIdx < GetCount(), "idx out of bound");
		std::vector<T>* pChunks = static_cast<ChunkData<T>*>(srcChunk)->m_data;
		if(chunkIdx < pChunks->size()-1)
			m_data[chunkIdx] = std::move(pChunks->back());
		pChunks->pop_back();
	}
	void AddChunk() override
	{
		_ASEERTION_NULCHK(GetCount() < GetCapacity(), "Size full");
		m_data.emplace_back();
	}
	size_t GetCount() const override
	{
		return m_data.size();
	}
	size_t GetCapacity() const override
	{
		return m_capacity;
	}
public:
	std::vector<T> m_data;
	const size_t m_capacity;
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
	bool NeedNewChunk();
	template<typename T>
	void CreateNewChunk();
	void ReserveIndexes(const size_t lookupIdx, size_t& out_idxRow, size_t& out_idxCol);
	template<typename T>
	void AddComponent(T&& component);
	template<typename T>
	std::vector<T>& GetComponents(size_t idxRow);
	size_t DeleteComponent(size_t idxRow, size_t idxCol);
	size_t GetCount_Chunks() const;
	size_t GetCount_Chunk() const;

private:
	std::unordered_map<std::type_index, std::vector<ComponentChunk*>> m_Components;
	std::vector<size_t> m_LookupIdxs;
};


inline Archetype::Archetype()
{
}

inline Archetype::~Archetype()
{
	for (auto& pair : m_Components)
	{
		for (ComponentChunk* chunk : pair.second)
			delete chunk;
	}
}

template<typename T>
inline void Archetype::RegisterComponent()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) == m_Components.end(), "Already Chunk exist");
	m_Components[type] = std::vector<ComponentChunk*>();
	m_Components[type].push_back(new ChunkData<T>());
}

inline bool Archetype::NeedNewChunk()
{
	ComponentChunk* lastChunk = m_Components.begin()->second.back();
	return lastChunk->GetCount() >= lastChunk->GetCapacity();
}

inline void Archetype::ReserveIndexes(const size_t lookupIdx, size_t& out_idxRow, size_t& out_idxCol)
{
	_ASEERTION_NULCHK(m_Components.size(), "Component Chunk not exist");

	m_LookupIdxs.push_back(lookupIdx);

	for (auto& iter : m_Components)
		iter.second.back()->AddChunk();
	auto& chunkList = m_Components.begin()->second;
	ComponentChunk* lastChunk = chunkList.back();
	
	out_idxRow = chunkList.size() - 1;
	out_idxCol = lastChunk->GetCount() - 1;
}

template<typename T>
inline void Archetype::CreateNewChunk()
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
	m_Components[type].push_back(new ChunkData<T>());
}

template<typename T>
inline void Archetype::AddComponent(T&& component)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component Chunk not exist");
	ComponentChunk* lastChunk = m_Components[type].back();
	ChunkData<T>* dataChunk = static_cast<ChunkData<T>*>(lastChunk);
	dataChunk->m_data.back() = std::forward<T>(component);
}

template<typename T>
inline std::vector<T>& Archetype::GetComponents(size_t idxRow)
{
	std::type_index type = typeid(T);
	_ASEERTION_NULCHK(m_Components.find(type) != m_Components.end(), "Component nullptr");
	_ASEERTION_NULCHK(idxRow < m_Components[type].size(), "Chunk row Out of Bound");

	ChunkData<T>* chunk = static_cast<ChunkData<T>*>(m_Components[type][idxRow]);
	return chunk->m_data;
}

inline size_t Archetype::DeleteComponent(size_t idxRow, size_t idxCol)
{
	const auto& chunks = m_Components.begin()->second;
	_ASEERTION_NULCHK(idxRow < chunks.size(), "Chunk row Out of Bound");
	_ASEERTION_NULCHK(!m_LookupIdxs.empty(), "Entity Lookup empty");

	//Archetype�� ��ҵ��� �� ���� ������ swap�ϰ� �ǵް� ����
	for (const auto& iter : m_Components)
		iter.second[idxRow]->SwapAndPop(idxCol, iter.second.back());

	//ûũ�� ���� ������ġ�� Lookup���� �� �������� �ٲ۴�
	size_t movedIdx = m_LookupIdxs.back();
	size_t capacity = (*m_Components.begin()->second.begin())->GetCapacity();
	size_t deletedIdx = capacity * idxRow + idxCol;
	if (deletedIdx < m_LookupIdxs.size() - 1)
		m_LookupIdxs[deletedIdx] = movedIdx;
	m_LookupIdxs.pop_back();

	//���� ûũ�� ��Ұ� ������ �����Ѵ�
	if (chunks[idxRow]->GetCount() <= 0)
	{
		for (auto& pair : m_Components)
		{
			delete pair.second[idxRow];
			pair.second.erase(pair.second.begin() + idxRow);
		}
	}
	
	//�̵���Lookup���� ����
	return movedIdx;
}

inline size_t Archetype::GetCount_Chunks() const
{
	const auto& chunks = m_Components.begin()->second;
	return chunks.size();
}

inline size_t Archetype::GetCount_Chunk() const
{
	const auto& chunks = m_Components.begin()->second;
	const auto& lastchunk = chunks.back();
	return lastchunk->GetCount();
}