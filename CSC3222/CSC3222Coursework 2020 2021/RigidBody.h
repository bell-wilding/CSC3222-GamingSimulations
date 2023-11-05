#pragma once
#include "../../Common/Vector2.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		class RigidBody	{
			friend class GameSimsPhysics;
		public:
			RigidBody(std::string tag = "", bool isStatic = false, Vector2 maxVelocity = Vector2(INFINITY, INFINITY));
			~RigidBody();

			Vector2 GetPosition() const {
				return position;
			}

			void SetPosition(const Vector2& newPosition) {
				position = newPosition;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			Vector2 GetVelocity() const {
				return velocity;
			}

			void SetVelocity(const Vector2& newVelocity) {
				velocity = newVelocity;
			}

			void AddForce(const Vector2& newForce) {
				force += newForce;
			}

			void AddImpulse(const Vector2& impulse) {
				velocity += (impulse * inverseMass);
			}

			void SetDampingValue(float dampingValue) {
				this->dampingValue = dampingValue;
			}

			void SetMaxVelocity(Vector2 max) {
				this->maxVelocity = max;
			}

			Vector2 GetMaxVelocity() {
				return maxVelocity;
			}

			void SetStatic(bool isStatic) {
				this->isStatic = isStatic;
			}

			bool BodyIsStatic() {
				return isStatic;
			}

			bool TagEqualTo(std::string compTag) {
				return tag == compTag;
			}

			std::string GetTag() {
				return tag;
			}


		protected:
			Vector2 position;
			Vector2 velocity;
			Vector2 force;

			Vector2 maxVelocity;

			float dampingValue;

			float inverseMass;
			float elasticity;

			std::string tag;

			bool isStatic = false;
			bool useGravity = false;

		};
	}
}
