/******************************************************************************/
/*!
\file   Shader.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for Shader

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef SHADER_H
#define SHADER_H

union Vector3;
union Vector4;
union Matrix4;

class Shader
{
public:
  Shader(const std::string&, unsigned);
  Shader(const Shader&);
  ~Shader();

  static Shader Load(const std::string&, unsigned);

  unsigned Object() const { return m_obj; }

private:
  unsigned m_obj;
  unsigned *m_count;
};

class ShaderProgram
{
public:
  ShaderProgram(const std::vector<Shader>&);
  ~ShaderProgram();

  unsigned Object() const { return m_prog; }

  int GetAttribute(const char*) const;
  int GetUniform(const char*) const;

  void BindShader() const;
  static void UnbindShader();

  void SetAttrib(const char*, float);
  void SetAttrib(const char*, const Vector4&);

  void SetUniform(const char*, int);
  void SetUniform(const char*, unsigned int);
  void SetUniform(const char*, float);

  void SetUniform(const char*, const unsigned(&)[3]);

  void SetUniform(const char*, const Vector2&);
  void SetUniform(const char*, const Vector3&);
  void SetUniform(const char*, const Vector4&);
  void SetUniform(const char*, const Matrix4&, bool = false);

  void SetUniformTexture(const char*, int textureID);

private:
  unsigned m_prog;

  typedef std::map<std::string, int> TextureHandle;
  TextureHandle uniqueTextures;
};


#endif