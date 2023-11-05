#include "Guard.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionBox.h"
#include "FruitWizardGame.h"

using namespace NCL;
using namespace CSC3222;

Vector4 guardIdleFrames[] = {
	Vector4(384,32, 64, 32),
	Vector4(448,32, 64, 32),	
	Vector4(512,32, 64, 32),	
	Vector4(576,32, 64, 32),	
	Vector4(448,64, 64, 32),	
	Vector4(512,64, 64, 32),
};

Vector4 guardWalkFrames[] = {
	Vector4(384,96, 64, 32),
	Vector4(448,96, 64, 32),
	Vector4(512,96, 64, 32),
	Vector4(576,96, 64, 32),
	Vector4(640,96, 64, 32),
	Vector4(704,96, 64, 32),
};

Vector4 guardAttackFrames[] = {
	Vector4(384,128, 64, 32),
	Vector4(448,128, 64, 32),
	Vector4(512,128, 64, 32),
	Vector4(576,128, 64, 32),
	Vector4(640,128, 64, 32),
	Vector4(704,128, 64, 32),
};

Vector4 guardStunFrames[] = {
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
};

Guard::Guard(Vector2 startingPosition) : SimObject("Guard") {
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount	= 6;
	SetCollider(new CollisionBox(Vector2(12, 15), this, Vector2(2, -1)));
	dampingValue = 0.98;
	maxVelocity = Vector2(200, 200);
	inverseMass = 0.5;
	startingPos = startingPosition;
	position = startingPos;
	activeState = GuardState::Right;
	hysteresisTimer = 0;
	stunLength = 5;
	stunTimer = 0;
	dropThreshold = 2;
	movementSpeed = 200;
	useGravity = true;
	falling = false;
	AddNonImpulseCollision("Floor");
	AddNonImpulseCollision("LadderTop");
	AddNonImpulseCollision("Ledge");
	AddNonPhysicalCollision("Guard");
	AddNonPhysicalCollision("Froggo");
}

Guard::~Guard() {
}

bool Guard::UpdateObject(float dt) {

	Vector4* animSource = guardIdleFrames;

	switch (activeState) {
		case GuardState::Attack:
			animSource = guardAttackFrames;
			AttackPlayer();
			break;
		case GuardState::Left:
			animSource = guardWalkFrames;
			flipAnimFrame = false;
			Walk(false);
			break;
		case GuardState::Right:
			animSource = guardWalkFrames;
			flipAnimFrame = true;
			Walk(true);
			break;
		case GuardState::Stunned:
			animSource = guardStunFrames;
			Stunned(dt);
			break;
		default:
			animSource = guardIdleFrames;
			break;
	}

	if (startingPos.y - position.y > dropThreshold * game->GetTileSize()) {
		startingPos = position;
	}

	activeState = DetermineState();
	hysteresisTimer += dt;

	if (falling) {
		useGravity = true;
	}
	else {
		useGravity = false;
	}

	animFrameData = animSource[currentanimFrame];

	falling = true;

	return active;
}

void NCL::CSC3222::Guard::Reset()
{
	inverseMass = 0.5;
	startingPos = position;
	activeState = GuardState::Right;
	hysteresisTimer = 0;
	stunLength = 5;
	stunTimer = 0;
	useGravity = true;
	falling = false;
}

void NCL::CSC3222::Guard::OnCollisionEnter(SimObject* col)
{
	if (col->TagEqualTo("Spell") && activeState != GuardState::Stunned) {
		activeState = GuardState::Stunned;
		game->IncrementScore(200);
		stunTimer = 0;
	}

	if (col->TagEqualTo("Player") && activeState != GuardState::Stunned) {
		game->LoseLife();
	}

	if (col->TagEqualTo("Wall") && hysteresisTimer > 1 && activeState != GuardState::Stunned) {
		activeState = activeState == GuardState::Left ? GuardState::Right : GuardState::Left;
		hysteresisTimer = 0;
	}
}

