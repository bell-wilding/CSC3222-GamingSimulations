#include "Fruit.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionCircle.h"
#include "FruitWizardGame.h"

using namespace NCL;
using namespace CSC3222;

Fruit::Fruit() : SimObject("Fruit") {
	texture = texManager->GetTexture("FruitWizard\\Fruit.png");

	int fruitID = rand() % 16;

	animFrameData = Vector4((fruitID / 4) * 16.0f, (fruitID % 4) * 16.0f, 16.0f, 16.0f);

	SetCollider(new CollisionCircle(6, this, Vector2(0, 0)));
	AddNonPhysicalCollision("Guard");
	AddNonPhysicalCollision("Froggo");
	AddNonPhysicalCollision("Player");
	AddNonPhysicalCollision("PixieDust");
	AddNonPhysicalCollision("Spell");
}

Fruit::~Fruit() {
}

bool Fruit::UpdateObject(float dt) {
	return !collected && active;
}

void NCL::CSC3222::Fruit::OnCollisionEnter(SimObject* col)
{
	if (col->TagEqualTo("Player")) {
		game->IncrementScore(1000);
		game->RemoveFruit(this);
		collected = true;
	}
}
