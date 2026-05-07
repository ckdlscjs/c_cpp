#pragma once
#include "Vector4.h"

__declspec(align(16))
struct Matrix4x4
{
    // --- 생성자 ---
    // 기본 생성자: 단위 행렬로 초기화
    inline Matrix4x4() : m_mat(DirectX::XMMatrixIdentity()) {}

    // 16개 float 성분으로 직접 초기화
    inline Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        m_mat = DirectX::XMMatrixSet(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        );
    }
    // DirectX::XMFLOAT4X4에서 로드
    inline Matrix4x4(const float* m) : Matrix4x4(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]) {}
    inline Matrix4x4(const DirectX::XMFLOAT4X4& m) : m_mat(DirectX::XMLoadFloat4x4(&m)) {}
    inline Matrix4x4(const DirectX::XMVECTOR& r0, const DirectX::XMVECTOR& r1, const DirectX::XMVECTOR& r2, const DirectX::XMVECTOR& r3) : m_mat(DirectX::XMMATRIX(r0, r1, r2, r3)) {}
    inline Matrix4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) : m_mat(DirectX::XMMATRIX(r0.ToXMVECTOR(), r1.ToXMVECTOR(), r2.ToXMVECTOR(), r3.ToXMVECTOR())) {}

    // DirectX::XMMATRIX에서 직접 초기화
    inline Matrix4x4(const DirectX::XMMATRIX& m) : m_mat(m) {}

    // Convert to aiMat
    inline Matrix4x4(const aiMatrix4x4& aiMat)
    {
        m_mat.r[0] = DirectX::XMVectorSet(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1);
        m_mat.r[1] = DirectX::XMVectorSet(aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2);
        m_mat.r[2] = DirectX::XMVectorSet(aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3);
        m_mat.r[3] = DirectX::XMVectorSet(aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
    }

    // 연산자재정의
    // 행렬 덧셈
    inline Matrix4x4 operator+(const Matrix4x4& other) const {
        return Matrix4x4(
            DirectX::XMVectorAdd(m_mat.r[0], other.m_mat.r[0]),
            DirectX::XMVectorAdd(m_mat.r[1], other.m_mat.r[1]),
            DirectX::XMVectorAdd(m_mat.r[2], other.m_mat.r[2]),
            DirectX::XMVectorAdd(m_mat.r[3], other.m_mat.r[3])
        );
    }
    inline Matrix4x4& operator+=(const Matrix4x4& other) {
        m_mat.r[0] = DirectX::XMVectorAdd(m_mat.r[0], other.m_mat.r[0]);
        m_mat.r[1] = DirectX::XMVectorAdd(m_mat.r[1], other.m_mat.r[1]);
        m_mat.r[2] = DirectX::XMVectorAdd(m_mat.r[2], other.m_mat.r[2]);
        m_mat.r[3] = DirectX::XMVectorAdd(m_mat.r[3], other.m_mat.r[3]);
        return *this;
    }
    // 행렬 뺄셈
    inline Matrix4x4 operator-(const Matrix4x4& other) const {
        return Matrix4x4(
            DirectX::XMVectorSubtract(m_mat.r[0], other.m_mat.r[0]),
            DirectX::XMVectorSubtract(m_mat.r[1], other.m_mat.r[1]),
            DirectX::XMVectorSubtract(m_mat.r[2], other.m_mat.r[2]),
            DirectX::XMVectorSubtract(m_mat.r[3], other.m_mat.r[3])
        );
    }
    inline Matrix4x4& operator-=(const Matrix4x4& other) {
        m_mat.r[0] = DirectX::XMVectorSubtract(m_mat.r[0], other.m_mat.r[0]);
        m_mat.r[1] = DirectX::XMVectorSubtract(m_mat.r[1], other.m_mat.r[1]);
        m_mat.r[2] = DirectX::XMVectorSubtract(m_mat.r[2], other.m_mat.r[2]);
        m_mat.r[3] = DirectX::XMVectorSubtract(m_mat.r[3], other.m_mat.r[3]);
        return *this;
    }
    // 행렬 곱셈
    inline Matrix4x4 operator*(const Matrix4x4& other) const { return Matrix4x4(DirectX::XMMatrixMultiply(m_mat, other.m_mat)); }
    inline Matrix4x4& operator*=(const Matrix4x4& other) { m_mat = DirectX::XMMatrixMultiply(m_mat, other.m_mat); return *this; }
    inline Matrix4x4 operator*(float scalar) const {
        return Matrix4x4(
            DirectX::XMVectorScale(m_mat.r[0], scalar),
            DirectX::XMVectorScale(m_mat.r[1], scalar),
            DirectX::XMVectorScale(m_mat.r[2], scalar),
            DirectX::XMVectorScale(m_mat.r[3], scalar)
        );
    }
    inline Matrix4x4& operator*=(float scalar) {
        m_mat.r[0] = DirectX::XMVectorScale(m_mat.r[0], scalar);
        m_mat.r[1] = DirectX::XMVectorScale(m_mat.r[1], scalar);
        m_mat.r[2] = DirectX::XMVectorScale(m_mat.r[2], scalar);
        m_mat.r[3] = DirectX::XMVectorScale(m_mat.r[3], scalar);
        return *this;
    }

    // 스칼라 나눗셈
    inline Matrix4x4 operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return (*this) * invScalar;
    }
    inline Matrix4x4& operator/=(float scalar) {
        float invScalar = 1.0f / scalar;
        return (*this) *= invScalar;
    }
    //벡터*행렬 계산
    inline Vector4 operator*(const Vector4& vec) const {
        return Vector4(DirectX::XMVector4Transform(vec.ToXMVECTOR(), m_mat));
    }
    
    //행 접근 (읽기/쓰기 가능)
    inline Vector4& operator[](int idx) { return r[idx]; }
    inline const Vector4& operator[](int idx) const { return r[idx]; }

    //유틸함수들
    inline Matrix4x4 Transpose() const { return Matrix4x4(DirectX::XMMatrixTranspose(m_mat)); }
    inline float Determinant() const { return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(m_mat)); }
	inline Matrix4x4 Identity() { return Matrix4x4(DirectX::XMMatrixIdentity()); }
    inline DirectX::XMMATRIX ToXMMATRIX() const { return m_mat; }
    inline DirectX::XMFLOAT4X4 ToXMFLOAT4X4() const
    {
        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, m_mat);
        return result;
    }
    friend inline Vector3 operator*(const Vector3& v, const Matrix4x4& m);
    friend inline Vector4 operator*(const Vector4& v, const Matrix4x4& m);
private:
    union
    {
        DirectX::XMMATRIX m_mat;
        Vector4 r[4];
    };
};

// v * m 연산을 위한 함수
inline Vector3 operator*(const Vector3& v, const Matrix4x4& m)
{
    // 결과를 다시 Vector3 객체로 반환, w를 1로 처리하고 연산해준다
    return Vector3(DirectX::XMVector3Transform(v.ToXMVECTOR(), m.ToXMMATRIX()));
}
inline Vector4 operator*(const Vector4& v, const Matrix4x4& m)
{
    // DirectXMath의 XMVector4Transform은 Vector * Matrix 연산을 수행합니다.
    return Vector4(DirectX::XMVector4Transform(v.ToXMVECTOR(), m.ToXMMATRIX()));
}

// 스칼라 * 행렬 연산 (이전에 있었던 내용, 다시 한번 확인차 포함)
inline Matrix4x4 operator*(float scalar, const Matrix4x4& mat) {
    return mat * scalar;
}

