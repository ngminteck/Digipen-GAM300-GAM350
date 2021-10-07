/******************************************************************************/
/*!
\file   Quaternion.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
Contains definition of Quaternion maths functions.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

union Quaternion
{
  float m[4];
  struct { float w, x, y, z; };

  Quaternion();
  Quaternion(float r, float i, float j, float k);
  Quaternion(const Quaternion& q);

  Quaternion& operator /= (float Scalar);
  Quaternion& operator *= (float Scalar);
  Quaternion& operator *= (const Quaternion& Q);
  Quaternion& operator -= (const Quaternion& Q);
  Quaternion& operator += (const Vector3& vec);

  bool operator==(const Quaternion &) const;
  bool operator!=(const Quaternion &) const;

  friend Quaternion operator * (const Quaternion&, const Quaternion&);
  friend Quaternion operator * (float, const Quaternion&);
  friend Quaternion operator * (const Quaternion&, float);
  friend Quaternion operator - (const Quaternion&);
  friend Quaternion operator - (const Quaternion&, const Quaternion&);

  inline void Set(float, float, float, float);
  inline void SetIdentity();

  Quaternion& SetRotationX(float deg);
  Quaternion& SetRotationY(float deg);
  Quaternion& SetRotationZ(float deg);

  float Dot(const Quaternion&) const;
  Quaternion Normalize() const;
  Quaternion Invert() const;
  Quaternion Inverse() const;
  float Length() const;
  float LengthSq() const;

  //Returns Euler angle in degrees
  //Vector3 GetEuler() const;
  Vector3 GetAxis() const;
  float GetAngle() const;
  Matrix4 GetMatrix(const Vector3& = Vector3()) const;

  static Quaternion LookAt(Vector3 direction, Vector3 desiredUp = Vec3::YAXIS);
  //static Quaternion LookAt(const Vec3 & pos, const Vec3 & direction, Vec3 up = Vec3::YAXIS);
};

//Quaternion EulerToQuaternion(float yaw_deg, float pitch_deg, float roll_deg);
Quaternion EulerToQuaternion(const Vector3& euler);
Vector3 QuaternionToEuler(const Quaternion& q);
Matrix4 QuaternionToMatrix(const Quaternion& q);
Quaternion AxisAngleToQuat(const Vector3& axis, float angle_deg);
Vec3 QuatToAxisAngle(const Quaternion &axis);
Quaternion RotationBetweenVectors(Vec3 start, Vec3 dest);
Quaternion QuaternionFromMatrix(Matrix4 const& m);

std::ostream& operator<<(std::ostream&, const Quaternion&);

using Quat = Quaternion;