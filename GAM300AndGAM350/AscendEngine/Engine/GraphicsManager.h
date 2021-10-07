/******************************************************************************/
/*!
\file   GraphicsManager.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for GraphicsManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

typedef ComponentManagerBase<GraphicsComp>::COMPHANDLETYPE GrapHandle;

#define ASC_GMAN GraphicsManager::Inst()
class GraphicsManager : public ComponentManagerBase<GraphicsComp>, public Singleton<GraphicsManager>
{
public:
  explicit GraphicsManager(HandleManager<TransformComp> &m_position);
  ~GraphicsManager();

  enum CameraIdx
  {
    EDITOR_CAM,
    PLAYER_CAM,
    LEVEL_CAM,

    MAX_CAMERA
  };

  //Virtual functions
  void Load() override;
  //void UnLoad() override;
  void Update(float dt) override;
  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;
  //Handle CreateComponent(const GameObject &gameObject) override;
  //GraphicsComp* GetComponent(Handle index) override;
  //void DestroyComponent(Handle hdl) override;
  //Handle CloneComponent(Handle hdl) override;
  void InitComponent(const GameObject & go, BaseHandle hdl) override;

  //function from CompManBase
  void CopyComponentToComponent(GraphicsComp& target, const GraphicsComp& source) override;

  void HandleMsg(MsgData_Base*) override;

  bool IsLoading() const { return m_objLoader.IsLoading(); }
  bool IsDebugDraw() const { return isDebugDraw; }

  //cameras
  int GetCameraIdx();
  size_t GetTotalCamera();
  Camera& GetCamera();
  Camera& GetCamera(int handle); //gives u the camera to adjust values but does not set it as active
  Camera& SetCamera(int handle);
  Camera& NextCamera();
  Camera& PrevCamera();

  Matrix4& GetUIMatrix() { return GetCamera().GetOrtho(); }
  std::string SetAndReflectModel(const Reflector&, const std::string&);
  std::string SetAndReflectAlbedo(const Reflector&, const std::string&);

  int AddCamera();
  void RemoveCamera(int handle); //not tested

  std::map<std::string, ShaderProgram*>& GetShaders() { return m_shaders; }

  unsigned int GetPickedObject();
  void SetHighlightCol(const Vec3&);
  void SetHighlightScale(float);
  
  void InitGL();
  inline void SetDebugMode() { isDebugDraw = true; }
  inline void SetAccum(bool flag) { isAccumOn  = flag;}

  ObjLoader m_objLoader;
  std::vector<std::string>const& GetTextureList() const { return textures; }

  Vector3 m_highlightCol;
  void SetGamma(float val);
  float GetGamma()const;
private:

  TextureLoader m_texLoader;
 

  //Camera m_camera;
  int m_cam_idx = 0;
  std::vector<Camera> m_cameraVec;

  std::vector<std::string> textures;
  std::map<std::string,std::string> texturesMap;

  Matrix4 m_uiSMat;
  float originalScale;

  Vector4 bgColor;

  HandleManager<TransformComp>& m_position;
  std::map<std::string, ShaderProgram*> m_shaders;
  std::vector<std::string> m_models;
  std::vector<std::string> m_albedoTextures;
  std::vector<std::string> m_ambientTextures;
  std::vector<std::string> m_specularTextures;
  std::vector<std::string> m_normalTextures;

  std::vector<int> m_uiList;
  std::vector<int> m_highlightList;
  std::vector<int> m_forwardList;
  std::vector<int> m_deferredList;

  void CleanUp();

  //Shaders
  void LoadShader(const std::string& name, const std::string& vs, const std::string& fs);

  //CubeMap aka Skybox
  GLuint skyboxID;
  void SetupSkyBox();
  void DrawSkybox();

  bool isDebugDraw, isAccumOn;
  bool showMetric;
  void DebugLine();
  void RenderTrail();
  //void DebugLights();
  LineRenderer lineR;
  TrailRenderer trailR;

  //Deferred Shading
  unsigned int m_DeferredFBO;
  unsigned int m_FSQuadVAO;
  unsigned int m_PickerFBO;
  std::vector<TextureData> m_buffers;
  void SetupGBuffers();
  void SetupFSQuad();
  void DebugGBuffers();

  void PrePass();
  void ForwardPass();
  void GeometryPass();
  void LightPass();
  void InterfacePass();
  void HighlightsPass();

  ParticleManager* m_particles;
  //Vector3 m_highlightCol;
  float m_highlightScale;
  float gammaValue=2.2f;
  //for cloning
  //Handle CloneComponent(const GraphicsComp& old);
  friend struct GraphicsComp;
};

#endif;