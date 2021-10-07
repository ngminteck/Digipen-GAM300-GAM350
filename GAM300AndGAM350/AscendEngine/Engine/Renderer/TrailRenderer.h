/******************************************************************************/
/*!
  \file   TrailRender.h
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

#define ASC_TRAIL TrailRenderer::Inst()
class TrailRenderer : public Singleton<TrailRenderer>
{
public:
  TrailRenderer();

  void AddTrail(const std::vector<Vec3> &trails, float spacing);
  void ClearTrail();
  ~TrailRenderer();
  void Render() const;

private:
  int drawCount;
  GLuint vao;
  GLuint vbo[2];
  Vec3 normal;

  void GenerateMesh(const std::vector<Vec3> &trails, float spacing, 
	  std::vector<Vec3> &vtx, std::vector<Vec3> &uv);

  Vec3 CalculateCubicBezierPoint(float t, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3);
};


