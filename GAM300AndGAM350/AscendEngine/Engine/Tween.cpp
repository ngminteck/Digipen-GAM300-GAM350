/******************************************************************************/
/*!
\file   Tween.h
\author Tan Hong Boon
\par    email: h.tan\@digipen.edu
\par    Digipen login: h.tan
\date   02/02/2020
\brief
Contains definitions required for TweenInfo, TweenInfoRef & Tween

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"

#define ENABLE_DEBUG 0

Tween::Tween() : tweenFnMap({
  { TweenType::Linear , &Tween::Linear },
  { TweenType::BounceEaseIn , &Tween::BounceEaseIn },
  { TweenType::BounceEaseOut , &Tween::BounceEaseOut },
  { TweenType::ElasticEaseIn , &Tween::ElasticEaseIn },
  { TweenType::ElasticEaseOut , &Tween::ElasticEaseOut },
  { TweenType::BackEaseIn , &Tween::BackEaseIn },
  { TweenType::BackEaseOut , &Tween::BackEaseOut },
  { TweenType::SineEaseIn , &Tween::SineEaseIn },
  { TweenType::SineEaseOut , &Tween::SineEaseOut },
  { TweenType::CircEaseIn , &Tween::CircEaseIn },
  { TweenType::CircEaseOut , &Tween::CircEaseOut },
  { TweenType::EaseOutCubic , &Tween::EaseOutCubic },
  { TweenType::Test , &Tween::Test },
  { TweenType::Custom1 , &Tween::Custom1},
})
{}


Tween::~Tween()
{
}

void Tween::Update(float dt)
{
  start = Clock::now();

  for (auto& elem : tweenVec.m_objects)
  {
    //Log(elem.start);
    if (elem.stop)
      continue;

    bool isAlive = elem.currTime < elem.duration;
    if (!isAlive && !elem.infinite)
    {
      //deregister tween upon completion
      deregVec.push_back(elem.GetSelfHandle());
      //DeregisterTween(elem.GetSelfHandle());
      continue;
    }

    //if (elem.infinite)
    if (!elem.pair.first)
      (this->*(tweenFnMap[elem.type]))(elem);
    else if (elem.pair.second.IsValid())  //is referencing object's variables
    {
      (this->*(tweenFnMap[elem.type]))(elem);
    }

    elem.currTime += dt;
  }

  for (auto& hdl : deregVec)
  {
    DeregisterTween(hdl);
  }
  deregVec.clear();

  duration = Clock::now() - start;
}

void Tween::Reflect()
{
#if 0
  bool open = true;
  ImGui::SetNextWindowPos({ 500.0f, ASC_SYSTEM.winHeight - 200.0f });
  ImGui::Begin("Tween Window", &open,
    ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoTitleBar
  );

  if (!open) return;

  std::vector<const char*> nameVec;
  for (auto& info : tweenVec.m_objects)
  {
    nameVec.push_back(info.name.data());
  }

  static int track = -1;
  ImGui::Combo("Tweens", &track, nameVec.data(), (int)nameVec.size());

  if (track < tweenVec.m_objects.size())
  {
    const float dragSpeed = 0.02f;
    TweenInfo& info = tweenVec.m_objects[track];
    //reset current time for preview button
    ImGui::DragFloat("Current Time", &info.currTime, dragSpeed, 0.0f, 10.0f, "%.1f");
    ImGui::DragFloat("Duration", &info.duration, dragSpeed, 0.0f, 10.0f, "%.1f");
    ImGui::DragFloat("Start value", info.start, dragSpeed, 0.0f, 10.0f, "%.1f");
    ImGui::DragFloat("End value", &info.end, dragSpeed, 0.0f, 10.0f, "%.1f");
    ImGui::DragFloat("Begin at", &info.begin, dragSpeed, 0.0f, 10.0f, "%.1f");

    ImGui::Checkbox("Always Update", &info.infinite);
    ImGui::SameLine();
    ImGui::Checkbox("Stop", &info.stop);
    /*const std::string("")'
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(450.0f);
      ImGui::TextUnformatted();
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
    */
  }

  nameVec.clear();
  ImGui::End();
#endif
}


TweenHdl Tween::RegisterTween(const GOHandle& h, TweenType type, std::string& name, float& start, float end, float duration)
{
  TweenHdl tween_h = tweenVec.GetNewObject(h, type, name, start, end, duration, type == TweenType::Test ? true : false);
#if !SUBMISSION && ENABLE_DEBUG
  Log("Tween Registered", name, h.Val());
#endif
  return tween_h;
}

