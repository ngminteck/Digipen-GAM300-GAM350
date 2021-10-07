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

#ifndef TWEEN_H
#define TWEEN_H

//class Blink
//{
//public:
//  Blink() {}
//  Blink(GOHandle h, int num, float duration)
//    : obj(h), numOfBlink(num), duration(duration)
//  {}
//  void Update(float dt);
//
//private:
//  float start = 0.5f;
//  float end = 1.0f;
//  GOHandle obj;
//  int numOfBlink = 10;
//  float duration = 0.0f;
//
//  float baseTime = duration /*/ 2.0f*/;
//  float currTime = 0.0f;
//  bool canBlink = false;
//  int currSlice = 1;
//
//  float sliceDuration = duration / numOfBlink;
//  float blinkDelay = sliceDuration / 15.0f;
//  float currblinkDelay = 0.0f;
//};

struct TweenInfo;
using TweenHdl = Handle<TweenInfo>;//std::vector<TweenInfo>::iterator;

class Tween;
enum TweenType
{
  Linear,
  BounceEaseIn,
  BounceEaseOut,
  ElasticEaseIn,
  ElasticEaseOut,
  BackEaseIn,
  BackEaseOut,
  SineEaseIn,
  SineEaseOut,
  CircEaseIn,
  CircEaseOut,
  EaseOutCubic,
  Custom1,
  Test, //infinite
  //Also add in map in Tween::constructor

  TWEEN_MAX
};

struct TweenInfo
{
  TweenInfo(const GOHandle& h, TweenType type, std::string name,
    float& start, float end, float duration, bool infinite);

  //TweenInfo& operator=(const TweenInfo& rhs);
  ASC_VarGetSet(TweenHdl, SelfHandle);
private:
  std::pair<bool, GOHandle > pair; //bool indicates if using handle(to check validity only)
  std::string name;
  TweenType type = TweenType::Linear;
  float* start;
  float end;
  float begin = *start;
  float duration = 0.0f;  //for infinite, use this as period T
  float currTime = 0.0f;
  bool infinite = false;
  bool stop = false;  //for editor

  friend Tween;
};

struct TweenInfoRef //do not use with things that will get destroyed
{
  //TweenInfoRef(TweenType type, std::string name,
  //  float& start, float end, float duration);
  TweenInfoRef() {}
  TweenInfoRef(GOHandle h) : obj_h(h) { assert(obj_h.IsValid()); }
  ~TweenInfoRef();

  /* **** float **** */
  void RegisterEnd(TweenType type, std::string name,
    float& start, float end, float duration);

  void RegisterOffset(TweenType type, std::string name,
    float& start, float offset, float duration);
  /* **** float **** */

  /* **** Vec3 **** */
  //Vec3 : diff durations for all components
  void RegisterEnd(TweenType type, std::string name, Vec3& startVec,
    const Vec3& endVec, const Vec3& duration);

  //Vec3 : same duration for all components
  void RegisterEnd(TweenType type, std::string name,
    Vec3& startVec, const Vec3& end, float duration);

  //Vec3 : diff durations and offsets for each component of startVec
  void RegisterOffset(TweenType type, std::string name, Vec3& startVec, 
    Vec3 offset, const Vec3& durationVec);

  //Vec3 : same durations and offsets for all component of startVec
  void RegisterOffset(TweenType type, std::string name,
    Vec3& startVec, float offset, float duration);

  /* **** Vec4 **** */
  //Vec4 : diff durations for all components
  void RegisterEnd(TweenType type, std::string name, 
    Vec4& startVec, const Vec4& endVec, const Vec4& duration);

  //Vec4 : same duration for all components
  void RegisterEnd(TweenType type, std::string name,
    Vec4& startVec, const Vec4& end, float duration);

  //Vec4 : diff durations and offsets for each component of startVec
  void RegisterOffset(TweenType type, std::string name, Vec4& startVec,
    float offset_x, float offset_y, float offset_z, float offset_w,
    float duration_x, float duration_y, float duration_z, float duration_w);

  //Vec4 : same durations and offsets for all component of startVec
  void RegisterOffset(TweenType type, std::string name,
    Vec4& startVec, float offset, float duration);

