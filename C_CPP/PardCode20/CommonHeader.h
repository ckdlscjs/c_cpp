#pragma once
#include "std.h"
//enum classes
enum class InputEventType
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

enum class Samplers
{
	WRAP_LINEAR,
	WRAP_ANISOTROPIC,

};

//Resources data struct
struct InputEvent
{
	InputEventType type;
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

struct Vertex_PC
{
	Vector3 pos0;
	Vector4 color0;
};
static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPC[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};
static UINT size_InputLayout_VertexPC = ARRAYSIZE(InputLayout_VertexPC);
struct Vertex_PTN
{
	Vector3 pos0;
	Vector2 tex0;
	Vector3 normal0;
};
static D3D11_INPUT_ELEMENT_DESC InputLayout_VertexPTN[] =
{
	//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0},
};
static UINT size_InputLayout_VertexPTN = ARRAYSIZE(InputLayout_VertexPTN);

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
static UINT size_InputLayout_VertexPCT = ARRAYSIZE(InputLayout_VertexPCT);
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
static UINT size_InputLayout_VertexPPCC = ARRAYSIZE(InputLayout_VertexPPCC);

//16바이트 단위로 gpu메모리에서 패딩되므로 단위를 맞춘다, 최대 16바이트 * 4096 이 가능하다(하나의레지스터), 최대14레지스터까지가능(0~13)
__declspec(align(16))
struct Constant_time
{
	float fTime;
};

__declspec(align(16))
struct CB_Campos
{
	Vector3 camPos;
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
	Vector4 m_Ambient;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
	Vector4 v_Direction;	//w is shiness
};

__declspec(align(16))
struct CB_PointLight
{
	Vector4 m_Ambient;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
	Vector4 v_Position;		//w is shiness
	Vector4 f_Attenuation;	//a0, a1, a2, range
};

__declspec(align(16))
struct CB_SpotLight
{
	Vector4 m_Ambient;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
	Vector4 v_Direction;	//w is padd1
	Vector4 v_Position;		//w is shiness
	Vector4 f_Attenuation;	//a0, a1, a2, range
	Vector4 f_Spots;		//spot, cosOuter, cosInner, padd2
};