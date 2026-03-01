struct GS_OUTPUT
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};
/*
void GeometryGenerator::CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData)
{
	// Put a cap on the number of subdivisions.
    numSubdivisions = MathHelper::Min(numSubdivisions, 5u);

	// Approximate a sphere by tessellating an icosahedron.

    const float X = 0.525731f;
    const float Z = 0.850651f;

    XMFLOAT3 pos[12] =
    {
        XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f)
    };

    DWORD k[60] =
    {
        1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4,
		1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2,
		3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0,
		10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7
    };

    meshData.Vertices.resize(12);
    meshData.Indices.resize(60);

    for (UINT i = 0; i < 12; ++i)
        meshData.Vertices[i].Position = pos[i];

    for (UINT i = 0; i < 60; ++i)
        meshData.Indices[i] = k[i];

    for (UINT i = 0; i < numSubdivisions; ++i)
        Subdivide(meshData);

	// Project vertices onto sphere and scale.
    for (UINT i = 0; i < meshData.Vertices.size(); ++i)
    {
		// Project onto unit sphere.
        XMVECTOR n = XMVector3Normalize(XMLoadFloat3( & meshData.Vertices[i].Position));

		// Project onto sphere.
        XMVECTOR p = radius * n;

        XMStoreFloat3( & meshData.Vertices[i].Position, p);
        XMStoreFloat3( & meshData.Vertices[i].Normal, n);

		// Derive texture coordinates from spherical coordinates.
        float theta = MathHelper::AngleFromXY(
			meshData.Vertices[i].Position.x,
			meshData.Vertices[i].Position.z);

        float phi = acosf(meshData.Vertices[i].Position.y / radius);

        meshData.Vertices[i].TexC.x = theta / XM_2PI;
        meshData.Vertices[i].TexC.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
        meshData.Vertices[i].TangentU.x = -radius * sinf(phi) * sinf(theta);
        meshData.Vertices[i].TangentU.y = 0.0f;
        meshData.Vertices[i].TangentU.z = +radius * sinf(phi) * cosf(theta);

        XMVECTOR T = XMLoadFloat3( & meshData.Vertices[i].TangentU);
        XMStoreFloat3( & meshData.Vertices[i].TangentU, XMVector3Normalize(T));
    }
}
*/
[maxvertexcount(60)]
void gsmain(point uint input[1] : ANY_ID, inout TriangleStream<GS_OUTPUT> tristream)
{
    //local variable
    float3 pos = float3(0.0f, 0.0f, 0.0f);
    float3 extent = float3(0.5f, 0.5f, 0.5f);
    float4 color = float4(0.0f, 1.0f, 0.0f, 1.0f);

    float3 vertices[8] =
    {
        pos + float3(-extent.x, +extent.y, -extent.z), //front lt
        pos + float3(+extent.x, +extent.y, -extent.z), //front rt
        pos + float3(-extent.x, -extent.y, -extent.z), //front lb
        pos + float3(+extent.x, -extent.y, -extent.z), //front rb
        pos + float3(-extent.x, +extent.y, +extent.z), //rear lt
        pos + float3(+extent.x, +extent.y, +extent.z), //rear rt
        pos + float3(-extent.x, -extent.y, +extent.z), //rear lb
        pos + float3(+extent.x, -extent.y, +extent.z), //rear rb
    };
    
    uint indices[36] =
    {
        0, 1, 2, 2, 1, 3, //front
        1, 5, 3, 3, 5, 7, //right
        5, 4, 7, 7, 4, 6, //rear
        4, 0, 6, 6, 0, 2, //left
        4, 5, 0, 0, 5, 1, //top
        2, 3, 6, 6, 3, 7, //bottom
    };
    
    [unroll]
    for (int i = 0; i < 36; ++i)
    {
        GS_OUTPUT output;
        output.pos = mul(float4(vertices[indices[i]], 1.0f), matWorld);
        output.pos = mul(output.pos, matView);
        output.pos = mul(output.pos, matProj);
        output.color = color;
        tristream.Append(output);

        // 삼각형 단위(3개 정점)로 끊어서 처리
        if ((i + 1) % 3 == 0)
        {
            tristream.RestartStrip();
        }
    }
}