#pragma once
//#define _FNV1A
#include "std.h"
#include "CommonMath.h"					

//enum classes
constexpr size_t MAX_COMPONENTS = 256;
using ArchetypeKey = std::bitset<MAX_COMPONENTS>;
class ComponentType
{
public:
	template<typename T>
	static size_t GetMask()
	{
		_ASEERTION_NULCHK(m_lCount < MAX_COMPONENTS, "Component limit exceeded");
		static size_t Mask = m_lCount++;
		return Mask;
	}
private:
	inline static size_t m_lCount = 0;
};

enum class E_InputEvent
{
	KEY_DOWN,
	KEY_UP,
	KEY_PRESSED,
	MOUSE_MOVE,
	MOUSE_L_DOWN,
	MOUSE_L_UP,
	MOUSE_R_DOWN,
	MOUSE_R_UP,
	MOUSE_WHEEL,
	NOTHING,
};

enum class E_Sampler
{
	LINEAR_WRAP,
	ANISOTROPIC_WRAP,
	POINT_CLAMP,
	POINT_CLAMP_COMPARISON,
	// 새롭게 추가된 샘플러 상태들
	//LINEAR_CLAMP,
	//ANISOTROPIC_CLAMP,
};

enum class E_RSState
{
	SOLID_CULLBACK_CW,
	SOLID_CULLBACK_CCW,
	SOLID_CULLFRONT_CW,
	SOLID_CULLFRONT_CCW,
	WIRE_CULLBACK_CW,
};

enum class E_DSState
{
	DEFAULT,
	SKYBOX,
	UI,
};

enum class E_BSState
{
	Opaque,
	Transparent,
	Additive
};

enum class E_Collider
{
	NONE,
	SPHERE,
	AABB,
	OBB,
	RAY,
};

enum class E_Texture
{
	Diffuse,
	Normal,
	Specular,
	Roughness,
	Metalic,
	AmbientOcclusion,
	Emissive,
	None,
	count,
};

enum class E_InputLayout
{
	PC,
	PT,
	PCT,
	PPCC,
	PTN,
	PTNTB,
	PTN_Skinned,
	PTNTB_Skinned,
};
/*
* 1. MAP_DATA     : 일반적인 모델 텍스처(Diffuse, Normal, Specular)
* 2. RENDER_TARGET : 화면에 그려질 중간 복사본(HDR, G - Buffer, UI Overlay)
* 3. DEPTH_STENCIL : 깊이 판정 및 그림자 생성용(Depth Buffer, Shadow Map)
* 4. DYNAMIC_INPUT : CPU에서 매 프레임 갱신하는 데이터(Constant Buffer, Dynamic VB / IB)
* 5. READ_BACK : GPU 데이터를 CPU로 가져오는 용도(Screenshot, Picking)
* 
*/

enum class E_ResourcesUsage
{
	TextureMap,
	RTV,
	DSV,
	RTV_CubeMap,
	DSV_CubeMap,
};

