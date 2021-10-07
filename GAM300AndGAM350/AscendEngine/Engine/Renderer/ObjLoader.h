/******************************************************************************/
/*!
\file   ObjLoader.h
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
#ifndef OBJLOADER_H
#define OBJLOADER_H

//MeshBoundingSphere
struct Mbsphere
{
  Mbsphere(Vec3 pos = Vec3(), float radius = 1.0f) : m_pos(pos), m_r(radius) {}
  Vec3 m_pos;
	float m_r;
};

struct KeysData
{
    float time;
    Vec3 value;
    KeysData(float t, Vec3 v) :
        time(t),
        value(v)
    {

    }
};

struct AnimationChannels
{
    std::string name;
    int positionKeysSize=0;
    int rotationKeysSize=0;
    int scaleKeysSize=0;
    std::vector<KeysData> posKey;
    std::vector<KeysData> rotKey;
    std::vector<KeysData> scaKey;
    AnimationChannels(std::string channelname, int possize, int rotsize, int scasize) :
        name(channelname),
        positionKeysSize(possize),
        rotationKeysSize(rotsize),
        scaleKeysSize(scasize)
    {

    }
};

struct BoneWeightData
{
    BoneWeightData(int id, float w):
        vertexID(id),
        weight(w)
    {

    }
    int vertexID;
    float weight;
};
struct BoneWeight
{
    std::string name;
    int weightSize;
    Matrix4 transformMat;
    std::vector<BoneWeightData> boneWeights;
    BoneWeight(std::string n, int size) :
        name(n),
        weightSize(size)
    {

    }
};
struct BoneData
{
    std::string name;
    float duration;
    float ticksPerSecond;
    int boneSize=0;
    int counterBoneSize=-1;
    std::vector<BoneWeight> bones;
    int channelSize=0;
    int counterChannelSize = -1;
    std::vector<AnimationChannels> channels;

};
struct Vertex {
    Vector3 pos, nrm, uv, tan, biTan;
    int index;
    Vertex() : pos(0, 0, 0), nrm(0, 0, 0), uv(0, 0, 0),
        tan(0, 0, 0), biTan(0, 0, 0), index(0) {}
    Vertex(Vertex const& temp):
        pos(temp.pos),
        nrm(temp.nrm),
        uv(temp.uv),
        tan(temp.tan),
        biTan(temp.biTan),
        index(temp.index)
    {

    }
    Vertex* operator=(Vertex const& temp)
    {
        pos = temp.pos;
        nrm = temp.nrm;
        uv = temp.uv;
        tan = temp.tan;
        biTan = temp.biTan;
        index = temp.index;
        return this;
    }
};
struct Mesh
{
  unsigned int triCount;
  unsigned int vertCount;
  unsigned int vao;

  float glint; bool dirty;
  Vec3 ambient, diffuse, specular;
  Mbsphere m_bs;
  std::vector<Vertex> output;
  Vec3 minPos, maxPos;
  Vec3 size;
  Vec3 pivot;
  BoneData bone;
  bool isEmpty() { return (triCount == 0); }
  Mesh();
};

class ObjLoader
{
public:
  //LoadModel Usage : ( <name> , <filepath> )
  bool LoadModel(const std::string&, const std::string&);

  std::vector<Mesh>& operator[](const std::string&);

  //Result
  std::map<std::string, std::vector<Mesh>> m_objs;



  bool IsLoading() const { return m_loading; }
  void Update();

  

  struct MeshData
  {
      std::string name;

      //Position
      std::vector<Vector3> pos;

      //Face Normal
      std::vector<Vector3> normal;
      //UV
      std::vector<Vector3> uvs;
      //color
      std::vector<Vector3> color;
      //tangent
      std::vector<Vector3> tangent;
      //bitangent
      std::vector<Vector3> biTangent;

      //Faces
      std::vector<Vertex> output;

      std::string materialFile;

      Vec3 minPos, maxPos;
      Vec3 size;
      Vec3 pivot;
      BoneData bone;

      void Clear() {
          pos.clear();
          normal.clear();
          uvs.clear();
          color.clear();
          tangent.clear();
          biTangent.clear();
          output.clear();
      }
  };

  std::map<std::string, std::vector<MeshData>> m_models;
  void MeshDataClear(std::string const &);

private:
  

  
    Matrix4 inverseMat;
  Mesh m_empty;
  void ParseObj(std::stringstream&, MeshData&);
  void GenMesh(const std::string&, std::vector<MeshData>&);
  void GenBoundSphere(Mesh& m, MeshData& data);

  bool m_loading;
  void LoadThread(const std::string&, const std::string&);
  void CompleteCallback(const std::string&, std::vector<MeshData>&);


  std::map<std::string, std::string> m_complete;
  std::map<std::string, std::thread> m_threads;

};

#endif