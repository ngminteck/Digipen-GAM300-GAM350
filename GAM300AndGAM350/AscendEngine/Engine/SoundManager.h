/******************************************************************************/
/*!
\file   SoundManager.h
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
#ifndef SOUND_H
#define SOUND_H

typedef ComponentManagerBase<SoundComp>::COMPHANDLETYPE SoundHandle;

#define ASC_SOUNDMAN SoundManager::Inst()
//#define ASC_PLAY(Track_Name, GO_Handle) ASC_SOUNDMAN.Play(ASC_GETCOMP_HANDLE(SOUND, GO_Handle), Track_Name)
#define ASC_LOOP_INFINITE 0

#define ASC_PLAY(sound_str) ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), sound_str)
#define ASC_PLAY_LOOP(sound_str, LC) ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), sound_str, LC)
#define ASC_LOOP(sound_str) ASC_SOUNDMAN.Play(goHdl->GetHdl<SoundComp>(), sound_str, 0)
#define ASC_PLAY_FROMGO(sound_str, GOHANDLE, loopCount)ASC_SOUNDMAN.Play(GOHANDLE->GetHdl<SoundComp>(), sound_str, loopCount)
class SoundManager : public ComponentManagerBase<SoundComp>, public Singleton<SoundManager>
{
public:
	SoundManager();
	~SoundManager();

	//virtual void Load() override;
	virtual void UnLoad() override;

	virtual void Update(float dt) override;

	virtual BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
	virtual void Serialize(Serializer &, BaseHandle) override;
	virtual void Reflect(BaseHandle, const Reflector &) override;

	//virtual Handle CreateComponent(const GameObject &gameObject) override;
	//virtual SoundComp* GetComponent(Handle index) override;
  void DestroyComponent(BaseHandle hdl) override;
  //Handle CloneComponent(Handle hdl) override;
  //void InitComponent(const GameObject & go, Handle hdl) override;

  //function from CompManBase
  void CopyComponentToComponent(SoundComp& target, const SoundComp& source) override;

	//For Msging
	virtual void HandleMsg(MsgData_Base *);

  //Pause/Stop all tracks in component
  void Pause(const SoundHandle& comp_whandle);
  void Stop(const SoundHandle& comp_handle);
  void StopIfCan(const SoundHandle& comp_handle);

  void Play(const SoundHandle& comp_handle, std::initializer_list<const char*> list, int loopCount = 1);


  //Pause/Stop specific tracks in component
  void Play(const SoundHandle& comp_handle, const std::string& track, int loopCount = 1);
  void Pause(const SoundHandle& comp_handle, const std::string& track);
  void Stop(const SoundHandle& comp_handle, const std::string& track);
  bool IsPlaying(const SoundHandle& comp_handle, const std::string& track);

  void MuteBGM(bool mute);
  bool IsBGMMuted() const;

  float globalVolume;
  //bool globalVolumeChanged;

  void SetGlobaVolume(float v);
  float GetGlobalVolume() const;

  void SetTrackVolume(float vol, const SoundHandle& comp_handle, const std::string& track);
  float GetTrackVolume(SoundHandle hdl, const std::string& track);

  float GetTrackLength(const std::string& track);
  std::vector<std::string> const& GetAllTrack() { return audioVec; }
  int tracks_select = -1;
  void FadeTracks(SoundHandle comp, const std::string&, const std::string&);
  std::string currentBGM;
private:
  typedef std::unordered_map<std::string, FMOD::Sound*>::iterator soundMapIter;

  //for cloning 
  //Handle CloneComponent(const SoundComp& comp);
  SinfoHandle CloneSoundInfo(SinfoHandle);

  SoundInfo& FindTrack(SoundHandle comp_handle, const std::string & track);
  void CheckInput();
  void HandleState(soundMapIter iter, SoundState& state, SoundInfo&); //std::pair<std::string, SoundInfo>& );
  void PauseAll(bool pause);
	FMOD_MODE InternalGetFlag(SoundInfo& comp);
	void InitFMOD();
	void FMODErrorCheck(FMOD_RESULT result);  //only runs in debug

  void UnloadSoundMap();
  void PopulateSoundMap(MsgData_Base* m = nullptr);
  std::vector<std::string> FindAudiofiles();
  void ProcessFiles(std::vector<std::string>&);

	//FMOD_VECTOR v = { 0.0f, 0.0f, 0.0f };

  FMOD_MODE defaultFlags = FMOD_3D_LINEARROLLOFF;
	FMOD::System* system = nullptr;
	unsigned version = 0;
	int numDrivers = 0;
	FMOD_RESULT result;

  //FMOD::ChannelGroup* masterGrp = nullptr;
  std::vector<std::string> audioVec;
  //std::vector<SoundComp> compVec;
  //HandleManager<SoundComp, Handle> compVec;
	std::unordered_map<std::string, FMOD::Sound*> soundMap;

  //list of sounds in a component
  //typedef std::pair<std::string, SoundInfo> SoundPair;
  HandleManager<SoundInfo> soundInfoVec;

  //testing
  SoundComp comp;

  bool isBGMMuted;


  bool fade;
  std::string from, to;
  float timer, startVol, endVol;
  SoundHandle hand;
};

#endif