inline D3D11_TEXTURE2D_DESC GetTex2DDesc(E_ResourcesUsage usage, UINT width, UINT height)
{
/*
Usage (어떻게 갱신하는가?)

	파일에서 한 번 읽고 끝이다: DEFAULT
	매 프레임 CPU에서 데이터를 넘겨준다 (Matrix, UI): DYNAMIC
	GPU가 그리고 GPU가 다시 읽는다 (Post-process): DEFAULT
	GPU가 그린 걸 CPU가 읽어야 한다 (Picking, Screenshot): STAGING
	
BindFlags (파이프라인 어디에 꽂히는가?)

	셰이더 Texture2D 변수에 들어간다: SHADER_RESOURCE
	OMSetRenderTargets의 첫 번째 인자다: RENDER_TARGET
	OMSetRenderTargets의 마지막 인자(깊이)다: DEPTH_STENCIL
	그림자 맵처럼 깊이를 기록하고 나중에 셰이더에서 읽는다: DEPTH_STENCIL | SHADER_RESOURCE
	
Format (무엇을 담는가?)
	
	일반 그림/색상: R8G8B8A8_UNORM (채널당 1바이트, 0~255)
	정밀한 물리계산/광해석: R16G16B16A16_FLOAT (채널당 2바이트, 부동소수점)
	단순 깊이 값: D24_UNORM_S8_UINT (일반 깊이 버퍼 전용)
	그림자 맵용 깊이: R32_TYPELESS (범용 32비트, 읽기용으로 변환 용이)

	UINT Width;
	UINT Height;
	UINT MipLevels;
	UINT ArraySize;
	DXGI_FORMAT Format;
	DXGI_SAMPLE_DESC SampleDesc;
	D3D11_USAGE Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
*/
	//기본초기화, static
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	switch (usage)
	{
		case E_ResourcesUsage::TextureMap :
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		}break;

		case E_ResourcesUsage::RTV :
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		}break;

		case E_ResourcesUsage::DSV :
		{
			desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		}break;

		case E_ResourcesUsage::RTV_CubeMap :
		{
			desc.MipLevels = 0;
			desc.ArraySize = 6;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
		}break;

		case E_ResourcesUsage::DSV_CubeMap:
		{
			desc.MipLevels = 1;
			desc.ArraySize = 6;
			desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
		}break;
	}

	return desc;
}

enum class E_RenderPass
{
	FORWARD_PASS,
	DEFERRED_PASS,
	SHADOW_PASS,
	COUNT
};

//Resources data struct
struct TX_HASH
{
	E_Texture tex;
	size_t hash;
};

struct TX_PATH
{
	E_Texture tex;
	std::string szPath;
};

struct MTL_TEXTURES
{
	std::string szMatName;
	std::unordered_map<aiTextureType, std::vector<DirectX::ScratchImage>> type_textures_image;
	std::unordered_map<aiTextureType, std::vector<std::string>> type_textures_path;
};

//뼈대
struct Bone
{
	UINT idx;
	Matrix4x4 matOffset;
};
struct NodeHierarchy
{
	int idx_parent;
	Matrix4x4 matRelative;
	std::string szName_Parent;
	std::string szName_Cur;
};

//가중치데이터
struct IWInfo
{
	UINT boneIdx;
	float weight;
};

//키프레임 데이터
struct KeyFrame_Vector
{
	float fTime;
	Vector3 vValue;
};
struct KeyFrame_Quarternion
{
	float fTime;
	Quarternion qValue;
};

//애니메이션클립(Sort by Bones)
struct AnimationClip
{
	std::string szName;
	float fDuration;
	float fTicksPerSecond;
	std::unordered_map<std::string, std::vector<KeyFrame_Vector>> boneFrames_Scale;
	std::unordered_map<std::string, std::vector<KeyFrame_Quarternion>> boneFrames_Rotate;
	std::unordered_map<std::string, std::vector<KeyFrame_Vector>> boneFrames_Translation;
};

inline E_Texture ConvETexture(const aiTextureType& aiTextype)
{
	/*
	* aiTextureType_REFLECTION = 11,
				aiTextureType_BASE_COLOR = 12,
				aiTextureType_NORMAL_CAMERA = 13,
				aiTextureType_EMISSION_COLOR = 14,
				aiTextureType_METALNESS = 15,
				aiTextureType_DIFFUSE_ROUGHNESS = 16,
				aiTextureType_AMBIENT_OCCLUSION = 17,
	*/
	switch (aiTextype)
	{
		case aiTextureType_DIFFUSE:
			return E_Texture::Diffuse;
		case aiTextureType_SPECULAR:
			return E_Texture::Specular;
			//case aiTextureType_AMBIENT: return E_Texture::
			//case aiTextureType_EMISSIVE: return E_Texture::Emissive;
			//case aiTextureType_HEIGHT: return E_Texture::
		case aiTextureType_NORMALS:
			return E_Texture::Normal;
			//case aiTextureType_SHININESS: return E_Texture::Diffuse;
			//case aiTextureType_OPACITY: return E_Texture::
			//case aiTextureType_DISPLACEMENT: return E_Texture::Diffuse;
			//case aiTextureType_LIGHTMAP: return E_Texture::Diffuse;
		default:
			return E_Texture::None;
	}
}
inline const std::wstring GetTexType(E_Texture eTexture)
{
	switch (eTexture)
	{
		case E_Texture::Diffuse:
			return L"Diffuse";
		case E_Texture::Specular:
			return L"Specular";
		case E_Texture::Normal:
			return L"Normal";
		default:
			return L"None";
	}
}

