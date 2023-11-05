#include "CollisionBox.h"

bool NCL::CSC3222::CollisionBox::CollidesWith(CollisionBox& secondBox)
{
	return
		GetX() - GetExtents().x < secondBox.GetX() + secondBox.GetExtents().x &&
		GetX() + GetExtents().x > secondBox.GetX() - secondBox.GetExtents().x &&
		GetY() + GetExtents().y > secondBox.GetY() - secondBox.GetExtents().y &&
		GetY() - GetExtents().y < secondBox.GetY() + secondBox.GetExtents().y;
}
