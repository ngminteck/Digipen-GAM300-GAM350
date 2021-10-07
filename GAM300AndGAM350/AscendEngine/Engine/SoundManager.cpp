/******************************************************************************/
/*!
\file   SoundManager.cpp
\author Tan Hong Boon
\par    email: h.tan\@digipen.edu
\par    Digipen login: h.tan
\date   02/02/2020
\brief
Contains definitions required for SoundManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

SoundManager::SoundManager()
{
#if 0
  FMOD::Sound     *sound;
  FMOD::Channel   *channel;
  //result = system->createSound("Audio/abc.mp3", FMOD_3D, 0, &sound);
  //result = system->createSound("Audio/c.ogg", FMOD_3D, 0, &sound);
  //system->set3DSettings(1.0f, 1.0f, 1.0f);
  //mindist = stay at loudest vol from source to mindist
  //max dist = minDist to max -> attenuate following the rolloff curve set, > max -> sound will no longer attenuate
  sound->set3DMinMaxDistance(0.5f, 5.0f);
  sound->setMode(FMOD_LOOP_NORMAL | FMOD_3D_LINEARROLLOFF);
  FMODErrorCheck(result);
  system->playSound(sound, nullptr, false, &channel);
  FMOD_VECTOR l{ 0.0f, 0.0f, 0.0f };
  channel->set3DAttributes(&l, &l);

  system->set3DListenerAttributes(0, &v, nullptr, nullptr, nullptr);
#endif

  //seed
  srand(static_cast<unsigned>(time(nullptr)));

  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &SoundManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_STOP, &SoundManager::HandleMsg, this);
  ASC_MSGSYS.RegisterCallBack(mTYPE_DRAGDROP, &SoundManager::PopulateSoundMap, this);

  InitFMOD();
  PopulateSoundMap();
  //system->getMasterChannelGroup(&masterGrp);

  system->set3DNumListeners(1);
  //int num = 0; 
  //system->getSoftwareChannels(&num);

  //FMOD_VECTOR v1 = { 5.0f, 0.0f, 0.0f };
  //system->set3DListenerAttributes(1, &v1, nullptr, nullptr, nullptr);
  //system->set3DListenerAttributes(0, &v, nullptr, nullptr, nullptr);
  //soundInfoVec.reserve(2000);

  globalVolume = 1.0f;
  //globalVolumeChanged = false;
}


SoundManager::~SoundManager()
{
  UnloadSoundMap();
  system->release();
  FMODErrorCheck(result);
}

//void SoundManager::Load()
//{
//}

void SoundManager::UnLoad()
{
  m_components.Clear();
  soundInfoVec.Clear();
}

void SoundManager::Update(float dt)
{
#if 0
  //test sound
  if (ASC_KEYTRIGGERED(ASC_KEY_I))
  {
    FMOD::Channel* c = nullptr;
    soundMapIter iter = soundMap.find("SFX_Tower_frost2_pulse_01.ogg");
    system->playSound(iter->second, nullptr, false, &c);
  }
#endif

  start = Clock::now();
  //CheckInput();

  for (auto& comp : m_components.m_objects)
  {
    GOHandle h = comp.GetGOHandle();
#if _DEBUG
    //std::string name = ASC_GETGO_PTR(h)->name;
    //Log("==== SoundUpdate: ", name, " ====");
#endif
    TransformComp* t = ASC_GETCOMP_PTR(TransformComp, h);
    FMOD_VECTOR v{ t->pos.x, t->pos.y, t->pos.z };

    for (auto& idx : comp.handles)
    {
      //std::pair<std::string, SoundInfo>& 
      SoundInfo& info = *soundInfoVec[idx.second];
      if (info.sound3d)
      {
          info.ch->set3DAttributes(&v, nullptr, nullptr);
      }
      //if (globalVolumeChanged)
        info.ch->setVolume(info.volume * globalVolume);

    /*  if (info.listener)
      {
        result = system->set3DListenerAttributes(0, &v, nullptr, nullptr, nullptr);
        FMODErrorCheck(result);
      }*/

      soundMapIter iter = soundMap.find(info.name);
      if (iter != soundMap.end())
      {
        if (info.state == PROCESSED)
          continue;

        FMOD_MODE flag = InternalGetFlag(info);
        FMOD::Sound* sound = iter->second;


        FMODErrorCheck(sound->setMode(flag));
        HandleState(iter, info.state, info);
      }
      else
        assert(true);
    }
  }
  if (hand.IsValid())
  {
      if (fade)
      {
          if (timer > 0.0f)
          {
              timer -= dt;

              FindTrack(hand, from).ch->setVolume((timer * startVol));

          }
          else
          {
              Stop(hand, from);
              Play(hand, to, 0);
              //ASC_LOOP(to);
              timer = 1.0f;
              fade = false;
              //Special
              if (to == "BGM_FireTime.ogg")
              {
                  currentBGM = to;
              }

          }
      }
      if (timer > 0.0f && !fade)
      {
          timer -= dt;
          FindTrack(hand, to).ch->setVolume((1.0f - timer) * endVol);
      }
  }
  auto& cam = ASC_GMAN.GetCamera(GraphicsManager::CameraIdx::PLAYER_CAM);
  FMOD_VECTOR pos = { cam.GetPos().x, cam.GetPos().y, cam.GetPos().z };
  FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
  FMOD_VECTOR forward = { -cam.GetForward().x, -cam.GetForward().y, -cam.GetForward().z };
  FMOD_VECTOR up = { cam.GetUp().x, cam.GetUp().y, cam.GetUp().z };

  //globalVolumeChanged = false;

  auto res = system->set3DListenerAttributes(0, &pos, &vel, &forward, &up);     // update 'ears'
  //Log(int(res));
  //fmod update
  system->update();

  duration = Clock::now() - start;

}
void SoundManager::FadeTracks(SoundHandle s, const std::string& fr, const std::string& tt)
{
    fade = true;
    hand = s;
    from = fr;
    to = tt;
    timer = 1.0f;
    FindTrack(s, fr).ch->getVolume(&startVol);
    FindTrack(s, tt).ch->setVolume(0.0f);
    FindTrack(s, tt).ch->getVolume(&endVol);

}
BaseHandle SoundManager::Deserialize(const GameObject & gameObject, Deserializer & deserializer)
{
#define REMOVE_IDX_INVALID -1
#define SERIAL_TRACK "Track"
#define SERIAL_TYPE "Type"
#define SERIAL_3D "sound3d"
#define SERIAL_VOL "Volume"
#define SERIAL_PITCH "Pitch"
#define SERIAL_LISTENER "Listener"
#define SERIAL_MIN_DIST "minDist"
#define SERIAL_MAX_DIST "maxDist"
#define SERIAL_NO_DIE "noDie"
#define SERIAL_LOOPCOUNT "loopCount"

  //load from scene file, for each object
  int count = 0;
  const char* name("Audio");
  //name += std::to_string(++count);

  SoundHandle idx(CreateComponent());
  SoundComp& tmp = *m_components[idx];
  tmp.SetGOHandle(gameObject.GetSelfHandle());

  std::string output_name;
  int output_type;
  bool output_sound3d;
  float output_volume;
  float output_pitch;
  //bool output_listener;
  float output_min;
  float output_max;
  bool output_noDie;
  int output_loopCount;

  //this is audio
  tinyxml2::XMLElement *next = deserializer.m_xmlElem.FirstChildElement();

  SoundInfo info;

  //hardcoded
  GOTYPE type = ASC_GETGO_PTR(tmp.GetGOHandle())->GetType();
  if (type == go_player)
  {
    info.listener = true;
    if (!next)
    {
      tmp.handles.insert({ info.name, soundInfoVec.GetNewObject(info) });
      return idx;
    }
  }

  while (next)
  {
    //Deserializer d1(*next);

    output_name = next->Attribute(SERIAL_TRACK);
    output_type = next->IntAttribute(SERIAL_TYPE);
    output_sound3d = next->BoolAttribute(SERIAL_3D);
    output_volume = next->FloatAttribute(SERIAL_VOL);
    output_pitch = next->FloatAttribute(SERIAL_PITCH);
    output_min = next->FloatAttribute(SERIAL_MIN_DIST);
    output_max = next->FloatAttribute(SERIAL_MAX_DIST);
    output_noDie = next->BoolAttribute(SERIAL_NO_DIE);
    output_loopCount = next->IntAttribute(SERIAL_LOOPCOUNT);
    //output_listener = next->BoolAttribute(SERIAL_LISTENER);

    info.name = output_name;
    info.type = (SoundType)output_type;
    info.sound3d = output_sound3d;
    info.volume = output_volume;
    info.pitch = output_pitch;
    info.minDist = output_min;
    info.maxDist = output_max;
    info.noDie = output_noDie;
    info.loopCount = output_loopCount;
    //info.listener = output_listener;

    //soundInfoVec.push_back({ output_name, info });
    //tmp.handles.push_back((int)soundInfoVec.size() - 1);
    //tmp.handles.push_back(soundInfoVec.GetNewObject(info));
    tmp.handles.insert({ info.name, soundInfoVec.GetNewObject(info) });

    result = soundMap[output_name]->set3DMinMaxDistance(output_min* 4.0f, output_max * 4.5f);
    FMODErrorCheck(result);

    //this is audio sibilings
    next = next->NextSiblingElement();
  }
  return idx;
}

