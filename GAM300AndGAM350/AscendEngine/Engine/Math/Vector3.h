/******************************************************************************/
/*!
\file   Vector3.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definition of Vector3 maths functions.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

union Vector2;
union Quaternion;

union Vector3
{
  float m[3];
  struct { float x, y, z; };
  struct { float xy[2]; float zVal; };
  //struct { float pitch, yaw, roll; };

  Vector3();
  Vector3(const float(&rhs)[3]);
  Vector3(float, float, float);
  Vector3(const Vector3&);
  Vector3(const Vector2&, float = 0.0f);
  void Set(float = 0.0f, float = 0.0f, float = 0.0f);

  Vector3& operator=(const Vector3&);
  Vector3& operator=(const float(&)[3]);

  float operator[](std::size_t index) const;

  Vector3 operator-() const;
  bool operator==(const Vector3&) const;
  bool operator!=(const Vector3&) const;

  Vector3 operator+(const Vector3&) const;
  Vector3 operator-(const Vector3&) const;
  Vector3 operator/(float) const;

  Vector3& operator+=(const Vector3&);
  Vector3& operator-=(const Vector3&);
  Vector3& operator/=(float);
  Vector3& operator*=(float);

  friend Vector3 operator*(const Quaternion &lhs, const Vector3 &rhs);

  Vector3 RotateZ(float);
  Vector3 RotateY(float);
  Vector3 RotateX(float);

  Vector3 Normalize() const;
  float Magnitude() const;
  float MagnitudeSq() const;

  float Dot(const Vector3&) const;
  Vector3 Cross(const Vector3&) const;

  Vector3 ComponentProduct(const Vector3&) const;
  float ScalarProduct(const Vector3 & Vector3) const;
  Vector3 VectorProduct(const Vector3 & Vector3) const;
  Vector3 VecQuatProduct(const Quaternion& Quat) const;
  static const Vector3 XAXIS;
  static const Vector3 YAXIS;
  static const Vector3 ZAXIS;
  static const Vector3 ZERO;
  static void OrthoNormalize(Vector3& normal, Vector3& tangent);
  static inline float MaxElem(Vector3& v) { return max(v.x, max(v.y, v.z)); }
};

using Vec3 = Vector3;
const std::size_t Vec3Size = sizeof(Vector3);

Vector3 operator*(const Vector3&, float);
Vector3 operator*(float, const Vector3&);
Vector3 operator*(const Vector3&, const Vector3&);
Vector3 operator*(const Quaternion &lhs, const Vector3 &rhs);

//this will make the vector components divide by the float
inline Vector3 operator/(float f, const Vector3& rhs) { return Vector3(f/rhs.x, f/rhs.y, f/rhs.z); }

// For reflection
std::stringstream &operator<<(std::stringstream&, const Vector3&);
std::stringstream &operator >> (std::stringstream&, Vector3&);
std::ostream& operator<<(std::ostream&, const Vector3&);