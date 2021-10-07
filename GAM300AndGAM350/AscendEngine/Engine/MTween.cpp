#include "pch.h"

MTween::MTween() : fn 
{
  &MTween::Linear,
  &MTween::BounceEaseIn,
  &MTween::BounceEaseOut,
  &MTween::ElasticEaseIn,
  &MTween::ElasticEaseOut,
  &MTween::BackEaseIn,
  &MTween::BackEaseOut,
  &MTween::SineEaseIn,
  &MTween::SineEaseOut,
  &MTween::CircEaseIn,
  &MTween::CircEaseOut,
  &MTween::EaseOutCubic,
  &MTween::QuinticIn,
  &MTween::Custom1 
}
{
}

MTweenHdl MTween::AddTween(MTweenType MTweenType, const std::function<float()> &get, const std::function<void(float)> &set, float end, float duration)
{
  return tweenComponents.GetNewObject(MTweenType, get, set, end, duration);
}

void MTween::RemoveTween(const MTweenHdl hdl)
{
  tweenComponents.ReleaseObj(hdl);
}

void MTween::Update(float dt)
{
  for (auto& elem : tweenComponents.m_objects)
  {
    (*fn[int(elem.type)])(elem);
    elem.currTime += dt;

    if (elem.currTime > elem.duration)
    {
      toRemove.push_back(elem.GetSelfHandle());
    }
  }

  for (auto hdl : toRemove)
  {
    RemoveTween(hdl);
  }

  toRemove.clear();
}

void MTweenFloat(MTweenType MTweenType, float &val, float endVal, float duration)
{
  auto get = [&val]() -> float
  {
    return val;
  };

  auto set = [&val](float val_)
  {
    val = val_;
  };
  ASC_MTWEEN.AddTween(MTweenType, get, set, endVal, duration);
}

#define TweenBase(NAME, COMP, VAR)                                                      \
void NAME(MTweenType MTweenType, GOHandle goHdl, float val, float duration)             \
{                                                                                       \
  auto get = [goHdl]() -> float                                                         \
  {                                                                                     \
    if (goHdl.IsValid())                                                                \
      return goHdl->GetHdl<COMP>()->VAR;                                                \
    throw std::exception("aa");                                                         \
  };                                                                                    \
                                                                                        \
  auto set = [goHdl](float val)                                                         \
  {                                                                                     \
    if (goHdl.IsValid())                                                                \
      goHdl->GetHdl<COMP>()->VAR = val;                                                 \
  };                                                                                    \
                                                                                        \
  ASC_MTWEEN.AddTween(MTweenType, get, set, val, duration);                             \
}

#define TweenScaleFnX TweenBase(MTweenScaleX, TransformComp, scale.x)
#define TweenScaleFnY TweenBase(MTweenScaleY, TransformComp, scale.y)
#define TweenScaleFnZ TweenBase(MTweenScaleZ, TransformComp, scale.z)

#define TweenLocalScaleFnX TweenBase(MTweenLocalScaleX, TransformComp, localscale.x)
#define TweenLocalScaleFnY TweenBase(MTweenLocalScaleY, TransformComp, localscale.y)
#define TweenLocalScaleFnZ TweenBase(MTweenLocalScaleZ, TransformComp, localscale.z)

#define TweenPosFnX TweenBase(MTweenPosX, TransformComp, pos.x)
#define TweenPosFnY TweenBase(MTweenPosY, TransformComp, pos.y)
#define TweenPosFnZ TweenBase(MTweenPosZ, TransformComp, pos.z)

#define TweenLocalPosFnX TweenBase(MTweenLocalPosX, TransformComp, localpos.x)
#define TweenLocalPosFnY TweenBase(MTweenLocalPosY, TransformComp, localpos.y)
#define TweenLocalPosFnZ TweenBase(MTweenLocalPosZ, TransformComp, localpos.z)

#define TweenClrFnCR TweenBase(MTweenClrR, GraphicsComp, tint.r)
#define TweenClrFnCG TweenBase(MTweenClrG, GraphicsComp, tint.g)
#define TweenClrFnCB TweenBase(MTweenClrB, GraphicsComp, tint.b)
#define TweenClrFnCA TweenBase(MTweenClrA, GraphicsComp, tint.a)

