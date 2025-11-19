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
	// 새롭게 추가된 샘플러 상태들
	//LINEAR_CLAMP,
	//ANISOTROPIC_CLAMP,
};

enum class E_RSState
{
	SOLID_CULLBACK_CW,
	SOLID_CULLFRONT_CW,
	WIRE_CULLBACK_CW,
	SOLID_CULLBACK_CCW,
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
};

//enum class E_RenderPass
//{
//	FORWARD_PASS,
//	DEFERRED_PASS,
//	UI_PASS,
//	SHADOW_PASS,
//	COUNT
//};


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

static std::pair<D3D11_INPUT_ELEMENT_DESC*, UINT> INPUT_ELMENTS[] =
{
	{InputLayout_VertexPC,		(UINT)ARRAYSIZE(InputLayout_VertexPC)},
	{InputLayout_VertexPT,		(UINT)ARRAYSIZE(InputLayout_VertexPT)},
	{InputLayout_VertexPCT,		(UINT)ARRAYSIZE(InputLayout_VertexPCT)},
	{InputLayout_VertexPPCC,	(UINT)ARRAYSIZE(InputLayout_VertexPPCC)},
	{InputLayout_VertexPTN,		(UINT)ARRAYSIZE(InputLayout_VertexPTN)},
	{InputLayout_VertexPTNTB,	(UINT)ARRAYSIZE(InputLayout_VertexPTNTB)},
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