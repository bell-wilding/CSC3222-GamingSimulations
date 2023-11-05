#include "CollisionVolume.h"
#include "CollisionBox.h"
#include "CollisionCircle.h"
#include "../../Common/Maths.h"

using namespace NCL;
using namespace CSC3222;

CollisionPair* NCL::CSC3222::CollisionVolume::Collides(CollisionVolume& otherVol)
{	
	if (volumeShape == Shape::BOX && otherVol.volumeShape == Shape::BOX) {
		return BoxBoxCollision(static_cast<CollisionBox&>(*this), static_cast<CollisionBox&>(otherVol));
	}
	else if (volumeShape == Shape::CIRCLE && otherVol.volumeShape == Shape::BOX) {
		return CircleBoxCollision(static_cast<CollisionCircle&>(*this), static_cast<CollisionBox&>(otherVol));
	}
	else if (volumeShape == Shape::BOX && otherVol.volumeShape == Shape::CIRCLE) {
		return CircleBoxCollision(static_cast<CollisionCircle&>(otherVol), static_cast<CollisionBox&>(*this));
	}
	else if (volumeShape == Shape::CIRCLE && otherVol.volumeShape == Shape::CIRCLE) {
		return CircleCircleCollision(static_cast<CollisionCircle&>(*this), static_cast<CollisionCircle&>(otherVol));
	}
	else {
		return nullptr;
	}
}

CollisionBox* NCL::CSC3222::CollisionVolume::VolumeToBox(CollisionVolume& volume)
{
	if (volume.GetShape() == Shape::BOX) {
		CollisionBox box = static_cast<CollisionBox&>(volume);
		return new CollisionBox(box.GetPosition(), box.GetExtents(), box.GetOffset());
	}
	else {
		CollisionBox box = (static_cast<CollisionCircle&>(volume)).CircleToBox();
		return new CollisionBox(box.GetPosition(), box.GetExtents(), box.GetOffset());
	}
}

CollisionVolume::~CollisionVolume()
{
}

CollisionPair* NCL::CSC3222::CollisionVolume::CircleCircleCollision(CollisionCircle& firstCircle, CollisionCircle& secondCircle)
{
	// Get distances between x and y coordinates
	float dx = firstCircle.GetX()  - secondCircle.GetX();
	float dy = firstCircle.GetY() - secondCircle.GetY();
	// Calculate distance between centers using pythagoras
	float dist = sqrtf(dx * dx + dy * dy);

	float penetrationDist = (firstCircle.GetRadius() + secondCircle.GetRadius()) - dist;

	// Determine if intersecting when dist between centers is less than sum of radiuses
	if (penetrationDist > 0) {
		CollisionPair* pair = new CollisionPair();
		pair->firstVol = &firstCircle;
		pair->secondVol = &secondCircle;
		pair->penetration = penetrationDist;
		pair->normal = (secondCircle.GetPosition() - firstCircle.GetPosition()).Normalised();
		return pair;
	}
	
	// Return null pointer if no collision
	return nullptr;
}

CollisionPair* NCL::CSC3222::CollisionVolume::CircleBoxCollision(CollisionCircle& circle, CollisionBox& box)
{
	// Circle is already inside box (Deep Collision)
	if (circle.GetX() > box.GetX() - box.GetExtents().x &&
		circle.GetX() < box.GetX() + box.GetExtents().x &&
		circle.GetY() > box.GetY() - box.GetExtents().y &&
		circle.GetY() < box.GetY() + box.GetExtents().y) {

		CollisionPair* pair = new CollisionPair();
		pair->firstVol = &circle;
		pair->secondVol = &box;
		pair->normal = circle.GetPosition() - box.GetPosition();
		pair->penetration = (circle.GetPosition() - box.GetPosition()).Length();
		return pair;
	}	
	
	Vector2 closestPoint = circle.GetPosition();

	closestPoint.x = Clamp(closestPoint.x, box.GetX() - box.GetExtents().x, box.GetX() + box.GetExtents().x);
	closestPoint.y = Clamp(closestPoint.y, box.GetY() - box.GetExtents().y, box.GetY() + box.GetExtents().y);

	Vector2 diff = circle.GetPosition() - closestPoint;

	if (diff.Length() < circle.GetRadius()) {

		CollisionPair* pair = new CollisionPair();
		pair->firstVol = &circle;
		pair->secondVol = &box;
		pair->normal = diff.Normalised();
		pair->penetration = diff.Length() - circle.GetRadius();
		return pair;
	}

	return nullptr;
}

CollisionPair* NCL::CSC3222::CollisionVolume::BoxBoxCollision(CollisionBox& firstBox, CollisionBox& secondBox)
{
	if (firstBox.CollidesWith(secondBox)) {

		static const Vector2 faces[4] =
		{
			Vector2(-1, 0), Vector2(1, 0),
			Vector2(0, -1), Vector2(0, 1)
		};

		Vector2 firstBoxMax = firstBox.GetPosition() + firstBox.GetExtents();
		Vector2 firstBoxMin = firstBox.GetPosition() - firstBox.GetExtents();

		Vector2 secondBoxMax = secondBox.GetPosition() + secondBox.GetExtents();
		Vector2 secondBoxMin = secondBox.GetPosition() - secondBox.GetExtents();

		float distances[4] = {
			(secondBoxMax.x - firstBoxMin.x),
			(firstBoxMax.x - secondBoxMin.x),
			(secondBoxMax.y - firstBoxMin.y),
			(firstBoxMax.y - secondBoxMin.y)
		};

		float penetration = FLT_MAX;
		Vector2 normalAxis;

		for (int i = 0; i < 4; i++) {
			if (distances[i] < penetration) {
				penetration = distances[i];
				normalAxis = faces[i];
			}
		}

		CollisionPair* pair = new CollisionPair();
		pair->firstVol = &firstBox;
		pair->secondVol = &secondBox;
		pair->penetration = penetration;
		pair->normal = normalAxis;
		
		return pair;
	}

	return nullptr;
}

float NCL::CSC3222::CollisionVolume::Clamp(float value, float minValue, float maxValue)
{
	value = fmin(value, maxValue);
	value = fmax(value, minValue);
	return value;
}
