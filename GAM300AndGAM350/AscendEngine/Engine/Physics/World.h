/******************************************************************************/
/*!
\file   World.h
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
#pragma once

void InitCollision();
void Step(float dt);
void AddBody(PhysicsComp &physicsComponent);
void AddShape(PhysicsComp &physicsComponent, ShapeType type); 
//void RemoveShape(Shape **shape);
void RemoveShape(Shape *shape);
void ClearCurrentCollision(size_t body);