#pragma once
#include "Resource.h"

class Material : public BaseResource<Material>
{
	friend class BaseResource<Material>;
public:
	Material(size_t hash, const std::wstring& szFilePath);
	~Material();
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;
	Material(Material&&) = delete;
	Material& operator=(Material&&) = delete;

private:
	size_t m_lVS;
	size_t m_lPS;
	std::vector<size_t> m_lCBs;
	std::vector<size_t> m_lTXs;
};