struct Mesh_Material
{
	size_t hash_mesh;
	size_t hash_material;
};

struct InputEvent 
{
	E_InputEvent type;
	int keyCode = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	int wheelDelta = 0;
};

using EventCallBack = std::function<void(const InputEvent&)>;
struct PointXY
{
	int x = 0;
	int y = 0;
	PointXY(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

struct RenderCounts
{
	UINT count;
	UINT idx;
};

struct Vertex_PC
{
	Vector3 pos0;
	Vector4 color0;
};

//align offset 을 D3D11_APPEND_ALIGNED_ELEMENT 로 대체 할 수 있다
static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPC[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Vertex_PT
{
	Vector3 pos0;
	Vector2 tex0;
};

//align offset 을 D3D11_APPEND_ALIGNED_ELEMENT 로 대체 할 수 있다
static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPT[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Vertex_PCT
{
	Vector3 pos0;
	Vector4 color0;
	Vector2 tex0;
};


static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPCT[] =
{

	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};


struct Vertex_PPCC
{
	Vector3 pos0;
	Vector3 pos1;
	Vector4 color0;
	Vector4 color1;
};


static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPPCC[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSITION",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Vertex_PTN
{
	Vector3 pos0;
	Vector2 tex0;
	Vector3 normal0;
	bool operator==(const Vertex_PTN& other) const
	{
		return (pos0 == other.pos0 && tex0 == other.tex0 && normal0 == other.normal0);
	}
};

struct Vertex_PTN_Hash
{
	size_t operator()(const Vertex_PTN& v) const
	{
		size_t hash = 0;
#ifndef _FNV1A
		//using std hash
		hash_combine(hash, std::hash<float>{}(v.pos0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetZ()));
#endif // !_FNV1A

#ifdef _FNV1A
		//using fnv-1a
		hash_combine(hash, HashingFloat(v.pos0.GetX()));
		hash_combine(hash, HashingFloat(v.pos0.GetY()));
		hash_combine(hash, HashingFloat(v.pos0.GetZ()));
		hash_combine(hash, HashingFloat(v.tex0.GetX()));
		hash_combine(hash, HashingFloat(v.tex0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetX()));
		hash_combine(hash, HashingFloat(v.normal0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetZ()));
#endif // _FNV1A
		return hash;
	}
};


static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPTN[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Vertex_PTNTB
{
	Vector3 pos0;
	Vector2 tex0;
	Vector3 normal0;
	Vector3 tangent0;
	Vector3 binormal0;

	bool operator==(const Vertex_PTNTB& other) const
	{
		return (pos0 == other.pos0 && tex0 == other.tex0 && normal0 == other.normal0 && tangent0 == other.tangent0 && binormal0 == other.binormal0);
	}
};

struct Vertex_PTNTB_Hash
{
	size_t operator()(const Vertex_PTNTB& v) const
	{
		size_t hash = 0;
#ifndef _FNV1A
		//using std hash
		hash_combine(hash, std::hash<float>{}(v.pos0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.tangent0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.tangent0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.tangent0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.binormal0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.binormal0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.binormal0.GetZ()));
#endif // !_FNV1A

#ifdef _FNV1A
		//using fnv-1a
		hash_combine(hash, HashingFloat(v.pos0.GetX()));
		hash_combine(hash, HashingFloat(v.pos0.GetY()));
		hash_combine(hash, HashingFloat(v.pos0.GetZ()));
		hash_combine(hash, HashingFloat(v.tex0.GetX()));
		hash_combine(hash, HashingFloat(v.tex0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetX()));
		hash_combine(hash, HashingFloat(v.normal0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetZ()));
		hash_combine(hash, HashingFloat(v.tangent0.GetX()));
		hash_combine(hash, HashingFloat(v.tangent0.GetY()));
		hash_combine(hash, HashingFloat(v.tangent0.GetZ()));
		hash_combine(hash, HashingFloat(v.binormal0.GetX()));
		hash_combine(hash, HashingFloat(v.binormal0.GetY()));
		hash_combine(hash, HashingFloat(v.binormal0.GetZ()));
#endif // _FNV1A
		return hash;
	}
};

static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPTNTB[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BINORMAL",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 64,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Vertex_PTN_Skinned
{
	Vector3 pos0;
	Vector2 tex0;
	Vector3 normal0;
	std::array<UINT, 4> bones;
	std::array<float, 4> weights;
	bool operator==(const Vertex_PTN_Skinned& other) const
	{
		return (pos0 == other.pos0 && tex0 == other.tex0 && normal0 == other.normal0);
	}
};
struct Vertex_PTN_Skinned_Hash
{
	size_t operator()(const Vertex_PTN_Skinned& v) const
	{
		size_t hash = 0;
#ifndef _FNV1A
		//using std hash
		hash_combine(hash, std::hash<float>{}(v.pos0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetZ()));

#endif // !_FNV1A

#ifdef _FNV1A
		//using fnv-1a
		hash_combine(hash, HashingFloat(v.pos0.GetX()));
		hash_combine(hash, HashingFloat(v.pos0.GetY()));
		hash_combine(hash, HashingFloat(v.pos0.GetZ()));
		hash_combine(hash, HashingFloat(v.tex0.GetX()));
		hash_combine(hash, HashingFloat(v.tex0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetX()));
		hash_combine(hash, HashingFloat(v.normal0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetZ()));
#endif // _FNV1A
		return hash;
	}
};

static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPTN_Skinned[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONES",		0, DXGI_FORMAT_R32G32B32A32_UINT,		0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 64,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Vertex_PTNTB_Skinned
{
	Vector3 pos0;
	Vector2 tex0;
	Vector3 normal0;
	Vector3 tangent0;
	Vector3 binormal0;
	std::array<UINT, 4> bones = { 0, 0, 0, 0 };
	std::array<float, 4> weights = { 1.0f, 0.0f, 0.0f, 0.0f };
	bool operator==(const Vertex_PTNTB_Skinned& other) const
	{
		return (pos0 == other.pos0 && tex0 == other.tex0 && normal0 == other.normal0 && tangent0 == other.tangent0 && binormal0 == other.binormal0);
	}
};
struct Vertex_PTNTB_Skinned_Hash
{
	size_t operator()(const Vertex_PTNTB_Skinned& v) const
	{
		size_t hash = 0;
#ifndef _FNV1A
		//using std hash
		hash_combine(hash, std::hash<float>{}(v.pos0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.pos0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.tex0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.normal0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.tangent0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.tangent0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.tangent0.GetZ()));
		hash_combine(hash, std::hash<float>{}(v.binormal0.GetX()));
		hash_combine(hash, std::hash<float>{}(v.binormal0.GetY()));
		hash_combine(hash, std::hash<float>{}(v.binormal0.GetZ()));
#endif // !_FNV1A

#ifdef _FNV1A
		//using fnv-1a
		hash_combine(hash, HashingFloat(v.pos0.GetX()));
		hash_combine(hash, HashingFloat(v.pos0.GetY()));
		hash_combine(hash, HashingFloat(v.pos0.GetZ()));
		hash_combine(hash, HashingFloat(v.tex0.GetX()));
		hash_combine(hash, HashingFloat(v.tex0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetX()));
		hash_combine(hash, HashingFloat(v.normal0.GetY()));
		hash_combine(hash, HashingFloat(v.normal0.GetZ()));
		hash_combine(hash, HashingFloat(v.tangent0.GetX()));
		hash_combine(hash, HashingFloat(v.tangent0.GetY()));
		hash_combine(hash, HashingFloat(v.tangent0.GetZ()));
		hash_combine(hash, HashingFloat(v.binormal0.GetX()));
		hash_combine(hash, HashingFloat(v.binormal0.GetY()));
		hash_combine(hash, HashingFloat(v.binormal0.GetZ()));
#endif // _FNV1A
		return hash;
	}
};

static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPTNTB_Skinned[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BINORMAL",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 64,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONES",		0, DXGI_FORMAT_R32G32B32A32_UINT,		0, 80,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 96,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};



static std::pair<D3D11_INPUT_ELEMENT_DESC*, UINT> INPUT_ELMENTS[] =
{
	{InputLayout_VertexPC,		(UINT)ARRAYSIZE(InputLayout_VertexPC)},
	{InputLayout_VertexPT,		(UINT)ARRAYSIZE(InputLayout_VertexPT)},
	{InputLayout_VertexPCT,		(UINT)ARRAYSIZE(InputLayout_VertexPCT)},
	{InputLayout_VertexPPCC,	(UINT)ARRAYSIZE(InputLayout_VertexPPCC)},
	{InputLayout_VertexPTN,		(UINT)ARRAYSIZE(InputLayout_VertexPTN)},
	{InputLayout_VertexPTNTB,	(UINT)ARRAYSIZE(InputLayout_VertexPTNTB)},
	{InputLayout_VertexPTN_Skinned,		(UINT)ARRAYSIZE(InputLayout_VertexPTN_Skinned)},
	{InputLayout_VertexPTNTB_Skinned,	(UINT)ARRAYSIZE(InputLayout_VertexPTNTB_Skinned)},
};

//TMP를 이용한 추론
template <typename T>
struct Traits_InputLayout;

template <>
struct Traits_InputLayout<Vertex_PC> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PC].first;
	}
	static constexpr UINT GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PC].second;
	}
};

template <>
struct Traits_InputLayout<Vertex_PT> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PT].first;
	}
	static constexpr UINT GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PT].second;
	}
};

