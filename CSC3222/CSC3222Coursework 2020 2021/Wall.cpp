#include "Wall.h"

NCL::CSC3222::Wall::Wall() : SimObject("Wall")
{
    isStatic = true;
    inverseMass = 0;
}

NCL::CSC3222::Wall::~Wall()
{
}

bool NCL::CSC3222::Wall::UpdateObject(float dt)
{
    return false;
}