void SoundManager::Serialize(Serializer & serializer, BaseHandle idx)
{
  //saving to file for each object

  //if (idx.Val() >= m_components.size() || m_components.empty()) return;

  //get component to serialize
  SoundComp& tmp = *m_components[SoundHandle(idx)];

  int count = 0;
  for (auto& index : tmp.handles)
  {
    SinfoHandle& h = index.second;
    std::string name("Audio");
    //name += std::to_string(++count);

    //this is audio
    tinyxml2::XMLElement& newelem = serializer.CreateNewChild(name.data());

    //SoundPair& pair = soundInfoVec[index];
    SoundInfo& info = *soundInfoVec[h];

    if (info.inUse)
    {
      newelem.SetAttribute(SERIAL_TRACK, info.name.c_str());
      newelem.SetAttribute(SERIAL_TYPE, (int)info.type);
      newelem.SetAttribute(SERIAL_3D, info.sound3d);
      newelem.SetAttribute(SERIAL_VOL, info.volume);
      newelem.SetAttribute(SERIAL_PITCH, info.pitch);
      newelem.SetAttribute(SERIAL_MIN_DIST, info.minDist);
      newelem.SetAttribute(SERIAL_MAX_DIST, info.maxDist);
      newelem.SetAttribute(SERIAL_NO_DIE, info.noDie);
      newelem.SetAttribute(SERIAL_LOOPCOUNT, info.loopCount);
      //newelem.SetAttribute(SERIAL_LISTENER, info.listener);
    }
  }

  //serializer.m_xmlElem.SetAttribute("count", (int)tmp.handles.size());
}