template <>
struct Traits_InputLayout<Vertex_PTN> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTN].first;
	}
	static constexpr UINT GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTN].second;
	}
};

template <>
struct Traits_InputLayout<Vertex_PTNTB> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTNTB].first;
	}
	static constexpr UINT GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTNTB].second;
	}
};

template <>
struct Traits_InputLayout<Vertex_PTN_Skinned> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTN_Skinned].first;
	}
	static constexpr UINT GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTN_Skinned].second;
	}
};

template <>
struct Traits_InputLayout<Vertex_PTNTB_Skinned> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTNTB_Skinned].first;
	}
	static constexpr UINT GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::PTNTB_Skinned].second;
	}
};



//16바이트 단위로 gpu메모리에서 패딩되므로 단위를 맞춘다, 최대 16바이트 * 4096 이 가능하다(하나의레지스터), 최대14레지스터까지가능(0~13)
__declspec(align(16))
struct CB_Time
{
	float fTime;
};

__declspec(align(16))
struct CB_Campos
{
	Vector3 vPosition;
};

__declspec(align(16))
struct CB_WVPMatrix
{
	Matrix4x4 matWorld;
	Matrix4x4 matView;
	Matrix4x4 matProj;
};

__declspec(align(16))
struct CB_WVPITMatrix
{
	Matrix4x4 matWorld;
	Matrix4x4 matView;
	Matrix4x4 matProj;
	Matrix4x4 matInvTrans;
};

