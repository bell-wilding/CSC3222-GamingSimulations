#include "Spell.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionCircle.h"

#include "../../Common/Maths.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL;
using namespace CSC3222;

Vector4 activeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
};

Vector4 explodeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(416,416, 32, 32),
	Vector4(448,416, 32, 32),
	Vector4(480,416, 32, 32),
	Vector4(512,416, 32, 32),	
};

Spell::Spell(Vector2 direction) : SimObject("Spell")	{
	texture		= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	speed = 60;
	AddImpulse(direction * speed);
	animFrameCount = 6;
	inverseMass = 3;
	dampingValue = 1;
	SetCollider(new CollisionCircle(7, this, Vector2(0.5, -1), false));
	bounces = 0;
	AddNonPhysicalCollision("Player");
}

Spell::~Spell()	{
}

void NCL::CSC3222::Spell::OnCollisionEnter(SimObject* col)
{
	if (!(col->TagEqualTo("Wall") || col->TagEqualTo("Floor") || col->TagEqualTo("Ledge"))) {
		std::cout << "Spell started colliding with: " << col->GetTag() << std::endl;
	}

	if (col->TagEqualTo("Wall") || col->TagEqualTo("Floor") || col->TagEqualTo("Ledge") || col->TagEqualTo("LadderTop")) {
		++bounces;
	}
}

void Spell::DrawObject(GameSimsRenderer &r) {
	Vector4	texData = explodeFrames[currentanimFrame];
	Vector2 texPos	= Vector2(texData.x, texData.y);
	Vector2 texSize = Vector2(texData.z, texData.w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}

bool Spell::UpdateObject(float dt) {
	animFrameData = explodeFrames[currentanimFrame];

	if (velocity.x == 0 && velocity.y == 0) {
		return false;
	}

	if (bounces >= 3) {
		return false;
	}

	return true;
}