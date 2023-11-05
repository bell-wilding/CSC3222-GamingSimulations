#pragma once

#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class LadderTop : public SimObject {
		public:
			LadderTop();
			~LadderTop();

			void OnCollisionEnter(SimObject* col) override;
			void OnCollision(SimObject* col) override;
			void OnCollisionExit(SimObject* col) override;

			bool PlayerOnLadderTop() { return playerOnLadderTop; };

			void SetPlayerCollision(bool hasPlayerCollision);

			bool UpdateObject(float dt) override;

		protected:

			bool playerOnLadderTop;
			bool ladderFrame = false;
		};
	}
}
