#include "Ladder.h"

NCL::CSC3222::Ladder::Ladder(LadderTop* ladderTop) : SimObject("Ladder")
{
	isStatic = true;
	inverseMass = 0;
	top = ladderTop;
}

NCL::CSC3222::Ladder::~Ladder()
{
}

bool NCL::CSC3222::Ladder::UpdateObject(float dt)
{
	return true;
}

void NCL::CSC3222::Ladder::OnCollision(SimObject* col)
{
	if (col->TagEqualTo("Player") && !top->PlayerOnLadderTop()) {
		top->SetPlayerCollision(false);
	}
}
