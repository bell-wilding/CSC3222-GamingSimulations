#include "Ledge.h"

NCL::CSC3222::Ledge::Ledge() : SimObject("Ledge")
{
	isStatic = true;
	inverseMass = 0;
}

NCL::CSC3222::Ledge::~Ledge()
{
}

bool NCL::CSC3222::Ledge::UpdateObject(float dt)
{
	return false;
}
