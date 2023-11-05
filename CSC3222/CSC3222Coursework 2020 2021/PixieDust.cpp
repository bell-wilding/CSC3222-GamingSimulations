#include "PixieDust.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionCircle.h"
#include "PlayerCharacter.h"
#include "FruitWizardGame.h"

using namespace NCL;
using namespace CSC3222;

Vector4 animFrames[] = {
	Vector4(64 , 192, 32, 32),
	Vector4(96, 192, 32, 32),
	Vector4(128, 192, 32, 32),
	Vector4(160, 192, 32, 32)
};

PixieDust::PixieDust() : SimObject("PixieDust") {
	animFrameCount = 4;
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	SetCollider(new CollisionCircle(10, this, Vector2(0, 0), true));
	spring = nullptr;
	maxVelocity = Vector2(100, 100);
	collected = false;
}

PixieDust::~PixieDust() {
	delete spring;
	spring = nullptr;
}

bool PixieDust::UpdateObject(float dt) {
	animFrameData = animFrames[currentanimFrame];

	if (spring != nullptr) {
		spring->ApplySpringForce();
	}

	return active;
}

void NCL::CSC3222::PixieDust::OnCollisionEnter(SimObject* col)
{
	if (col->TagEqualTo("Player") && !collected) {
		PlayerCharacter* player = static_cast<PlayerCharacter*>(col);
		if (player->PixieDustCount() == 0) {
			spring = new Spring(20, 0.98, player->GetLastDust(), this);
		}
		else {
			spring = new Spring(40, 0.99, player->GetLastDust(), this);
		}
		player->AddPixieDust(this);
		collected = true;
		game->IncrementScore(500);
		game->IncrementDust(1);
		game->CollectDust(this);
	}
}