TweenPosFnX
TweenPosFnY
TweenPosFnZ

TweenLocalPosFnX
TweenLocalPosFnY
TweenLocalPosFnZ

TweenScaleFnX
TweenScaleFnY
TweenScaleFnZ

TweenLocalScaleFnX
TweenLocalScaleFnY
TweenLocalScaleFnZ

TweenClrFnCR
TweenClrFnCG
TweenClrFnCB
TweenClrFnCA

#undef TweenPosFnX
#undef TweenPosFnY
#undef TweenPosFnZ

#undef TweenScaleFnX
#undef TweenScaleFnY
#undef TweenScaleFnZ

#undef TweenLocalScaleFnX
#undef TweenLocalScaleFnY
#undef TweenLocalScaleFnZ

#undef TweenLocalPosFnX
#undef TweenLocalPosFnY
#undef TweenLocalPosFnZ

#undef TweenPosFnCR
#undef TweenPosFnCG
#undef TweenPosFnCB
#undef TweenPosFnCA
#undef TweenBase

void MTweenLocalPos(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration)
{
  MTweenLocalPosX(MTweenType, goHdl, val.x, duration);
  MTweenLocalPosY(MTweenType, goHdl, val.y, duration);
  MTweenLocalPosZ(MTweenType, goHdl, val.z, duration);
}

void MTweenPos(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration)
{
  MTweenPosX(MTweenType, goHdl, val.x, duration);
  MTweenPosY(MTweenType, goHdl, val.y, duration);
  MTweenPosZ(MTweenType, goHdl, val.z, duration);
}

void MTweenLocalScale(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration)
{
  MTweenLocalScaleX(MTweenType, goHdl, val.x, duration);
  MTweenLocalScaleY(MTweenType, goHdl, val.y, duration);
  MTweenLocalScaleZ(MTweenType, goHdl, val.z, duration);
}

void MTweenScale(MTweenType MTweenType, GOHandle goHdl, Vec3 val, float duration)
{
  MTweenScaleX(MTweenType, goHdl, val.x, duration);
  MTweenScaleY(MTweenType, goHdl, val.y, duration);
  MTweenScaleZ(MTweenType, goHdl, val.z, duration);
}

void MTweenClr(MTweenType MTweenType, GOHandle goHdl, Vec4 val, float duration)
{
  MTweenClrR(MTweenType, goHdl, val.x, duration);
  MTweenClrG(MTweenType, goHdl, val.y, duration);
  MTweenClrB(MTweenType, goHdl, val.z, duration);
  MTweenClrA(MTweenType, goHdl, val.w, duration);
}

void MTween::Linear(MTweenInfo& info)
{
  const auto end = info.end;
  const auto duration = info.duration;
  const auto currTime = info.currTime;
  const auto change = end - info.start;
  info.set(change * (currTime / duration) + info.start);
}

void MTween::BounceEaseIn(MTweenInfo& info)
{
  const  auto c = info.end - info.start;
  BounceEaseOut(info);
  info.set(c - info.get() + info.start);
}

void MTween::BounceEaseOut(MTweenInfo& info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  if ((t /= d) < 1 / 2.75f) 
  {
    info.set(c * (7.5625f * t * t) + b);
  }
  else if (t < 2 / 2.75f) 
  {
    auto postFix = t -= 1.5f / 2.75f;
    info.set(c * (7.5625f * postFix * t + 0.75f) + b);
  }
  else if (t < 2.5 / 2.75) 
  {
    auto postFix = t -= 2.25f / 2.75f;
    info.set(c * (7.5625f * postFix) * t + 0.9375f + b);
  }
  else 
  {
    auto postFix = t -= 2.625f / 2.75f;
    info.set(c * (7.5625f * postFix * t + 0.984375f) + b);
  }
}

