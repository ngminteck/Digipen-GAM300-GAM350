#include "pch.h"

AnimationManager::AnimationManager(HandleManager<TransformComp>& transform_comps):
    m_transform(transform_comps)
{
    /*std::ifstream file;
    std::string filename = "ListOfAnimations.txt";
    file.open(ASC_ANIMATION_FLD + filename);

    if (!file.is_open()) {
        LogError("Animation List Loading Failed :", filename);
        return;
    }
    //listOfAnimation.push_back("Blank");
    std::string buffer;
    bool firstMesh = true;
    while (!file.eof())
    {
        std::getline(file, buffer);
        if (!buffer.size())
            continue;

        listOfAnimation.push_back(buffer);
    }*/
    animationTypeList.push_back("Texture");
    animationTypeList.push_back("Skeleton");
    animationTypeList.push_back("Both");
}

AnimationManager::~AnimationManager()
{

}

void AnimationManager::Load()
{
    //auto& obj = m_components.m_objects;
    //std::vector<Mesh> temp = ASC_GMAN.m_objLoader.m_objs[listOfAnimation[1]];
    
}

void AnimationManager::Update(float dt)
{
	/*AnimationHandle id = m_components.GetNewObject();
	AnimationComp& aComp = *m_components.GetObj(id);*/
	start = Clock::now();
    //Reset();
    //if (aComp.isPlaying)
    Running();
	duration = Clock::now() - start;
    
}

void AnimationManager::Reset()
{
    /*AnimationHandle id = m_components.GetNewObject();
    AnimationComp& aComp = *m_components.GetObj(id);

    if (aComp.reset)
    {
        aComp.currFrame = 0.0f;
        aComp.reset = false;

    }
    if (aComp.currFrame > aComp.maxFrame)
    {
        if (isLoop)
            aComp.currFrame = 0.0f;
        else
            aComp.isPlaying = false;
    }*/
}
void AnimationManager::Running()
{
    
    auto& objs = m_components.m_objects;
    for (AnimationComp& curr : objs)
    {
        
        if (curr.isPlaying)
        {
            
                
                    switch (curr.type)
                    {
                    case AnimationType::TEXTURE: 
                        if (curr.currTime[curr.type] > curr.maxTime[curr.type])
                        {
                            if (curr.currIndex[curr.type] < listOfAnimation[curr.GetGOHandle()->name].size())
                            {
                                TextureAnimation(curr);
                                ++curr.currIndex[curr.type];
                            }
                            else
                                curr.currIndex[curr.type] = 0;
                            curr.currTime[curr.type] = 0.0f;
                        }
                            
                        break;
                    case AnimationType::SKELETON:
                        break;
                    case AnimationType::BOTH:
                        if (curr.currTime[0] > curr.maxTime[0])
                        {
                            if (curr.currIndex[0] < listOfAnimation[curr.GetGOHandle()->name].size())
                            {

                                TextureAnimation(curr);
                                ++curr.currIndex[0];
                            }
                            else
                                curr.currIndex[0] = 0;
                            curr.currTime[0] = 0.0f;
                        }
                       


                        if (curr.currTime[curr.type] > curr.maxTime[curr.type])
                        {
                            if (curr.currIndex[curr.type] < listOfModel[curr.GetGOHandle()->name].size())
                            {
                                ModelAnimation(curr);
                                ++curr.currIndex[curr.type];
                            }
                            else
                                curr.currIndex[curr.type] = 0; 
                            
                            curr.currTime[curr.type] = 0.0f;
                        }
                       
                           
                        break;
                    }
                    
                
                
            

        }
            
         curr.currTime[0] += speed;
         curr.currTime[1] += speed;
         curr.currTime[2] += speed;


    }
    
  
}

void AnimationManager::ModelAnimation(AnimationComp& aComp)
{
    GOHandle obj = aComp.GetGOHandle();
    obj->GetHdl<GraphicsComp>()->modelfile = listOfModel[obj->name][aComp.currIndex[aComp.type]];
    
}

