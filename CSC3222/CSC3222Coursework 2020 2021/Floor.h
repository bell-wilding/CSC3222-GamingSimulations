#pragma once

#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Floor : public SimObject {
		public:
			Floor();
			~Floor();

			bool UpdateObject(float dt) override;
		};
	}
}

