/******************************************************************************/
/*!
\file   GLRenderer.cpp
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
#include "pch.h"
#include <excpt.h>

LineRenderer::LineRenderer(bool& debugDraw) : lineVBO(0), lineVAO(0),
m_currlineNumber(0), isDebugDraw(debugDraw)
{
}

LineRenderer::~LineRenderer()
{
  glDeleteBuffers(1, &lineVBO);
  glDeleteVertexArrays(1, &lineVAO);
}

void LineRenderer::InitGL()
{
  glGenVertexArrays(1, &lineVAO);
  glBindVertexArray(lineVAO);

  glGenBuffers(1, &lineVBO);
  glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
  /* Copy vertex attributes to GPU */
  glBufferData(GL_ARRAY_BUFFER, Vec3Size * MAX_LINES, nullptr, GL_STREAM_DRAW);

  /* Binding Position */
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vec3Size, nullptr);

  glGenBuffers(1, &colorVBO);
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  /* Copy Color attributes to GPU */
  glBufferData(GL_ARRAY_BUFFER, Vec4Size * (MAX_LINES), nullptr, GL_STREAM_DRAW);

  /* Binding Color */
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, Vec4Size, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  SetLineWidth(1.0f);
}

void LineRenderer::AddLine(const Vec3 & start, const Vec3 & end, const Vec4 & Color)
{
  AddLine(start, end, Color, Color);
}

void LineRenderer::AddLine(const Vec3 & start, const Vec3 & end, const Vec4 & startCol, const Vec4 & endCol)
{
  if (!isDebugDraw) return;

  unsigned int idx = m_currlineNumber % MAX_LINES;

  m_currlineNumber += 2;

  if (m_currlineNumber > m_lines.size() * MAX_LINES)
  {
    m_lines.push_back(std::array<Vector3, MAX_LINES>());
    m_colors.push_back(std::array<Vector4, MAX_LINES>());
  }
  m_lines.back()[idx + 0] = start;
  m_lines.back()[idx + 1] = end;

  m_colors.back()[idx + 0] = startCol;
  m_colors.back()[idx + 1] = endCol;
}

void LineRenderer::AddCube(const Vec3 & pos, const Vec3& min, const Vec3& max, const Vec4 & color)
{
  if (!isDebugDraw) return;

  //TODO: For aabb for now
  Vec3 arr[8];
  arr[0] = Vec3(min.x, min.y, min.z);
  arr[1] = Vec3(max.x, min.y, min.z);
  arr[2] = Vec3(min.x, max.y, min.z);
  arr[3] = Vec3(max.x, max.y, min.z);
  arr[4] = Vec3(min.x, min.y, max.z);
  arr[5] = Vec3(max.x, min.y, max.z);
  arr[6] = Vec3(min.x, max.y, max.z);
  arr[7] = Vec3(max.x, max.y, max.z);
  for (auto &elem : arr)
    elem += pos;

  AddLine(arr[0], arr[1], color);
  AddLine(arr[2], arr[3], color);
  AddLine(arr[0], arr[2], color);
  AddLine(arr[1], arr[3], color);

  AddLine(arr[4], arr[5], color);
  AddLine(arr[6], arr[7], color);
  AddLine(arr[4], arr[6], color);
  AddLine(arr[5], arr[7], color);

  AddLine(arr[2], arr[6], color);
  AddLine(arr[3], arr[7], color);
  AddLine(arr[0], arr[4], color);
  AddLine(arr[1], arr[5], color);
}

void LineRenderer::AddSphere(const Vec3 & pos, float radius, const Vec4& Color)
{
  if (!isDebugDraw) return;

  static const int lats = 10;
  static const int longs = lats;

  for (int i = 0; i <= lats; ++i)
  {
    float lat0 = ASC_PI * (-0.5f + static_cast<float>(i - 1) / lats);
    float y0 = sin(lat0) * radius;
    float yr0 = cos(lat0);

    float lat1 = ASC_PI * (-0.5f + static_cast<float>(i) / lats);
    float y1 = sin(lat1) * radius;
    float yr1 = cos(lat1);

    for (int j = 0; j <= longs; ++j)
    {
      float lng = 2.0f * ASC_PI * static_cast<float>(j - 1) / longs;
      float x = cos(lng);
      float z = sin(lng);

      AddLine(Vec3(pos.x + x * yr0 * radius, pos.y + y0, pos.z + z * yr0 * radius),
        Vec3(pos.x + x * yr1 * radius, pos.y + y1, pos.z + z * yr1 * radius), Color);
    }
  }
}

void LineRenderer::SetLineWidth(float width)
{
  m_width = width;
  glLineWidth(width);
}

void LineRenderer::RenderLines()
{
  if (!m_currlineNumber || !isDebugDraw) return;

  glLineWidth(m_width);
  glBindVertexArray(lineVAO);

  auto lineItr = m_lines.begin();
  auto colorItr = m_colors.begin();

  while (lineItr != m_lines.end())
  {
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vec3Size * MAX_LINES, &(*lineItr)[0]);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Vec4Size * MAX_LINES, &(*colorItr)[0]);

    glDrawArrays(GL_LINES, 0, MAX_LINES);

    ++lineItr;
    ++colorItr;
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void LineRenderer::ClearLines()
{
  m_currlineNumber = 0;
  m_lines.clear();
  m_colors.clear();
}