  //bool IsValid();
  //TweenHdl handle;
  GOHandle obj_h;
  //std::unordered_map<std::string, TweenHdl> handles_map;
  std::vector<std::pair<std::string, TweenHdl>> handles_map;
};


//if using this mtd of tweening, 
//then you need to update the value(that is to be tweened) urself, 
//using updated_val
struct TweenInfoCopy
{
  //deregister using tween handle
  ~TweenInfoCopy();

  // float
  void RegisterEnd(TweenType type, std::string name,
    float& start, float end, float duration);

  void RegisterOffset(TweenType type, std::string name,
    float& start, float offset, float duration);
  /* **** float **** */

  /* **** Vec3 **** */
  //Vec3 : diff durations for all components
  void RegisterEnd(TweenType type, std::string name, Vec3& startVec,
    const Vec3& endVec, const Vec3& duration);

  //Vec3 : same duration for all components
  void RegisterEnd(TweenType type, std::string name,
    Vec3& startVec, const Vec3& end, float duration);

  //Vec3 : diff durations and offsets for each component of startVec
  void RegisterOffset(TweenType type, std::string name, Vec3& startVec,
    Vec3 offset, const Vec3& durationVec);

  //Vec3 : same durations and offsets for all component of startVec
  void RegisterOffset(TweenType type, std::string name,
    Vec3& startVec, float offset, float duration);

  /* **** Vec4 **** */
  //Vec4 : diff durations for all components
  void RegisterEnd(TweenType type, std::string name,
    Vec4& startVec, const Vec4& endVec, const Vec4& duration);

  //Vec4 : same duration for all components
  void RegisterEnd(TweenType type, std::string name,
    Vec4& startVec, const Vec4& end, float duration);

  //Vec4 : diff durations and offsets for each component of startVec
  void RegisterOffset(TweenType type, std::string name, Vec4& startVec,
    float offset_x, float offset_y, float offset_z, float offset_w,
    float duration_x, float duration_y, float duration_z, float duration_w);

  //Vec4 : same durations and offsets for all component of startVec
  void RegisterOffset(TweenType type, std::string name,
    Vec4& startVec, float offset, float duration);

  void Update();

  //bool IsValid();
  //TweenHdl handle;
private:
  struct TIC_info
  {
    std::string name;
    TweenHdl hdl;

    float* val = nullptr;
    float tweenUpdateThisVal;  //updated_val is taken in as ref(tween directly chg this value)
  }; 

  //std::unordered_map<std::string, TweenHdl> handles_map;
  std::vector<TIC_info> handles_map;
};


#define ASC_TWEEN Tween::Inst()
class Tween : public Singleton<Tween>
{
public:

  Tween();
  ~Tween();
  void Update(float dt);
  void Reflect();

  TweenHdl RegisterTween(const GOHandle& h, TweenType type, std::string& name, float& start, float end, float duration);
  void DeregisterTween(const TweenHdl& h);  
  void ResetTime(const TweenHdl& h);
  void Stop(const TweenHdl& h, bool stop);

  typedef std::chrono::high_resolution_clock Clock;
  Clock::time_point start;
  std::chrono::duration<float, std::milli> duration;
  
private:
  typedef void (Tween::*TweenFunction)(TweenInfo&);

  //http://gizma.com/easing/
  void Linear(TweenInfo& info);
  void BounceEaseIn(TweenInfo& info);
  void BounceEaseOut(TweenInfo& info);
  void ElasticEaseIn(TweenInfo& info);
  void ElasticEaseOut(TweenInfo& info);
  void BackEaseIn(TweenInfo& info);
  void BackEaseOut(TweenInfo& info);
  void SineEaseIn(TweenInfo& info);
  void SineEaseOut(TweenInfo& info);
  void CircEaseIn(TweenInfo& info);
  void CircEaseOut(TweenInfo& info);
  void EaseOutCubic(TweenInfo& info);
  void Test(TweenInfo& info);
  void Custom1(TweenInfo& info);

  std::unordered_map<TweenType, TweenFunction> tweenFnMap;
  HandleManager<TweenInfo> tweenVec;
  std::vector<TweenHdl> deregVec;
  //std::vector<TweenInfo> tweenVec;
};

#endif
