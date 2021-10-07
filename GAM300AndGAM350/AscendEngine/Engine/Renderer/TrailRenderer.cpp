/******************************************************************************/
/*!
  \file   TrailRender.cpp
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
#include "pch.h"
#include "TrailRenderer.h"
#include <gl/gl3w.h>

TrailRenderer::TrailRenderer()
  : drawCount(0), vao(0), normal(Vec3::XAXIS)
{
}

void TrailRenderer::AddTrail(const std::vector<Vec3> &trails, float steps)
{
  std::vector<Vec3> vtx;
  std::vector<Vec3> uv;
  GenerateMesh(trails, steps, vtx, uv);

  glGenVertexArrays(1, &vao);
  glGenBuffers(2, vbo);

  glBindVertexArray(vao);
  {
    // vtx
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vtx.size() * sizeof(Vec3), vtx.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // uv
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(Vec3), uv.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  }
  glBindVertexArray(0);
}

void TrailRenderer::ClearTrail()
{
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, &vao);
  drawCount = 0;
  vao = 0;
  vbo[0] = vbo[1] = 0;
}

TrailRenderer::~TrailRenderer()
{
  ClearTrail();
}

void TrailRenderer::Render() const
{
  glDisable(GL_CULL_FACE);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, drawCount);
  //glDrawElements(GL_TRIANGLE_STRIP, drawCount * 2, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glEnable(GL_CULL_FACE);
}

void TrailRenderer::GenerateMesh(const std::vector<Vec3> &trails, float spacing,
  std::vector<Vec3> &vtx, std::vector<Vec3> &uv)
{
  drawCount = 2 * static_cast<int>(trails.size() - 2) + 4;

  vtx.push_back(trails[0]);
  vtx.push_back(trails[0]);

  const auto uvSteps = static_cast<float>(trails.size());

  Vec3 prevNormal;
  Vec3 prevUp = Vec3::YAXIS;

  auto i = 0;
  auto currentNorm = Vec3();
  auto currentUp = prevUp;

  const auto tSize = trails.size();
  for (; i < tSize; ++i)
  {
    const auto &v0 = trails[i];
    const auto &v1 = trails[(i + 1) % tSize];
    const auto dir = v1 - v0;

    currentNorm = 0.2f * dir.Cross(currentUp).Normalize().Normalize() + prevNormal;
    currentUp   = currentNorm.Cross(dir).Normalize();

    vtx[2 * i + 0] -= currentNorm * 2;
    vtx[2 * i + 1] += currentNorm * 2;

    vtx.push_back(v1);
    vtx.push_back(v1);
    uv.push_back({ i / uvSteps,0,0 });
    uv.push_back({ i / uvSteps,1,0 });

    prevNormal = currentNorm;
    prevUp = currentUp;
  }

  vtx[2 * (i % tSize) + 0] -= currentNorm * 2;
  vtx[2 * (i % tSize) + 1] += currentNorm * 2;

  uv.push_back({ 1,0,0 });
  uv.push_back({ 1,1,0 });
}

Vector3 TrailRenderer::CalculateCubicBezierPoint(float t, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3)
{
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	Vector3 p = uuu * p0;
	p += 3 * uu * t * p1;
	p += 3 * u * tt * p2;
	p += ttt * p3;

	return p;
}