void SoundManager::Reflect(BaseHandle index, const Reflector &r)
{
  SoundHandle idx(index);
  if (idx.Val() > m_components.m_objects.size()) return;

  //idx is index to component
  SoundComp &sComp = *m_components[idx];

  //std::vector<const char*>names;
  std::vector<std::string>names;
  //for (unsigned int i = 0; i < sComp.handles.size(); ++i)
  for (auto& elem : sComp.handles)
  {
    SinfoHandle& h = elem.second;
    //auto& idx = sComp.handles[i];
    //SoundPair& pair = soundInfoVec[sComp.handles[i]];
    SoundInfo& info = *soundInfoVec[h];

    if (!info.inUse)
      continue;
    names.push_back(info.name);
  }

  //static std::vector<int> listsoundpair;
  static int old_tracks_select = -1;
  static int type_idx = 1;
  static int sound3d_idx = 1;
  static int selectedIdx = 0;
  static bool retrieved = false;

  std::vector<std::string> option3d{ "2D", "3D" };
  //std::vector<std::string> type{ "INVALID", "BGM", "SFX" };
  std::vector<std::string> type{ "BGM", "SFX" };

  ImGui::PushID(12);
  r.Reflect("", audioVec, &selectedIdx);
  ImGui::PopID();
  ImGui::SameLine();
  //add new sound with default settings
  if (r.Reflect("+ Track"))
  {
    SoundInfo info;

    info.name = audioVec[selectedIdx];
    //SoundPair pair{ audioVec[selectedIdx], info };
    // .push_back(pair);
    //sComp.handles.push_back((int)(soundInfoVec.size() - 1));
    //sComp.handles.push_back(soundInfoVec.GetNewObject(info));
    sComp.handles.insert({ info.name, soundInfoVec.GetNewObject(info) });
    tracks_select = (int)(names.size());
    type_idx = 0;
    sound3d_idx = 1;

    //update names vector
    names.push_back(info.name.data());
  }

  ImGui::Separator();
  ImGui::Separator();

  //ImGui::Combo(
  //  ("(" + std::to_string(names.size()) + ") Tracks").data(),
  //  &tracks_select, names.data(), (int)names.size());
  std::string label = "(" + std::to_string(names.size()) + ") Tracks";
  r.Reflect(label.data(),
    names, &tracks_select);
  if (sComp.handles.size())
  {
    //if there's a change in track selected, update values
    if (old_tracks_select != tracks_select)
    {
      retrieved = false;
      old_tracks_select = tracks_select;
    }

    if (tracks_select >= (int)sComp.handles.size())
      tracks_select = (int)sComp.handles.size() - 1;

    //editing added sound
    if (tracks_select >= 0)
    {
      std::string editTrack = names[tracks_select];
      SoundInfo& info = *soundInfoVec[sComp.handles[editTrack]];

      if (info.inUse)
      {
        if (!retrieved)
        {
          type_idx = (int)info.type;
          sound3d_idx = (info.sound3d == false ? 0 : 1);
          retrieved = true;
        }

        r.Reflect("2d/3d", option3d, &sound3d_idx);
        info.sound3d = (sound3d_idx == 0 ? false : true);
        r.Reflect("Type", type, &type_idx);
        info.type = (SoundType)(type_idx);
        ImGui::SliderFloat("Pitch Scale", &info.pitch, -5.0f, 5.0f, "%.1f", 1.0f);
        ASC_EDITOR.HelpMarker("Scales existing frequency values by Pitch Scale", true);
        ImGui::SliderFloat("Volume", &info.volume, -5.0f, 5.0f, "%.1f", 1.0f);
        ASC_EDITOR.HelpMarker("Volume level can be below 0 to invert a signal and above 1 to amplify the signal", true);
        ImGui::SliderFloat("Min", &info.minDist, 0.0f, 300.0f, "%.1f", 1.0f);
        ASC_EDITOR.HelpMarker("Stay at loudest vol from source to min dist", true);
        ImGui::SliderFloat("Max", &info.maxDist, 5.0f, 400.0f, "%.1f", 1.0f);
        ASC_EDITOR.HelpMarker("Min to Max -> attenuate following the rolloff curve set\n Above max -> sound will no longer attenuate", true);
        //ImGui::Checkbox("Listener", &info.listener);
        //ASC_EDITOR.HelpMarker("3D sound listener", true);

        ImGui::Checkbox("NoStop", &info.noDie);
        ASC_EDITOR.HelpMarker("Don't stop sound upon death", true);

        if (ImGui::Button("Edit done"))
        {
          tracks_select = -1;

          retrieved = false;
        }
        ImGui::SameLine();

        if (ImGui::Button("- Track"))
        {
          info.inUse = false;
          //std::vector<SinfoHandle>::iterator iter = 
          auto iter = sComp.handles.begin();
          std::advance(iter, tracks_select);
          sComp.handles.erase(iter);
          tracks_select = -1;
        }
        ImGui::SameLine();

        static std::string buttonPreview = "Preview";
        bool playing = false;
        info.ch->isPlaying(&playing);
        if (!playing)
        {
          buttonPreview = "Preview";
        }
        else
        {
          buttonPreview = "Stop";
        }

        if (ImGui::Button(buttonPreview.data()))
        {
          info.state = playing ? STOP : PLAY;
          buttonPreview = playing ? "Stop" : "Preview";
        }
        ASC_EDITOR.ItemHoveredToolTip("Not playing? Make sure game is running!");

      }
    }

  }
  names.clear();
}

