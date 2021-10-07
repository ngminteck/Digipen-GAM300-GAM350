/******************************************************************************/
/*!
\file   MathExt.cpp
\author James Leong Jia Hui
\par    email: james.leong\@digipen.edu
\par    Digipen login: james.leong
\date   12/3/2015
\brief
Contains multiple definitions required for Vector2, Vector3, Vector4, Matrix4D

Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"
#pragma warning ( disable : 4100 )

//const Vector2 Vector2::XAXIS = Vector2(1.0f, 0.0f);
//const Vector2 Vector2::YAXIS = Vector2(0.0f, 1.0f);
//const Vector2 Vector2::ZERO = Vector2();
//
//const Vector3 Vector3::XAXIS = Vector3(1.0f, 0.0f, 0.0f);
//const Vector3 Vector3::YAXIS = Vector3(0.0f, 1.0f, 0.0f);
//const Vector3 Vector3::ZAXIS = Vector3(0.0f, 0.0f, 1.0f);
//const Vector3 Vector3::ZERO = Vector3();
//
//const Matrix4 Matrix4::IDENTITY = Matrix4();
//
//// Default Constructor
//Vector2::Vector2() : x(0), y(0) {}
//
//// Conversion Constructors
//Vector2::Vector2(float X, float Y) : x(X), y(Y) {}
//Vector2::Vector2(int x, int y) : x((float)x), y((float)y) {}
////Vector2::Vector2(const Vector3& rhs) : x(rhs.x), y(rhs.y) {}
//
////Copy Constructor
//Vector2::Vector2(const Vector2& rhs) : x(rhs.x), y(rhs.y) {}
//
//
////Assignment Operator
//Vector2& Vector2::operator=(const Vector2& rhs)
//{
//  x = rhs.x; y = rhs.y;
//  return *this;
//}
//
//void Vector2::Set(float x, float y) { this->x = x; this->y = y; }
//void Vector2::SetX(float x) { this->x = x; }
//void Vector2::SetY(float y) { this->y = y; }
//
//Vector2& Vector2::operator-()
//{
//  x = -x; y = -y;
//  return *this;
//}
//
//bool Vector2::operator==(const Vector2& rhs) const
//{
//  if (abs(x - rhs.x) > ASC_EPSILON ||
//      abs(y - rhs.y) > ASC_EPSILON) return false;
//  return true;
//}
//
//bool Vector2::operator!=(const Vector2& rhs) const
//{
//  return !(operator==(rhs));
//}
//
//Vector2 Vector2::operator+(const Vector2& rhs) const
//{
//  return Vector2(x + rhs.x, y + rhs.y);
//}
//
//Vector2 Vector2::operator-(const Vector2& rhs) const
//{
//  return Vector2(x - rhs.x, y - rhs.y);
//}
//
//Vector2 Vector2::operator/(float rhs) const
//{
//  return Vector2(x / rhs, y / rhs);
//}
//
//Vector2 Vector2::operator/(const Vector2& rhs) const
//{
//  return Vector2(x / rhs.x, y / rhs.y);
//}
//
//Vector2& Vector2::operator+=(const Vector2& rhs)
//{
//  x += rhs.x; y += rhs.y;
//  return *this;
//}
//
//Vector2& Vector2::operator-=(const Vector2& rhs)
//{
//  x -= rhs.x; y -= rhs.y;
//  return *this;
//}
//
//Vector2& Vector2::operator/=(float rhs)
//{
//  x /= rhs; y /= rhs;
//  return *this;
//}
//
//Vector2& Vector2::operator*=(float rhs)
//{
//  x *= rhs; y *= rhs;
//  return *this;
//}
//
//Vector2 Vector2::Normalize() const
//{
//  float mag = Magnitude();
//  if (!mag) return Vec2(0, 0); //Check for infinity
//  else return Vector2(x / mag, y / mag);
//}
//
//Vector2 Vector2::RotateZ(float angle)
//{
//  angle = ASC_DEG2RAD(angle);
//  float sinA = sin(angle);
//  float cosA = cos(angle);
//  Vector2 tmp;
//  tmp.x = (x * cosA) - (y * sinA);
//  tmp.y = (x * sinA) + (y * cosA);
//  return tmp;
//}
//
//float Vector2::Magnitude() const
//{
//  return sqrt(MagnitudeSq());
//}
//
//float Vector2::MagnitudeSq() const
//{
//  return x*x + y*y;
//}
//
////Returns the length of the projected vector
//float Vector2::Dot(const Vector2& rhs) const
//{
//  return x*rhs.x + y*rhs.y;
//}
//
//float Vector2::GetAngle(const Vector2& rhs) const
//{
//  float magMult = this->Magnitude() * rhs.Magnitude();
//  if (abs(magMult) < ASC_EPSILON*ASC_EPSILON) return 0;
//  return ASC_RAD2DEG(acos(Dot(rhs) / magMult));
//}
//
//// Static function to return a vector from an angle in radians.
//// Optimize: Compute using sin/cos directly.
//Vector2 Vector2::Vec2FromAngle(float deg)
//{
//  deg = ASC_DEG2RAD(deg);
//  return Vector2(cos(deg), sin(deg)).Normalize();
//}
//
//Vector2 operator*(const Vector2& lhs, float rhs)
//{
//  return Vector2(lhs.x * rhs, lhs.y * rhs);
//}
//
//Vector2 operator*(float lhs, const Vector2& rhs)
//{
//  return Vector2(rhs.x * lhs, rhs.y * lhs);
//}
//
//std::stringstream &operator<<(std::stringstream &os, const Vector2& rhs)
//{
//  os << rhs.x << " " << rhs.y;
//  return os;
//}
//
//std::stringstream &operator >> (std::stringstream &is, Vector2& rhs)
//{
//  is >> rhs.x >> rhs.y;
//  return is;
//}
//
//std::ostream& operator<<(std::ostream& os, const Vector2& rhs)
//{
//  os << "< " << rhs.x << " : " << rhs.y << " >";
//  return os;
//}

////Default Constructor
//Vector3::Vector3() : x(0), y(0), z(0) {}
//
////Conversion Constructor
//Vector3::Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
//
////Copy Constructor
//Vector3::Vector3(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
//
//Vector3::Vector3(const Vector2& rhs, float depth) : x(rhs.x), y(rhs.y), z(depth) {}
//
//Vector3& Vector3::operator = (const Vector3& rhs)
//{
//  x = rhs.x; y = rhs.y; z = rhs.z;
//  return *this;
//}
//
//float Vector3::operator[](size_t index) const
//{
//  return *(&x + index);
//}
//
//Vector3 Vector3::operator-() const
//{
//  return Vec3(-x, -y, -z);
//}
//
//bool Vector3::operator==(const Vector3& rhs) const
//{
//  if (abs(x - rhs.x) > ASC_EPSILON ||
//      abs(y - rhs.y) > ASC_EPSILON ||
//      abs(z - rhs.z) > ASC_EPSILON) return false;
//  return true;
//}
//
//bool Vector3::operator!=(const Vector3& rhs) const
//{
//  return !(operator==(rhs));
//}
//
//void Vector3::Set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
//
//Vector3 Vector3::operator+(const Vector3& rhs) const
//{
//  return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
//}
//
//Vector3 Vector3::operator-(const Vector3& rhs) const
//{
//  return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
//}
//
//Vector3 Vector3::operator/(float rhs) const
//{
//  return Vector3(x / rhs, y / rhs, z / rhs);
//}
//
//Vector3& Vector3::operator+=(const Vector3& rhs)
//{
//  x += rhs.x; y += rhs.y; z += rhs.z;
//  return *this;
//}
//
//Vector3& Vector3::operator-=(const Vector3& rhs)
//{
//  x -= rhs.x; y -= rhs.y; z -= rhs.z;
//  return *this;
//}
//
//Vector3& Vector3::operator/=(float rhs)
//{
//  x /= rhs; y /= rhs; z /= rhs;
//  return *this;
//}
//
//Vector3& Vector3::operator*=(float rhs)
//{
//  x *= rhs; y *= rhs; z *= rhs;
//  return *this;
//}
//
//Vector3 Vector3::RotateZ(float angle)
//{
//  angle = ASC_DEG2RAD(angle);
//  float sinA = sin(angle);
//  float cosA = cos(angle);
//  Vector3 tmp;
//  tmp.x = (x * cosA) - (y * sinA);
//  tmp.y = (x * sinA) + (y * cosA);
//  tmp.z = z;
//  return tmp;
//}
//
//Vector3 Vector3::RotateY(float angle)
//{
//  angle = ASC_DEG2RAD(angle);
//  float sinA = sin(angle);
//  float cosA = cos(angle);
//  Vector3 tmp;
//  tmp.x = (x * cosA) + (z * sinA);
//  tmp.y = y;
//  tmp.z = -(x * sinA) + (z * cosA);
//  return tmp;
//}
//
//Vector3 Vector3::RotateX(float angle)
//{
//  angle = ASC_DEG2RAD(angle);
//  float sinA = sin(angle);
//  float cosA = cos(angle);
//  Vector3 tmp;
//  tmp.x = x;
//  tmp.y = (y * cosA) - (z * sinA);
//  tmp.z = (y * sinA) + (z * cosA);
//  return tmp;
//}
//
//Vector3 Vector3::Normalize() const
//{
//  float mag = InvSqrt(MagnitudeSq());
//  return Vector3(x * mag, y * mag, z * mag);
//}
//
//float Vector3::Magnitude() const
//{
//  return sqrt(MagnitudeSq());
//}
//
//float Vector3::MagnitudeSq() const
//{
//  return x*x + y*y + z*z;
//}
//
////Returns the length of the projected vector
//float Vector3::Dot(const Vector3& rhs) const
//{
//  return x*rhs.x + y*rhs.y + z*rhs.z;
//}
//
//Vector3 operator*(const Vector3& lhs, float rhs)
//{
//  return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
//}
//
//Vector3 operator*(float lhs, const Vector3& rhs)
//{
//  return Vector3(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
//}
//
//Vector3 operator*(const Vector3 &a, const Vector3 &b)
//{
//  return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
//}
//
//Vector3 operator*(const Quaternion &q, const Vec3 &v)
//{
//  const Vec3 QuatVector(q.x, q.y, q.z);
//  const Vec3 uv(QuatVector.Cross(v));
//  const Vec3 uuv(QuatVector.Cross(uv));
//
//  return v + ((uv * q.w) + uuv) * 2.0f;
//}
//
//Vector3 Vector3::Cross(const Vector3& r) const
//{
//  return Vector3(y*r.z - z*r.y, z*r.x - x*r.z, x*r.y - y*r.x);
//}
//
//Vector3 Vector3::ComponentProduct(const Vector3 &rhs) const
//{
//  return Vector3(x * rhs.x, y * rhs.y, rhs.z);
//}
//
//float Vector3::ScalarProduct(const Vector3 &vec) const
//{
//  return x*vec.x + y*vec.y + z*vec.z;
//}
//
//Vector3 Vector3::VectorProduct(const Vector3 &vec) const
//{
//  return Vector3(y*vec.z - z*vec.y,
//                 z*vec.x - x*vec.z,
//                 x*vec.y - y*vec.x);
//}
//
//std::stringstream &operator<<(std::stringstream &os, const Vector3& rhs)
//{
//  os << rhs.x << " " << rhs.y << " " << rhs.z;
//  return os;
//}
//
//std::stringstream &operator >> (std::stringstream &is, Vector3& rhs)
//{
//  is >> rhs.x >> rhs.y >> rhs.z;
//  return is;
//}
//
//std::ostream& operator<<(std::ostream& os, const Vector3& rhs)
//{
//  os << "< " << rhs.x << " : " << rhs.y << " : " << rhs.z << " >";
//  return os;
//}

////Default Constructor
//Vector4::Vector4() : x(0), y(0), z(0), w(0) {}
//
////Conversion Constructor
//Vector4::Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
//
////Copy Constructor
//Vector4::Vector4(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
//
//Vector4::Vector4(const Vector3& rhs, float W) : x(rhs.x), y(rhs.y), z(rhs.z), w(W) {}
//
//void Vector4::Set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
//
//float& Vector4::operator[](int index)
//{
//  return m[index];
//}
//
//const float& Vector4::operator[](int index) const
//{
//  return m[index];
//}
//
//bool Vector4::operator==(const Vector4& rhs)
//{
//  if (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w) return true;
//  return false;
//}
//
//bool Vector4::operator!=(const Vector4& rhs)
//{
//  if (operator==(rhs)) return false;
//  return true;
//}
//
//Vector4& Vector4::operator-()
//{
//  x = -x; y = -y; z = -z; w = -w;
//  return *this;
//}
//
//bool Vector4::operator==(const Vector4& rhs) const
//{
//  if (abs(x - rhs.x) > ASC_EPSILON ||
//      abs(y - rhs.y) > ASC_EPSILON ||
//      abs(z - rhs.z) > ASC_EPSILON ||
//      abs(w - rhs.w) > ASC_EPSILON) return false;
//  return true;
//}
//
//bool Vector4::operator!=(const Vector4& rhs) const
//{
//  return !(operator==(rhs));
//}
//
//Vector4 Vector4::operator+(const Vector4& rhs) const
//{
//  return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
//}
//
//Vector4 Vector4::operator-(const Vector4& rhs) const
//{
//  return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
//}
//
//Vector4 Vector4::operator/(float rhs) const
//{
//  return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
//}
//
//Vector4& Vector4::operator+=(const Vector4& rhs)
//{
//  x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
//  return *this;
//}
//
//Vector4& Vector4::operator-=(const Vector4& rhs)
//{
//  x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
//  return *this;
//}
//
//Vector4& Vector4::operator/=(float rhs)
//{
//  x /= rhs; y /= rhs; z /= rhs; w /= rhs;
//  return *this;
//}
//
//Vector4& Vector4::operator*=(float rhs)
//{
//  x *= rhs; y *= rhs; z *= rhs; w *= rhs;
//  return *this;
//}
//
//Vector4 Vector4::Normalize() const
//{
//  float mag = Magnitude();
//  return Vector4(x / mag, y / mag, z / mag, w / mag);
//}
//
//float Vector4::Magnitude() const
//{
//  return sqrt(MagnitudeSq());
//}
//
//float Vector4::MagnitudeSq() const
//{
//  return x*x + y*y + z*z + w*w;
//}
//
////Returns the length of the projected vector
//float Vector4::Dot(const Vector4& rhs) const
//{
//  return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
//}
//
//Vector4 operator*(const Vector4& lhs, float rhs)
//{
//  return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
//}
//
//Vector4 operator*(float lhs, const Vector4& rhs)
//{
//  return Vector4(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs);
//}
//
//std::ostream& operator<<(std::ostream& os, const Vector4& rhs)
//{
//  os << "< " << rhs.x << " : " << rhs.y << " : " << rhs.z << " : " << rhs.w << " >";
//  return os;
//}

///*-----------------------------------------------------------
//MATRIX 4x4
//-----------------------------------------------------------*/
//Matrix4::Matrix4()
//{
//  for (int i = 0; i < 4; ++i)
//    for (int y = 0; y < 4; ++y)
//      m[i][y] = 0;
//
//  m00 = m11 = m22 = m33 = 1;
//}
//
//Vector3 Matrix4::operator*(const Vector3 &rhs) const
//{
//  return Vector3(
//    rhs.x * m[0][0] +
//    rhs.y * m[0][1] +
//    rhs.z * m[0][2] + m[0][3],
//
//    rhs.x * m[1][0] +
//    rhs.y * m[1][1] +
//    rhs.z * m[1][2] + m[1][3],
//
//    rhs.x * m[2][0] +
//    rhs.y * m[2][1] +
//    rhs.z * m[2][2] + m[2][3]
//  );
//}
//
//Vector4 Matrix4::operator*(const Vector4& rhs) const
//{
//  Vector4 result{ 0, 0, 0, 0 };
//
//  for (int i = 0; i < 4; ++i)
//  {
//    // Traversing l row and r column
//    for (int j = 0; j < 4; ++j)
//    {
//      result[i] += m[j][i] * rhs[j];
//    }
//  }
//  return result;
//}
//
//Matrix4 Matrix4::operator*(const Matrix4& rhs) const
//{
//  Matrix4 temp;
//  for (int x = 0; x < 16; ++x)
//  {
//    float sum = 0;
//    int t = x % 4;
//    for (int y = 0; y < 4; ++y)
//      sum += m[(int)(x*0.25f)][y] * rhs.m[y][t];
//
//    temp.m[t][x - (t * 4)] = sum;
//  }
//  return temp;
//}
//
//Matrix4& Matrix4::operator*=(const Matrix4& rhs)
//{
//  return *this = (*this) * rhs;
//}
//
//Matrix4& Matrix4::operator=(const Matrix4& rhs)
//{
//  for (int i = 0; i < 4; ++i)
//    for (int y = 0; y < 4; ++y)
//      m[i][y] = rhs.m[i][y];
//  return *this;
//}
//
//Matrix4 Matrix4::Transpose() const
//{
//  Matrix4 out = *this;
//  for (int i = 0; i < 4; ++i)
//    for (int y = 0; y < 4; ++y)
//      out.m[y][i] = m[i][y];
//
//  return out;
//}
//
//Vec3 Matrix4::Transform(const Vec3 &point) const
//{
//  return (*this) * point;
//}
//
//Vec3 Matrix4::TransformInverse(const Vec3 &vector) const
//{
//  Vector3 tmp = vector;
//  tmp.x -= m[0][3];
//  tmp.y -= m[1][3];
//  tmp.z -= m[2][3];
//
//  return Vector3(
//    tmp.x * m[0][0] +
//    tmp.y * m[1][0] +
//    tmp.z * m[2][0],
//
//    tmp.x * m[0][1] +
//    tmp.y * m[1][1] +
//    tmp.z * m[2][1],
//
//    tmp.x * m[0][2] +
//    tmp.y * m[1][2] +
//    tmp.z * m[2][2]
//  );
//}
//
//void Matrix4::SetIdentity()
//{
//  for (int i = 0; i < 4; ++i)
//    for (int j = 0; j < 4; ++j)
//      m[i][j] = 0.0f;
//
//  m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
//}
//
//Vector3 Matrix4::GetAxisVector(int i) const
//{
//  int r = i / 4;
//  int c = i % 4;
//
//  return Vector3(m[r][c], m[r + 1][c], m[r + 2][c]);
//}
//
//Matrix4 Matrix4::Translate(const Vec3& pos)
//{
//  Matrix4 m;
//  m.m30 = pos.x;
//  m.m31 = pos.y;
//  m.m32 = pos.z;
//  return m;
//}
//
//Matrix4 Matrix4::ConcatModelMat(const Vector3 & pos, const Vector3 & scale, const Vector3 & axis, float angle_deg)
//{
//  Matrix4 scaleMat;
//  scaleMat.m00 = scale.x;
//  scaleMat.m11 = scale.y;
//  scaleMat.m22 = scale.z;
//  Matrix4 rotMat = Matrix4::Rotate(axis, angle_deg);
//
//  Matrix4 result = rotMat * scaleMat;
//  result.m30 = pos.x;
//  result.m31 = pos.y;
//  result.m32 = pos.z;
//  return result;
//}
//
//Matrix4 Matrix4::ConcatModelMat(const Vector3 & pos, const Vector3 & scale, const Quaternion & q)
//{
//  Matrix4 scaleMat;
//  scaleMat.m00 = scale.x;
//  scaleMat.m11 = scale.y;
//  scaleMat.m22 = scale.z;
//  Matrix4 rotMat = q.GetMatrix();
//
//  Matrix4 result = scaleMat * rotMat;
//  result.m30 = pos.x;
//  result.m31 = pos.y;
//  result.m32 = pos.z;
//  return result;
//}
//
//Matrix4 Matrix4::Rotate(const Vector3 & axis, float angle_deg)
//{
//  float rad = ASC_DEG2RAD(angle_deg);
//  float c = cosf(rad);
//  float s = sinf(rad);
//  float r = 1 - c;
//  Vec3 vec = axis.Normalize();
//  float xs = vec.x * s;
//  float ys = vec.y * s;
//  float zs = vec.z * s;
//
//  Matrix4 tmp;
//  tmp.m[0][0] = r * vec.x * vec.x + c;
//  tmp.m[0][1] = r * vec.x * vec.y - zs;
//  tmp.m[0][2] = r * vec.x * vec.z + ys;
//
//  tmp.m[1][0] = r * vec.y * vec.x + zs;
//  tmp.m[1][1] = r * vec.y * vec.y + c;
//  tmp.m[1][2] = r * vec.y * vec.z - xs;
//
//  tmp.m[2][0] = r * vec.z * vec.x - ys;
//  tmp.m[2][1] = r * vec.z * vec.y + xs;
//  tmp.m[2][2] = r * vec.z * vec.z + c;
//  return tmp;
//}
//
//Matrix4 Matrix4::SymPerspective(float fov, float aspect, float zNear, float zFar)
//{
//  Matrix4 tmp;
//  float tanHalfVerticalFov = tan(ASC_DEG2RAD(fov * 0.5f));
//  tmp.m00 = 1.0f / (aspect * tanHalfVerticalFov);
//  tmp.m11 = 1.0f / (tanHalfVerticalFov);
//  tmp.m22 = -(zFar + zNear) / (zFar - zNear);
//  tmp.m32 = -(2.0f * zFar * zNear) / (zFar - zNear);
//  tmp.m23 = -1.0f;
//  tmp.m33 = 0.0f;
//  return tmp;
//}
//
//Matrix4 Matrix4::SymOrthographic(float width, float height, float zNear, float zFar)
//{
//  float hWidth = width * 0.5f;
//  float hHeight = height * 0.5f;
//  return NonSymOrthographic(-hWidth, hWidth, hHeight, -hHeight, zNear, zFar);
//}
//
//Matrix4 Matrix4::NonSymOrthographic(float l, float r, float t, float b, float n, float f)
//{
//  Matrix4 tmp;
//
//  tmp.m00 =  2.0f / (r - l);
//  tmp.m11 =  2.0f / (t - b);
//  tmp.m22 = -2.0f / (f - n);
//  tmp.m33 = 1.0f;
//
//  tmp.m30 = -((r + l) / (r - l));
//  tmp.m31 = -((t + b) / (t - b));
//  tmp.m32 = -((f + n) / (f - n));
//
//  return tmp;
//}
//
//Matrix4 Matrix4::NonSymPerspective(float l, float r, float t, float b, float n, float f)
//{
//  Matrix4 tmp;
//  float rl = (r - l), tb = (t - b), fn = (f - n);
//  tmp.m00 = (2.0f * n) / rl; tmp.m02 = (r + l) / rl;
//  tmp.m11 = (2.0f * n) / tb; tmp.m12 = (t + b) / tb;
//  tmp.m22 = -(f + n) / fn; tmp.m23 = -(2.0f * f*n) / fn;
//  tmp.m32 = -1.0f;
//  tmp.m33 = 0.0f;
//  return tmp;
//}
//
//Matrix4 Matrix4::LookAt(const Vector3& pos, const Vector3& look, const Vector3& right)
//{
//  Matrix4 tmp;
//
//  Vec3 up = right.Cross(look).Normalize();
//
//  tmp.m00 = right.x; tmp.m10 = right.y; tmp.m20 = right.z;
//  tmp.m01 = up.x;    tmp.m11 = up.y;    tmp.m21 = up.z;
//  tmp.m02 = -look.x;  tmp.m12 = -look.y;  tmp.m22 = -look.z;
//
//  tmp.m30 = -right.Dot(pos);
//  tmp.m31 = -up.Dot(pos);
//  tmp.m32 = look.Dot(pos);
//
//  return tmp;
//}
//
//Vector3 Matrix3::Transform(Vector3 &vector3) const
//{
//  return (*this) * vector3;
//}
//
//void Matrix3::SetIdentity()
//{
//  for (int i = 0; i < 9; ++i)
//    data[i] = 0.0f;
//
//  data[0] = data[4] = data[8] = 1.0f;
//}
//
//Matrix3 Matrix3::linearInterpolate(const Matrix3 &a, const Matrix3 &b, float prop)
//{
//  Matrix3 result;
//  for (unsigned i = 0; i < 9; i++) {
//    result.data[i] = a.data[i] * (1 - prop) + b.data[i] * prop;
//  }
//
//  return result;
//}

