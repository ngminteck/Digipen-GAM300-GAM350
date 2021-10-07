/******************************************************************************/
/*!
\file   Matrix4.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definition of Matrix4 maths functions.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

/*-----------------------------------------------------------
MATRIX 4x4
-----------------------------------------------------------*/
const Matrix4 Matrix4::IDENTITY = Matrix4();

Matrix4::Matrix4()
{
  for (int i = 0; i < 4; ++i)
    m[i][0] = m[i][1] = m[i][2] = m[i][3] = 0;

  m00 = m11 = m22 = m33 = 1;
}

Vector3 Matrix4::operator*(const Vector3 &rhs) const
{
  return Vector3(
    rhs.x * m[0][0] +
    rhs.y * m[0][1] +
    rhs.z * m[0][2] + m[0][3],

    rhs.x * m[1][0] +
    rhs.y * m[1][1] +
    rhs.z * m[1][2] + m[1][3],

    rhs.x * m[2][0] +
    rhs.y * m[2][1] +
    rhs.z * m[2][2] + m[2][3]
  );
}

Vector4 Matrix4::operator*(const Vector4& rhs) const
{
  Vector4 result{ 0, 0, 0, 0 };

  for (int i = 0; i < 4; ++i)
  {
    // Traversing l row and r column
    for (int j = 0; j < 4; ++j)
    {
      result[i] += m[j][i] * rhs[j];
    }
  }
  return result;
}

Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
  Matrix4 temp;
  for (int x = 0; x < 16; ++x)
  {
    float sum = 0;
    int t = x % 4;
    for (int y = 0; y < 4; ++y)
      sum += m[(int)(x*0.25f)][y] * rhs.m[y][t];

    temp.m[t][x - (t * 4)] = sum;
  }
  return temp;
}

Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
  return *this = (*this) * rhs;
}

Matrix4& Matrix4::operator=(const Matrix4& rhs)
{
  for (int i = 0; i < 4; ++i)
  {
    m[i][0] = rhs.m[i][0];
    m[i][1] = rhs.m[i][1];
    m[i][2] = rhs.m[i][2];
    m[i][3] = rhs.m[i][3];
  }
  return *this;
}

Matrix4& Matrix4::Concat(const Vector3 & pos, const Vector3 & scale, const Quaternion & q)
{
  for (int i = 0; i < 4; ++i)
      m[i][0] = m[i][1] = m[i][2] = m[i][3] = 0;
  m33 = 1;

  m00 = scale.x; m11 = scale.y; m22 = scale.z;
  *this *= q.GetMatrix();

  m30 = pos.x; m31 = pos.y; m32 = pos.z;
  return *this;
}

Matrix4 Matrix4::Transpose() const
{
  Matrix4 out = *this;
  for (int i = 0; i < 4; ++i)
    for (int y = 0; y < 4; ++y)
      out.m[y][i] = m[i][y];

  return out;
}

Vec3 Matrix4::Transform(const Vec3 &point) const
{
  return (*this) * point;
}

Vec3 Matrix4::TransformInverse(const Vec3 &vector) const
{
  Vector3 tmp = vector;
  tmp.x -= m[0][3];
  tmp.y -= m[1][3];
  tmp.z -= m[2][3];

  return Vector3(
    tmp.x * m[0][0] +
    tmp.y * m[1][0] +
    tmp.z * m[2][0],

    tmp.x * m[0][1] +
    tmp.y * m[1][1] +
    tmp.z * m[2][1],

    tmp.x * m[0][2] +
    tmp.y * m[1][2] +
    tmp.z * m[2][2]
  );
}

