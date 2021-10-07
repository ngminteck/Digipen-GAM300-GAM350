#include "pch.h"

DialogBoxSystem::DialogBoxSystem()
{

}

DialogBoxSystem::~DialogBoxSystem()
{

}

void DialogBoxSystem::Load()
{

}

void DialogBoxSystem::UnLoad()
{
}

void DialogBoxSystem::Update(float dt)
{
}

BaseHandle DialogBoxSystem::Deserialize(const GameObject& gameObject, Deserializer& deser)
{


  /*  auto taghandle  (CreateComponent());
    auto &tagComp = *GetComponent(taghandle);
    tagComp.SetGOHandle(gameObject.GetSelfHandle());
    
    tagComp.SetSelfHandle(Handle<TagComp>(gameObject[TAG]));

    deser.Read("TagComp", tagComp.tag_container);
   */
    

  return BaseHandle{};
}

void DialogBoxSystem::Serialize(Serializer& serializer, BaseHandle handle)
{
	//if (m_components.m_objects.empty()) return;

	//TagComp &tagComp = *GetComponent(handle);

	//int index = 1;
	////for (auto it = tagComp.tag_container.begin(); it != tagComp.tag_container.end(); ++it)
	////{
	////	std::stringstream elemname;
	////	elemname.str(std::string());
	////	elemname << index;

	//////	serializer.Write(elemname.str().c_str(),it->first,it->second);
	////	++index;
	//}
    //serializer.Write("TagComp",  tagComp.tag_container);

}

void DialogBoxSystem::Reflect(BaseHandle hdl, const Reflector& reflector)
{
	//TagComp& tagComponent = *m_components[TagHandle(hdl)];

	//ImGui::Columns(3);

	//ImGui::TextColored({ 0,10,255, 1 }, "Key");
	//ImGui::NextColumn();
	//ImGui::TextColored({ 0,10,255, 1 }, "value");
	//ImGui::NextColumn();

	//ImGui::NextColumn();

	//ImGui::Separator();

	//std::string nk = "";
	//std::string nv = "";

	//static char ck[128] = "";
	//static char cv[128] = "";

	//ImGui::InputText("##Add Key", ck, IM_ARRAYSIZE(ck));
	//ImGui::NextColumn();
	//ImGui::InputText("##Add Value", cv, IM_ARRAYSIZE(cv));
	//ImGui::NextColumn();
	//nk = ck;
	//nv = cv;
	//if (ImGui::Button("Add Tag", ImVec2{ 150, 0 }))
	//{
	//	tagComponent.AddTag(nk, nv);
	//}
	//ImGui::NextColumn();

	//ImGui::Separator();

	//std::string key = "";
	//std::string newkey = "";
	//std::string newvalue = "";

	//int function = 0;

	//for (auto it = tagComponent.tag_container.begin(); it != tagComponent.tag_container.end(); ++it)
	//{
	//	std::stringstream edit_key_label;
	//	edit_key_label.str(std::string());

	//	std::stringstream edit_value_label;
	//	edit_value_label.str(std::string());

	//	std::stringstream delete_label;
	//	delete_label.str(std::string());

	//	edit_key_label << "##Edit Key" << it->first;
	//	edit_value_label << "##Edit Value" << it->first;
	//	delete_label << "Delete Tag##" << it->first;
	//
	//	char k[128] = "";
	//	strcpy(k, it->first.c_str());
	//	if (ImGui::InputText(edit_key_label.str().c_str(), k, IM_ARRAYSIZE(k)))
	//	{
	//		function = 1;
	//		key = it->first;
	//		newkey = k;
	//	}

	//	ImGui::NextColumn();

	//	char v[128] = "";
	//	strcpy(v, it->second.c_str());

	//	if (ImGui::InputText(edit_value_label.str().c_str(), v, IM_ARRAYSIZE(v)))
	//	{
	//		function = 2;
	//		key = it->first;
	//		newvalue = v;
	//	}


	//	ImGui::NextColumn();

	//	if (ImGui::Button(delete_label.str().c_str(), ImVec2{ 150, 0 }))
	//	{
	//		function = 3;
	//		key = it->first;

	//	}
	//	ImGui::NextColumn();
	//	ImGui::Separator();

	//	
	//}
	//switch (function)
	//{
	//case 1:
	//	tagComponent.ChangeExistedTagKey(key, newkey);
	//	break;

	//case 2:
	//	tagComponent.ChangeExistedTagValue(key, newvalue);
	//	break;

	//case 3:
	//	tagComponent.DeleteTag(key);
	//	break;

	//}
	//ImGui::Columns(1);
}

/*
BaseHandle DialogBoxSystem::CloneComponent(BaseHandle)
{
	return BaseHandle();
}
*/
BaseHandle DialogBoxSystem::CreateComponent()
{


  return  BaseHandle{};
}

void DialogBoxSystem::DestroyComponent(BaseHandle hdl)
{
	/*TagHandle thdl(hdl);

	TagComp& tagComponent = *m_components[thdl];

	tagComponent.tag_container.clear();

	m_components.ReleaseObj(thdl);*/
}

void DialogBoxSystem::InitComponent(const GameObject& go, BaseHandle hdl)
{
}



void DialogBoxSystem::HandleMsg(MsgData_Base*)
{
}
