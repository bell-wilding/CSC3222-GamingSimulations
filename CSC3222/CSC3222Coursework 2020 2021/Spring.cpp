#include "Spring.h"
#include "../../Common/Vector2.cpp"

NCL::CSC3222::Spring::Spring(float k, float dampingConstant, SimObject* attachedBody, SimObject* thisBody) 
{
    this->k = k;
    this->dampingConstant = dampingConstant;
    objA = attachedBody;
    objB = thisBody;
}

NCL::CSC3222::Spring::~Spring()
{
    objA = nullptr;
    objB = nullptr;
}

void NCL::CSC3222::Spring::ApplySpringForce()
{
    Vector2 x = objA->GetPosition() - objB->GetPosition();
    Vector2 v = objA->GetVelocity() - objB->GetVelocity();
    Vector2 springForce = x * k + v * dampingConstant;
    objB->AddForce(springForce);
}
