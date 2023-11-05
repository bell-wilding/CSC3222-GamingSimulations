#include "Floor.h"

NCL::CSC3222::Floor::Floor() : SimObject("Floor")
{
    isStatic = true;
    inverseMass = 0;
}

NCL::CSC3222::Floor::~Floor()
{
}

bool NCL::CSC3222::Floor::UpdateObject(float dt)
{
    return false;
}
