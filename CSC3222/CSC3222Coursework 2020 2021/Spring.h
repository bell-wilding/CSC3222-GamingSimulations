#pragma once

#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Spring {
		public:
			Spring(float k, float dampingConstant, SimObject* attachedBody, SimObject* thisBody);
			~Spring();

			void ApplySpringForce();
			
		protected:
			SimObject* objA;
			SimObject* objB;
			
			float dampingConstant;
			float k;

		};
	}
}