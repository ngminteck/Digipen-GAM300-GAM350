#include "pch.h"
StopFMovin::StopFMovin() :
	ScriptCommonIF(ASC_GETSCRIPTNAME(StopFMovin))
{}
StopFMovin::~StopFMovin() {}


void StopFMovin::OnStart()
{
	MyPos = t->pos;
}
void StopFMovin::Update(float dt)
{
	np->obj_phy->BodySetLinearVel(Vec3::ZERO);
	t->pos = MyPos;
}