#pragma once
#include "Vector4.h"

__declspec(align(16))
struct Matrix4x4
{
    // --- ������ ---
    // �⺻ ������: ���� ��ķ� �ʱ�ȭ
    inline Matrix4x4() : m_mat(DirectX::XMMatrixIdentity()) {}

    // 16�� float �������� ���� �ʱ�ȭ
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
    // DirectX::XMFLOAT4X4���� �ε�
    inline Matrix4x4(const DirectX::XMFLOAT4X4& m) : m_mat(DirectX::XMLoadFloat4x4(&m)) {}
    inline Matrix4x4(const DirectX::XMVECTOR& r0, const DirectX::XMVECTOR& r1, const DirectX::XMVECTOR& r2, const DirectX::XMVECTOR& r3) : m_mat(DirectX::XMMATRIX(r0, r1, r2, r3)) {}
    inline Matrix4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) : m_mat(DirectX::XMMATRIX(r0.ToXMVECTOR(), r1.ToXMVECTOR(), r2.ToXMVECTOR(), r3.ToXMVECTOR())) {}

    // DirectX::XMMATRIX���� ���� �ʱ�ȭ
    inline Matrix4x4(const DirectX::XMMATRIX& m) : m_mat(m) {}

    // ������������
    // ��� ����
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
    // ��� ����
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
    // ��� ����
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

    // ��Į�� ������
    inline Matrix4x4 operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return (*this) * invScalar;
    }
    inline Matrix4x4& operator/=(float scalar) {
        float invScalar = 1.0f / scalar;
        return (*this) *= invScalar;
    }
    //����*��� ���
    inline Vector4 operator*(const Vector4& vec) const {
        return Vector4(DirectX::XMVector4Transform(vec.ToXMVECTOR(), m_mat));
    }
    
    //�� ���� (�б�/���� ����)
    inline Vector4& operator[](int idx) { return r[idx]; }
    inline const Vector4& operator[](int idx) const { return r[idx]; }

    //��ƿ�Լ���
    inline Matrix4x4 Transpose() const { return Matrix4x4(DirectX::XMMatrixTranspose(m_mat)); }
    inline float Determinant() const { return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(m_mat)); }
    static inline Matrix4x4 Identity() { return Matrix4x4(DirectX::XMMatrixIdentity()); }
    inline DirectX::XMMATRIX ToXMMATRIX() const { return m_mat; }
    inline DirectX::XMFLOAT4X4 ToXMFLOAT4X4() const
    {
        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, m_mat);
        return result;
    }
    friend inline Vector4 operator*(const Vector4& v, const Matrix4x4& m);
    friend inline Vector3 operator*(const Vector3& v, const Matrix4x4& m);
private:
    union
    {
        DirectX::XMMATRIX m_mat;
        Vector4 r[4];
    };
};

// v * m ������ ���� �Լ�
inline Vector3 operator*(const Vector3& v, const Matrix4x4& m)
{
    // 1. Vector3�� XMVECTOR�� ��ȯ
    DirectX::XMVECTOR v_vec = v.ToXMVECTOR();

    // 2. XMMatrix�� XMVector3TransformNormal �Լ� ����
    //    �� �Լ��� w=0�� ���͸� ��� ��ȯ�ϰ�, ����ȭ(Normalize)���� ���ݴϴ�.
    DirectX::XMVECTOR result_vec = DirectX::XMVector3TransformNormal(v_vec, m.ToXMMATRIX());

    // 3. ����� �ٽ� Vector3 ��ü�� ��ȯ
    return Vector3(result_vec);
}

// Vector4 * Matrix4x4 �����ڸ� ���� �Լ��� �����ε�
inline Vector4 operator*(const Vector4& v, const Matrix4x4& m)
{
    // DirectXMath�� XMVector4Transform�� Vector * Matrix ������ �����մϴ�.
    return Vector4(DirectX::XMVector4Transform(v.ToXMVECTOR(), m.ToXMMATRIX()));
}

// ��Į�� * ��� ���� (������ �־��� ����, �ٽ� �ѹ� Ȯ���� ����)
inline Matrix4x4 operator*(float scalar, const Matrix4x4& mat) {
    return mat * scalar;
}
