/******************************************************************************/
/*!
\file   ObjLoader.cpp
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

// For synchronisation
std::mutex g_mutex;

Mesh::Mesh() : triCount(0), vao(0), glint(75.0f), dirty(false),
ambient(ASC_RAND_COL * 0.5f, ASC_RAND_COL * 0.5f, ASC_RAND_COL * 0.5f), diffuse(ASC_RAND_COL, ASC_RAND_COL, ASC_RAND_COL),
specular(1.0f, 1.0f, 1.0f)
{
}

void replace(std::string& str, char key, char value)
{
  std::replace(str.begin(), str.end(), key, value);
}

std::vector<Mesh>& ObjLoader::operator[](const std::string& name)
{
  if (m_objs.find(name) != m_objs.end())
    return m_objs[name];
  std::vector<Mesh> temp;
  temp.push_back(m_empty);
  m_objs["empty"] = temp;
  return m_objs["empty"];
}

//#define FIND_IN_CONT(cont,value) (std::find(cont.begin(), cont.end(),value) != cont.end())
bool ObjLoader::LoadModel(const std::string& name, const std::string& filepath)
{
  if (m_objs.find(name) != m_objs.end() || m_threads.find(name) != m_threads.end()) {
    return false;
  }

  m_loading = true;
  m_threads[name] = std::thread(&ObjLoader::LoadThread, this, name, filepath);
  return true;
}

void ObjLoader::ParseObj(std::stringstream& line, MeshData& data)
{
  std::string placeholder;
  line >> placeholder;
  Vec3 temp;

 

  //vertex point pos
  if (placeholder == "V")
  {
    line >> temp.x >> temp.y >> temp.z;

    data.pos.push_back(temp);

	data.minPos.x = min(data.minPos.x, temp.x);
	data.minPos.y = min(data.minPos.y, temp.y);
	data.minPos.z = min(data.minPos.z, temp.z);

	data.maxPos.x = max(data.maxPos.x, temp.x);
	data.maxPos.y = max(data.maxPos.y, temp.y);
	data.maxPos.z = max(data.maxPos.z, temp.z);

  }
  //model color
  else if (placeholder == "C")
  {
    line >> temp.x >> temp.y >> temp.z;
    data.color.push_back(temp);
  }
  //model normal
  else if (placeholder == "N")
  {
    line >> temp.x >> temp.y >> temp.z;
    data.normal.push_back(temp);

  }
  //texture coordinates
  else if (placeholder == "TC")
  {
	  line >> temp.x >> temp.y >> temp.z;
	  data.uvs.push_back(temp);

  }
  //tangent
  else if (placeholder == "TAN")
  {
	  line >> temp.x >> temp.y >> temp.z;
	  data.tangent.push_back(temp);
  }
  //bitangent
  else if (placeholder == "BITAN")
  {
	  line >> temp.x >> temp.y >> temp.z;
	  data.biTangent.push_back(temp);
  }
  //faces
  else if (placeholder == "F")
  {
    Vertex tempV;
    std::string value;

    //F 0 1 2
    while (line >> value)
    {
      Vertex tempV;
      tempV.index = std::atoi(value.c_str());
      tempV.pos = data.pos[tempV.index];

      if (!data.uvs.empty())
        tempV.uv = data.uvs[tempV.index];

      if (!data.normal.empty())
        tempV.nrm = data.normal[tempV.index];

      data.output.push_back(tempV);
    }
  }
  else if (placeholder == "MAT")
  {
	  //save material name
  }
  else if (placeholder == "ambient")
  {
	  //ambient data
  }
  else if (placeholder == "diffuse")
  {
	  //diffuse data
  }
  else if (placeholder == "specular")
  {
	  //specular data
  }
  else if (placeholder == "AmbientTexture")
  {
	  //ambient Texture data
  }
  else if (placeholder == "DiffuseTexture")
  {
	  //diffuse Texture data
  }
  else if (placeholder == "SpecularTexture")
  {
	  //specular Texture data
  }
  else if (placeholder == "InverseMat")
  {
      line >> inverseMat.m00 >> inverseMat.m01 >> inverseMat.m02 >> inverseMat.m03;
      line >> inverseMat.m10 >> inverseMat.m11 >> inverseMat.m12 >> inverseMat.m13;
      line >> inverseMat.m20 >> inverseMat.m21 >> inverseMat.m22 >> inverseMat.m23;
      line >> inverseMat.m30 >> inverseMat.m31 >> inverseMat.m32 >> inverseMat.m33;
     
  }
  else if (placeholder == "Animation")
  {
	  //name
      std::string name;
      line >> name;
      data.bone.name = name;
  }
  else if (placeholder == "Duration")
  {
    float value = 0.0f;
    line >> value;
    data.bone.duration = value;
  }
  else if (placeholder == "Ticks")
  {
    float value= 0.0f;
    line >> value;
    data.bone.ticksPerSecond = value;
 }
  else if (placeholder == "Bone")
  {
    int value=0;
    line >> value;
    data.bone.boneSize = value;
  }
  else if (placeholder == "bName")
  {
      if (data.bone.counterBoneSize < data.bone.boneSize)
      {
          std::string name;
          int noOfweight=0;
          line >> name >> noOfweight;
          BoneWeight boneW(name, noOfweight);
          data.bone.bones.push_back(boneW);
          ++data.bone.counterBoneSize;
      }

 }
  else if (placeholder == "bID")
  {
  int id=0;
  float w=0.0f;
  line >> id >> w;
  if (data.bone.counterBoneSize < data.bone.bones.size())
  {
      BoneWeightData bonedata(id, w);
      data.bone.bones[data.bone.counterBoneSize].boneWeights.push_back(bonedata);
  }
 }
  else if (placeholder == "bMatOffset")
  {
  Mat4 mat;
  line >> mat.m00 >> mat.m01 >> mat.m02 >> mat.m03;
  line >> mat.m10 >> mat.m11 >> mat.m12 >> mat.m13;
  line >> mat.m20 >> mat.m21 >> mat.m22 >> mat.m23;
  line >> mat.m30 >> mat.m31 >> mat.m32 >> mat.m33;
  if (data.bone.counterBoneSize < data.bone.bones.size())
  {
      data.bone.bones[data.bone.counterBoneSize].transformMat = mat;
  }
 }
  else if (placeholder == "Channels")
  {
  int size = 0;
  line >> size;
  data.bone.channelSize = size;
 }
  else if (placeholder == "cName")
  {
      if (data.bone.counterChannelSize < data.bone.channelSize)
      {
          //name and number of position keys, rot keys, scale keys
          std::string name;
          int posKeys = 0, rotKeys = 0, scaKeys = 0;
          line >> name >> posKeys >> rotKeys >> scaKeys;
          AnimationChannels AniChannel(name, posKeys, rotKeys, scaKeys);
          data.bone.channels.push_back(AniChannel);
          ++data.bone.counterChannelSize;
      }
 }
  else if (placeholder == "cpTime")
  {
    //position keys
    float time = 0.0f;
    Vec3 value;
    line >> time >> value.x >> value.y >> value.z;
    if (data.bone.counterChannelSize < data.bone.channels.size())
    {
        KeysData keydata(time, value);
        data.bone.channels[data.bone.counterChannelSize].posKey.push_back(keydata);
    }
 }
  else if (placeholder == "crTime")
  {
  //rotation keys
  float time = 0.0f;
  Vec3 value;
  line >> time >> value.x >> value.y >> value.z;
  if (data.bone.counterChannelSize < data.bone.channels.size())
  {
      KeysData keydata(time, value);
      data.bone.channels[data.bone.counterChannelSize].rotKey.push_back(keydata);
  }
 }
  else if (placeholder == "csTime")
  {
  //scale keys
  float time = 0.0f;
  Vec3 value;
  line >> time >> value.x >> value.y >> value.z;
  if (data.bone.counterChannelSize < data.bone.channels.size())
  {
      KeysData keydata(time, value);
      data.bone.channels[data.bone.counterChannelSize].scaKey.push_back(keydata);
  }
 }
}

void ObjLoader::GenMesh(const std::string& name, std::vector<MeshData>& mesh)
{
  size_t size = sizeof(Vertex);
  std::vector<Mesh> meshList;
  for (size_t i = 0; i < mesh.size(); ++i)
  {
      Mesh tmp;
      tmp.triCount = (int)mesh[i].output.size();
      tmp.vertCount = (int)mesh[i].pos.size();
      tmp.minPos = mesh[i].minPos;
      tmp.maxPos = mesh[i].maxPos;
      tmp.size = mesh[i].size;
      tmp.pivot = mesh[i].pivot;
      for (size_t j = 0; j < mesh[i].output.size(); ++j)
          tmp.output.push_back(mesh[i].output[j]);
      tmp.bone = mesh[i].bone;
      /* Generate Mesh VAO */
      glGenVertexArrays(1, &tmp.vao);
      glBindVertexArray(tmp.vao);

      GLuint vertBuffer;
      glGenBuffers(1, &vertBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
      /* Copy vertex attributes to GPU */
      glBufferData(GL_ARRAY_BUFFER, mesh[i].output.size() * size, mesh[i].output.data(), GL_STATIC_DRAW);

      /* Binding Position */
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

      /* Binding Normal */
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));

      /* Binding UV */
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));

      /* Binding Tangent */
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(Vec3)));

      /* Binding Bitangent */
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(Vec3)));

      /* Binding BoneIDs */
      //glEnableVertexAttribArray(5);
      //glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(Vec3)));

      /* Binding BoneWeight */
     // glEnableVertexAttribArray(6);
      //glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(Vec3)));
      

      glBindVertexArray(0);

      //std::ofstream ofs("DEBUG_RITTER.txt", std::ofstream::app);
      //ofs << name << std::endl;
      //ofs.close();

      //if (name == "Quad" 
      //  || name == "Cube")
      //{
      //  int i = 0;
      //}
      GenBoundSphere(tmp, mesh[i]);
      meshList.push_back(tmp);
  }
  m_objs[name] = meshList;
}