//Handle SoundManager::CreateComponent(const GameObject & gameObject)
//{
//  SoundComp soundcomp;
//  //m_components.push_back(soundcomp);
//  return m_components.GetNewObject(std::move(soundcomp));
//  //return (int)(m_components.size() - 1);
//}

//SoundComp * SoundManager::GetComponent(Handle index)
//{
//  return m_components[index];
//}

void SoundManager::DestroyComponent(BaseHandle hdl)
{
  SoundHandle shdl(hdl);
  SoundComp& sc = *m_components.GetObj(shdl);
  StopIfCan(shdl);
  for (auto& sch : sc.handles)
  {
    soundInfoVec.ReleaseObj(sch.second);
  }
  m_components.ReleaseObj(shdl);
}

//Handle SoundManager::CloneComponent(Handle hdl)
//{
//  Handle chdl = CloneComponent(*m_components.GetObj(hdl));
//  return chdl;
//}

//void SoundManager::InitComponent(const GameObject & go, Handle hdl)
//{
//  m_components[hdl]->SetGOHandle(go.GetSelfHandle());
//}

void SoundManager::CopyComponentToComponent(SoundComp & target, const SoundComp & source)
{
  //need to clear because the new component is still holding the old comp Sinfohandle
  target.handles.clear();
  for (auto& elem : source.handles)
  {
    SinfoHandle shdl = elem.second;
    //target.handles.push_back(CloneSoundInfo(shdl));
    target.handles.insert({ elem.first, CloneSoundInfo(shdl) });
  }
}

