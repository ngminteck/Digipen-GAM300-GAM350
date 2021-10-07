/******************************************************************************/
/*!
\file   Quaternion.cpp
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
#include "pch.h"

Quaternion::Quaternion() :
  Quaternion(1.0f, 0.0f, 0.0f, 0.0f)
{
}

Quaternion::Quaternion(float r, float i, float j, float k) :
  w(r), x(i), y(j), z(k)
{
}

Quaternion::Quaternion(const Quaternion& q) :
  Quaternion(q.w, q.x, q.y, q.z)
{
}

Quaternion& Quaternion::operator/=(float Scalar)
{
  x /= Scalar; y /= Scalar; z /= Scalar; w /= Scalar;
  return *this;
}

Quaternion& Quaternion::operator*=(float Scalar)
{
  x *= Scalar; y *= Scalar; z *= Scalar; w *= Scalar;
  return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
#if 0
  x = (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y);
  y = (w * q.y) + (y * q.w) + (z * q.x) - (x * q.z);
  z = (w * q.z) + (z * q.w) + (x * q.y) - (y * q.x);
  w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
#else
  float qax = x, qay = y, qaz = z, qaw = w;
  float qbx = q.x, qby = q.y, qbz = q.z, qbw = q.w;

  //b1a2 + a1b2 + c2d2 - d1c2
  x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
  //c1a2 + a1c2 + d1b2 - b1d2
  y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
  //d1a2 + a1d2 + b1c2 - c1b2
  z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
  w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;
#endif
  return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
  x -= q.x; y -= q.y; z -= q.z; w -= q.w;
  return *this;
}

Quaternion & Quaternion::operator+=(const Vector3& vec)
{
  Quaternion q(vec.x, vec.y, vec.z, 0.0f);
  q = (*this * q) * 0.5f;
  x += q.x; y += q.y; z += q.z; w += 0.0f;
  return *this;
}

bool Quaternion::operator==(const Quaternion &rhs) const
{
  return (fabs(x - rhs.x) < FLT_EPSILON &&
          fabs(y - rhs.y) < FLT_EPSILON &&
          fabs(z - rhs.z) < FLT_EPSILON &&
          fabs(w - rhs.w) < FLT_EPSILON);
}

bool Quaternion::operator!=(const Quaternion &rhs) const
{
  return !(*this == rhs);
}

Quaternion operator*(const Quaternion & lhs, const Quaternion & rhs)
{
  Quaternion result(lhs);
  return result *= rhs;
}

Quaternion operator*(float Scalar, const Quaternion & rhs)
{
  return Quaternion(rhs) *= Scalar;
}

Quaternion operator*(const Quaternion & lhs, float Scalar)
{
  return Quaternion(lhs) *= Scalar;
}

Quaternion operator-(const Quaternion & q)
{
  return Quaternion(-q.x, -q.y, -q.z, -q.w);
}

Quaternion operator-(const Quaternion & lhs, const Quaternion & rhs)
{
  return Quaternion(lhs) -= rhs;
}

void Quaternion::Set(float x_, float y_, float z_, float w_)
{
  x = x_; y = y_; z = z_; w = w_;
}

void Quaternion::SetIdentity()
{
  Set(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion & Quaternion::SetRotationX(float deg)
{
  deg = ASC_DEG2RAD(deg * 0.5f);
  float s = sinf(deg), c = cosf(deg);
  *this *= Quaternion(s, 0, 0, c);
  return *this;
}

Quaternion & Quaternion::SetRotationY(float deg)
{
  deg = ASC_DEG2RAD(deg * 0.5f);
  float s = sinf(deg), c = cosf(deg);
  *this *= Quaternion(0, s, 0, c);
  return *this;
}

Quaternion & Quaternion::SetRotationZ(float deg)
{
  deg = ASC_DEG2RAD(deg * 0.5f);
  float s = sinf(deg), c = cosf(deg);
  *this *= Quaternion(0, 0, s, c);
  return *this;
}

float Quaternion::Dot(const Quaternion &rhs) const
{
  return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

Quaternion Quaternion::Normalize() const
{
  Quaternion q(*this);
  const float lenSq = Dot(*this);
  if (lenSq < ASC_EPSILON) q.Set(0.0f, 0.0f, 0.0f, 1.0f);
  else
    q /= sqrtf(lenSq);
  return q;
}

Quaternion Quaternion::Invert() const
{
  return (-Quaternion(*this));
}

Quaternion Quaternion::Inverse() const
{
  float sqlen = LengthSq();
  return Quaternion(w / sqlen, -x / sqlen, -y / sqlen, -z / sqlen);
}

float Quaternion::Length() const
{
  return sqrtf(LengthSq());
}

float Quaternion::LengthSq() const
{
  return this->Dot(*this);
}

/*Vector3 Quaternion::GetEuler() const
{
float sqw = w*w;
float sqx = x*x;
float sqy = y*y;
float sqz = z*z;
float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
float test = x*y + z*w;

Vec3 euler;
if (test > 0.499f*unit) { // singularity at north pole
euler.y = 2 * atan2(x, w);
euler.z = PI * 0.5f;
euler.x = 0;
}
else if (test < -0.499f*unit) { // singularity at south pole
euler.y = -2 * atan2(x, w);
euler.z = -PI * 0.5f;
euler.x = 0;
}
else
{
euler.y = atan2(2 * y*w - 2 * x*z, sqx - sqy - sqz + sqw);
euler.z = asin(2 * test / unit);
euler.x = atan2(2 * x*w - 2 * y*z, -sqx + sqy - sqz + sqw);
}

euler.x = ASC_RAD2DEG(euler.x);
euler.y = ASC_RAD2DEG(euler.y);
euler.z = ASC_RAD2DEG(euler.z);

return euler;
}*/

