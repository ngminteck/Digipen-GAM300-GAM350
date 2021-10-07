/******************************************************************************/
/*!
\file   Shape.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
enum ShapeType : int
{
  eSPHERE,
  eAABB,
  ePLANE,
  eNOCOLLIDER,
  eMAX
};

struct Shape
{
  virtual ~Shape();
  virtual ShapeType GetType() const = 0;
  virtual void DrawDebug() const;
  virtual void Init(size_t body_) { body = body_; }
  virtual bool Ray(const Vec3 &pos, const Vec3 &nDir, float &oldTime) = 0;
  size_t body;
};

struct Sphere : Shape
{
  explicit Sphere(float r = 0.5f);
  ShapeType GetType() const override;
  void DrawDebug() const override;
  bool Ray(const Vec3 &pos, const Vec3 &nDir, float &t) override;

  // For circle shape
  float radius;
};

struct AABB : Shape
{
  explicit AABB(Vec3 min = Vec3(-0.5f, -0.5f, -0.5f), Vec3 max = Vec3(0.5f, 0.5f, 0.5f));
  ShapeType GetType() const override;
  void DrawDebug() const override;
  bool Ray(const Vec3 &pos, const Vec3 &nDir, float &t) override;

  Vec3 min;
  Vec3 max;
};

struct Plane : Shape
{
  explicit Plane(Vec3 norm = Vec3::YAXIS, Vec3 v1 = Vec3::XAXIS, Vec3 v2 = Vec3::ZAXIS, Vec2 = Vec2(0.0f, 0.0f));
  ShapeType GetType() const override;
  void DrawDebug() const override;
  void Init(size_t body_) override;
  void GetCorners(Vec3(&corners)[4]) const;
  void GetCorners(Vec3(&corners)[4], Vec3 plpos) const;
  bool Ray(const Vec3 &pos, const Vec3 &nDir, float &t) override;

  // Normal and basis must be normalized
  Vec3 normal;
  Vec3 basis[2];
  Vec2 dimension;

  //void fromPoints(const Vec3& p0, const Vec3& p1, const Vec3& p2);
  //void fromPointAndNormal(const Vec3 &p, const Vec3 &n);
  //float dist(const Vec3& center) const;
  //Vec3 project(const Vec3& center) const;
  //float a, b, c, d;
};

struct Empty : Shape
{
  //explicit Empty();
  ShapeType GetType() const override;
  //void DrawDebug() const override;
  bool Ray(const Vec3 &pos, const Vec3 &nDir, float &t) override;
};