void ObjLoader::GenBoundSphere(Mesh & m, MeshData & data)
{
  //std::string const nametest = data.name;
  //Log("testeetet: ", nametest);
  //if (std::string(nametest.begin(), nametest.begin() + 3) == "Sph")
  //  int dsaf = 0;
  std::vector<Vertex>& verts = data.output;
  //std::vector<Vertex>& verts = data.pos;
  unsigned NUM_POINTS = static_cast<unsigned>(verts.size());
  if (!NUM_POINTS) return;// (gfxSphere());

  typedef Vec3 gfxVector3;
  gfxVector3 tempVec;
  gfxVector3 xmin, xmax, ymin, ymax, zmin, zmax, dia1, dia2;

  xmin.x = ymin.y = zmin.z = FLT_MAX;// MAX_F;
  xmax.x = ymax.y = zmax.z = FLT_MIN;//MIN_F;
  //FIRST PASS : find 6 minima / maxima points
  for (const Vertex& elemVtx : verts)
  {
    Vec3 const& elem = elemVtx.pos;
    if (elem.x < xmin.x)
      xmin = elem; // New xminimum point
    if (elem.x > xmax.x)
      xmax = elem;
    if (elem.y < ymin.y)
      ymin = elem;
    if (elem.y > ymax.y)
      ymax = elem;
    if (elem.z < zmin.z)
      zmin = elem;
    if (elem.z > zmax.z)
      zmax = elem;
  }


  //Set xspan = distance between the 2 points xmin & xmax (squared)
  //Same for y & z spans
  gfxVector3 xspanVec(xmax - xmin);
  gfxVector3 yspanVec(ymax - ymin);
  gfxVector3 zspanVec(zmax - zmin);


  float xspan = xspanVec.MagnitudeSq();
  float yspan = yspanVec.MagnitudeSq();
  float zspan = zspanVec.MagnitudeSq();

  //Set points dia1 & dia2 to the maximally separated pair
  //assume xspan biggest
  dia1 = xmin; dia2 = xmax;
  float maxspan = xspan;

  if (yspan>maxspan)
  {
    maxspan = yspan;
    dia1 = ymin; dia2 = ymax;
  }
  if (zspan>maxspan)
  {
    dia1 = zmin; dia2 = zmax;
  }

  // dia1, dia2 is a diameter of initial sphere
  // calc initial center
  gfxVector3 cen = (dia1 + dia2);
  cen /= 2.f;
  Vec3 firstcen = cen;

  //calculate initial radius^2 and radius
  tempVec = dia2 - cen;
  float rad_sq = tempVec.MagnitudeSq();
  float rad = sqrt(rad_sq);

  //SECOND PASS : increment current sphere
  for (const Vertex& elemVtx : verts)
  {
    Vec3 const& elem = elemVtx.pos;
    tempVec = elem - cen;
    float ptdist = tempVec.MagnitudeSq();

    // r^2 test
    if (ptdist > rad_sq)
    {
      //this point is outside of current sphere!
      ptdist = sqrt(ptdist);
      //calc radius of new sphere 
      rad = (rad + ptdist) / 2.0f;
      // for next r^2 compare 
      rad_sq = rad * rad;
      float ptdist_new = ptdist - rad;
      // calculate center of new sphere 
      cen = (rad * cen + ptdist_new * elem);
      cen /= ptdist;
    }
  }

  Vec3 mycen((xmax.x + xmin.x) / 2, (ymax.y + ymin.y) / 2, (zmax.z + zmin.z) / 2);
  //std::ofstream ofs("DEBUG_RITTER.txt", std::ofstream::app);
  //ofs << mycen << "\n" << cen << std::endl;
  //ofs << mycen - cen << std::endl;
  cen = mycen;
  m.m_bs = Mbsphere(cen, rad);
  //return gfxSphere(cen.x, cen.y, cen.z, rad);
}

