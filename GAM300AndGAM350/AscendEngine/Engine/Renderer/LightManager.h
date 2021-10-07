/******************************************************************************/
/*!
\file   LightManager.h
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
#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

struct LightData
{
  Vec4 position;
  Vec4 color;
  float intensity;
  float radius;
  Vec2 pad;

  LightData() : LightData({ 0.0f,0.0f,0.0f,1.0f },
  { ASC_RAND_COL, ASC_RAND_COL, ASC_RAND_COL, 1.0f }) {}

  LightData(const Vec4& pos, const Vec4& col) :
    position(pos), color(col), intensity(1.0f), radius(1.0f) {}

  void SetType(int t) { position.w = (float)t; }
  int GetType() { return static_cast<int>(position.w); }
};

typedef ComponentManagerBase<LightComp>::COMPHANDLETYPE LightHandle;

#define ASC_LIGHTS LightManager::Inst()
class LightManager : public ComponentManagerBase<LightComp>, public Singleton<LightManager>
{
public:
  LightManager();
  ~LightManager();

  void   InitGL();

  //Virtual functions
  void   Load          () override;
  void   UnLoad        () override;
  void   Update        (float dt) override;

  BaseHandle Deserialize   (const GameObject &gameObject, Deserializer &) override;
  void   Serialize     (Serializer &, BaseHandle) override;

  void   Reflect       (BaseHandle, const Reflector &) override;
  BaseHandle CreateComponent() override;
  void   DestroyComponent(BaseHandle hdl) override;
  //void   InitComponent (const GameObject & go, Handle hdl) override;

  void   HandleMsg     (MsgData_Base*) override;

  void BindLightBuffer(const std::string&);
  size_t NumOfLights();
  void DebugLights(Mesh& md);
  void SetCamera(Camera* cam);
  //this function every manager needs to implement as it is a deep copy
  void CopyComponentToComponent(LightComp& target, const LightComp& source) override;

  void AddSkyLights(const Vec4& pos, const Vec4& color, float intensity = 1.0f);
  LightData& GetLightData(const LightHandle&);

  enum LightType
  {
    DISABLE = -1,
    DIRECTIONAL = 0,
    POINT = 1,
  };
private:
  //
  //float 

  //Lighting
  unsigned int m_lightBuffer;

  std::stack<size_t> m_pool;
  std::vector<LightData> m_Lights;
  Camera* m_camera;
  std::map<std::string, ShaderProgram*>* m_shaders;

  size_t GetLightHdl();
  void ReleaseHdl(size_t idx);
};

#endif