#pragma once
#include "SimObject.h"
#include "Spring.h"
#include <vector>

namespace NCL::CSC3222 {
	class Pixie : public SimObject	{
	public:
		Pixie();
		~Pixie();

		bool UpdateObject(float dt) override;

	protected:

		Vector2 Alignment();
		Vector2 Separation();
		Vector2 Cohesion();
		Vector2 Avoidance(const std::vector<SimObject*> &enemies);

		// Avoid flying offscreen outside map bounds
		Vector2 AdjustForMapBounds();

		bool collected;
		float separationThreshold;
		float alignmentThreshold;
		float cohesionThreshold;
		float avoidanceThreshold;

		float speed;

		float playerDraw;

		int mapBoundsAdjustmentThreshold;
		float adjustmentAmount;

	private:
		static std::vector<Pixie*> allPixies;
	};
}