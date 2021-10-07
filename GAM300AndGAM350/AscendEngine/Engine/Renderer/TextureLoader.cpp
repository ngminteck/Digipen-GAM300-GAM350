/******************************************************************************/
/*!
\file   TextureLoader.cpp
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
#include "pch.h"

TextureLoader::TextureLoader()
{
}

TextureLoader::~TextureLoader()
{
  for (auto &elem : textures)
    glDeleteTextures(1, &elem.second.handle);
}

/*void TextureLoader::Load(const std::string& name, const std::string& path)
{
  TextureMap::iterator itr = textures.find(name);
  if (itr != textures.end()) return;

  int x = 0, y = 0, channels = 0;
  unsigned char * img = nullptr;
  unsigned int internalChannel = 0;
  unsigned int colChannel = 0;

  img = SOIL_load_image(path.c_str(), &x, &y, &channels, SOIL_LOAD_AUTO);

  if (!img) {
    Log("Load Texture : ", name, "failed");
    SOIL_free_image_data(img);
    return;
  }

  TextureData newTexture;
  glGenTextures(1, &newTexture.handle);
  glBindTexture(GL_TEXTURE_2D, newTexture.handle);
  switch (channels)
  {
    case TextureData::CHANNEL::RGB: 
      internalChannel = GL_RGB;
      colChannel = GL_RGB;
      break;
    case TextureData::CHANNEL::RGBA: 
      internalChannel = GL_SRGB_ALPHA;
      colChannel = GL_RGBA;
      break;
    case TextureData::CHANNEL::GRAYSCALE: 
      internalChannel = GL_R32F; 
      colChannel = GL_R32F;
      break;
    default : 
      internalChannel = GL_R32F; 
      colChannel = GL_R32F;
      break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, internalChannel, x, y, 0, colChannel, GL_UNSIGNED_BYTE, img);

  glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  SOIL_free_image_data(img);

  newTexture.width = x;
  newTexture.height = y;
  newTexture.aspRatio = (float)x / (float)y;
  newTexture.numChannel = (TextureData::CHANNEL)channels;

  textures[name] = newTexture;
}*/
void TextureLoader::Load(const std::string& name, const std::string& path)
{
    TextureMap::iterator itr = textures.find(name);
    if (itr != textures.end())
        return;
    FILE* fp = 0;
    errno_t temp = fopen_s(&fp, path.c_str(), "rb");
    /* try to open the file */
    if (temp != 0)
        return;
    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0)
    {
        TextureData newTexture;
        int x = 0, y = 0, channels = 0;
        unsigned char* image = nullptr;
        unsigned int internalChannel = 0;
        unsigned int colChannel = 0;
        //Load using soil
        image = SOIL_load_image(path.c_str(), &x, &y, &channels, SOIL_LOAD_AUTO);

        if (!image) {
            Log("Load Texture : ", name, "failed");
            SOIL_free_image_data(image);
            return;
        }
        glGenTextures(1, &newTexture.handle);
        glBindTexture(GL_TEXTURE_2D, newTexture.handle);

        switch (channels)
        {
        case TextureData::CHANNEL::RGB:
            internalChannel = GL_RGB;
            colChannel = GL_RGB;
            break;
        case TextureData::CHANNEL::RGBA:
            internalChannel = GL_SRGB_ALPHA;
            colChannel = GL_RGBA;
            break;
        case TextureData::CHANNEL::GRAYSCALE:
            internalChannel = GL_R32F;
            colChannel = GL_R32F;
            break;
        default:
            internalChannel = GL_R32F;
            colChannel = GL_R32F;
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, internalChannel, x, y, 0, colChannel, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        SOIL_free_image_data(image);
        newTexture.width = x;
        newTexture.height = y;
        newTexture.aspRatio = (float)x / (float)y;
        newTexture.numChannel = (TextureData::CHANNEL)channels;

        textures[name] = newTexture;
    }
    else
    {
        LoadDDS(fp, name);
    }
 }

void TextureLoader::LoadDDS(FILE* fp, std::string name, GLenum textureType)
{
    //load dds

    TextureData newTexture;
    unsigned char header[124];
    fread(&header, 124, 1, fp);
    //load(fp, GL_RGBA, GL_UNSIGNED_BYTE, header);
    GLint internalFormat;
    int x = *(GLuint*)&header[8];
    int y = *(GLuint*)&header[12];
    int width = x;
    int height = y;
    GLuint linearSize = *(GLuint*)&header[16];
    GLuint mipMapCount = *(GLuint*)&header[24];
    GLuint fourCC = *(GLuint*)&header[80];

    GLuint imageSize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    unsigned char* image = (unsigned char*)malloc(imageSize * sizeof(unsigned char));
    fread(image, 1, imageSize, fp);
    fclose(fp);
    //unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        free(image);
        return;
    }
    // Create Texture 
    glGenTextures(1, &newTexture.handle);
    glBindTexture(textureType, newTexture.handle);


    unsigned int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(textureType, level, internalFormat, width, height, 0, size, image + offset);

        offset += size;
        width /= 2;
        height /= 2;
    }

    glGenerateMipmap(textureType);  //Generate num_mipmaps number of mipmaps here.
    glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(textureType, 0);

    free(image);
    newTexture.width = x;
    newTexture.height = y;
    newTexture.aspRatio = (float)x / (float)y;
    newTexture.numChannel = TextureData::CHANNEL::RGBA;

    textures[name] = newTexture;
}

unsigned int TextureLoader::LoadCubeMap(const std::string& dir, const std::vector<std::string>& faces)
{
  GLuint textureID;
  glGenTextures(1, &textureID);

  bool failed = false;
  int width, height;
  unsigned char* image;

  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  for (GLuint i = 0; i < faces.size(); i++)
  {
    std::string path = dir + faces[i];
    image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    if (image == nullptr)
    {
      Log("Failed to load texture : ", path);
      failed = true;
    }
    else
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    SOIL_free_image_data(image);
  }

  if (!failed)
  {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return failed ? -1 : textureID;
}

TextureData* TextureLoader::operator[](const std::string & key)
{
  TextureMap::iterator itr = textures.find(key);
  return itr != textures.end() ? &itr->second : &textures["Blank"];
}

TextureData TextureLoader::GenFrameBuffer(const TextureData::CHANNEL& c, unsigned int attachment, unsigned int type)
{
  unsigned int iFormat = 0, format = 0;
  TextureData fbo;
  fbo.width = ASC_SYSTEM.fbWidth; fbo.height = ASC_SYSTEM.fbHeight;
  fbo.numChannel = (TextureData::CHANNEL)c;
  glGenTextures(1, &fbo.handle);
  glBindTexture(GL_TEXTURE_2D, fbo.handle);

  switch (c)
  {
  case TextureData::CHANNEL::RGB: iFormat = GL_RGB32F; break;
  case TextureData::CHANNEL::RGBA: iFormat = GL_RGBA32F; break;
  case TextureData::CHANNEL::GRAYSCALE: iFormat = GL_R32F; break;
  case TextureData::CHANNEL::DEPTH: iFormat = GL_DEPTH_COMPONENT32F; break;
  default: iFormat = GL_R32F; break;
  }
  
  glTexStorage2D(GL_TEXTURE_2D, 0, iFormat, fbo.width, fbo.height);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

  return fbo;
}
