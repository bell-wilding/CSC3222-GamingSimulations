#pragma once

#include "../../Common/Vector2.h"
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {

		struct CollisionPair {
			CollisionVolume* firstVol;
			CollisionVolume* secondVol;
			float penetration;
			Vector2 normal;
		};

		enum class Shape {
			BOX,
			CIRCLE
		};

		// Forward Declaration
		class CollisionCircle;
		class CollisionBox;

		class CollisionVolume {
		public:
			~CollisionVolume();

			CollisionPair* Collides(CollisionVolume& otherVol);

			SimObject* GetSimObject() { return simObject; }

			Shape GetShape() { return volumeShape; }

			static CollisionBox* VolumeToBox(CollisionVolume& volume);

			bool BodyIsStatic() { return simObject ? simObject->BodyIsStatic() : true; }

			float GetX() { return simObject ? simObject->GetPosition().x + offset.x : staticPosition.x + offset.x; }
			float GetY() { return simObject ? simObject->GetPosition().y + offset.y : staticPosition.y + offset.y; }
			Vector2 GetPosition() { return simObject ? simObject->GetPosition() + offset : staticPosition + offset; }

			Vector2 GetOffset() { return offset; }
			void SetOffset(Vector2 newOffset) { offset = newOffset; };

			bool IsTrigger() { return isTrigger; }

			bool PhysicalCollisionAllowed(std::string tag) { return simObject ? simObject->PhysicalCollisionAllowed(tag) : true; }
			bool ImpulseCollisionAllowed(std::string tag) { return simObject ? simObject->ImpulseCollisionAllowed(tag) : true; }

			std::string GetSimObjectTag() { return simObject ? simObject->GetTag() : ""; }

		protected:			

			CollisionVolume(Shape volumeShape, Vector2 offset, SimObject* simObject, bool isTrigger) : volumeShape(volumeShape), offset(offset), simObject(simObject), isTrigger(isTrigger) {};		
			CollisionVolume(Shape volumeShape, Vector2 staticPosition, Vector2 offset, bool isTrigger, std::string tag = "") : volumeShape(volumeShape), staticPosition(staticPosition), offset(offset), isTrigger(isTrigger), simObject(nullptr) {};

			CollisionPair* CircleCircleCollision(CollisionCircle& firstVol, CollisionCircle& secondVol);
			CollisionPair* CircleBoxCollision(CollisionCircle& firstVol, CollisionBox& secondVol);
			CollisionPair* BoxBoxCollision(CollisionBox& firstVol, CollisionBox& secondVol);

			Vector2 offset;
			Vector2 staticPosition;

			Shape volumeShape;

			SimObject* simObject;

			bool isTrigger;

		private:
			float Clamp(float value, float minValue, float maxValue);
		};
	}
}