__declspec(align(16))
struct CB_LightMatrix
{
	Matrix4x4 matLightView;
	Matrix4x4 matLightProj;
	Vector4	vPos;
};

__declspec(align(16))
struct CB_DirectionalLight
{
	Vector4 mAmbient;
	Vector4 mDiffuse;
	Vector4 mSpecular;
	Vector4 vDirection;		//w is shiness
};

__declspec(align(16))
struct CB_PointLight
{
	Vector4 mAmbient;
	Vector4 mDiffuse;
	Vector4 mSpecular;
	Vector4 vPosition;		//w is shiness
	Vector4 fAttenuations;	//a0, a1, a2, range
};

__declspec(align(16))
struct CB_SpotLight
{
	Vector4 mAmbient;
	Vector4 mDiffuse;
	Vector4 mSpecular;
	Vector4 vDirection;		//w is padd1
	Vector4 vPosition;		//w is shiness
	Vector4 fAttenuations;	//a0, a1, a2, range
	Vector4 fSpots;			//spot, cosOuter, cosInner, padd2
};

__declspec(align(16))
struct CB_Fog
{
	float fFogNear;
	float fFogFar;
	float fFogDensity;
	float padding;
	Vector4 vFogColor;
};

__declspec(align(16))
struct CB_BoneMatrix
{
	Matrix4x4 bones[256];
};

