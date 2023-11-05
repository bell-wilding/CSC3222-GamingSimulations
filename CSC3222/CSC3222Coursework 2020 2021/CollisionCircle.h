#pragma once

#include "CollisionVolume.h"
#include "CollisionBox.h"

namespace NCL {
	namespace CSC3222 {
		class CollisionCircle: public CollisionVolume {
		public:
			CollisionCircle(float radius, SimObject* simObject, Vector2 offset = Vector2(0, 0), bool isTrigger = false) : radius(radius), CollisionVolume(Shape::CIRCLE, offset, simObject, isTrigger) {};
			CollisionCircle(Vector2 staticPosition, float radius, Vector2 offset = Vector2(0, 0), bool isTrigger = false) : radius(radius), CollisionVolume(Shape::BOX, staticPosition, offset, isTrigger) {};

			float GetRadius() { return radius;  }

			CollisionBox CircleToBox() { return simObject ? CollisionBox(Vector2(GetRadius(), GetRadius()), GetSimObject(), GetOffset()) : CollisionBox(GetPosition(), Vector2(GetRadius(), GetRadius()), GetOffset());

			};

		protected:
			float radius;
		};
	}
}