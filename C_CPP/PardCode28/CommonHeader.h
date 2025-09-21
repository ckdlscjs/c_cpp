#pragma once
//#define _FNV1A
#include "std.h"

//enum classes
enum class E_InputEventType

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
};

enum class E_Samplers
{
	WRAP_LINEAR,
	WRAP_ANISOTROPIC,

	// 새롭게 추가된 샘플러 상태들
	//POINT_CLAMP,
	//LINEAR_CLAMP,
	//ANISOTROPIC_CLAMP,
};

enum class E_Rasterizers
{
	SOLID_CULLBACK_CW,
	SOLID_CULLFRONT_CW,
	WIRE_CULLBACK_CW,
};

enum class E_Colliders
{
	NONE,
	SPHERE,
	AABB,
	OBB,
	RAY,
};

enum class E_Textures
{
	Diffuse,
	Normal,
	Specular,
	Roughness,
	Metalic,
	AmbientOcclusion,
	Emissive,
	count,
};

enum class E_InputLayout
{
	IL_PC,
	IL_PCT,
	IL_PT,
	IL_PPCC,
	IL_PTN,
	IL_PTNTB,
};

struct TX_HASH
{
	E_Textures tex;
	size_t hash;
};

struct TX_PATH
{
	E_Textures tex;
	std::string szPath;
};

struct Mesh_Material
{
	size_t hash_mesh;
	size_t hash_material;
};
//Resources data struct
struct InputEvent
{
	E_InputEventType type;
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
	size_t count;
	size_t idx;
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

static std::pair<D3D11_INPUT_ELEMENT_DESC*, UINT> INPUT_ELMENTS[] =
{
	{InputLayout_VertexPC,		(UINT)ARRAYSIZE(InputLayout_VertexPC)},
	{InputLayout_VertexPCT,		(UINT)ARRAYSIZE(InputLayout_VertexPCT)},
	{InputLayout_VertexPCT,		(UINT)ARRAYSIZE(InputLayout_VertexPCT)},
	{InputLayout_VertexPPCC,	(UINT)ARRAYSIZE(InputLayout_VertexPPCC)},
	{InputLayout_VertexPTN,		(UINT)ARRAYSIZE(InputLayout_VertexPTN)},
	{InputLayout_VertexPTNTB,	(UINT)ARRAYSIZE(InputLayout_VertexPTNTB)},
};

//TMP를 이용한 추론
template <typename T>
struct Traits_InputLayout;

template <>
struct Traits_InputLayout<Vertex_PTN> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::IL_PTN].first;
	}
	static constexpr size_t GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::IL_PTN].second;
	}
};

template <>
struct Traits_InputLayout<Vertex_PTNTB> {
	static constexpr D3D11_INPUT_ELEMENT_DESC* GetLayout() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::IL_PTNTB].first;
	}
	static constexpr size_t GetSize() {
		return INPUT_ELMENTS[(UINT)E_InputLayout::IL_PTNTB].second;
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
struct CB_DirectionalLight
{
	Vector4 mAmbient;
	Vector4 mDiffuse;
	Vector4 mSpecular;
	Vector4 vDirection;	//w is shiness
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