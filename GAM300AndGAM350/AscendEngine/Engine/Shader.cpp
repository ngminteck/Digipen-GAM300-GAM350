/******************************************************************************/
/*!
\file   Shader.cpp
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

#include "pch.h"

Shader::Shader(const std::string& file, unsigned type) : m_count(nullptr)
{
  m_obj = glCreateShader(type);

  if (m_obj == 0)
  {
    LogError("Error : glCreateShader failed");
    return;
  }

  //set the source code
  const char* code = file.c_str();
  glShaderSource(m_obj, 1, (const GLchar**)&code, nullptr);
  glCompileShader(m_obj);

  //throw exception if compile error occurred
  GLint status;
  glGetShaderiv(m_obj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    int bufferSize;
    glGetShaderiv(m_obj, GL_INFO_LOG_LENGTH, &bufferSize);

    char *buffer = new char[bufferSize];
    glGetShaderInfoLog(m_obj, bufferSize, NULL, buffer);

    glDeleteShader(m_obj);
    m_obj = 0;

    LogError("Error :", file);
    LogError("Compile failure in ", type == GL_VERTEX_SHADER ? "vertex" : "fragment", " shader\n", buffer);

    delete[]buffer;
    return;
  }

  m_count = new unsigned;
  *m_count = 1;
}

Shader::Shader(const Shader& rhs) : m_obj(rhs.m_obj), m_count(rhs.m_count) {
  if(m_count) ++(*m_count);
}

Shader::~Shader() {
  if (!m_count) return;

  --(*m_count);
  if ((*m_count) == 0 && m_obj != 0) {
    glDeleteShader(m_obj);
    delete m_count;
  }
}

Shader Shader::Load(const std::string& file, unsigned type)
{
  std::ifstream fstream;
  fstream.open(file.c_str(), std::ios::in | std::ios::binary);
  if (!fstream.is_open())
  {
    //std::cout << "Error : Failed to open file : " << file << std::endl;
    std::string tmp = std::string("Failed to open file: ") + file;
    throw std::runtime_error(tmp);
  }

  //read whole file into stringstream buffer
  std::stringstream buffer;
  buffer << fstream.rdbuf();

  Shader temp(buffer.str(), type);
  return temp;
}

ShaderProgram::ShaderProgram(const std::vector<Shader>& rhs)
{
  m_prog = glCreateProgram();
  if (m_prog == 0) {
    LogError("Error : glCreateProgram failed");
    return;
  }

  //attach all shaders 
  for (unsigned i = 0; i < rhs.size(); ++i)
    glAttachShader(m_prog, rhs[i].Object());

  //Link all shaders together
  glLinkProgram(m_prog);

  //detach all shaders
  for (unsigned i = 0; i < rhs.size(); ++i)
    glDetachShader(m_prog, rhs[i].Object());

  //throw exception if linking failed
  GLint status;
  glGetProgramiv(m_prog, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    glDeleteProgram(m_prog);
    m_prog = 0;
    LogError("Error : Shader Program linking failed");
    return;
  }
}

ShaderProgram::~ShaderProgram()
{
  if (m_prog != 0) glDeleteProgram(m_prog);
}

int ShaderProgram::GetAttribute(const char* name) const
{
  if (!name){
    LogError("Error : attribName was NULL");
    return -1;
  }

  GLint attrib = glGetAttribLocation(m_prog, name);

  if (attrib == -1){
    LogError("Error : Program attribute not found: ", name);
  }

  return attrib;
}

int ShaderProgram::GetUniform(const char* name) const
{
  if (!name) {
    LogError("Error : uniformName was NULL");
    return -1;
  }

  GLint uniform = glGetUniformLocation(m_prog, name);
  //if (uniform == -1)
  //std::cout << "Error : Program uniform not found: " << name << std::endl;

  return uniform;
}

void ShaderProgram::BindShader() const
{
  glUseProgram(m_prog);
}

void ShaderProgram::UnbindShader()
{
  glUseProgram(0);
}

void ShaderProgram::SetAttrib(const char* name, float v)
{
  GLint uID = GetAttribute(name);
  if (uID < 0) return;
  glVertexAttrib1f(uID, v);
}

void ShaderProgram::SetAttrib(const char* name, const Vector4& v)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glVertexAttrib4f(uID, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SetUniform(const char* name, int i)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniform1i(uID, i);
}

void ShaderProgram::SetUniform(const char* name, unsigned int i)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniform1ui(uID, i);
}

void ShaderProgram::SetUniform(const char* name, float i)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniform1f(uID, i);
}

void ShaderProgram::SetUniform(const char * name, const unsigned(&m)[3])
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniform3ui(uID, m[0], m[1], m[2]);
}

void ShaderProgram::SetUniform(const char * name, const Vector2& v)
{
	GLint uID = GetUniform(name);
	if (uID < 0) return;
	glUniform2f(uID, v.x, v.y);
}

void ShaderProgram::SetUniform(const char* name, const Vector3& v)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniform3f(uID, v.x, v.y, v.z);
}

void ShaderProgram::SetUniform(const char* name, const Vector4& v)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniform4f(uID, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SetUniform(const char* name, const Matrix4& mat4, bool transpose)
{
  GLint uID = GetUniform(name);
  if (uID < 0) return;
  glUniformMatrix4fv(uID, 1, transpose, &mat4.m[0][0]);
}

void ShaderProgram::SetUniformTexture(const char * name, int textureID)
{
  TextureHandle::iterator itr = uniqueTextures.find(name);
  int index = static_cast<int>(uniqueTextures.size());

  if (itr == uniqueTextures.end())
    uniqueTextures[name] = index;
  else index = itr->second;

  SetUniform(name, index);
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, textureID);
}
