#include "pch.h"

DirectorTrigger::DirectorTrigger():
	ScriptCommonIF(ASC_GETSCRIPTNAME(DirectorTrigger))
{

}
DirectorTrigger::~DirectorTrigger()
{
}
void DirectorTrigger::Initialize()
{

	FindDirectedObject();
}
void DirectorTrigger::OnStart()
{
	FindDirectedObject();
}
void DirectorTrigger::FindDirectedObject()
{
	if(!Multiple)
	DirectedObject = ASC_FACTORY.GetGameObjectByName(DirectObject.c_str());
	else
	{
		DirectedObjects.clear();
		for (int i = 0; i < DirectObjects.size(); i++)
		{
			GOHandle obj = ASC_FACTORY.GetGameObjectByName(DirectObjects[i].c_str());
			if(obj.IsValid())
			DirectedObjects.push_back(obj);
		}
	}

}
void DirectorTrigger::Update(float dt)
{
	if (!DirectedObject.IsValid())
	{
		DirectedObject = ASC_FACTORY.GetGameObjectByName(DirectObject);
	}
}
void DirectorTrigger::OnDestroy()
{}
void DirectorTrigger::Serialize(Serializer& sr, LogicHandle idx)
{
	sr.Write(GetName());
	sr.Write("Multiple", Multiple);
	if (!Multiple)
	{
		sr.Write("DirectedObject", DirectObject);
		sr.Write("RunOnce", runOneAction);
	}
	else
	{
		sr.Write("DirectedObject", DirectObjects);
	}
}
void DirectorTrigger::Deserialize(Deserializer& d)
{
	d.Read("Multiple", Multiple);
	if (!Multiple)
	{
		d.Read("DirectedObject", DirectObject);
		d.Read("RunOnce", runOneAction);
	}
	else
	{
		d.Read("DirectedObject", DirectObjects);
	}
}
void DirectorTrigger::Reflect(const Reflector& r)
{
	
	if (!Multiple)
	{
		ImGui::InputText("DirectedObject", &DirectObject[0], 128);
		if (ImGui::Button("Find Object", ImVec2(300, 30)))
		{
			FindDirectedObject();
		}
		if (DirectedObject.IsValid())
		{
			ImGui::Text(DirectedObject->name.c_str());

		}
	}
	else
	{
		if (ImGui::Button("Add Entry", ImVec2(300, 30)))
		{
			DirectObjects.resize(DirectObjects.size() + 1);
			DirectObjects[DirectObjects.size() - 1] = "";
		}
		for (int i = 0; i < DirectObjects.size(); i++)
		{
			ImGui::InputText("DirectedObject" + i, &DirectObjects[i][0], 128);
		}
		if (ImGui::Button("Find Objects", ImVec2(300, 30)))
		{
			FindDirectedObject();
		}
		if (DirectedObjects.size() > 0)
		{
			for (int i = 0; i < DirectObjects.size(); i++)
			{
				if (DirectedObjects[i].IsValid())
				{
					ImGui::Text(DirectedObjects[i]->name.c_str());
					ImGui::Text(" is Valid");

				}
			}
		}
	}
	ImGui::Checkbox("Multiple Directed Objects?", &Multiple);
	ImGui::Checkbox("Run One Action?", &runOneAction);
}
void DirectorTrigger::HandleMessage(MsgData_Base* m)
{

}
void DirectorTrigger::OnCollisionEnter(GOHandle other)
{
	if (other->name == "DogPlayer")
	{
		StartDirector();
	}
}

void DirectorTrigger::StartDirector()
{
	BaseTrigger* b = ASC_GETSCRIPT_PTR(BaseTrigger, goHdl);
	if (b != nullptr)
	{
		b->RoombaEnd();
	}
	if (!Multiple)
	{
		if (DirectedObject.IsValid())
		{
			Director* d = ASC_GETSCRIPT_PTR(Director, DirectedObject);
			DirectedObject->GetHdl<GraphicsComp>()->Enabled = true;
			BaseTrigger* b = ASC_GETSCRIPT_PTR(BaseTrigger, DirectedObject);
			if (b != nullptr)
			{
				b->RoombaEnd();
			}
			d->Pause = runOneAction;
			d->direct = true;
		}
	}
	else
	{

		for (int i = 0; i < DirectedObjects.size(); i++)
		{
			if (DirectedObjects[i].IsValid())
			{
				Director* d = ASC_GETSCRIPT_PTR(Director, DirectedObjects[i]);
				DirectedObjects[i]->GetHdl<GraphicsComp>()->Enabled = true;
	
				d->Pause = runOneAction;
				d->direct = true;
			}
		}
	}
}