Vector3 Quaternion::GetAxis() const
{
  Vector3 Axis;
  float s = sinf(acosf(ASC_RANGE(w, -1.0f, 1.0f)));

  if (abs(s) < ASC_EPSILON)
    Axis.Set(0.0f, 1.0f, 0.0f);
  else
  {
    Axis.Set(x, y, z);
    Axis /= s;
  }
  return Axis;
}

float Quaternion::GetAngle() const
{
  return ASC_RAD2DEG(acosf(ASC_RANGE(w, -1.0f, 1.0f)) * ASC_DEG2RAD(2.0f));
}

Matrix4 Quaternion::GetMatrix(const Vector3& center) const
{
  Quaternion q(*this);
  q = q.Normalize();
  Matrix4 dest;
  dest.m00 = 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z;
  dest.m10 = 2.0f*q.x*q.y - 2.0f*q.z*q.w;
  dest.m20 = 2.0f*q.x*q.z + 2.0f*q.y*q.w;
  dest.m30 = 0.0f;

  dest.m01 = 2.0f*q.x*q.y + 2.0f*q.z*q.w;
  dest.m11 = 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z;
  dest.m21 = 2.0f*q.z*q.y - 2.0f*q.x*q.w;
  dest.m31 = 0.0f;

  dest.m02 = 2.0f*q.x*q.z - 2.0f*q.y*q.w;
  dest.m12 = 2.0f*q.z*q.y + 2.0f*q.x*q.w;
  dest.m22 = 1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y;
  dest.m32 = 0.0f;

  dest.m03 = center.x;
  dest.m13 = center.y;
  dest.m23 = center.z;
  dest.m33 = 1.f;
  return dest;
}

Quaternion Quaternion::LookAt(Vector3 direction, Vector3 desiredUp)
//Quaternion Quaternion::LookAt(const Vec3 &pos, const Vec3 &direction, Vec3 up)
{
  //// Find the rotation between the front of the object (that we assume towards +Z,
  //// but this depends on your model) and the desired direction
  //auto rot1 = RotationBetweenVectors(pos, direction);

  //// Recompute desiredUp so that it's perpendicular to the direction
  //// You can skip that part if you really want to force desiredUp
  //auto desiredUp = up;
  //auto newUp = rot1 * desiredUp;

  //auto right = direction.Cross(desiredUp);
  //desiredUp = right.Cross(direction);

  //// Because of the 1rst rotation, the up is probably completely screwed up.
  //// Find the rotation between the "up" of the rotated object, and the desired up

  //auto rot2 = RotationBetweenVectors(newUp, desiredUp);
  //auto targetOrientation = rot2 * rot1; // remember, in reverse order.
  //return targetOrientation;

  direction = direction.Normalize();
  Vector3 forward = direction; 
  Vector3 up = desiredUp.Normalize();

  Vector3::OrthoNormalize(forward, up);
  Vector3 right = up.Cross(forward);
  right = right.Normalize();

#define m00 right.x
#define m01 up.x
#define m02 forward.x
#define m10 right.y
#define m11 up.y
#define m12 forward.y
#define m20 right.z
#define m21 up.z
#define m22 forward.z

  Quaternion ret;
  ret.w = sqrtf(abs(1.0f + m00 + m11 + m22)) * 0.5f;

  if (ret.w <= 0)
  {
    ret.w = 0.1f;
  }

  float w4_recip = 1.0f / (4.0f * ret.w);
  ret.x = (m21 - m12) * w4_recip;
  ret.y = (m02 - m20) * w4_recip;
  ret.z = (m10 - m01) * w4_recip;

#undef m00
#undef m01
#undef m02
#undef m10
#undef m11
#undef m12
#undef m20
#undef m21
#undef m22 

  return ret;
}

Quaternion EulerToQuaternion(const Vector3& rot)
{
  Quaternion q;
  float t0 = cos(ASC_DEG2RAD(rot.z * 0.5f));
  float t1 = sin(ASC_DEG2RAD(rot.z * 0.5f));
  float t2 = cos(ASC_DEG2RAD(rot.x * 0.5f));
  float t3 = sin(ASC_DEG2RAD(rot.x * 0.5f));
  float t4 = cos(ASC_DEG2RAD(rot.y * 0.5f));
  float t5 = sin(ASC_DEG2RAD(rot.y * 0.5f));

  q.w = t0 * t2 * t4 + t1 * t3 * t5;
  q.x = t0 * t3 * t4 - t1 * t2 * t5;
  q.y = t0 * t2 * t5 + t1 * t3 * t4;
  q.z = t1 * t2 * t4 - t0 * t3 * t5;
  return q;
}

