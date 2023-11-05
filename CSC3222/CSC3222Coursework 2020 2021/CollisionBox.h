#pragma once

#include "CollisionVolume.h"

namespace NCL {
	namespace CSC3222 {
		class CollisionBox : public CollisionVolume {
		public:
			CollisionBox(Vector2 extents, SimObject* simObject, Vector2 offset = Vector2(0, 0), bool isTrigger = false) : extents(extents), CollisionVolume(Shape::BOX, offset, simObject, isTrigger) {};
			CollisionBox(Vector2 staticPosition, Vector2 extents, Vector2 offset = Vector2(0, 0), bool isTrigger = false) : extents(extents), CollisionVolume(Shape::BOX, staticPosition, offset, isTrigger) {};

			Vector2 GetExtents() { return extents; };
			void SetExtents(Vector2 newExtents) { extents = newExtents; };

			bool CollidesWith(CollisionBox& secondBox);

		protected:
			Vector2 extents;
		};
	}
}