#pragma once
#include "SimObject.h"
#include "GameMap.h"

namespace NCL::CSC3222 {
	class Froggo : public SimObject {
	public:
		Froggo(GameMap* map);
		~Froggo();

		bool UpdateObject(float dt) override;
		void OnCollisionEnter(SimObject* col) override;
		void OnCollisionExit(SimObject* col) override;
		void OnCollision(SimObject* col) override;

		SimObject* GetTarget() { return target; };

		void TargetObject(SimObject* object);

		std::vector<std::pair<Vector2, char>> GetPath() { return pathToTarget; };

	protected:
		enum class FroggoState {
			Idle,
			Walk,
			Attack,
			Stunned,
			Jump,
			Climb,
			Guarding
		};

		FroggoState DetermineState();
		void FollowTarget(float speed);
		void AttackPlayer();
		void Stunned(float dt);

		Vector2 movementDirection;

		bool destinationReached = true;

		SimObject* target;
		SimObject* player;

		std::vector<std::pair<Vector2, char>> pathToTarget;

		Vector2 currentPathTarget;

		float distanceThreshold = 12;
		float climbingThreshold = 8;
		float prevY;
		float yChange;

		float hysteresisTimer;
		float stunTimer;
		float stunLength;

		bool falling = false;

		int currentPathIndex = 0;
		bool foundPath;

		float movementSpeed;

		FroggoState activeState;

		GameMap* map;
	};
}