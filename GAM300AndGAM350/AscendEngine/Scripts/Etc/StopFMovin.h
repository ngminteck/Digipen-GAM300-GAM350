#pragma once

class StopFMovin : public SCRIPTING::ScriptCommonIF<StopFMovin>
{
private:
	std::string scriptName = "StopFMovin";
public:
    void Initialize() {}
    StopFMovin();
    ~StopFMovin();
    Vec3 MyPos;
    void OnStart() override;
    void Update(float dt) override;


};
