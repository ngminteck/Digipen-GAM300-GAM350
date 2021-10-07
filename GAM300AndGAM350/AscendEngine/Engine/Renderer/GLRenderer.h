/******************************************************************************/
/*!
\file   GLRenderer.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definition of LineRenderer

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef GLRENDERER_H
#define GLRENDERER_H

#define ASC_LINERENDER LineRenderer::Inst()
class LineRenderer : public Singleton<LineRenderer>
{
public:
  LineRenderer(bool&);
  ~LineRenderer();
  void InitGL();

  void AddLine(const Vec3& start, const Vec3& end, const Vec4& Color = Vec4(1.0f, 0.0f, 0.0f,1.0f));
  void AddLine(const Vec3& start, const Vec3& end, const Vec4& startCol, const Vec4& endCol);
  void AddCube(const Vec3& pos, const Vec3& min, const Vec3& max, const Vec4 & color);
  void AddSphere(const Vec3& pos, float radius, const Vec4& Color = Vec4(1.0f, 0.0f, 0.0f, 1.0f));

  void SetLineWidth(float);
  void RenderLines();
  void ClearLines();

private:
  const static unsigned int MAX_LINES = 2000;

  std::list<std::array<Vector3, MAX_LINES>> m_lines;
  std::list<std::array<Vector4, MAX_LINES>> m_colors;
  GLuint lineVBO, lineVAO;
  GLuint colorVBO;

  unsigned int m_currlineNumber;
  bool& isDebugDraw;
  float m_width;
};

#endif