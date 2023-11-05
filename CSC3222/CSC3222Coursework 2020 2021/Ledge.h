#pragma once

#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Ledge : public SimObject {
		public:
			Ledge();
			~Ledge();

			bool UpdateObject(float dt) override;
		};
	}
}