void AnimationManager::TextureAnimation(AnimationComp & aComp)
{
    std::string temp = "_albedo";
    GOHandle obj = aComp.GetGOHandle();
    if(aComp.type == AnimationType::BOTH)
    obj->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = listOfAnimation[obj->name][aComp.currIndex[0]] + temp;
    else
    obj->GetHdl<GraphicsComp>()->materialList[0].AlbedoMap = listOfAnimation[obj->name][aComp.currIndex[aComp.type]] + temp;

    //obj->GetHdl<GraphicsComp>()->modelfile;
}

BaseHandle AnimationManager::CreateComponent()
{
    AnimationHandle hdl = m_components.GetNewObject();
    AnimationComp& aComp = *m_components[hdl];

    return hdl;
}

BaseHandle AnimationManager::Deserialize(const GameObject& gameObject, Deserializer& deserializer)
{
	//read from file
	AnimationHandle id = m_components.GetNewObject();
	AnimationComp& aComp = *m_components.GetObj(id);
	aComp.SetGOHandle(gameObject.GetSelfHandle());
    aComp.positionHdl = Handle<TransformComp>(gameObject[TRANSFORM]);
	deserializer.Read("NameTexture", aComp.nameText);
	deserializer.Read("isPlaying", aComp.isPlaying);
	deserializer.Read("isLoop", aComp.isLoop);
	deserializer.Read("maxTime0", aComp.maxTime[0]);
	deserializer.Read("maxTime1", aComp.maxTime[1]);
	deserializer.Read("maxTime2", aComp.maxTime[2]);
	deserializer.Read("NumberOfTex", aComp.numOfTex);
	deserializer.Read("NameModel", aComp.nameModel);
    deserializer.Read("NumberOfModel", aComp.numOfModel);

    int type=0;
    deserializer.Read("Type", type);
     aComp.type = (AnimationType)type;
        
    if (aComp.type == AnimationType::TEXTURE)
    {
        numOftexture[aComp.GetGOHandle()->name] = aComp.numOfTex;
        std::vector<std::string> name;
        for (int i = 0; i < aComp.numOfTex; ++i)
            name.push_back(aComp.nameText + std::to_string(i));
        listOfAnimation[aComp.GetGOHandle()->name] = name;
    }
    if (aComp.type == AnimationType::SKELETON )
    {

    }
    if (aComp.type == AnimationType::BOTH)
    {
        //texture
        numOftexture[aComp.GetGOHandle()->name] = aComp.numOfTex;
        std::vector<std::string> nametemp;
        for (int i = 0; i < aComp.numOfTex; ++i)
            nametemp.push_back(aComp.nameText + std::to_string(i));
        listOfAnimation[aComp.GetGOHandle()->name] = nametemp;

        //model
        /*numOfModel[aComp.GetGOHandle()->name] = aComp.numOfModel;
        std::vector<std::string> temp;
        for (int i = 0; i < aComp.numOfModel; ++i)
            temp.push_back(aComp.nameModel + std::to_string(i));
        listOfModel[aComp.GetGOHandle()->name] = temp;*/
        ChangeModel(aComp.GetGOHandle()->name, aComp.nameModel, aComp.numOfModel);
    }
	return id;
}


void AnimationManager::ChangeModel(std::string nameModel, std::string name, int num)
{
    numOfModel[nameModel] = num;
    std::vector<std::string> temp;
    for (int i = 0; i < num; ++i)
        temp.push_back(name + std::to_string(i));
    listOfModel[nameModel] = temp;
}

void AnimationManager::Serialize(Serializer& serializer, BaseHandle index)
{
	//write to file
	if (m_components.m_objects.empty()) return;

	// Get the current transform to serialize
	AnimationComp& aComp = *m_components.GetObj(AnimationHandle(index));
    serializer.Write("NameTexture", aComp.nameText);
	serializer.Write("isPlaying", aComp.isPlaying);
	serializer.Write("isLoop", aComp.isLoop);
	serializer.Write("maxTime0", aComp.maxTime[0]);
	serializer.Write("maxTime1", aComp.maxTime[1]);
	serializer.Write("maxTime2", aComp.maxTime[2]);
	serializer.Write("NumberOfTex", aComp.numOfTex);
	serializer.Write("Type", aComp.type);
	serializer.Write("NameModel", aComp.nameModel);
    serializer.Write("NumberOfModel", aComp.numOfModel);
	//serializer.Write("IsEnable", aComp.Enabled);
}

