#pragma once

#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Wall : public SimObject {
		public:
			Wall();
			~Wall();

			bool UpdateObject(float dt) override;
		};
	}
}

