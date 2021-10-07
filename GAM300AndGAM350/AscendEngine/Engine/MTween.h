#pragma once

class MTween;
struct MTweenInfo;

using MTweenHdl = Handle<MTweenInfo>;

enum class MTweenType : int
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
  QuinticIn,
  Custom1,
  Test, //infinite
        //Also add in map in Tween::constructor
  TWEEN_MAX
};

struct MTweenInfo
{
  MTweenInfo() {}
  MTweenInfo(MTweenType type, const std::function<float()> &get, const std::function<void(float)> &set, float end, float duration) :
  type(type), get(get), set(set), start(get()), end(end), duration(duration)  {}

  ASC_VarGetSet(MTweenHdl, SelfHandle)
  MTweenType type = MTweenType::Linear;

  std::function<float()> get;
  std::function<void(float)> set;

  float start = 0.0f;
  float end = 0.0f;

  float duration = 0.0f;
  float currTime = 0.0f;
};

#define ASC_MTWEEN MTween::Inst()
class MTween : public Singleton<MTween>
{
public:
  MTween();
  MTweenHdl AddTween(MTweenType MTweenType, const std::function<float()> &begin, const std::function<void(float)> &set, float end, float duration);

  void RemoveTween(const MTweenHdl h);
  void Update(float dt);

private:
  typedef void (*MTweenFunction)(MTweenInfo &);

  //http://gizma.com/easing/
  static void Linear(MTweenInfo& info);
  static void BounceEaseIn(MTweenInfo& info);
  static void BounceEaseOut(MTweenInfo& info);
  static void ElasticEaseIn(MTweenInfo& info);
  static void ElasticEaseOut(MTweenInfo& info);
  static void BackEaseIn(MTweenInfo& info);
  static void BackEaseOut(MTweenInfo& info);
  static void SineEaseIn(MTweenInfo& info);
  static void SineEaseOut(MTweenInfo& info);
  static void CircEaseIn(MTweenInfo& info);
  static void CircEaseOut(MTweenInfo& info);
  static void EaseOutCubic(MTweenInfo& info);
  static void QuinticIn(MTweenInfo& info);
  static void Test(MTweenInfo& info);
  static void Custom1(MTweenInfo& info);

  //void RemoveUnused();
  HandleManager<MTweenInfo> tweenComponents;
  std::vector<MTweenHdl> toRemove;
  std::vector<MTweenFunction> fn;
};

void MTweenFloat(MTweenType MTweenType, float &val, float endVal, float duration);

void MTweenLocalPosX(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenLocalPosY(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenLocalPosZ(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenLocalPos(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration);

void MTweenPosX(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenPosY(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenPosZ(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenPos(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration);

void MTweenLocalScaleX(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenLocalScaleY(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenLocalScaleZ(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenLocalScale(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration);

void MTweenScaleX(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenScaleY(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenScaleZ(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenScale(MTweenType MTweenType, GOHandle goHdl, float val, float duration);

void MTweenClrR(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenClrG(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenClrB(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenClrA(MTweenType MTweenType, GOHandle goHdl, float val, float duration);
void MTweenClr(MTweenType MTweenType, GOHandle goHdl, Vec4 val, float duration);

//void AddTweenPosX(MTweenType MTweenType, GOHandle goHdl, float endX, float duration);
//void AddTweenPosY(MTweenType MTweenType, GOHandle goHdl, float endY, float duration);
//void AddTweenPosZ(MTweenType MTweenType, GOHandle goHdl, float endZ, float duration);
//
//void AddTweenAlpha(MTweenType MTweenType, GOHandle goHdl, float endA, float duration);
//
//void AddTweenPos(MTweenType MTweenType, GOHandle goHdl, Vec3 endPos, float duration); 
//void AddTweenLocalPos(MTweenType MTweenType, GOHandle goHdl, Vec3 endPos, float duration);