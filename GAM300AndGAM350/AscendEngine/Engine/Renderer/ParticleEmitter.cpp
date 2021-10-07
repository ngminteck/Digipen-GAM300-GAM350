/******************************************************************************/
/*!
\file   ParticleEmitter.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
  functions for particle emitter

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <pch.h>
//#include <concurrent_vector.h>

const float gQuad[] = {
  /*Position*/        /*UV Coord*/
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f
};

void Particle::Invalidate()
{
  lifetime = oLife = -1.0f;
  rotationSpeed = rotation = dragFactor = 0.0f;
  dAngle = rotate = Quat();
  scale = position = dScale = oScale = Vec3::ZERO;
  velocity = accl = Vec3::ZERO;
  color = oColor = Vec4::BLACK;
  camDistance = FLT_MAX;
}

ParticleEmitter::ParticleEmitter() : isBillboarded(true), isLooping(false),
isBlending(false), isDoubleSided(false), emissionType(EMISSION::OVERTIME), textureName("Blank"), 
startMinCol(Vec4::WHITE), endMinCol(Vec4::WHITE), startMaxCol(Vec4::WHITE), endMaxCol(Vec4::WHITE), 
spawnDelay(0.0f), spawnRate(0.1f), currentTime(0.0f), startMinScale(1,1,1), countdown(FLT_MAX), selfDestruct(false),
endMinScale(1,1,1), startMaxScale(1,1,1), endMaxScale(1,1,1), minVelocity(2,2,2), isLocal(false),
maxVelocity(-2,-2,-2), acceleration(0, -9.8f, 0), faceDirection(Vec3::ZAXIS), startMinSpeed(2),
endMinSpeed(2), startMaxSpeed(2), endMaxSpeed(2), minLifetime(2), maxLifetime(2),
minRotSpeed(0), maxRotSpeed(0), activeCount(0), pauseSimulation(false), GenGL(true)
{
  SetMaxParticle(20);
}

ParticleEmitter::~ParticleEmitter()
{
  DelGLBuffers();
}

void ParticleEmitter::Deserialize(Deserializer& xml)
{
  int mSize = 0;
  xml.Read("MAX_SIZE", mSize);
  maxSize = mSize;

  int eType = 0;
  xml.Read("EMIT_TYPE", eType);
  emissionType = (EMISSION)eType;
  xml.Read("Enabled",Enabled);
  xml.Read("OffsetLocation", this->spawnLocOffSet);
  xml.Read("TextureName",textureName);
  xml.Read("startMinCol", startMinCol);
  xml.Read("endMinCol", endMinCol);
  xml.Read("startMaxCol", startMaxCol);
  xml.Read("endMaxCol",endMaxCol);
  xml.Read("spawnTime", spawnRate);
  xml.Read("spawnDelay", spawnDelay);

  xml.Read("startMinScale", startMinScale);
  xml.Read("endMinScale", endMinScale);
  xml.Read("startMaxScale", startMaxScale);
  xml.Read("endMaxScale", endMaxScale);

  xml.Read("faceDirection", faceDirection);
  xml.Read("minVelocity", minVelocity);
  xml.Read("maxVelocity", maxVelocity);
  xml.Read("acceleration", acceleration);

  xml.Read("startMinSpeed", startMinSpeed);
  xml.Read("endMinSpeed", endMinSpeed);
  xml.Read("startMaxSpeed", startMaxSpeed); 
  xml.Read("endMaxSpeed", endMaxSpeed);
  xml.Read("minLifetime", minLifetime);
  xml.Read("maxLifetime", maxLifetime);
  xml.Read("minRotSpeed", minRotSpeed);
  xml.Read("maxRotSpeed", maxRotSpeed);

  xml.Read("Billboard", isBillboarded);
  xml.Read("isLooping", isLooping);
  xml.Read("isBlending", isBlending);
  xml.Read("isDoubleSided", isDoubleSided);

  xml.Read("countdown", countdown);
  xml.Read("selfDestruct", selfDestruct);
  xml.Read("isLocal", isLocal);

  currentDelay = spawnDelay;
  currentTime = spawnRate;
  SetMaxParticle(maxSize);
}