void SoundManager::HandleMsg(MsgData_Base* msg)
{
  //if game is paused. Does not change the State in Sound Component
  switch (msg->type)
  {
    case mTYPE_STOP:
      PauseAll(true);
      break;
    case mTYPE_PLAY:
      PauseAll(false);
      break;
    default:
      break;
  }

}

static SoundInfo soundInfo;
SoundInfo & SoundManager::FindTrack(SoundHandle comp_handle, const std::string & track)
{
  SoundComp& comp = *m_components[comp_handle];
#if _DEBUG
  assert(comp.handles.size() > 0);
#endif
  SoundComp::sIter iter = comp.handles.find(track);
  if (iter != comp.handles.end())
  {
    SinfoHandle& info_handle = iter->second;
    SoundInfo& info = *soundInfoVec[info_handle];
    return info;
  }
  else
  {  // Reach here if we cant find the file
      SoundInfo info;

      info.name = track;
      comp.handles.insert({ info.name, soundInfoVec.GetNewObject(info) });
      SoundComp::sIter iter = comp.handles.find(track);
      if (iter != comp.handles.end())
      {
          SinfoHandle& info_handle = iter->second;
          SoundInfo& info = *soundInfoVec[info_handle];
          return info;
      }
  }
  //
#if !_SUBMISSION
  //shouldnt reach here, 
    //1) did the input string has .ogg extension? 
    //2) did you include in prefab?
    //3) is the sound file in the correct directory?
    //4) is your min and max dist correct?
    //5) 'Count' in xml does not matter!
 // assert(false);
  return *soundInfoVec[SinfoHandle()];
#else
  return soundInfo;
#endif
}

void SoundManager::Play(const SoundHandle & comp_handle, std::initializer_list<const char*> list, int loopCount)
{
    if (comp_handle.IsValid())
    {
        int idx = rand() % list.size();
        auto iter = list.begin();

        std::advance(iter, idx);
        Play(comp_handle, *iter, loopCount);
    }
}

void SoundManager::Play(const SoundHandle& comp_handle, const std::string & track, int loopCount)
{

    if (comp_handle.IsValid())
    {
        if (isBGMMuted &&
            track == "BGM_ambience_01.ogg" ||
            track == "BGM_level_start.ogg" ||
            track == "BGM_wave_start.ogg")
        {
            return;
        }

        SoundInfo& info = FindTrack(comp_handle, track);
        info.loopCount = loopCount;
        info.state = PLAY;
        info.sound3d = false;
        if (loopCount == 0 || track == "SFX_Hover.ogg")
        {
          std::cout <<" is playng :"<< comp_handle->GetGOHandle()->name << std::endl;

        }
        info.ch->set3DMinMaxDistance(info.minDist, info.maxDist);
    }
}

