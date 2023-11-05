#include "RigidBody.h"

using namespace NCL;
using namespace CSC3222;

RigidBody::RigidBody(std::string tag, bool isStatic, Vector2 maxVelocity)	{
	this->tag = tag;
	this->isStatic = isStatic;
	inverseMass = isStatic ? 0 : 1.0f;
	elasticity  = 0.5f;
	dampingValue = 0.9999f;
	this->maxVelocity = maxVelocity;
}


RigidBody::~RigidBody()	{
}
