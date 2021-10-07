/******************************************************************************/
/*!
\file   Matrix4.h
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
#pragma once

union Matrix4
{
  float m[4][4];
  struct { float mat[16]; };
  struct {
    float
      m00, m01, m02, m03,
      m10, m11, m12, m13,
      m20, m21, m22, m23,
      m30, m31, m32, m33;
  };

  Matrix4();

  Vector3 operator*(const Vector3 &vector) const;
  Vector4 operator*(const Vector4&) const;
  Matrix4 operator*(const Matrix4&) const;
  Matrix4& operator*=(const Matrix4&);
  Matrix4& operator=(const Matrix4&);

  Matrix4& Concat(const Vector3& pos, const Vector3& scale, const Quaternion& q);
  Matrix4 Transpose() const;
  Vec3 Transform(const Vec3 & point) const;
  Vec3 TransformInverse(const Vec3 & point) const;
  void SetIdentity();
  Vector3 GetAxisVector(int i) const;
  static Matrix4 Translate(const Vec3 &pos);
  static Matrix4 Scale(const Vec3& scale);

  static Matrix4 ConcatModelMat(const Vector3& pos, const Vector3& scale, const Vector3& axis, float angle_deg);
  static Matrix4 ConcatModelMat(const Vector3& pos, const Vector3& scale, const Quaternion& q);
  static Matrix4 ConcatModelMat(const Vector3& pos, const Vector3& scale);
  static Matrix4 Rotate(const Vector3& axis, float angle_deg);
  static Matrix4 SymPerspective(float fov, float aspect, float zNear, float zFar);
  static Matrix4 SymOrthographic(float width, float height, float zNear, float zFar);
  static Matrix4 NonSymOrthographic(float l, float r, float t, float b, float n, float f);
  static Matrix4 NonSymPerspective(float l, float r, float t, float b, float n, float f);
  static Matrix4 LookAt(const Vector3& pos, const Vector3& look, const Vector3& right);
  static Matrix4 FormRotationMatrix(const Vector3& forward, const Vector3& up);

  static const Matrix4 IDENTITY;
};

using Mat4 = Matrix4;
const size_t Mat4Size = sizeof(Mat4);