void SoundManager::Pause(const SoundHandle& comp_handle)
{
  SoundComp& comp = *m_components[comp_handle];
  for (auto& track : comp.handles)
  {
    SinfoHandle& info_handle = track.second;
    SoundInfo& info = *soundInfoVec[info_handle];
    info.state = PAUSE;
  }
}

void SoundManager::Pause(const SoundHandle& comp_handle, const std::string & track)
{
  SoundInfo& info = FindTrack(comp_handle, track);
  info.state = PAUSE;
}

void SoundManager::Stop(const SoundHandle& comp_handle)
{
  SoundComp& comp = *m_components[comp_handle];
  for (auto& track : comp.handles)
  {
    SinfoHandle& info_handle = track.second;
    SoundInfo& info = *soundInfoVec[info_handle];

    soundMapIter iter = soundMap.find(info.name);
    if (iter != soundMap.end())
    {
      SoundState stop = STOP;
      HandleState(iter, stop, info);
    }
  }
}

void SoundManager::StopIfCan(const SoundHandle& comp_handle)
{
  SoundComp& comp = *m_components[comp_handle];
  for (auto& track : comp.handles)
  {
    SinfoHandle& info_handle = track.second;
    SoundInfo& info = *soundInfoVec[info_handle];
    if (!info.noDie)
    {
      soundMapIter iter = soundMap.find(info.name);
      if (iter != soundMap.end())
      {
        SoundState stop = STOP;
        HandleState(iter, stop, info);
      }
    }
#if !_SUBMISSION
    else
    {
      //LogWarning("noDie is active");
    }
#endif
  }
}

void SoundManager::Stop(const SoundHandle& comp_handle, const std::string & track)
{
  SoundInfo& info = FindTrack(comp_handle, track);
  soundMapIter iter = soundMap.find(info.name);
  if (iter != soundMap.end())
  {
    SoundState stop = SoundState::STOP;
    HandleState(iter, stop, info);
  }
}

bool SoundManager::IsPlaying(const SoundHandle & comp_handle, const std::string & track)
{
  SoundInfo& info = FindTrack(comp_handle, track);
  FMOD::Sound* s = nullptr;
  bool isPlaying = false;
  info.ch->isPlaying(&isPlaying);

  return isPlaying;
}

void SoundManager::MuteBGM(bool mute)
{
  isBGMMuted = mute;
  for (auto &elem : m_components.m_objects)
  {
    for (auto handle : elem.handles)
    {
      SoundInfo& info = *soundInfoVec[handle.second];
      if (info.type == BGM)
        //FMODErrorCheck(info.ch->setVolume(mute ? 0.f : 1.f));
        FMODErrorCheck(info.ch->setMute(mute));
    }
  }

}

bool SoundManager::IsBGMMuted() const
{
  return isBGMMuted;
}

void SoundManager::SetGlobaVolume(float v)
{
  //globalVolumeChanged = true;
  globalVolume = v < 0.0f ? 0.0f : v;
}

float SoundManager::GetGlobalVolume() const
{
  return globalVolume;
}
float SoundManager::GetTrackLength(const std::string& track)
{
    unsigned int val;
    float time;
    auto sound = soundMap.find(track);
    if (sound != soundMap.end())
    {
        sound->second->getLength(&val, FMOD_TIMEUNIT_MS);
        time = (val * 0.001f);
        return time;
    }
    else
        return 0.0f;
    //return
}
void SoundManager::SetTrackVolume(float vol, const SoundHandle & comp_handle, const std::string & track)
{
	SoundInfo& info = FindTrack(comp_handle, track);
	info.volume = vol < 0.0f ? 0.0f : vol;
}

float SoundManager::GetTrackVolume(SoundHandle hdl, const std::string & track)
{
  SoundInfo& info = FindTrack(hdl, track);
  return info.volume;
}



