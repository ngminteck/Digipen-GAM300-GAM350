/******************************************************************************/
/*!
\file   ParticleEmitter.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
  Declarations for particle component.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

struct Particle
{
  float camDistance;
  float lifetime, oLife;

  Vector3 position, scale, oScale;
  Quaternion rotate;
  Vector4 color, oColor;

  Quaternion dAngle;
  Vector4 dColor;
  Vector3 dScale;
  float rotation;

  Vector3 velocity;
  float dragFactor, rotationSpeed;
  Vector3 accl; //Acceleration

  Particle() { Invalidate(); }

  void Invalidate();
};

class ParticleEmitter : public Component<ParticleEmitter>
{
public:
  static constexpr COMPONENT_TYPE CompEnum = COMPONENT_TYPE::PARTICLE;
  ASC_VarGetSet(Handle<TransformComp>, TransHdl);

  enum class SHAPE{ POINT, SPHERE, MAX };
  enum class EMISSION { OVERTIME, BURST, MAX };

  ParticleEmitter();
  ~ParticleEmitter();
  void Update(float);
  void Deserialize(Deserializer&);
  void Serialize(Serializer&);

  void SetCamera(Camera&);
  unsigned int GetMaxParticleCount();
  void SetMaxParticle(unsigned int);
  void StartCountdown(float, bool = true);

  /* True if user wants billboarded particles else false */
  bool isBillboarded;

  /* True if user wants particles Forever else false */
  bool isLooping;

  /* True if user wants particles to blend their colors else false */
  bool isBlending;
  bool isDoubleSided;
  bool isLocal;

  float countdown;
  bool selfDestruct;

  EMISSION emissionType;

  std::string textureName;
  Vector4 startMinCol, endMinCol;
  Vector4 startMaxCol, endMaxCol;

  float spawnDelay, spawnRate, currentTime;
  float currentDelay;

  Vector3 startMinScale, endMinScale;
  Vector3 startMaxScale, endMaxScale;
  
  //Rmb to normalize value upon setting
  Vector3 minVelocity, maxVelocity;
  Vector3 acceleration;

  Vector3 faceDirection;
  Vector3 minAABB, maxAABB;
  Vector3 spawnLocOffSet;

  float startMinSpeed, endMinSpeed;
  float startMaxSpeed, endMaxSpeed;
  float minLifetime, maxLifetime;
  float minRotSpeed, maxRotSpeed;

  int activeCount;

  GLuint mVAO;
  GLuint mVBO_vert;
  GLuint mVBO_mat, mVBO_col;

  bool pauseSimulation;

  void Render();
  void Emit();
private:
  Camera* curr_cam;

  unsigned int maxSize, spawnCounter;
  std::vector<Particle> m_objPool;
  std::vector<unsigned int> m_inactivePool;

  std::vector<Matrix4> m_matrices;
  std::vector<Vec4> m_colors;

  void CheckSpawn(float dt);
  void GenerateParticle(Particle&, const Vec3& pos);

  bool GenGL;
  void GenGLBuffers();
  void DelGLBuffers();
  void LookAt(Particle&, const Vec3& newZ);
};

#endif