void Tween::DeregisterTween(const TweenHdl & handle)
{
  //std::vector<TweenInfo>::iterator iter = tweenVec.begin();
  //std::advance(iter, handle);
  if (handle.IsValid())
  {
#if !SUBMISSION && ENABLE_DEBUG
    Log("Tween Dereg", tweenVec[handle]->name, handle.Val());
#endif
    tweenVec.ReleaseObj(handle);
  }
  //else
    //LogError("Tween Dereg :  Handle not valid");
}

void Tween::ResetTime(const TweenHdl & h)
{
  TweenInfo& info = *tweenVec[h];
  info.currTime = 0.0f;
  //info.done = false;
}

void Tween::Stop(const TweenHdl & h, bool stop)
{
  TweenInfo& info = *tweenVec[h];
  info.stop = stop;
}


void Tween::Linear(TweenInfo& info)
{
  //if (compare_str(info.name, "uii"))
  //  int a = 1;

  float & start = *info.start;
  float& end = info.end;
  //float incrementPerFrame = info->incrementPerFrame;
  float duration = info.duration;
  float currTime = info.currTime;

  //start = end; return;
  //std::cout << "end : "<<end << "-" << "start :" << start <<"\n";
  //std::cout  << currTime << " * " << end - start << "/" << duration << "+" << start << "\n";
  
  //Log(info.name, " : ", currTime, "*(", end, "-", start, ")/", duration);
  float change = end - info.begin;
  start = change* ( currTime / duration) + info.begin;
}


void Tween::BounceEaseIn(TweenInfo& info)
{
  float c = info.end - info.begin;
  BounceEaseOut(info);
  *info.start = c - *info.start + info.begin;
}

void Tween::BounceEaseOut(TweenInfo& info)
{
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;

  float& output = *info.start;

  if ((t /= d) < (1 / 2.75f)) {
    output = c*(7.5625f*t*t) + b;
  }
  else if (t < (2 / 2.75f)) {
    float postFix = t -= (1.5f / 2.75f);
    output = c*(7.5625f*(postFix)*t + .75f) + b;
  }
  else if (t < (2.5 / 2.75)) {
    float postFix = t -= (2.25f / 2.75f);
    output = c*(7.5625f*(postFix)*t + .9375f) + b;
  }
  else {
    float postFix = t -= (2.625f / 2.75f);
    output = c*(7.5625f*(postFix)*t + .984375f) + b;
  }
}

void Tween::ElasticEaseIn(TweenInfo& info)
{
  //
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;

  float& output = *info.start;
  //

  if (t == 0) output = b;
  if ((t /= d) == 1) output = b + c;
  float p = d*.3f;
  float a = c;
  float s = p / 4;
  float postFix = (float)(a*pow(2, 10 * (t -= 1))); // this is a fix, again, with post-increment operators
  output = (float)(-(postFix * sin((t*d - s)*(2 * ASC_PI) / p)) + b);
}

void Tween::ElasticEaseOut(TweenInfo& info)
{
  //
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;

  float& output = *info.start;
  //

  if (t == 0) output = b;
  if ((t /= d) == 1) output = b + c;
  float p = d*.3f;
  float a = c;
  float s = p / 4;
  output = (float)(a*pow(2, -10 * t) * sin((t*d - s)*(2 * ASC_PI) / p) + c + b);
}

void Tween::BackEaseIn(TweenInfo& info)
{
  //
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;

  float& output = *info.start;
  //

  float s = 1.70158f;
  float postFix = t /= d;
  output = c*(postFix)*t*((s + 1)*t - s) + b;
}

void Tween::BackEaseOut(TweenInfo& info)
{
  //
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;

  float& output = *info.start;
  //

  float s = 1.70158f;
  output = c*((t = t / d - 1)*t*((s + 1)*t + s) + 1) + b;
}

void Tween::SineEaseIn(TweenInfo &info)
{
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;  //period
  float& output = *info.start;

  output = -c * cos(t/d * ASC_HALFPI) + c + b;
}

void Tween::SineEaseOut(TweenInfo & info)
{
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;  //period
  float& output = *info.start;

  output = c * sin(t/d * ASC_HALFPI) + b;
}

void Tween::CircEaseIn(TweenInfo & info)
{
  auto t = info.currTime;
  auto b = info.begin;
  auto c = info.end - b;
  auto d = info.duration;  //period
  auto& output = *info.start;

  output = -c * (sqrt(1.0f - (t/=d)*t) - 1.0f) + b;
}

