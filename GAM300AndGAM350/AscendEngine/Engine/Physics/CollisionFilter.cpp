#include "pch.h"

GOCollisionResponse::GOCollisionResponse()
{
	collision_response_list.push_back("IGNORE");
	collision_response_list.push_back("OVERLAPPING");
	collision_response_list.push_back("BLOCK");
}

GOCollisionResponse::~GOCollisionResponse()
{
	collision_response_list.clear();
}

GOCollisionType::GOCollisionType()
{
	collision_type_list.push_back("STATIC");
	collision_type_list.push_back("DYNAMIC");
	collision_type_list.push_back("TRIGGER");
	collision_type_list.push_back("PLAYER");
	collision_type_list.push_back("OTHER");
}

GOCollisionType::~GOCollisionType()
{
	collision_type_list.clear();
}

