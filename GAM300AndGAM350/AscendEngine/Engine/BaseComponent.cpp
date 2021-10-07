/******************************************************************************/
/*!
\file   BaseComponent.cpp
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include "pch.h"


#define dTRIMESH_ENABLED 1


Handle<TransformComp> TransformComp::GetParent() const
{
  return ASC_TRANSMAN.GetParent(*this);
}

Handle<TransformComp> TransformComp::GetChild(int index) const
{
  return ASC_TRANSMAN.GetChild(*this, index);
}

Handle<TransformComp> TransformComp::FindChild(std::string const & cname) const
{
  return ASC_TRANSMAN.FindChild(*this, cname);
}

void GraphicsComp::SetVisible(bool val)
{
  tint.a = val ? 1.0f : 0.0f;
}

bool GraphicsComp::GetVisible() const
{
  return Equal(tint.a, 1.0f);
}

void GraphicsComp::SetHighlight(bool val)
{
  isHighlighted = val;
}

void GraphicsComp::SetColor3(Vec4 c)
{
  c.a = tint.a;
  tint = c;
}


void UITextComp::SetVisible(bool val)
{
  tint.a = val ? 1.0f : 0.0f;
}

bool UITextComp::GetVisible() const
{
  return Equal(tint.a, 1.0f);
}

Handle<TransformComp> ChildIterate::Next()
{
  if (m_curr != m_pobj->m_child.end())
  {
    currTransHdl = ASC_TRANSMAN.GetTransHdl(*m_curr);
    ++m_curr;
  }
  else
    currTransHdl.AssignNewInValid();

  return currTransHdl;
}
void UITextComp::SetText(std::string&& str)
{
  text = move(str);
  ASC_UITEXT.InitFont(*this);
}

void UITextComp::SetText(const std::string & str)
{
  text = str;
  ASC_UITEXT.InitFont(*this);
}

TagComp::TagComp()
{

}

TagComp::TagComp(const TagComp& tag)
{
    tag_container.clear();

    for (auto it = tag.tag_container.begin(); it != tag.tag_container.end(); ++it)
    {
        tag_container.emplace(it->first, it->second);
    }
}

TagComp::~TagComp()
{

}



std::string TagComp::GetTagValue(std::string key)
{
    if (tag_container.find(key) != tag_container.end())
    {
        return tag_container.find(key)->second;
    }
    else
    {
        std::cout << key << " tagging is not found." << std::endl;
        return "";
    }
}

int TagComp::GetTagValueInt(std::string key)
{
    if (tag_container.find(key) != tag_container.end())
    {
        return std::stoi(tag_container.find(key)->second);
    }
    else
    {
        std::cout << key << " tagging is not found." << std::endl;
        return 0;
    }
}

double TagComp::GetTagValueDouble(std::string key)
{
    if (tag_container.find(key) != tag_container.end())
    {
        return std::stod(tag_container.find(key)->second);
    }
    else
    {
        std::cout << key << " tagging is not found." << std::endl;
        return 0.0;
    }
}

bool TagComp::GetTagValueBoolean(std::string key)
{
    if (tag_container.find(key) != tag_container.end())
    {
        std::string value = tag_container.find(key)->second;

        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });

        if (value.compare("true") == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::cout << key << " tagging is not found." << std::endl;
        return false;
    }
}

void TagComp::ChangeExistedTagKey(std::string old_key, std::string new_key)
{
    if (tag_container.find(old_key) != tag_container.end())
    {
        auto editing_pair = tag_container.extract(old_key);
        editing_pair.key() = MakeUniqueKey(new_key);
        auto no_warning = tag_container.insert(std::move(editing_pair));
    }
    else
    {
        std::cout << old_key << " tagging is not found, new tag create instead." << std::endl;
        AddTag(new_key, "");
    }
}

void TagComp::ChangeExistedTagValue(std::string key, std::string new_value)
{
    if (tag_container.find(key) != tag_container.end())
    {
        tag_container.find(key)->second = new_value;
    }
    else
    {
        std::cout << key << " tagging is not found, new tag create instead." << std::endl;
        AddTag(key, new_value);
    }
}

void TagComp::AddTag(std::string key, std::string value)
{
    tag_container.emplace(MakeUniqueKey(key), value);
}

void TagComp::DeleteTag(std::string key)
{
    if (tag_container.find(key) != tag_container.end())
    {
        tag_container.erase(tag_container.find(key));
    }
}

std::string TagComp::MakeUniqueKey(std::string key, int counter)
{
    if (tag_container.find(key) != tag_container.end())
    {
        std::string new_keyname = "";

    
       new_keyname.append(key, 0, key.find("("));
       
       std::cout << new_keyname << std::endl;



        std::stringstream new_key;
        new_key.str(std::string());
        new_key << new_keyname << "(" << counter<< ")";
        return MakeUniqueKey(new_key.str(), counter + 1);
    }
    else
    {
        return key;
    }
}


NewPhysicsComp::NewPhysicsComp()
{
    obj_phy = new  ODENode{this};
    obj_trigger = new  ODENode{ this};
    obj_ray = new  ODENode{ this };

    obj_floor_ray = new  ODENode{ this };
    obj_floor_ray_platform = new  ODENode{ this };

    obj_trigger->apply_physics_surface_collision = false;
    obj_trigger->hack_type = "TRIGGER";
    obj_trigger->GeomCollisionSetEnable(false);

    obj_ray->apply_physics_surface_collision = false;
    obj_ray->hack_type = "RAY";
    obj_ray->geometry = "RAY";
    obj_ray->CreateGeometry();
    obj_ray->GeomCollisionSetEnable(false);

    obj_floor_ray->apply_physics_surface_collision = false;
    obj_floor_ray->hack_type = "RAY";
    obj_floor_ray->geometry = "RAY";
    obj_floor_ray->CreateGeometry();
    obj_floor_ray->GeomCollisionSetEnable(false);

    obj_floor_ray_platform->apply_physics_surface_collision = false;
    obj_floor_ray_platform->hack_type = "RAYPLATFORM";
    obj_floor_ray_platform->GeomCollisionSetEnable(false);


    prevsBoolForGrav=  obj_phy->BodyGetGravityMode();

   
}




NewPhysicsComp::~NewPhysicsComp()
{

    
    DestroyPhysicsComp();
 
}

void NewPhysicsComp::DestroyPhysicsComp()
{
    if (obj_floor_ray_platform != nullptr)
    {
        //if (ASC_NEWPHYSICSMANAGER.world_space_is_alive)
        //{
        //    obj_floor_ray_platform->GeomCollisionSetEnable(false);
        //}
        delete obj_floor_ray_platform;
        obj_floor_ray_platform = nullptr;

    }

    if (obj_floor_ray != nullptr)
    {
        //if (ASC_NEWPHYSICSMANAGER.world_space_is_alive)
        //
        //{
        //    obj_floor_ray->GeomCollisionSetEnable(false);
        //}
        delete obj_floor_ray;
        obj_floor_ray = nullptr;

    }

    if (obj_ray != nullptr)
    {
        //if (ASC_NEWPHYSICSMANAGER.world_space_is_alive)
        //
        //{
        //    obj_ray->GeomCollisionSetEnable(false);
        //}
        delete obj_ray;
        obj_ray = nullptr;

    }

    if (obj_trigger != nullptr)
    {
        //if (ASC_NEWPHYSICSMANAGER.world_space_is_alive)
        //{
        //    obj_trigger->GeomCollisionSetEnable(false);
        //}
        delete obj_trigger;
        obj_trigger = nullptr;
    }

    if (obj_phy != nullptr)
    {
        //if (ASC_NEWPHYSICSMANAGER.world_space_is_alive)
        //{
        //    obj_phy->GeomCollisionSetEnable(false);
        //
        //    obj_phy->have_body = false;
        //}
        delete obj_phy;
        obj_phy = nullptr;
    }
}

bool NewPhysicsComp::RayCastObjectPhysics(const Vec3& pos, const Vec3& dir, GOHandle go, dReal length)
{
    Vec3 nDir = dir.Normalize();

    auto gameObject = ASC_FACTORY.GetGameObjectPtr(go);
    NewPhyHandle hdl = gameObject->GetHdl<NewPhysicsComp>();

    if (hdl.IsValid())
    {
        NewPhysicsComp* pc = ASC_NEWPHYSICSMANAGER.GetComponent(hdl);
        assert(pc);

        dGeomRaySet(obj_ray->geom, pos.m[0], pos.m[1], pos.m[2], dir.m[0], dir.m[1], dir.m[2]);

        dGeomEnable(obj_ray->geom);

        dContact contact;
        if (dCollide(obj_ray->geom, hdl->obj_phy->geom, 1, &contact.geom, sizeof(dContact)))
        {

            if (!obj_phy->hack_always_able_raycast)
                dGeomDisable(obj_ray->geom);
            return true;
        }
        else
        {

            if (!obj_phy->hack_always_able_raycast)
                dGeomDisable(obj_ray->geom);
            return false;
        }

    }
    return false;
}

bool NewPhysicsComp::RayCastObjectTrigger(const Vec3& pos, const Vec3& dir, GOHandle go, dReal length)
{
    Vec3 nDir = dir.Normalize();

    auto gameObject = ASC_FACTORY.GetGameObjectPtr(go);
    NewPhyHandle hdl = gameObject->GetHdl<NewPhysicsComp>();

    if (hdl.IsValid())
    {
        NewPhysicsComp* pc = ASC_NEWPHYSICSMANAGER.GetComponent(hdl);
        assert(pc);

        dGeomRaySet(obj_ray->geom, pos.m[0], pos.m[1], pos.m[2], dir.m[0], dir.m[1], dir.m[2]);

        dGeomEnable(obj_ray->geom);

        dContact contact;
        if (dCollide(obj_ray->geom, hdl->obj_trigger->geom, 1, &contact.geom, sizeof(dContact)))
        {

            if (!obj_phy->hack_always_able_raycast)
                dGeomDisable(obj_ray->geom);
            return true;
        }
        else
        {
            if (!obj_phy->hack_always_able_raycast)
              dGeomDisable(obj_ray->geom);
            return false;
        }

    }
    return false;
}


GOHandle NewPhysicsComp::RayCast(const Vec3& pos, const Vec3& dir, dContact& return_contact_info,  dReal length)
{
    GOHandle retObj;
    const Vec3 nDir = dir.Normalize();

    dGeomRaySetLength(obj_ray->geom, length);

    dGeomRaySet(obj_ray->geom, pos.m[0], pos.m[1], pos.m[2], dir.m[0], dir.m[1], dir.m[2]);

    dGeomRaySetClosestHit(obj_ray->geom, 0);

    dGeomEnable(obj_ray->geom);

   

    dContact contact[4];


    int num = dCollide(obj_ray->geom, (dGeomID)ASC_NEWPHYSICSMANAGER.space, 2, &contact[0].geom, sizeof(dContact));


    for (int i = 0; i < num; ++i)
    {
        ODENode* ode_node = static_cast<ODENode*>(dGeomGetData(contact[i].geom.g2));

        if (ode_node != nullptr)
        {
            // both not same physic comp *
            if (ode_node->npc == obj_ray->npc)
                continue;

            GOHandle handle = ode_node->npc->GetGOHandle();

            

            if(handle.IsValid())
            {
                bool gg = false;
                for (auto& it = obj_ray->ignore_collision_list.begin(); it != obj_ray->ignore_collision_list.end(); ++it)
                {
                    if (handle->name.compare(*it) == 0)
                    {
                        gg = true;
                        break;
                    }
                      
                }
                if (gg)
                {
                    continue;
                }
                else
                {
                    return_contact_info = contact[i];
                    retObj = handle;
                    break;
                }
            }
        }
    }

     dGeomDisable(obj_ray->geom);

    return retObj;
}

GOHandle NewPhysicsComp::RayCastFloor(const Vec3& pos, const Vec3& dir, dContact& return_contact_info, dReal length)
{
    GOHandle retObj;
    const Vec3 nDir = dir.Normalize();


    dGeomRaySetLength(obj_floor_ray->geom, length);

    dGeomRaySet(obj_floor_ray->geom, pos.m[0], pos.m[1], pos.m[2], dir.m[0], dir.m[1], dir.m[2]);

    



    dGeomRaySetBackfaceCull(obj_floor_ray->geom, 1);

    dGeomRaySetClosestHit(obj_floor_ray->geom, 1);

    dGeomEnable(obj_floor_ray->geom);


    dContact contact[4];


    int num = dCollide(obj_floor_ray->geom, (dGeomID)ASC_NEWPHYSICSMANAGER.space, 4, &contact[0].geom, sizeof(dContact));

    for (int i = 0; i < num; ++i)
    {
        ODENode* ode_node = static_cast<ODENode*>(dGeomGetData(contact[i].geom.g2));

        if (ode_node != nullptr)
        {
            // both not same physic comp *
            if (ode_node->npc == obj_floor_ray->npc)
                continue;

            GOHandle handle = ode_node->npc->GetGOHandle();

            if (handle.IsValid())
            {
                bool gg = false;
                for (auto& it = obj_floor_ray->ignore_collision_list.begin(); it != obj_floor_ray->ignore_collision_list.end(); ++it)
                {
                    if (handle->name.compare(*it) == 0)
                    {
                        gg = true;
                        break;
                    }

                }
                if (gg)
                {
                    continue;
                }
                else
                {
                   // std::cout << "floor ray hit:" << handle->name <<   std::endl;
                    return_contact_info = contact[i];
                    retObj = handle;
                    break;
                }
            }
        }
    }

    if (!obj_phy->hack_always_able_raycast)
        dGeomDisable(obj_floor_ray->geom);

    return retObj;
}

void NewPhysicsComp::InitForwardRay(const Vec3& pos, const Vec3& dir, dReal length)
{
    dGeomRaySet(obj_ray->geom, pos.m[0], pos.m[1], pos.m[2], dir.m[0], dir.m[1], dir.m[2]);
    dGeomRaySetLength(obj_ray->geom, length);
}

void NewPhysicsComp::InitFloorRay(const Vec3& pos, const Vec3& dir, dReal length)
{
    dGeomRaySet(obj_floor_ray->geom, pos.m[0], pos.m[1], pos.m[2], dir.m[0], dir.m[1], dir.m[2]);
    dGeomRaySetLength(obj_floor_ray->geom, length);
}