void SoundManager::PauseAll(bool pause)
{
  for (auto& c_elem : m_components.m_objects)
  {
    for (auto & idx : c_elem.handles)
    {
      SinfoHandle& h = idx.second;
      SoundInfo& pair = *soundInfoVec[h];
      FMOD::Channel* ch = pair.ch;
      ch->setPaused(pause);
    }
  }
}

FMOD_MODE SoundManager::InternalGetFlag(SoundInfo& info)
{
  FMOD_MODE flag = 0;
  if (info.type == SoundType::BGM)
    flag |= FMOD_LOOP_NORMAL;
  if (info.sound3d)
    flag |= FMOD_3D;
  else 
    flag |= FMOD_2D;

  //hardcoded
  flag |= defaultFlags;

  return flag;
}

void SoundManager::InitFMOD()
{
  FMODErrorCheck(FMOD::System_Create(&system));      // Create the main system object.
  FMODErrorCheck(system->init(512, FMOD_INIT_NORMAL, 0));    // Initialize FMOD.
  system->getVersion(&version);

  if (version < FMOD_VERSION)
  {
    std::cout << "Error! You are using an old version of FMOD "
      << version
      << ". This program requires "
      << FMOD_VERSION
      << std::endl;
  }

  // Get number of sound cards
  result = system->getNumDrivers(&numDrivers);
  FMODErrorCheck(result);

  // No sound cards (disable sound)
  if (numDrivers == 0)
  {
    result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
    FMODErrorCheck(result);
  }

  std::cout << "FMOD started successfully\n";
  // At least one sound card
  //else
  //{
  //  // Get the capabilities of the default (0) sound card
  //  result = system->getDriverCaps(0, &caps, 0, &speakerMode);
  //  FMODErrorCheck(result);

  //  // Set the speaker mode to match that in Control Panel
  //  result = system->setSpeakerMode(speakerMode);
  //  FMODErrorCheck(result);

  //  // Increase buffer size if user has Acceleration slider set to off
  //  if (caps & FMOD_CAPS_HARDWARE_EMULATED)
  //  {
  //    result = system->setDSPBufferSize(1024, 10);
  //    FMODErrorCheck(result);
  //  }

  //  // Get name of driver
    //char name[256];
    //result = system->getDriverInfo(0, name, 256, 0);
  //  FMODErrorCheck(result);

  //  // SigmaTel sound devices crackle for some reason if the format is PCM 16-bit.
  //  // PCM floating point output seems to solve it.
  //  if (strstr(name, "SigmaTel"))
  //  {
  //    result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
  //    FMODErrorCheck(result);
  //  }
  //}
}

void SoundManager::UnloadSoundMap()
{
  for (auto& elem : soundMap)
  {
    result = elem.second->release();
    FMODErrorCheck(result);
  }
  soundMap.clear();
}

void SoundManager::PopulateSoundMap(MsgData_Base* m)
{
  audioVec = FindAudiofiles();
  ProcessFiles(audioVec);
}

std::vector<std::string>  SoundManager::FindAudiofiles()
{
  return GetFilesInDir(ASC_AUDIOS_FLD, "*");
}

void SoundManager::ProcessFiles(std::vector<std::string>& vec)
{
  UnloadSoundMap();

  for (auto& filename : vec)
  {
    std::string path(ASC_AUDIOS_FLD + filename);
    FMOD::Sound* sound;
    FMODErrorCheck(system->createSound(path.data(), 0, 0, &sound));
    soundMap.insert({ filename, sound });

    //hardcoded
    //float minDist, max;
    //sound->get3DMinMaxDistance(&minDist, &max);
    //sound->set3DMinMaxDistance(5.0f, 10.0f);
  }
}

void SoundManager::FMODErrorCheck(FMOD_RESULT result)
{
  //assert(result == FMOD_ERR_CHANNEL_STOLEN);

  if (result != FMOD_OK)
  {
    std::string error = FMOD_ErrorString(result);
    LogError(error);
  }
#if !_SUBMISSION
  //why ? 
 // assert(result == FMOD_OK || result == FMOD_ERR_CHANNEL_STOLEN);
#endif

}