void ParticleEmitter::Serialize(Serializer& xml)
{
  xml.Write("Enabled", Enabled);
  xml.Write("OffsetLocation", this->spawnLocOffSet);
  xml.Write("MAX_SIZE", (int)maxSize);
  xml.Write("EMIT_TYPE", (int)emissionType);

  xml.Write("TextureName", textureName);
  xml.Write("startMinCol", startMinCol);
  xml.Write("endMinCol", endMinCol);
  xml.Write("startMaxCol", startMaxCol);
  xml.Write("endMaxCol", endMaxCol);

  xml.Write("spawnTime", spawnRate);
  xml.Write("spawnDelay", spawnDelay);
  xml.Write("startMinScale", startMinScale);
  xml.Write("endMinScale", endMinScale);
  xml.Write("startMaxScale", startMaxScale);
  xml.Write("endMaxScale", endMaxScale);

  xml.Write("faceDirection", faceDirection);
  xml.Write("minVelocity", minVelocity);
  xml.Write("maxVelocity", maxVelocity);
  xml.Write("acceleration", acceleration);

  xml.Write("startMinSpeed", startMinSpeed);
  xml.Write("endMinSpeed", endMinSpeed);
  xml.Write("startMaxSpeed", startMaxSpeed);
  xml.Write("endMaxSpeed", endMaxSpeed);
  xml.Write("minLifetime", minLifetime);
  xml.Write("maxLifetime", maxLifetime);
  xml.Write("minRotSpeed", minRotSpeed);
  xml.Write("maxRotSpeed", maxRotSpeed);

  xml.Write("Billboard", isBillboarded);
  xml.Write("isLooping", isLooping);
  xml.Write("isBlending", isBlending);
  xml.Write("isDoubleSided", isDoubleSided);

  xml.Write("countdown", countdown);
  xml.Write("selfDestruct", selfDestruct);
  xml.Write("isLocal", isLocal);
}

void ParticleEmitter::Update(float dt)
{
    if (pauseSimulation)
        return;




  Vec3 camPos = curr_cam->GetPos();
  Matrix4 vMat = curr_cam->GetV();

  activeCount = 0;
  m_inactivePool.clear();

  //Parent position
  Vec3 pPos = m_TransHdl->pos;

  std::sort(m_objPool.begin(), m_objPool.end(),
    [](Particle& lhs, Particle& rhs) {
    return (lhs.camDistance > rhs.camDistance);
  });

  for (unsigned int i = 0; i < maxSize; ++i)
  {
    Particle& p = m_objPool[i];
    if (p.lifetime <= 0.0f)
      m_inactivePool.push_back(i);
  }

  minAABB.Set(FLT_MAX, FLT_MAX, FLT_MAX);
  maxAABB.Set(FLT_MIN, FLT_MIN, FLT_MIN);

  float maxRadius = 0.f;
  for (unsigned int i = 0; i < maxSize; ++i)
  {
    Particle& p = m_objPool[i];

    if (p.lifetime == -1.0f) continue;
    if (p.lifetime + dt > p.oLife)
    {
      p.Invalidate();
      continue;
    }

    Vec3 dir = (camPos - p.position);
    p.camDistance = dir.MagnitudeSq();

    p.lifetime += dt;
    float t = (p.lifetime / p.oLife);
    p.velocity += p.accl * dt;
    p.position += p.velocity * p.dragFactor * dt;
    p.rotation += p.rotationSpeed * dt;

    Vec3 lookDir;
    if (isBillboarded) lookDir = dir.Normalize();
    else lookDir = faceDirection.Normalize();

    p.color = p.oColor + (t * p.dColor);
    p.scale = p.oScale + (t * p.dScale);

    //float radius = p.scale[0] > p.scale[1] ? p.scale[1] > p.scale[2] ? p.scale[1] : p.scale[2] : p.scale[0];
    float rad = Vec3::MaxElem(p.scale);
    if (rad > maxRadius) maxRadius = rad;
    for (unsigned int i = 0; i < 3; ++i)
    {
      minAABB.m[i] = p.position[i] < minAABB[i] ? p.position[i] : minAABB[i];
      maxAABB.m[i] = p.position[i] > maxAABB[i] ? p.position[i] : maxAABB[i];
    }

    LookAt(p, lookDir);
  }

  Vec3 offset{ maxRadius, maxRadius, maxRadius };
  minAABB -= offset;
  maxAABB += offset;

  if (isLocal)
  {
    minAABB += m_TransHdl->pos;
    maxAABB += m_TransHdl->pos;
  }

  CheckSpawn(dt);
}

