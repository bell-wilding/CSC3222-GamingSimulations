#include "LadderTop.h"
#include "../../Common/Window.h"

NCL::CSC3222::LadderTop::LadderTop() : SimObject("LadderTop")
{
	isStatic = true;
	inverseMass = 0;
	playerOnLadderTop = false;
}

NCL::CSC3222::LadderTop::~LadderTop()
{
}

void NCL::CSC3222::LadderTop::OnCollisionEnter(SimObject* col)
{
	if (col->TagEqualTo("Player")) {
		playerOnLadderTop = true;
	}
}

void NCL::CSC3222::LadderTop::OnCollision(SimObject* col)
{
	if (col->TagEqualTo("Player") && Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		AddNonPhysicalCollision("Player");
		ladderFrame = true;
	}
}

void NCL::CSC3222::LadderTop::OnCollisionExit(SimObject* col)
{
	if (col->TagEqualTo("Player")) {	
		RemoveNonPhysicalCollision("Player");
		playerOnLadderTop = false;
	}
}

void NCL::CSC3222::LadderTop::SetPlayerCollision(bool hasPlayerCollision)
{	
	if (hasPlayerCollision) {
		RemoveNonPhysicalCollision("Player");
	}
	else {
		AddNonPhysicalCollision("Player");
	}
}

bool NCL::CSC3222::LadderTop::UpdateObject(float dt)
{	
	return true;
}