//Handle SoundManager::CloneComponent(const SoundComp & comp)
//{
//  Handle chdl = m_components.GetNewObject(comp);
//  SoundComp* newcomp = m_components[chdl];
//  //need to clear because the new component is still holding the old comp Sinfohandle
//  newcomp->handles.clear();
//  for (SinfoHandle shdl : comp.handles)
//  {
//    newcomp->handles.push_back(CloneSoundInfo(shdl));
//  }
//
//  return chdl;
//}

SinfoHandle SoundManager::CloneSoundInfo(SinfoHandle hdl)
{
  SinfoHandle shdl = soundInfoVec.MakeCopy(hdl);
  SoundInfo* info = soundInfoVec.GetObj(shdl);

  info->state = PROCESSED;
  info->ch = nullptr;

  return shdl;
}


void SoundManager::CheckInput()
{
#if 0
  if (ASC_KEYDOWN(ASC_KEY_F3))
  {
    //v.x = (float)sin(t * 0.05f) * 24.0f;
    v.x += 0.1f;
    result = system->set3DListenerAttributes(0, &v, nullptr, nullptr, nullptr);
    Log("Sound :", v.x, v.y, v.z);
  }

  if (ASC_KEYDOWN(ASC_KEY_F4))
  {
    v.x -= 0.1f;
    result = system->set3DListenerAttributes(0, &v, nullptr, nullptr, nullptr);
    Log("Sound :", v.x, v.y, v.z);
  }

  if (ASC_KEYDOWN(ASC_KEY_Z))
  {
    //v.x = (float)sin(t * 0.05f) * 24.0f;
    v.x += 0.1f;
    result = system->set3DListenerAttributes(1, &v, nullptr, nullptr, nullptr);
    Log("Sound :", v.x, v.y, v.z);
  }

  if (ASC_KEYDOWN(ASC_KEY_X))
  {
    v.x -= 0.1f;
    result = system->set3DListenerAttributes(1, &v, nullptr, nullptr, nullptr);
    Log("Sound :", v.x, v.y, v.z);
  }
#endif
#if 0
  if (ASC_KEYTRIGGERED(KEY_F6) && soundInfoVec.m_objects.size())
  {
    //soundInfoVec.m_objects[0].state = PLAY;
    //soundInfoVec[0].state = PLAY;
    Log("F6 play");
  }
  if (ASC_KEYTRIGGERED(KEY_F7) && soundInfoVec.m_objects.size())
  {
    soundInfoVec[0]->state = PAUSE;
    //soundInfoVec[1].state = PAUSE;
    Log("F7 pause");
  }
#endif
}

void SoundManager::HandleState(soundMapIter iter, SoundState& state, SoundInfo& elem)
//std::pair<std::string, SoundInfo>& elem)
{
  SoundInfo& info = elem;
  //FMOD::Channel* ch = info.ch;
  switch (state)
  {
    case PLAY:
    {
      if (fabs(globalVolume) < 0.1f)
      {
        return;
      }

      iter->second->setMode(FMOD_LOOP_NORMAL);
      iter->second->setLoopCount(info.loopCount - 1);
      system->playSound(iter->second, nullptr, false, &info.ch);
      info.ch->setPitch(info.pitch);
      info.ch->setVolume(info.volume);

      state = PROCESSED;

      //Log("Sound Played : ", iter->first);
      break;
    }

    case PAUSE:
    {
      info.ch->setPaused(true);
      state = PROCESSED;

      //Log("Sound Paused : ", iter->first);
      break;
    }

    case STOP:
    {
      result = info.ch->stop();
      //result = ch->stop();
      bool isplaying = true;
      info.ch->isPlaying(&isplaying);
      assert(isplaying == false);

      state = PROCESSED;
      //Log("Sound Stopped : ", iter->first);
      break;
    }
    case LOOP:
    {
      if (!info.ch)
      {
        iter->second->setMode(FMOD_LOOP_NORMAL);
        iter->second->setLoopCount(0);
        system->playSound(iter->second, nullptr, false, &info.ch);
      }
      //FMODErrorCheck(info.ch->setMode(a | FMOD_LOOP_NORMAL));
      state = PROCESSED;
      break;
    }

    default:
      break;
  }
}

