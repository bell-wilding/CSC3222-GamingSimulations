#pragma once
#include "SimObject.h"
#include "StateMachine.h"

namespace NCL::CSC3222 {
	class Guard : public SimObject	{
	public:
		Guard(Vector2 startingPosition);
		~Guard();

		bool UpdateObject(float dt) override;

		void Reset();

	protected:
		enum class GuardState {
			Idle,
			Left,
			Right,
			Attack,
			Stunned
		};

		void OnCollisionEnter(SimObject* col) override;
		void OnCollision(SimObject* col) override;
		void OnCollisionExit(SimObject* col) override;

		GuardState DetermineState();
		void AttackPlayer();
		void Walk(bool leftToRight);
		void Stunned(float dt);

		float dropThreshold;
		float hysteresisTimer;
		float stunTimer;
		float stunLength;

		float movementSpeed;

		bool falling;

		GuardState activeState;
		Vector2 startingPos;
	};
}