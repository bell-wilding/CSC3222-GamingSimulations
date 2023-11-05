#pragma once
#include <vector>
#include "../../Common/Vector2.h"
#include "CollisionVolume.h"
#include "QuadTree.h"

namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;

		class GameSimsPhysics	{
		public:
			GameSimsPhysics();
			~GameSimsPhysics();

			void Update(float dt);
			void FixedUpdate(float dt);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);

			std::vector<CollisionVolume*> AllColliders() { return allColliders; }

		protected:
			void Integration(float dt);
			void CollisionDetection(float dt);
			void CollisionResolution();

			void Projection(CollisionPair* collision);
			void Impulse(CollisionPair* collision);

			void IntegrateAcceleration(float dt);
			void IntegrateVelocity(float dt);

			void NarrowPhase(QuadTree* leafNode);

			float timeRemaining;

			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;
			std::vector<CollisionPair*>		collisionPairs;
		};
	}
}