void Tween::CircEaseOut(TweenInfo & info)
{
  auto t = info.currTime;
  auto b = info.begin;
  auto c = info.end - b;
  auto d = info.duration;  //period
  auto& output = *info.start;

  output = c * sqrt(1.0f - (t=t/d-1.0f)*t) + b;
}

void Tween::EaseOutCubic(TweenInfo & info)
{
  //
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;  //period
  float& output = *info.start;
  //
  float ts = (t /= d)*t;
  float tc = ts*t;
  output = b + c*(tc + -3 * ts + 3 * t);
}

void Tween::Test(TweenInfo & info)
{
  //https://chicounity3d.wordpress.com/2014/05/23/how-to-lerp-like-a-pro/
  //
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;  //period
  float& output = *info.start;
  //

  //const float min = 0.01;
  //info.currTime = info.currTime > (d - min) ? 0 : t;

  float ts = (t /= d)*t;
  float tc = ts*t;

  float amplitude = 1.0f;
  float theta = (t) / d;
  float distance = amplitude * sin(theta);
  //transform.position = startPos + Vector3.up * distance;
  output = b + distance;
  //output = b + c*(20.2*tc*ts + -66.9425*ts*ts + 77.385*tc + -35.19*ts + 5.5475*t);
}

void Tween::Custom1(TweenInfo & info)
{
  float t = info.currTime;
  float b = info.begin;
  float c = info.end - b;
  float d = info.duration;  //period
  float& output = *info.start;

  float ts = (t /= d)*t;
  float tc = ts*t;

  //... 1....4......| ......3...|.........................2....
  output = b + c*(-23.495f*tc*ts + 94.735f*ts*ts + -118.785f*tc + 55.295f*ts + -6.75f*t);
}


TweenInfo::TweenInfo(const GOHandle& obj_h, TweenType type, std::string name,
  float & start, float end, float duration, bool infinite_)
  : type(type), name(name), start(&start), end(end), begin(start),
  duration(duration), currTime(0.0f), infinite(infinite_)
{
  pair = obj_h.IsValid() ?
    std::pair<bool, GOHandle>(true, obj_h) :
    std::pair<bool, GOHandle>(false, GOHandle());
}

//TweenInfo & TweenInfo::operator=(const TweenInfo & rhs)
//{
//  name = rhs.name;
//  type = rhs.type;
//  start = rhs.start;
//  begin = rhs.begin;
//  duration = rhs.duration;
//  currTime = rhs.currTime;
//
//  return *this;
//}

TweenInfoRef::~TweenInfoRef()
{
  for (auto& pair : handles_map)
  {
    ASC_TWEEN.DeregisterTween(pair.second);
  }

  //handles_map.clear();
}

/* **** float **** */
void TweenInfoRef::RegisterEnd(TweenType type, std::string name,
  float& start, float end, float duration)
{
  TweenHdl handle = ASC_TWEEN.RegisterTween(obj_h, type, name, start, end, duration);
  //auto iter = handles_map.find(name);
  //if (iter == handles_map.end())
  //  handles_map.insert({ name, handle });
  handles_map.push_back({ name, handle });
}

void TweenInfoRef::RegisterOffset(TweenType type, std::string name, 
  float & start, float offset, float duration)
{
  RegisterEnd(type, name, start, start + offset, duration);
}

/* **** Vec3 **** */
void TweenInfoRef::RegisterEnd(TweenType type, std::string name, Vec3 & startVec, 
  const Vec3& endVec,
  const Vec3& duration)
{
  RegisterEnd(type, name, startVec.x, endVec.x, duration.x);
  RegisterEnd(type, name, startVec.y, endVec.y, duration.y);
  RegisterEnd(type, name, startVec.z, endVec.z, duration.z);
}

void TweenInfoRef::RegisterEnd(TweenType type, std::string name, Vec3 & startVec, 
  const Vec3& end, float duration)
{
  RegisterEnd(type, name, startVec.x, end.x, duration);
  RegisterEnd(type, name, startVec.y, end.y, duration);
  RegisterEnd(type, name, startVec.z, end.z, duration);
}

void TweenInfoRef::RegisterOffset(TweenType type, std::string name, Vec3 & startVec,
  Vec3 offsetVec,
  const Vec3& durationVec)
{
  float end_x = startVec.x + offsetVec.x;
  float end_y = startVec.y + offsetVec.y;
  float end_z = startVec.z + offsetVec.z;

  RegisterEnd(type, name, startVec, Vec3(end_x, end_y, end_z), durationVec);
}


