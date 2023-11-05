#pragma once
#include "SimObject.h"
#include "PixieDust.h"
#include <vector>

namespace NCL::CSC3222 {
	class PlayerCharacter : public SimObject	{
	public:
		PlayerCharacter();
		~PlayerCharacter();

		bool UpdateObject(float dt) override;
		void OnCollisionEnter(SimObject* col) override;
		void OnCollision(SimObject* col) override;

		SimObject* GetLastDust();
		int PixieDustCount();
		void AddPixieDust(SimObject* dust);
		void RemoveAllDust() { collectedDust.clear(); };

		void Reset();

	protected:
		enum class PlayerState {
			Left,
			Right,
			Attack,
			Climb,
			Fall,
			Die,
			Idle
		};
		PlayerState		currentAnimState;

		bool canClimb		= false;
		bool onLadderTop	= false;
		bool onLadderBody	= false;
		bool falling		= false;

		float fallDist		= 0;
		float prevY			= 0;

		std::vector<SimObject*> collectedDust;
	};
}