void NCL::CSC3222::Guard::OnCollision(SimObject* col)
{
	if ((col->TagEqualTo("Floor") || col->TagEqualTo("LadderTop") || col->TagEqualTo("Ledge")) && col->GetPosition().y < position.y) {
		falling = false;
	}

	// Walk in opposite direction once encountering a wall or ledge
	if (col->TagEqualTo("Ledge") && hysteresisTimer > 1 && activeState != GuardState::Stunned) {
		activeState = activeState == GuardState::Left ? GuardState::Right : GuardState::Left;
		hysteresisTimer = 0;
	}
}

void NCL::CSC3222::Guard::OnCollisionExit(SimObject* col)
{
	// Ensure collision doesn't return when player is colliding with guard during stun
	if (col->TagEqualTo("Player") && activeState != GuardState::Stunned && !PhysicalCollisionAllowed("Player")) {
		RemoveNonPhysicalCollision("Player");
	}
}

NCL::CSC3222::Guard::GuardState NCL::CSC3222::Guard::DetermineState()
{
	GuardState state = activeState;

	float distanceFromStart = abs(position.x - startingPos.x) / game->GetTileSize();
	float distanceFromPlayer = abs(position.x - game->GetPlayerPosition().x) / game->GetTileSize();
	bool playerOnSameLevel = abs(position.y - game->GetPlayerPosition().y) < 5;

	switch (activeState) {
		case GuardState::Attack:
			if (distanceFromStart > 10 && hysteresisTimer > 1.5) {				
				hysteresisTimer = 0;
				state = position.x < startingPos.x ? GuardState::Right : GuardState::Left;
			} 
			else if (distanceFromPlayer > 4 || !playerOnSameLevel) {
				state = position.x < startingPos.x ? GuardState::Left : GuardState::Right;
			} 
			break;
		case GuardState::Left:
			if (distanceFromStart > 10) {
				state = position.x < startingPos.x ? GuardState::Right : GuardState::Left;
				hysteresisTimer = 0;
			} 
			else if (distanceFromPlayer <= 3 && playerOnSameLevel && hysteresisTimer > 1.5) {
				state = GuardState::Attack;
				hysteresisTimer = 0;
			}
			break;
		case GuardState::Right:
			if (distanceFromStart > 10) {
				state = position.x < startingPos.x ? GuardState::Right : GuardState::Left;
				hysteresisTimer = 0;
			} 
			else if (distanceFromPlayer <= 3 && playerOnSameLevel && hysteresisTimer > 1.5) {
				state = GuardState::Attack;
				hysteresisTimer = 0;
			}
			break;
		case GuardState::Stunned:
			if (stunTimer >= stunLength) {
				state = position.x < startingPos.x ? GuardState::Left : GuardState::Right;
				stunTimer = 0;
				// Ensure collision doesn't return when player is colliding with guard during stun
				if (CollidingWith("Player")) {
					RemoveNonPhysicalCollision("Player");
				}
				// Reset collider for walking animation
				static_cast<CollisionBox*>(collider)->SetExtents(Vector2(12, 15));
			}
			break;
	}

	return state;
}

void NCL::CSC3222::Guard::AttackPlayer()
{
	AddForce(Vector2(movementSpeed, 0) * (game->GetPlayerPosition().x < position.x ? -1 : 1));
	flipAnimFrame = game->GetPlayerPosition().x > position.x;
}

void NCL::CSC3222::Guard::Walk(bool leftToRight)
{
	AddForce(Vector2(movementSpeed, 0) * (leftToRight ? 1 : -1));
}

void NCL::CSC3222::Guard::Stunned(float dt)
{
	if (stunTimer == 0) {
		float knockbackForce = 500;
		float knockback = (game->GetPlayerPosition().x < position.x ? 1 : -1) * knockbackForce;		
		AddImpulse(Vector2(knockback, 0));
		AddNonPhysicalCollision("Player");	
		// Change to suit animation
		static_cast<CollisionBox*>(collider)->SetExtents(Vector2(12, 9));
	}

	if (stunTimer < stunLength) {
		stunTimer += dt;
	}
}