Matrix4 QuaternionToMatrix(const Quaternion & q)
{
  Matrix4 mat;
  float x = q.x, y = q.y, z = q.z, w = q.w;
  float x2 = x + x, y2 = y + y, z2 = z + z;
  float xx = x * x2, xy = x * y2, xz = x * z2;
  float yy = y * y2, yz = y * z2, zz = z * z2;
  float wx = w * x2, wy = w * y2, wz = w * z2;

  mat.m00 = 1 - (yy + zz);
  mat.m10 = xy - wz;
  mat.m20 = xz + wy;

  mat.m01 = xy + wz;
  mat.m11 = 1 - (xx + zz);
  mat.m21 = yz - wx;

  mat.m02 = xz - wy;
  mat.m12 = yz + wx;
  mat.m22 = 1 - (xx + yy);

  // last column
  mat.m03 = 0;
  mat.m13 = 0;
  mat.m23 = 0;

  return mat;
}

Vector3 QuaternionToEuler(const Quaternion & q)
{
  Vector3 v;
  float ysqr = q.y * q.y;

  // roll (x-axis rotation)
  float t0 = 2.0f * (q.w * q.x + q.y * q.z);
  float t1 = 1.0f - 2.0f * (q.x * q.x + ysqr);
  v.x = ASC_RAD2DEG(atan2(t0, t1));

  // pitch (y-axis rotation)
  float t2 = 2.0f * (q.w * q.y - q.z * q.x);
  t2 = t2 > 1.0f ? 1.0f : t2;
  t2 = t2 < -1.0f ? -1.0f : t2;
  v.y = ASC_RAD2DEG(asin(t2));

  // yaw (z-axis rotation)
  float t3 = 2.0f * (q.w * q.z + q.x * q.y);
  float t4 = 1.0f - 2.0f * (ysqr + q.z * q.z);
  v.z = ASC_RAD2DEG(atan2(t3, t4));

  return v;
}

Quaternion AxisAngleToQuat(const Vector3 & axis, float angle_deg)
{
  Vec3 nAxis = axis.Normalize();
  float halfAngle = (ASC_DEG2RAD(angle_deg * 0.5f));
  float s = sinf(halfAngle);
  Quaternion q;
  q.x = nAxis.x * s;
  q.y = nAxis.y * s;
  q.z = nAxis.z * s;
  q.w = cosf(halfAngle);
  return q;
}

Quaternion RotationBetweenVectors(Vec3 start, Vec3 dest) {
  start = start.Normalize();
  dest = dest.Normalize();

  float cosTheta = start.Dot(dest);
  Vec3 rotationAxis;

  if (cosTheta < -1 + 0.001f) {
    // special case when vectors in opposite directions:
    // there is no "ideal" rotation axis
    // So guess one; any will do as long as it's perpendicular to start
    rotationAxis = Vec3::ZAXIS.Cross(start);// (0.0f, 0.0f, 1.0f));
    if (rotationAxis.MagnitudeSq() < FLT_EPSILON) // bad luck, they were parallel, try again!
      rotationAxis = Vec3::XAXIS.Cross(start);

    rotationAxis = rotationAxis.Normalize();
    return AxisAngleToQuat(rotationAxis, 180.0f);
  }

  rotationAxis = start.Cross(dest);

  float s = sqrt((1.0f + cosTheta) * 2.0f);
  float invs = 1.0f / s;

  return Quaternion(
    s * 0.5f,
    rotationAxis.x * invs,
    rotationAxis.y * invs,
    rotationAxis.z * invs
  );

}

Quaternion QuaternionFromMatrix(Matrix4 const& m) {
  // Adapted from: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
  Quaternion q;
  q.w = sqrtf(max(0, 1 + m.m00 + m.m11 + m.m22)) * 0.5f;
  q.x = sqrtf(max(0, 1 + m.m00 - m.m11 - m.m22)) * 0.5f;
  q.y = sqrtf(max(0, 1 - m.m00 + m.m11 - m.m22)) * 0.5f;
  q.z = sqrtf(max(0, 1 - m.m00 - m.m11 + m.m22)) * 0.5f;

  q.x *= ASC_SIGN(q.x * (m.m21 - m.m12));
  q.y *= ASC_SIGN(q.y * (m.m02 - m.m20));
  q.z *= ASC_SIGN(q.z * (m.m10 - m.m01));
  return q;
}

Vec3 QuatToAxisAngle(const Quat& axis)
{
  return axis.GetAxis();
}

std::ostream & operator<<(std::ostream & os, const Quaternion & rhs)
{
  os << "< " << rhs.x << " : " << rhs.y << " : " << rhs.z << " : " << rhs.w << " >";
  return os;
}