unsigned int ParticleEmitter::GetMaxParticleCount() {
  return maxSize;
}

void ParticleEmitter::SetMaxParticle(unsigned int newSize) 
{
  maxSize = newSize;
  m_objPool.resize(maxSize);
  m_inactivePool.resize(maxSize);
  m_matrices.resize(maxSize);
  m_colors.resize(maxSize);

  DelGLBuffers();
}

void ParticleEmitter::StartCountdown(float cd, bool sd)
{
  countdown = cd;
  selfDestruct = sd;
}

void ParticleEmitter::Render()
{
  GenGLBuffers();

  if (activeCount == 0) return;

  if (isDoubleSided && !isBillboarded) glDisable(GL_CULL_FACE);
  else glEnable(GL_CULL_FACE);

  if (isBlending) 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  else
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindVertexArray(mVAO);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO_col);
  glBufferSubData(GL_ARRAY_BUFFER, 0, activeCount * Vec4Size, m_colors.data());

  //Vec4 vecCheck[5];
  //glGetBufferSubData(GL_ARRAY_BUFFER, 0, Vec4Size * 5, vecCheck);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO_mat);
  glBufferSubData(GL_ARRAY_BUFFER, 0, activeCount * Mat4Size, m_matrices.data());

  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, activeCount);

  glBindVertexArray(0);
}

void ParticleEmitter::Emit()
{
  if (!GetGOHandle()->isEnable || !Enabled) return;
  switch (emissionType)
  {
  case EMISSION::BURST:
  {
    ++spawnCounter;

    for (auto& i : m_objPool)
      GenerateParticle(i, m_TransHdl->pos + this->spawnLocOffSet);

    currentTime = 0;
  }
  break;

  case EMISSION::OVERTIME:
  {
    int i = 0;
    if (currentTime >= spawnRate)
    {
      if (i >= m_inactivePool.size()) break;
      GenerateParticle(m_objPool[m_inactivePool[i]], m_TransHdl->pos +this->spawnLocOffSet);
      currentTime -= spawnRate;
      ++spawnCounter; ++i;
    }
  }
  break;

  };
}

void ParticleEmitter::CheckSpawn(float dt)
{
  currentDelay -= dt;

  if (countdown < 0.0f && selfDestruct) return;
  if (currentDelay > 0) return;

  currentTime += dt;

  if (!isLooping && spawnCounter > 0) return;
  if (spawnRate < 0) return;
  if (currentTime >= spawnRate)
    Emit();
}

