#include "PlayerCharacter.h"
#include "TextureManager.h"
#include "../../Common/Window.h"
#include "GameSimsRenderer.h"
#include "CollisionCircle.h"
#include "FruitWizardGame.h"
#include "Spell.h"
#include "Ladder.h"
#include "LadderTop.h"
#include <time.h>

using namespace NCL;
using namespace CSC3222;

Vector4 runFrames[] = {
	Vector4(64,160, 32, 32),
	Vector4(96,160, 32, 32),
	Vector4(128,160, 32, 32),
	Vector4(160,160, 32, 32),
	Vector4(192,160, 32, 32),
	Vector4(224,160, 32, 32),
};

Vector4 attackFrames[] = {
	Vector4(128,288, 32, 30),
	Vector4(64,224, 32, 32),
	Vector4(160,288, 32, 30),
	Vector4(96,224, 32, 32),
	Vector4(192,288, 32, 29),
	Vector4(64,256, 32, 32)
};

Vector4 idleFrames[] = {
	Vector4(64,128, 32, 32),
	Vector4(96,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(160,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(224,128, 32, 32)
};

Vector4 fallFrames[] = {
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32)
};

Vector4 deathFrames[] = {
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),	
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
};

Vector4 ladderFrames[] = {//Not an ideal spritesheet for ladders!
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
};


PlayerCharacter::PlayerCharacter() : SimObject("Player") {
	currentAnimState	= PlayerState::Left;
	texture				= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount		= 6;
	inverseMass			= 0.5;
	dampingValue		= 0.99;
	maxVelocity			= Vector2(300, INFINITY);
	useGravity			= false;

	SetCollider(new CollisionCircle(10.5, this, Vector2(0, -5)));

	AddNonImpulseCollision("Floor");
	AddNonImpulseCollision("LadderTop");
	AddNonImpulseCollision("Ledge");

	prevY = position.y;

	srand((unsigned)time(0)); // Used for random spell directions
}

PlayerCharacter::~PlayerCharacter() {
}

bool PlayerCharacter::UpdateObject(float dt) {

	float movementSpeed = 450;
	Vector4* animSource = idleFrames;

	Vector2 newForce;

	// Animations
	if (currentAnimState == PlayerState::Attack) {
		animSource = attackFrames;
		if (currentanimFrame >= 5) {
			currentAnimState = PlayerState::Idle;
		}
	}
	else {
		currentAnimState = PlayerState::Idle;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			animSource = runFrames;
			currentAnimState = PlayerState::Left;
			flipAnimFrame = true;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			animSource = runFrames;
			currentAnimState = PlayerState::Right;
			flipAnimFrame = false;
		}	
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) && canClimb) {
			animSource = ladderFrames;
			currentAnimState = PlayerState::Climb;
			flipAnimFrame = false;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN) && canClimb) {
			animSource = ladderFrames;
			currentAnimState = PlayerState::Climb;
			flipAnimFrame = true;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && game->GetSpellCount() > 0) {
			currentAnimState = PlayerState::Attack;
			currentanimFrame = 0;
			float spellX = flipAnimFrame ? -1 : 1;
			float spellY = (((float)rand()) / (float)RAND_MAX) * 1.6 + (-0.8);
			Vector2 spellDirection = Vector2(spellX, spellY).Normalised();
			Spell* newSpell = new Spell(spellDirection);
			newSpell->SetPosition(position + Vector2(flipAnimFrame ? -10 : 10, 0));
			game->AddNewObject(newSpell);
			game->SetSpellCount(game->GetSpellCount() - 1);
		}
	}

	// Actions
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		newForce.x = -movementSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		newForce.x = movementSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) && canClimb) {
		newForce.y = movementSpeed * 0.5;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN) && canClimb) {
		newForce.y = -movementSpeed * 0.5;
	}

	if (falling) {
		useGravity = true;

		// Used to limit impulse-based bounces after a fall
		if (prevY > position.y) {
			fallDist += prevY - position.y;
		}
		else {
			fallDist = 0;
			AddNonImpulseCollision("Floor");
			AddNonImpulseCollision("LadderTop");
			AddNonImpulseCollision("Ledge");
		}

		if (fallDist >= 5) {
			RemoveNonImpulseCollision("Floor");
			RemoveNonImpulseCollision("LadderTop");
			RemoveNonImpulseCollision("Ledge");
			fallDist = 0;
		}
	}
	else {
		useGravity = false;
	}

	AddForce(newForce);

	animFrameData = animSource[currentanimFrame];

	falling = true;
	canClimb = false;
	prevY = position.y;

	return true;
}

void NCL::CSC3222::PlayerCharacter::OnCollisionEnter(SimObject* col)
{
	if (!col->TagEqualTo("Floor")) {
		std::cout << "Player started colliding with: " << col->GetTag() << std::endl;
	}
}

void NCL::CSC3222::PlayerCharacter::OnCollision(SimObject* col)
{
	// Fall and don't stick to tiles above player
	if ((col->TagEqualTo("Floor") || col->TagEqualTo("Ledge")) && col->GetPosition().y < position.y) {
		falling = false;
	}

	if (col->TagEqualTo("Ladder") || col->TagEqualTo("LadderTop")) {
		falling = false;
	}

	if (col->TagEqualTo("Ladder") || col->TagEqualTo("LadderTop")) {
		canClimb = true;
	}
}

SimObject* NCL::CSC3222::PlayerCharacter::GetLastDust()
{
	return collectedDust.empty() ? this : collectedDust.back();
}

int NCL::CSC3222::PlayerCharacter::PixieDustCount()
{
	return collectedDust.size();
}

void NCL::CSC3222::PlayerCharacter::AddPixieDust(SimObject* dust)
{
	collectedDust.emplace_back(dust);
}

void NCL::CSC3222::PlayerCharacter::Reset()
{
	position = Vector2(32, 32);
	velocity = Vector2(0, 0);
	collectedDust.clear();
}