void ObjLoader::LoadThread(const std::string &name, const std::string &filepath)
{
  std::ifstream file;
  file.open(filepath);

  if (!file.is_open()) {
    LogError("Model Loading Failed :", filepath);
    return;
  }

  MeshData data;
  std::vector<MeshData> dataList;
  std::string buffer;
  std::string keyword;
  bool firstMesh = true;
  while (!file.eof())
  {
    std::getline(file, buffer);
    if (!buffer.size()) 
		continue;

    std::stringstream line(buffer);
	if (firstMesh)
	{
		if (buffer == "M")
		{
			firstMesh = false;
			continue;
		}
	}
	else		
	{
		if (buffer == "M")
		{
            data.size = data.maxPos - data.minPos;
            data.pivot = Vec3(-(data.size[0] / 2.0f + data.minPos[0]),
                -(data.size[1] / 2.0f + data.minPos[1]),
                -(data.size[2] / 2.0f + data.minPos[2]));
			dataList.push_back(data);
			data.Clear();
			continue;
		}
		
	}
    ParseObj(line, data);
  }
  data.size = data.maxPos - data.minPos;
  data.pivot = Vec3(-(data.size[0] / 2.0f + data.minPos[0]),
      -(data.size[1] / 2.0f + data.minPos[1]),
      -(data.size[2] / 2.0f + data.minPos[2]));
  dataList.push_back(data);
  //printout
  //AppConsole::AddLog("")
  file.close();
  /*
  unsigned int SZ = static_cast<unsigned>(data.output.size());
  for (unsigned i = 0; i < SZ; i += 3)
  {
    Vertex& v1 = data.output[i + 0];
    Vertex& v2 = data.output[i + 1];
    Vertex& v3 = data.output[i + 2];

    Vec3 e1 = v2.pos - v1.pos;
    Vec3 e2 = v3.pos - v1.pos;
    Vec3 dUV1 = (v2.uv - v1.uv);
    Vec3 dUV2 = (v3.uv - v1.uv);

    GLfloat f = 1.0f / (dUV1.x * dUV2.y - dUV1.x * dUV2.y);

    v1.tan.x = f * (dUV2.y * e1.x - dUV1.y * e2.x);
    v1.tan.y = f * (dUV2.y * e1.y - dUV1.y * e2.y);
    v1.tan.z = f * (dUV2.y * e1.z - dUV1.y * e2.z);
    v3.tan = v2.tan = v1.tan = v1.tan.Normalize();

    v1.biTan.x = f * (-dUV2.x * e1.x + dUV1.x * e2.x);
    v1.biTan.y = f * (-dUV2.x * e1.y + dUV1.x * e2.y);
    v1.biTan.z = f * (-dUV2.x * e1.z + dUV1.x * e2.z);
    v3.biTan = v2.biTan = v1.biTan = v1.biTan.Normalize();
  }*/

  CompleteCallback(name, dataList);
}

void ObjLoader::CompleteCallback(const std::string &name, std::vector<MeshData> & data)
{
  {
    // To prevent m_complete from being incremented
    std::unique_lock<std::mutex> lock(g_mutex);
    m_models[name] = data;
    m_complete[name] = name;
  }
}

void ObjLoader::MeshDataClear(std::string const& name)
{
    for (size_t i = 0; i < m_models[name].size(); ++i)
        m_models[name][i].Clear();
}

void ObjLoader::Update()
{
  {
    // To prevent m_complete from being incremented
    std::unique_lock<std::mutex> lock(g_mutex);

    for (auto& elem : m_complete)
    {
      if (m_threads[elem.first].joinable())
      {
        m_threads[elem.first].join();
        GenMesh(elem.first, m_models[elem.first]);
        MeshDataClear(elem.first);
        //Log("Loading Complete : ", elem.second);
      }
    }

    if ((m_complete.size() == m_threads.size()) && m_loading)
    {
      m_loading = false;
      m_complete.clear();
      m_threads.clear();
    }
  }
}