void ParticleEmitter::GenerateParticle(Particle& p, const Vec3& pos)
{
  //Generate new particle here!
  p.oLife = minLifetime + ((maxLifetime - minLifetime) * ASC_RAND_COL);
  p.lifetime = 0.0f;

  Vec4 newMinCol = startMinCol + ((startMaxCol - startMinCol) * ASC_RAND_COL);
  Vec4 newMaxCol = endMinCol + ((endMaxCol - endMinCol) * ASC_RAND_COL);
  p.dColor = (newMaxCol - newMinCol);
  p.oColor = p.color = newMinCol;

  Vec3 newMinScale = startMinScale + ((startMaxScale - startMinScale) * ASC_RAND_COL);
  Vec3 newMaxScale = endMinScale + ((endMaxScale - endMinScale) * ASC_RAND_COL);
  p.dScale = (newMaxScale - newMinScale);
  p.oScale = p.scale = newMinScale;

  float newRotSpeed = minRotSpeed + ((maxRotSpeed - minRotSpeed) * ASC_RAND_COL);
  p.rotationSpeed = newRotSpeed;

  float newStartSpeed = startMinSpeed + ((startMaxSpeed - startMinSpeed) * ASC_RAND_COL);
  float newEndSpeed = endMinSpeed + ((endMaxSpeed - endMinSpeed) * ASC_RAND_COL);
  p.dragFactor = 1.0f - ((newEndSpeed - newStartSpeed) / newEndSpeed);

  Vec3 dVel = maxVelocity - minVelocity;
  p.velocity = minVelocity + Vec3(dVel.x * ASC_RAND_COL, dVel.y * ASC_RAND_COL, dVel.z * ASC_RAND_COL);
  p.accl = acceleration;

  p.position = isLocal ? Vec3::ZERO : pos;
}

void ParticleEmitter::GenGLBuffers()
{
  if (GenGL)
  {
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO_vert);
    glGenBuffers(1, &mVBO_mat);
    glGenBuffers(1, &mVBO_col);
    GenGL = false;
  }

  glBindVertexArray(mVAO);
  
  int stride = sizeof(Vec3) + sizeof(Vec2);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO_vert);
  /* Copy vertex attributes to GPU */
  glBufferData(GL_ARRAY_BUFFER, sizeof(gQuad), gQuad, GL_STATIC_DRAW);

  /* Binding Position */
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

  /* Binding UV */
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(Vec3)));

  glBindBuffer(GL_ARRAY_BUFFER, mVBO_col);
  glBufferData(GL_ARRAY_BUFFER, maxSize * Vec4Size, nullptr, GL_STREAM_DRAW);

  /* Binding Color */
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO_mat);
  glBufferData(GL_ARRAY_BUFFER, maxSize * Mat4Size, nullptr, GL_STREAM_DRAW);

  /* Binding Matrix */
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, Mat4Size, (GLvoid*)0);
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, Mat4Size, (GLvoid*)(Vec4Size));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, Mat4Size, (GLvoid*)(2 * Vec4Size));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, Mat4Size, (GLvoid*)(3 * Vec4Size));

  glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 set -> 0
  glVertexAttribDivisor(1, 0); // particles UV : always reuse the same 4 set -> 0
  glVertexAttribDivisor(2, 1); // Color : one per quad -> 1

  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1); // Matrix : one per quad -> 1

  glBindVertexArray(0);
}

void ParticleEmitter::DelGLBuffers()
{
  if (!GenGL)
  {
    glDeleteBuffers(1, &mVBO_vert);
    glDeleteBuffers(1, &mVBO_mat);
    glDeleteBuffers(1, &mVBO_col);
    glDeleteVertexArrays(1, &mVAO);
    GenGL = true;
  }
}

void ParticleEmitter::LookAt(Particle & p, const Vec3 & newZ)
{
  Vec3 upVec = curr_cam->GetUp();

  Quaternion q = Quat::LookAt(newZ, upVec);
  //Scale object to user specified values
  m_matrices[activeCount] = Matrix4::Scale(p.scale);
  //Apply Axis angle rotation
  m_matrices[activeCount] *= Matrix4::Rotate(Vec3::ZAXIS, p.rotation);
  //Look at specified direction
  m_matrices[activeCount] *= q.GetMatrix();

  m_matrices[activeCount].m30 = p.position.x;
  m_matrices[activeCount].m31 = p.position.y;
  m_matrices[activeCount].m32 = p.position.z;

  if (isLocal)
  {
    m_matrices[activeCount] = m_matrices[activeCount] * m_TransHdl->mat;
  }

  m_colors[activeCount] = p.color;
  ++activeCount;
}

void ParticleEmitter::SetCamera(Camera &cam)
{
  curr_cam = &cam;
}