void Matrix4::SetIdentity()
{
  for (int i = 0; i < 4; ++i)
    m[i][0] = m[i][1] = m[i][2] = m[i][3] = 0;
      
  m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

Vector3 Matrix4::GetAxisVector(int i) const
{
  int r = i / 4;
  int c = i % 4;

  return Vector3(m[r][c], m[r + 1][c], m[r + 2][c]);
}

Matrix4 Matrix4::Translate(const Vec3& pos)
{
  Matrix4 m;
  m.m30 = pos.x;
  m.m31 = pos.y;
  m.m32 = pos.z;
  return m;
}

Matrix4 Matrix4::Scale(const Vec3& scale)
{
  Matrix4 m;
  m.m00 = scale.x;
  m.m11 = scale.y;
  m.m22 = scale.z;
  return m;
}

Matrix4 Matrix4::ConcatModelMat(const Vector3 & pos, const Vector3 & scale, const Vector3 & axis, float angle_deg)
{
  Matrix4 scaleMat;
  scaleMat.m00 = scale.x;
  scaleMat.m11 = scale.y;
  scaleMat.m22 = scale.z;
  Matrix4 rotMat = Matrix4::Rotate(axis, angle_deg);

  Matrix4 result = rotMat * scaleMat;
  result.m30 = pos.x;
  result.m31 = pos.y;
  result.m32 = pos.z;
  return result;
}

Matrix4 Matrix4::ConcatModelMat(const Vector3 & pos, const Vector3 & scale, const Quaternion & q)
{
  Matrix4 scaleMat;
  scaleMat.m00 = scale.x;
  scaleMat.m11 = scale.y;
  scaleMat.m22 = scale.z;
  Matrix4 rotMat = q.GetMatrix();

  Matrix4 result = scaleMat * rotMat;
  result.m30 = pos.x;
  result.m31 = pos.y;
  result.m32 = pos.z;
  return result;
}

Matrix4 Matrix4::ConcatModelMat(const Vector3 & pos, const Vector3 & scale)
{
  Matrix4 Mat;
  Mat.m00 = scale.x;
  Mat.m11 = scale.y;
  Mat.m22 = scale.z;
  Mat.m30 = pos.x;
  Mat.m31 = pos.y;
  Mat.m32 = pos.z;
  return Mat;
}

Matrix4 Matrix4::Rotate(const Vector3 & axis, float angle_deg)
{
  float rad = ASC_DEG2RAD(angle_deg);
  float c = cosf(rad);
  float s = sinf(rad);
  float r = 1 - c;
  Vec3 vec = axis.Normalize();
  float xs = vec.x * s;
  float ys = vec.y * s;
  float zs = vec.z * s;

  Matrix4 tmp;
  tmp.m[0][0] = r * vec.x * vec.x + c;
  tmp.m[0][1] = r * vec.x * vec.y - zs;
  tmp.m[0][2] = r * vec.x * vec.z + ys;

  tmp.m[1][0] = r * vec.y * vec.x + zs;
  tmp.m[1][1] = r * vec.y * vec.y + c;
  tmp.m[1][2] = r * vec.y * vec.z - xs;

  tmp.m[2][0] = r * vec.z * vec.x - ys;
  tmp.m[2][1] = r * vec.z * vec.y + xs;
  tmp.m[2][2] = r * vec.z * vec.z + c;
  return tmp;
}

Matrix4 Matrix4::SymPerspective(float fov, float aspect, float zNear, float zFar)
{
  Matrix4 tmp;
  float tanHalfVerticalFov = tan(ASC_DEG2RAD(fov * 0.5f));
  tmp.m00 = 1.0f / (aspect * tanHalfVerticalFov);
  tmp.m11 = 1.0f / (tanHalfVerticalFov);
  tmp.m22 = -(zFar + zNear) / (zFar - zNear);
  tmp.m32 = -(2.0f * zFar * zNear) / (zFar - zNear);
  tmp.m23 = -1.0f;
  tmp.m33 = 0.0f;
  return tmp;
}

Matrix4 Matrix4::SymOrthographic(float width, float height, float zNear, float zFar)
{
  float hWidth = width * 0.5f;
  float hHeight = height * 0.5f;
  return NonSymOrthographic(-hWidth, hWidth, hHeight, -hHeight, zNear, zFar);
}

Matrix4 Matrix4::NonSymOrthographic(float l, float r, float t, float b, float n, float f)
{
  Matrix4 tmp;

  tmp.m00 = 2.0f / (r - l);
  tmp.m11 = 2.0f / (t - b);
  tmp.m22 = -2.0f / (f - n);
  tmp.m33 = 1.0f;

  tmp.m30 = -((r + l) / (r - l));
  tmp.m31 = -((t + b) / (t - b));
  tmp.m32 = -((f + n) / (f - n));

  return tmp;
}

Matrix4 Matrix4::NonSymPerspective(float l, float r, float t, float b, float n, float f)
{
  Matrix4 tmp;
  float rl = (r - l), tb = (t - b), fn = (f - n);
  tmp.m00 = (2.0f * n) / rl; tmp.m02 = (r + l) / rl;
  tmp.m11 = (2.0f * n) / tb; tmp.m12 = (t + b) / tb;
  tmp.m22 = -(f + n) / fn; tmp.m23 = -(2.0f * f*n) / fn;
  tmp.m32 = -1.0f;
  tmp.m33 = 0.0f;
  return tmp;
}

Matrix4 Matrix4::LookAt(const Vector3& pos, const Vector3& look, const Vector3& right)
{
  Matrix4 tmp;

  Vec3 nRight = right.Normalize();
  Vec3 nLook = look.Normalize();
  Vec3 up = nRight.Cross(nLook).Normalize();

  tmp.m00 = nRight.x; tmp.m10 = nRight.y; tmp.m20 = nRight.z;
  tmp.m01 = up.x;    tmp.m11 = up.y;    tmp.m21 = up.z;
  tmp.m02 = -nLook.x;  tmp.m12 = -nLook.y;  tmp.m22 = -nLook.z;

  tmp.m30 = -nRight.Dot(pos);
  tmp.m31 = -up.Dot(pos);
  tmp.m32 = nLook.Dot(pos);

  return tmp;
}

Matrix4 Matrix4::FormRotationMatrix(const Vector3& forward, const Vector3& up)
{
    Matrix4 tmp;
    Vec3 xaxis =up.Cross( forward);
    xaxis = xaxis.Normalize();

    Vec3 yaxis =forward.Cross( xaxis);
    yaxis = yaxis.Normalize();

    tmp.m00 = xaxis.x;
    tmp.m01 = yaxis.x;
    tmp.m02 = forward.x;

    tmp.m10 = xaxis.y;
    tmp.m11 = yaxis.y;
    tmp.m12 = forward.y;

    tmp.m20 = xaxis.z;
    tmp.m21 = yaxis.z;
    tmp.m22 = forward.z;

    return tmp;
}