/******************************************************************************/
/*!
\file   TextureLoader.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

#ifndef GL_TEXTURE_LOADER_H
#define GL_TEXTURE_LOADER_H

const unsigned long FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'));
const unsigned long FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'));
const unsigned long FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'));

struct TextureData
{
  enum CHANNEL { INVALID = 0, GRAYSCALE = 1, RGB = 3, RGBA = 4, DEPTH = 5 };
  unsigned int handle;
  int width, height;
  float aspRatio;
  CHANNEL numChannel;
};

#define TEXTURELOADER TextureLoader::Inst()
class TextureLoader : public Singleton<TextureLoader>
{
public:
  TextureLoader();
  ~TextureLoader();

  void Load(const std::string& name, const std::string& path);
  /*
  // Load in order for skybox
  // +X (right)
  // -X (left)
  // +Y (top)
  // -Y (bottom)
  // +Z (front)
  // -Z (back)
  */
  void LoadDDS(FILE* fp, std::string name, GLenum textureType = GL_TEXTURE_2D);
  unsigned int LoadCubeMap(const std::string& dir, const std::vector<std::string>&);
  TextureData GenFrameBuffer(const TextureData::CHANNEL&, unsigned int attachment, unsigned int = GL_FLOAT);

  TextureData* operator[](const std::string&);
private:
  typedef std::map<std::string, TextureData> TextureMap;
  TextureMap textures;
};

#endif