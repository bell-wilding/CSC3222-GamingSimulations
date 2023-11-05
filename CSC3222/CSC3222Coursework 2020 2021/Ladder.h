#pragma once

#include "SimObject.h"
#include "LadderTop.h"

namespace NCL {
	namespace CSC3222 {
		class Ladder : public SimObject {
		public:
			Ladder(LadderTop* ladderTop);
			~Ladder();

			bool UpdateObject(float dt) override;
			void OnCollision(SimObject* col) override;

		protected:
			LadderTop* top;

		};
	}
}