/*
		TBN구성에 사용할 tangent(접벡터), binormal(종벡터)를 계산한다
		MatE = deltaUV * MatTB
		MatTB = deltaUV^-1 * MatE
		* 역행렬
		* AdjMat(A*) = C(A)^T , 여인수행렬의 전치를 의미, 수반행렬
		* InvMat = AdjMat * 1.0f/detA
		deltaUV^-1 = 1/(ad-bc) * AdjMat

		deltaUV = du0 dv0 (a, b)
				  du1 dv1 (c, d)

		adjMat = d -b
				-c  a

		invMat = denominator * adjMat
*/
inline void ComputeTangentBinormal(const std::vector<UINT>& indicies, std::vector<Vertex_PTNTB>& vertices)
{
	//메쉬(세 정점)을 기준으로 tangent, binormal의 누적을 계산한다
	for (UINT idx = 0; idx < indicies.size(); idx += 3)
	{
		UINT i0 = indicies[idx];
		UINT i1 = indicies[idx + 1];
		UINT i2 = indicies[idx + 2];
		Vector3 e0 = vertices[i1].pos0 - vertices[i0].pos0;
		Vector3 e1 = vertices[i2].pos0 - vertices[i0].pos0;
		Vector2 uv_e0 = vertices[i1].tex0 - vertices[i0].tex0;	//a, b
		Vector2 uv_e1 = vertices[i2].tex0 - vertices[i0].tex0;	//c, d
		float det = uv_e0.GetX() * uv_e1.GetY() - uv_e0.GetY() * uv_e1.GetX();	//ad - bc
		if (std::fabs(det) <= _EPSILON) continue;
		det = 1.0f / det;
		float tx, ty, tz;
		tx = (uv_e1.GetY() * e0.GetX() - uv_e0.GetY() * e1.GetX()) * det;
		ty = (uv_e1.GetY() * e0.GetY() - uv_e0.GetY() * e1.GetY()) * det;
		tz = (uv_e1.GetY() * e0.GetZ() - uv_e0.GetY() * e1.GetZ()) * det;

		Vector3 tangent(tx, ty, tz);
		vertices[i0].tangent0 += tangent;
		vertices[i1].tangent0 += tangent;
		vertices[i2].tangent0 += tangent;

		//float bx, by, bz;
		//bx = (-uv_e1.GetX() * e0.GetX() + uv_e0.GetX() * e1.GetX()) * det;
		//by = (-uv_e1.GetX() * e0.GetY() + uv_e0.GetX() * e1.GetY()) * det;
		//bz = (-uv_e1.GetX() * e0.GetZ() + uv_e0.GetX() * e1.GetZ()) * det;
		//Vector3 binormal(bx, by, bz);
		////binormal = binormal.Normalize();
		//vertices[i0].binormal0 += binormal;
		//vertices[i1].binormal0 += binormal;
		//vertices[i2].binormal0 += binormal;
	}

	//그람슈미트직교화를이용
	for (auto& vertex : vertices)
	{
		vertex.normal0 = vertex.normal0.Normalize();
		vertex.tangent0 = (vertex.tangent0 - (vertex.normal0 * vertex.normal0.DotProduct(vertex.tangent0))).Normalize();
		vertex.binormal0 = vertex.normal0.CrossProduct(vertex.tangent0);
	}
}