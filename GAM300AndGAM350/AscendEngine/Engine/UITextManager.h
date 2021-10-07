/******************************************************************************/
/*!
\file   UITextManager.h
\author Goh zihe
\par    email: Zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for UITextManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once
typedef ComponentManagerBase<UITextComp>::COMPHANDLETYPE UIHandle;

#define ASC_UITEXT UITextManager::Inst()
class UITextManager : public ComponentManagerBase<UITextComp> , public Singleton<UITextManager>
{
public:
  explicit UITextManager(HandleManager<TransformComp> &transform_comps);
  ~UITextManager();

  //Virtual functions
  void Load() override;
  //void UnLoad() override;
  void Update(float dt) override;
  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;

  BaseHandle CreateComponent() override;
  void DestroyComponent(BaseHandle hdl) override;
  //TransformComp* GetComponent(Handle index) override;
  void InitComponent(const GameObject& go, BaseHandle hdl) override;
  void CopyComponentToComponent(UITextComp& target, const UITextComp& source) override;
  void HandleMsg(MsgData_Base*) override;
  void InitFont(UITextComp &) const;

  void CheckInput();

  void EnableInputCheck(bool);

private:
  Matrix4 *m_UImatrix;
  ShaderProgram *shaderProgram;
  HandleManager<TransformComp> &m_transformComps;

  GLuint vao;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  bool isCheckingInput, isCutScene;
  float fadeVal;

  bool isPaused = false;

  std::unordered_map<std::string, std::vector<uint8_t>> fontDataMap;
};
