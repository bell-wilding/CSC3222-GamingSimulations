#include "Pixie.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionCircle.h"
#include "FruitWizardGame.h"
#include <algorithm>

using namespace NCL;
using namespace CSC3222;

std::vector<Pixie*> NCL::CSC3222::Pixie::allPixies;

Vector4 pixieFrames[] = {
	Vector4(32 * 1,32 * 4, 32, 32),
	Vector4(32 * 2,32 * 4, 32, 32),
	Vector4(32 * 3,32 * 4, 32, 32),
	Vector4(32 * 4,32 * 4, 32, 32),
};

Pixie::Pixie() : SimObject("Pixie") {
	texture			= texManager->GetTexture("FruitWizard\\super_random_sprites.png");
	animFrameCount	= 4;
	collected = false;
	separationThreshold = 35;
	alignmentThreshold = 20;
	cohesionThreshold = 120;
	avoidanceThreshold = 65;	
	dampingValue = 0.96;
	inverseMass = 3.5;
	speed = 10;
	playerDraw = 0.1;
	mapBoundsAdjustmentThreshold = 8;
	adjustmentAmount = 0.5;
	allPixies.emplace_back(this);
}

Pixie::~Pixie() {
}

bool Pixie::UpdateObject(float dt) {
	animFrameData = pixieFrames[currentanimFrame];

	if (allPixies.size() > 1) {
		Vector2 direction;
		direction += Alignment();
		direction += Separation() * 2;
		direction += Cohesion();
		direction += Avoidance(game->GetActiveEnemies());	

		AddImpulse(direction);
	}

	// Move slowly towards the player
	AddImpulse((game->GetPlayerPosition() - position).Normalised() * (allPixies.size() > 1 ? playerDraw : playerDraw * 10));

	AdjustForMapBounds();

	if ((game->GetPlayerPosition() - position).Length() < 20) {
		collected = true;
		game->IncrementScore(1000);
		game->IncrementSpells(1);
		allPixies.erase(std::remove(allPixies.begin(), allPixies.end(), this), allPixies.end());
	} 
	else if (!active) {
		allPixies.erase(std::remove(allPixies.begin(), allPixies.end(), this), allPixies.end());
	}

	return !collected && active;
}

Vector2 NCL::CSC3222::Pixie::Alignment()
{
	Vector2 direction = this->velocity;

	for (auto& pixie : allPixies) {
		if (pixie == this) {
			continue;
		}
		float distance = (this->position - pixie->position).Length();

		if (distance > alignmentThreshold) {
			continue;
		}
		direction += pixie->velocity;
	}	
	return direction.Normalised();
}

Vector2 NCL::CSC3222::Pixie::Separation()
{
	Vector2 direction;

	for (auto& pixie : allPixies) {
		if (pixie == this) {
			continue;
		}
		float distance = (this->position - pixie->position).Length();

		if (distance > separationThreshold) {
			continue;
		}
		float strength = 1.0f - (distance / separationThreshold);
		direction += (this->position - pixie->position).Normalised() * strength;
	}
	return direction.Normalised();
}

Vector2 NCL::CSC3222::Pixie::Cohesion()
{
	Vector2 avgPosition = this->position;
	float count = 1;

	for (auto& pixie : allPixies) {
		if (pixie == this) {
			continue;
		}
		float distance = (this->position - pixie->position).Length();

		if (distance > cohesionThreshold) {
			continue;
		}
		avgPosition += pixie->position;
		count++;
	}
	
	avgPosition /= count;
	Vector2 direction = avgPosition - this->position;
	return direction.Normalised();
}

Vector2 NCL::CSC3222::Pixie::Avoidance(const std::vector<SimObject*> &enemies)
{
	Vector2 direction;

	for (auto& enemy : enemies) {
		float distance = (this->position - enemy->GetPosition()).Length();

		if (distance > avoidanceThreshold) {
			continue;
		}
		direction += (this->position - enemy->GetPosition()).Normalised();
	}
	return direction.Normalised();
}

Vector2 NCL::CSC3222::Pixie::AdjustForMapBounds()
{
	Vector2 direction;

	if (position.y - 16 < mapBoundsAdjustmentThreshold) {
		direction += Vector2(0, adjustmentAmount);
		AddImpulse(Vector2(0, adjustmentAmount));
	}

	if (320 - position.y < mapBoundsAdjustmentThreshold) {
		AddImpulse(Vector2(0, -adjustmentAmount));
	}

	if (position.x - 24 < mapBoundsAdjustmentThreshold) {
		AddImpulse(Vector2(adjustmentAmount, 0));
	}

	if (464 - position.x < mapBoundsAdjustmentThreshold) {
		direction -= Vector2(adjustmentAmount, 0);
		AddImpulse(Vector2(-adjustmentAmount, 0));
	}

	return direction;
}