void TweenInfoRef::RegisterOffset(TweenType type, std::string name, Vec3 & startVec, 
  float offset, float duration)
{
  Vec3 endVec = startVec + Vec3(offset, offset, offset);

  RegisterEnd(type, name, startVec.x, endVec.x, duration);
  RegisterEnd(type, name, startVec.y, endVec.y, duration);
  RegisterEnd(type, name, startVec.z, endVec.z, duration);
}

/* **** Vec4 **** */
void TweenInfoRef::RegisterEnd(TweenType type, std::string name, 
  Vec4 & startVec, const Vec4 & endVec, const Vec4 & duration)
{
  RegisterEnd(type, name, startVec.x, endVec.x, duration.x);
  RegisterEnd(type, name, startVec.y, endVec.y, duration.y);
  RegisterEnd(type, name, startVec.z, endVec.z, duration.z);
  RegisterEnd(type, name, startVec.w, endVec.w, duration.w);
}

void TweenInfoRef::RegisterEnd(TweenType type, std::string name, 
  Vec4 & startVec, const Vec4 & end, float duration)
{
  RegisterEnd(type, name, startVec.x, end.x, duration);
  RegisterEnd(type, name, startVec.y, end.y, duration);
  RegisterEnd(type, name, startVec.z, end.z, duration);
  RegisterEnd(type, name, startVec.w, end.w, duration);
}

void TweenInfoRef::RegisterOffset(TweenType type, std::string name, Vec4 & startVec,
  float offset_x, float offset_y, float offset_z, float offset_w,
  float duration_x, float duration_y, float duration_z, float duration_w)
{
  float end_x = startVec.x + offset_x;
  float end_y = startVec.y + offset_y;
  float end_z = startVec.z + offset_z;
  float end_w = startVec.w + offset_w;

  RegisterEnd(type, name, startVec.x, end_x, duration_x);
  RegisterEnd(type, name, startVec.y, end_y, duration_y);
  RegisterEnd(type, name, startVec.z, end_z, duration_z);
  RegisterEnd(type, name, startVec.w, end_w, duration_w);
}

void TweenInfoRef::RegisterOffset(TweenType type, std::string name, Vec4 & startVec,
  float offset, float duration)
{
  Vec4 endVec = startVec + Vec4(offset, offset, offset, offset);

  RegisterEnd(type, name, startVec.x, endVec.x, duration);
  RegisterEnd(type, name, startVec.y, endVec.y, duration);
  RegisterEnd(type, name, startVec.z, endVec.z, duration);
  RegisterEnd(type, name, startVec.w, endVec.w, duration);
}

//bool TweenInfoRef::IsValid()
//{
//  return handle.IsValid();
//}


TweenInfoCopy::~TweenInfoCopy()
{
  for (auto& tic_info : handles_map)
  {
    ASC_TWEEN.DeregisterTween(tic_info.hdl);
  }
}

void TweenInfoCopy::RegisterEnd(TweenType type, std::string name, float& startRef_, float end, float duration)
{
  //updated_val = startRef_;
  TIC_info info;
  info.tweenUpdateThisVal = startRef_;

  //updated_val is taken in as ref(tween directly chg this value)
  TweenHdl handle = ASC_TWEEN.RegisterTween(GOHandle(), type, name, info.tweenUpdateThisVal, end, duration);

  info.val = static_cast<float*>(&startRef_);
  info.hdl = handle;
  info.name = name;

  //auto iter = handles_map.find(name);
  //if (iter == handles_map.end())
  //  handles_map.insert({ name, handle });
  handles_map.push_back(info);
}

void TweenInfoCopy::Update()
{
  //prev_val = updated_val;

  for (auto& info : handles_map)
  {
    float offset = *info.val - info.tweenUpdateThisVal;
    //info.val needs to be decreased : info.val needs to be increased
    offset > 0 ? *info.val -= fabs(offset) : *info.val += fabs(offset);
    //Log("tweeninfocopy ref val ", *info.val);
  }
}

void TweenInfoCopy::RegisterEnd(TweenType type, std::string name, Vec3 & startVec,
  const Vec3& endVec,
  const Vec3& duration)
{
  RegisterEnd(type, name, startVec.x, endVec.x, duration.x);
  RegisterEnd(type, name, startVec.y, endVec.y, duration.y);
  RegisterEnd(type, name, startVec.z, endVec.z, duration.z);
}