//Quaternion& Quaternion::operator/=(float Scalar)
//{
//  x /= Scalar; y /= Scalar; z /= Scalar; w /= Scalar;
//  return *this;
//}
//
//Quaternion& Quaternion::operator*=(float Scalar)
//{
//  x *= Scalar; y *= Scalar; z *= Scalar; w *= Scalar;
//  return *this;
//}
//
//Quaternion& Quaternion::operator*=(const Quaternion& q)
//{
//#if 0
//  x = (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y);
//  y = (w * q.y) + (y * q.w) + (z * q.x) - (x * q.z);
//  z = (w * q.z) + (z * q.w) + (x * q.y) - (y * q.x);
//  w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
//#else
//  float qax = x, qay = y, qaz = z, qaw = w;
//  float qbx = q.x, qby = q.y, qbz = q.z, qbw = q.w;
//
//  x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
//  y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
//  z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
//  w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;
//#endif
//  return *this;
//}
//
//Quaternion& Quaternion::operator-=(const Quaternion& q)
//{
//  x -= q.x; y -= q.y; z -= q.z; w -= q.w;
//  return *this;
//}
//
//Quaternion & Quaternion::operator+=(const Vector3& vec)
//{
//  Quaternion q(vec.x, vec.y, vec.z, 0.0f);
//  q = (*this * q) * 0.5f;
//  x += q.x; y += q.y; z += q.z; w += 0.0f;
//  return *this;
//}
//
//bool Quaternion::operator==(const Quaternion &rhs) const
//{
//  return (fabs(x - rhs.x) < FLT_EPSILON &&
//          fabs(y - rhs.y) < FLT_EPSILON &&
//          fabs(z - rhs.z) < FLT_EPSILON &&
//          fabs(w - rhs.w) < FLT_EPSILON);
//}
//
//bool Quaternion::operator!=(const Quaternion &rhs) const
//{
//  return !(*this == rhs);
//}
//
//Quaternion operator*(const Quaternion & lhs, const Quaternion & rhs)
//{
//  Quaternion result(lhs);
//  return result *= rhs;
//}
//
//Quaternion operator*(float Scalar, const Quaternion & rhs)
//{
//  return Quaternion(rhs) *= Scalar;
//}
//
//Quaternion operator*(const Quaternion & lhs, float Scalar)
//{
//  return Quaternion(lhs) *= Scalar;
//}
//
//Quaternion operator-(const Quaternion & q)
//{
//  return Quaternion(-q.x, -q.y, -q.z, -q.w);
//}
//
//Quaternion operator-(const Quaternion & lhs, const Quaternion & rhs)
//{
//  return Quaternion(lhs) -= rhs;
//}
//
//void Quaternion::Set(float x_, float y_, float z_, float w_)
//{
//  x = x_; y = y_; z = z_; w = w_;
//}
//
//void Quaternion::SetIdentity()
//{
//  Set(0.0f, 0.0f, 0.0f, 1.0f);
//}
//
//Quaternion & Quaternion::SetRotationX(float deg)
//{
//  deg = ASC_DEG2RAD(deg * 0.5f);
//  float s = sinf(deg), c = cosf(deg);
//  *this *= Quaternion(s, 0, 0, c);
//  return *this;
//}
//
//Quaternion & Quaternion::SetRotationY(float deg)
//{
//  deg = ASC_DEG2RAD(deg * 0.5f);
//  float s = sinf(deg), c = cosf(deg);
//  *this *= Quaternion(0, s, 0, c);
//  return *this;
//}
//
//Quaternion & Quaternion::SetRotationZ(float deg)
//{
//  deg = ASC_DEG2RAD(deg * 0.5f);
//  float s = sinf(deg), c = cosf(deg);
//  *this *= Quaternion(0, 0, s, c);
//  return *this;
//}
//
//float Quaternion::Dot(const Quaternion &rhs) const
//{
//  return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
//}
//
//Quaternion Quaternion::Normalize() const
//{
//  Quaternion q(*this);
//  const float lenSq = Dot(*this);
//  if (lenSq < ASC_EPSILON) q.Set(0.0f, 0.0f, 0.0f, 1.0f);
//  else
//    q /= sqrtf(lenSq);
//  return q;
//}
//
//Quaternion Quaternion::Invert() const
//{
//  return (-Quaternion(*this));
//}
//
//float Quaternion::Length() const
//{
//  return sqrtf(LengthSq());
//}
//
//float Quaternion::LengthSq() const
//{
//  return this->Dot(*this);
//}
//
///*Vector3 Quaternion::GetEuler() const
//{
//  float sqw = w*w;
//  float sqx = x*x;
//  float sqy = y*y;
//  float sqz = z*z;
//  float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
//  float test = x*y + z*w;
//
//  Vec3 euler;
//  if (test > 0.499f*unit) { // singularity at north pole
//    euler.y = 2 * atan2(x, w);
//    euler.z = PI * 0.5f;
//    euler.x = 0;
//  }
//  else if (test < -0.499f*unit) { // singularity at south pole
//    euler.y = -2 * atan2(x, w);
//    euler.z = -PI * 0.5f;
//    euler.x = 0;
//  }
//  else
//  {
//    euler.y = atan2(2 * y*w - 2 * x*z, sqx - sqy - sqz + sqw);
//    euler.z = asin(2 * test / unit);
//    euler.x = atan2(2 * x*w - 2 * y*z, -sqx + sqy - sqz + sqw);
//  }
//
//  euler.x = ASC_RAD2DEG(euler.x);
//  euler.y = ASC_RAD2DEG(euler.y);
//  euler.z = ASC_RAD2DEG(euler.z);
//
//  return euler;
//}*/
//
//Vector3 Quaternion::GetAxis() const
//{
//  Vector3 Axis;
//  float s = sinf(acosf(ASC_RANGE(w, -1.0f, 1.0f)));
//
//  if (abs(s) < ASC_EPSILON)
//    Axis.Set(0.0f, 1.0f, 0.0f);
//  else
//  {
//    Axis.Set(x, y, z);
//    Axis /= s;
//  }
//  return Axis;
//}
//
//float Quaternion::GetAngle() const
//{
//  return ASC_RAD2DEG(acosf(ASC_RANGE(w, -1.0f, 1.0f)) * ASC_DEG2RAD(2.0f));
//}
//
//Matrix4 Quaternion::GetMatrix(const Vector3& center) const
//{
//  Quaternion q(*this);
//  Matrix4 dest;
//  dest.m00 = 1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z;
//  dest.m10 = 2.0f*q.x*q.y - 2.0f*q.z*q.w;
//  dest.m20 = 2.0f*q.x*q.z + 2.0f*q.y*q.w;
//  dest.m30 = 0.0f;
//
//  dest.m01 = 2.0f*q.x*q.y + 2.0f*q.z*q.w;
//  dest.m11 = 1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z;
//  dest.m21 = 2.0f*q.z*q.y - 2.0f*q.x*q.w;
//  dest.m31 = 0.0f;
//
//  dest.m02 = 2.0f*q.x*q.z - 2.0f*q.y*q.w;
//  dest.m12 = 2.0f*q.z*q.y + 2.0f*q.x*q.w;
//  dest.m22 = 1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y;
//  dest.m32 = 0.0f;
//
//  dest.m03 = center.x;
//  dest.m13 = center.y;
//  dest.m23 = center.z;
//  dest.m33 = 1.f;
//  return dest;
//}
//
//Quaternion Quaternion::LookAt(const Vec3 &pos, const Vec3 &direction, Vec3 up)
//{
//  // Find the rotation between the front of the object (that we assume towards +Z,
//  // but this depends on your model) and the desired direction
//  auto rot1 = RotationBetweenVectors(pos, direction);
//
//  // Recompute desiredUp so that it's perpendicular to the direction
//  // You can skip that part if you really want to force desiredUp
//  auto desiredUp = up;
//  auto newUp = rot1 * desiredUp;
//
//  auto right = direction.Cross(desiredUp);
//  desiredUp = right.Cross(direction);
//
//  // Because of the 1rst rotation, the up is probably completely screwed up.
//  // Find the rotation between the "up" of the rotated object, and the desired up
// 
//  auto rot2 = RotationBetweenVectors(newUp, desiredUp);
//  auto targetOrientation = rot2 * rot1; // remember, in reverse order.
//  return targetOrientation;
//}
//
//Quaternion EulerToQuaternion(const Vector3& rot)
//{
//  Quaternion q;
//  float t0 = cos(ASC_DEG2RAD(rot.z * 0.5f));
//  float t1 = sin(ASC_DEG2RAD(rot.z * 0.5f));
//  float t2 = cos(ASC_DEG2RAD(rot.x * 0.5f));
//  float t3 = sin(ASC_DEG2RAD(rot.x * 0.5f));
//  float t4 = cos(ASC_DEG2RAD(rot.y * 0.5f));
//  float t5 = sin(ASC_DEG2RAD(rot.y * 0.5f));
//
//  q.w = t0 * t2 * t4 + t1 * t3 * t5;
//  q.x = t0 * t3 * t4 - t1 * t2 * t5;
//  q.y = t0 * t2 * t5 + t1 * t3 * t4;
//  q.z = t1 * t2 * t4 - t0 * t3 * t5;
//  return q;
//}
//
//Matrix4 QuaternionToMatrix(const Quaternion & q)
//{
//  Matrix4 mat;
//  float x = q.x, y = q.y, z = q.z, w = q.w;
//  float x2 = x + x, y2 = y + y, z2 = z + z;
//  float xx = x * x2, xy = x * y2, xz = x * z2;
//  float yy = y * y2, yz = y * z2, zz = z * z2;
//  float wx = w * x2, wy = w * y2, wz = w * z2;
//
//  mat.m00 = 1 - (yy + zz);
//  mat.m10 = xy - wz;
//  mat.m20 = xz + wy;
//
//  mat.m01 = xy + wz;
//  mat.m11 = 1 - (xx + zz);
//  mat.m21 = yz - wx;
//
//  mat.m02 = xz - wy;
//  mat.m12 = yz + wx;
//  mat.m22 = 1 - (xx + yy);
//
//  // last column
//  mat.m03 = 0;
//  mat.m13 = 0;
//  mat.m23 = 0;
//
//  return mat;
//}
//
//Vector3 QuaternionToEuler(const Quaternion & q)
//{
//  Vector3 v;
//  float ysqr = q.y * q.y;
//
//  // roll (x-axis rotation)
//  float t0 = 2.0f * (q.w * q.x + q.y * q.z);
//  float t1 = 1.0f - 2.0f * (q.x * q.x + ysqr);
//  v.x = ASC_RAD2DEG(atan2(t0, t1));
//
//  // pitch (y-axis rotation)
//  float t2 = 2.0f * (q.w * q.y - q.z * q.x);
//  t2 = t2 > 1.0f ? 1.0f : t2;
//  t2 = t2 < -1.0f ? -1.0f : t2;
//  v.y = ASC_RAD2DEG(asin(t2));
//
//  // yaw (z-axis rotation)
//  float t3 = 2.0f * (q.w * q.z + q.x * q.y);
//  float t4 = 1.0f - 2.0f * (ysqr + q.z * q.z);
//  v.z = ASC_RAD2DEG(atan2(t3, t4));
//
//  return v;
//}
//
//Quaternion AxisAngleToQuat(const Vector3 & axis, float angle_deg)
//{
//  Vec3 nAxis = axis.Normalize();
//  float halfAngle = (ASC_DEG2RAD(angle_deg * 0.5f));
//  float s = sinf(halfAngle);
//  Quaternion q;
//  q.x = nAxis.x * s;
//  q.y = nAxis.y * s;
//  q.z = nAxis.z * s;
//  q.w = cosf(halfAngle);
//  return q;
//}
//
//Quaternion RotationBetweenVectors(Vec3 start, Vec3 dest) {
//  start = start.Normalize();
//  dest = dest.Normalize();
//
//  float cosTheta = start.Dot(dest);
//  Vec3 rotationAxis;
//
//  if (cosTheta < -1 + 0.001f) {
//    // special case when vectors in opposite directions:
//    // there is no "ideal" rotation axis
//    // So guess one; any will do as long as it's perpendicular to start
//    rotationAxis = start.Cross(Vec3::ZAXIS);// (0.0f, 0.0f, 1.0f));
//    if (rotationAxis.MagnitudeSq() < FLT_EPSILON) // bad luck, they were parallel, try again!
//      rotationAxis = start.Cross(Vec3::XAXIS);
//
//    rotationAxis = rotationAxis.Normalize();
//    return AxisAngleToQuat(rotationAxis, 180.0f);
//  }
//
//  rotationAxis = start.Cross(dest);
//
//  float s = sqrt((1 + cosTheta) * 2);
//  float invs = 1 / s;
//
//  return Quaternion(
//    s * 0.5f,
//    rotationAxis.x * invs,
//    rotationAxis.y * invs,
//    rotationAxis.z * invs
//  );
//
//}
//
//Quaternion QuaternionFromMatrix(Matrix4 const& m) {
//  // Adapted from: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
//  Quaternion q;
//  q.w = sqrtf(max(0, 1 + m.m00 + m.m11 + m.m22)) * 0.5f;
//  q.x = sqrtf(max(0, 1 + m.m00 - m.m11 - m.m22)) * 0.5f;
//  q.y = sqrtf(max(0, 1 - m.m00 + m.m11 - m.m22)) * 0.5f;
//  q.z = sqrtf(max(0, 1 - m.m00 - m.m11 + m.m22)) * 0.5f;
//
//  q.x *= ASC_SIGN(q.x * (m.m21 - m.m12));
//  q.y *= ASC_SIGN(q.y * (m.m02 - m.m20));
//  q.z *= ASC_SIGN(q.z * (m.m10 - m.m01));
//  return q;
//}
//
//Vec3 QuatToAxisAngle(const Quat& axis)
//{
//  return axis.GetAxis();
//}
//
//std::ostream & operator<<(std::ostream & os, const Quaternion & rhs)
//{
//  os << "< " << rhs.x << " : " << rhs.y << " : " << rhs.z << " : " << rhs.w << " >";
//  return os;
//}
