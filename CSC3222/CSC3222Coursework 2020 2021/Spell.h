#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Spell : public SimObject {
		public:
			Spell(Vector2 direction);
			~Spell();

			void OnCollisionEnter(SimObject* col) override;
			void DrawObject(GameSimsRenderer &r) override;
			bool UpdateObject(float dt) override;

		private:
			int bounces;
			float speed;
		};
	}
}