void TweenInfoCopy::RegisterEnd(TweenType type, std::string name, Vec3 & startVec,
  const Vec3& end, float duration)
{
  RegisterEnd(type, name, startVec.x, end.x, duration);
  RegisterEnd(type, name, startVec.y, end.y, duration);
  RegisterEnd(type, name, startVec.z, end.z, duration);
}

void TweenInfoCopy::RegisterOffset(TweenType type, std::string name, Vec3 & startVec,
  Vec3 offsetVec,
  const Vec3& durationVec)
{
  float end_x = startVec.x + offsetVec.x;
  float end_y = startVec.y + offsetVec.y;
  float end_z = startVec.z + offsetVec.z;

  RegisterEnd(type, name, startVec, Vec3(end_x, end_y, end_z), durationVec);
}


void TweenInfoCopy::RegisterOffset(TweenType type, std::string name, Vec3 & startVec,
  float offset, float duration)
{
  Vec3 endVec = startVec + Vec3(offset, offset, offset);

  RegisterEnd(type, name, startVec.x, endVec.x, duration);
  RegisterEnd(type, name, startVec.y, endVec.y, duration);
  RegisterEnd(type, name, startVec.z, endVec.z, duration);
}

/* **** Vec4 **** */
void TweenInfoCopy::RegisterEnd(TweenType type, std::string name,
  Vec4 & startVec, const Vec4 & endVec, const Vec4 & duration)
{
  RegisterEnd(type, name, startVec.x, endVec.x, duration.x);
  RegisterEnd(type, name, startVec.y, endVec.y, duration.y);
  RegisterEnd(type, name, startVec.z, endVec.z, duration.z);
  RegisterEnd(type, name, startVec.w, endVec.w, duration.w);
}

void TweenInfoCopy::RegisterEnd(TweenType type, std::string name,
  Vec4 & startVec, const Vec4 & end, float duration)
{
  RegisterEnd(type, name, startVec.x, end.x, duration);
  RegisterEnd(type, name, startVec.y, end.y, duration);
  RegisterEnd(type, name, startVec.z, end.z, duration);
  RegisterEnd(type, name, startVec.w, end.w, duration);
}

void TweenInfoCopy::RegisterOffset(TweenType type, std::string name, Vec4 & startVec,
  float offset_x, float offset_y, float offset_z, float offset_w,
  float duration_x, float duration_y, float duration_z, float duration_w)
{
  float end_x = startVec.x + offset_x;
  float end_y = startVec.y + offset_y;
  float end_z = startVec.z + offset_z;
  float end_w = startVec.w + offset_w;

  RegisterEnd(type, name, startVec.x, end_x, duration_x);
  RegisterEnd(type, name, startVec.y, end_y, duration_y);
  RegisterEnd(type, name, startVec.z, end_z, duration_z);
  RegisterEnd(type, name, startVec.w, end_w, duration_w);
}

void TweenInfoCopy::RegisterOffset(TweenType type, std::string name, Vec4 & startVec,
  float offset, float duration)
{
  Vec4 endVec = startVec + Vec4(offset, offset, offset, offset);

  RegisterEnd(type, name, startVec.x, endVec.x, duration);
  RegisterEnd(type, name, startVec.y, endVec.y, duration);
  RegisterEnd(type, name, startVec.z, endVec.z, duration);
  RegisterEnd(type, name, startVec.w, endVec.w, duration);
}



//bool TweenInfoCopy::IsValid()
//{
//  return handle.IsValid();
//}

//void Blink::Update(float dt)
//{
//  //CURRENT TIME is duration /2.0f !!!
//
//  currTime += dt;
//  float& alpha = obj->GetHdl<GraphicsComp>()->tint.a;
//  Vec4& c= obj->GetHdl<GraphicsComp>()->tint;
//  
//  //float sliceDuration = currTime / numOfBlink;
//
//  // time to blink once in new slice
//  if (canBlink)
//  {
//    currblinkDelay += dt;
//    if (currblinkDelay > blinkDelay)
//    {
//      alpha = end;
//      currblinkDelay = 0.0f;
//      canBlink = false;
//      Log("time to blink", " ", alpha);
//    }
//
//  }
//
//  //progress to next slice
//  if (currTime > (currSlice * sliceDuration /*+ baseTime*/))
//  {
//    currSlice++;
//    canBlink = true;
//    alpha = start;
//    Log("progress to slice : ", " ",alpha);
//  }
//
//
//}