void AnimationManager::Reflect(BaseHandle index, const Reflector& reflector)
{
    std::string name;
    AnimationComp& aComp = *m_components.GetObj(AnimationHandle(index));

    int type = aComp.type;
    reflector.Reflect("Type", animationTypeList, &type);
    aComp.type = (AnimationType)type;
    switch (type)
    {
    case 0: //texture
        reflector.Reflect("Name of Texture", aComp.nameText);
        reflector.Reflect("Number Of Texture", aComp.numOfTex);
        ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            numOftexture[aComp.GetGOHandle()->name] = aComp.numOfTex;
            std::vector<std::string> name;
            for (int i = 0; i < aComp.numOfTex; ++i)
                name.push_back(aComp.nameText + std::to_string(i));
            listOfAnimation[aComp.GetGOHandle()->name] = name;
        }
        break;
    case 1: //skeleton
        break;
    case 2: //both
        reflector.Reflect("Name of Texture", aComp.nameText);
        reflector.Reflect("Number Of Texture", aComp.numOfTex);
        //ImGui::SameLine();
        

        ImGui::Separator();
        reflector.Reflect("Name of Model", aComp.nameModel);
        reflector.Reflect("Number Of Model", aComp.numOfModel);
        //ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            numOftexture[aComp.GetGOHandle()->name] = aComp.numOfTex;
            std::vector<std::string> nameText;
            for (int i = 0; i < aComp.numOfTex; ++i)
                nameText.push_back(aComp.nameText + std::to_string(i));
            listOfAnimation[aComp.GetGOHandle()->name] = nameText;

            numOfModel[aComp.GetGOHandle()->name] = aComp.numOfModel;
            std::vector<std::string> name;
            for (int i = 0; i < aComp.numOfModel; ++i)
                name.push_back(aComp.nameModel + std::to_string(i));
            listOfModel[aComp.GetGOHandle()->name] = name;
        }
        break;
    }
    
	/*bool loop = aComp.isLoop;
	if (ImGui::Checkbox("Loop", &loop))
	{
		aComp.isLoop = !aComp.isLoop;
	}*/
	bool playing = aComp.isPlaying;
	if (ImGui::Checkbox("Playing Animation", &playing))
	{
		aComp.isPlaying = !aComp.isPlaying;
	}
    float maxF = 0.0f;
    ImGui::DragFloat("Maximum Time Texture", &aComp.maxTime[0], 0.01f, 0.0f, 0.0f, "%.3f");
    //ImGui::DragFloat("Maximum Time", &aComp.maxTime[1], 0.1f, 0.0f, 0.0f, "%.3f");
    ImGui::DragFloat("Maximum Time Model", &aComp.maxTime[2], 0.01f, 0.0f, 0.0f, "%.3f");
    
	//color picker
    //reflector.ColorPicker("Color", aComp.tint.m);
	//select names drop bar
	//reflector.Reflect(std::string("AlbedoMap" + index).c_str(), m_albedoTextures, &albedo_select);
	
	//line separator
	//ImGui::DragFloat2("PositionOffset", gComp.posOffset.m, 0.01f, 0.f, 0.f, "%.4f");
	//ImGui::DragFloat("DepthOffset", &gComp.posOffset.m[2], 0.01f, -1.5f, 0.5f, "%.4f");
}
void AnimationManager::InitComponent(const GameObject& go, BaseHandle hdl)
{
	AnimationComp* newcomp = m_components[AnimationHandle(hdl)];
	newcomp->SetGOHandle(go.GetSelfHandle());
	//set the transform component of the new comp game object
	//newcomp->positionHdl = Handle<TransformComp>(go[TRANSFORM]);
}

void AnimationManager::CopyComponentToComponent(AnimationComp& target, const AnimationComp& source)
{
    target = source;
}

void AnimationManager::HandleMsg(MsgData_Base*)
{

}

