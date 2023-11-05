#pragma once
#include "SimObject.h"
#include "Spring.h"

namespace NCL::CSC3222 {
	class PixieDust :	public SimObject	{
	public:
		PixieDust();
		~PixieDust();

		bool UpdateObject(float dt) override;
		void OnCollisionEnter(SimObject* col) override;

	protected:
		Spring* spring;
		bool collected;
	};
}