void MTween::ElasticEaseIn(MTweenInfo& info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  if (fabs(t) < FLT_EPSILON) 
    info.set(b);

  if (fabs((t /= d) - 1.0f) < FLT_EPSILON) 
    info.set(b + c);

  const auto p = d * 0.3f;
  const auto a = c;
  const auto s = p / 4.0f;
  const auto postFix = static_cast<float>(a * pow(2.0f, 10 * (t -= 1.0f))); // this is a fix, again, with post-increment operators
  info.set(-(postFix * sin((t * d - s) * (2.0f * ASC_PI) / p)) + b);
}

void MTween::ElasticEaseOut(MTweenInfo& info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  if (fabs(t) < FLT_EPSILON) 
    info.set(b);

  if (fabs((t /= d) - 1.0f) < FLT_EPSILON) 
    info.set(b + c);

  const auto p = d * 0.3f;
  const auto a = c;
  const auto s = p / 4.0f;
  info.set(a * pow(2.0f, -10.0f * t) * sin((t * d - s) * (2.0f * ASC_PI) / p) + c + b);
}

void MTween::BackEaseIn(MTweenInfo& info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  const auto s = 1.70158f;
  const auto postFix = t /= d;
  info.set(c * postFix * t * ((s + 1.0f) * t - s) + b);
}

void MTween::BackEaseOut(MTweenInfo& info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  const auto s = 1.70158f;
  info.set(c*((t = t / d - 1.0f) * t * ((s + 1.0f) * t + s) + 1.0f) + b);
}

void MTween::SineEaseIn(MTweenInfo &info)
{
  const auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  info.set(-c * cos(t / d * ASC_HALFPI) + c + b);
}

void MTween::SineEaseOut(MTweenInfo & info)
{
  const auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  info.set(c * sin(t / d * ASC_HALFPI) + b);
}

void MTween::CircEaseIn(MTweenInfo & info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  info.set(-c * (sqrt(1.0f - (t /= d) * t) - 1.0f) + b);
}

void MTween::CircEaseOut(MTweenInfo &info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;  //period

  info.set(c * sqrt(1.0f - (t /= d - 1.0f) * t) + b);
}

void MTween::EaseOutCubic(MTweenInfo & info)
{
  float t = info.currTime;
  float b = info.start;
  float c = info.end - b;
  float d = info.duration;  //period
  
  float ts = (t /= d)*t;
  float tc = ts*t;

  info.set(
    b + c*(-6.94749999999999f*tc*ts + 19.7925f*ts*ts + -18.69f*tc + 4.595f*ts + 2.25f*t)
  );
  //info.set(b + c*(-6.94749999999999f*tc*ts + 19.7925f*ts*ts + -18.69f*tc + 4.595f*ts + 2.25f*t));
}

void MTween::QuinticIn(MTweenInfo & info)
{
  float t = info.currTime;
  float b = info.start;
  float c = info.end - b;
  float d = info.duration;  //period

  float ts = (t /= d)*t;
  float tc = ts*t;
  info.set(b + c*(tc*ts));

}

//
//void MTween::Test(MTweenInfo & info)
//{
//  //https://chicounity3d.wordpress.com/2014/05/23/how-to-lerp-like-a-pro/
//
//  const auto t = info.currTime;
//  const auto b = info.start;
//  const auto c = info.end - b;
//  const auto d = info.duration;  //period
//  float& output = *info.start;
//  //
//
//  //const float min = 0.01;
//  //info.currTime = info.currTime > (d - min) ? 0 : t;
//
//  float ts = (t /= d)*t;
//  float tc = ts*t;
//
//  float amplitude = 1.0f;
//  float theta = (t) / d;
//  float distance = amplitude * sin(theta);
//  //transform.position = startPos + Vector3.up * distance;
//  output = b + distance;
//  //output = b + c*(20.2*tc*ts + -66.9425*ts*ts + 77.385*tc + -35.19*ts + 5.5475*t);
//}

void MTween::Custom1(MTweenInfo & info)
{
  auto t = info.currTime;
  const auto b = info.start;
  const auto c = info.end - b;
  const auto d = info.duration;

  const auto ts = (t /= d) * t;
  const auto tc = ts*t;

  //... 1....4......| ......3...|.........................2....
  info.set(b + c*(-23.495f*tc*ts + 94.735f*ts*ts + -118.785f*tc + 55.295f*ts + -6.75f*t));
}