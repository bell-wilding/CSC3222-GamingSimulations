#include "Froggo.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionCircle.h"
#include "FruitWizardGame.h"

using namespace NCL;
using namespace CSC3222;

Vector4 froggoIdleFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoWalkFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoLadderFrames[] = {
	Vector4(0  ,(96 * 4) + 32, 80, 64),
	Vector4(80 ,(96 * 4) + 32, 80, 64),
	Vector4(160,(96 * 4) + 32, 80, 64),
	Vector4(240,(96 * 4) + 32, 80, 64),
	Vector4(320,(96 * 4) + 32, 80, 64),
	Vector4(400,(96 * 4) + 32, 80, 64),
};

Vector4 froggoStunFrames[] = {
	Vector4(0  ,(96 * 8) + 32, 80, 64),
	Vector4(80, (96 * 8) + 32, 80, 64),
	Vector4(160,(96 * 8) + 32, 80, 64),
	Vector4(240,(96 * 8) + 32, 80, 64),
	Vector4(320,(96 * 8) + 32, 80, 64),
	Vector4(400,(96 * 8) + 32, 80, 64),
};

Vector4 froggoAttackFrames[] = {
	Vector4(0  ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(240,(96 * 1) + 32, 80, 64),
};

Vector4 froggoJumpFrames[] = {
	Vector4(0  ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(240,(96 * 7) + 32, 80, 64),
};

Froggo::Froggo(GameMap* map) : SimObject("Froggo") {
	texture			 = texManager->GetTexture("FruitWizard\\frogman_crown.png");
	animFrameCount	 = 6;
	SetCollider(new CollisionCircle(12, this, Vector2(3, -2)));
	dampingValue = 0.98;
	maxVelocity = Vector2(75, 75);
	inverseMass = 0.6;
	activeState = FroggoState::Walk;
	useGravity = false;
	prevY = position.y;
	player = nullptr;
	yChange = 0;
	stunTimer = 0;
	stunLength = 3;
	movementSpeed = 500;
	hysteresisTimer = 0;
	foundPath = false;
	AddNonImpulseCollision("Floor");
	AddNonImpulseCollision("Wall");
	AddNonImpulseCollision("Ledge");
	AddNonPhysicalCollision("LadderTop");
	this->map = map;
}

Froggo::~Froggo() {
}

bool Froggo::UpdateObject(float dt) {

	Vector4* animSource = froggoIdleFrames;

	switch (activeState) {
		case FroggoState::Attack:
			animSource = froggoAttackFrames;
			AttackPlayer();
			break;
		case FroggoState::Walk:
			animSource = froggoWalkFrames;
			FollowTarget(movementSpeed);
			break;
		case FroggoState::Stunned:
			animSource = froggoStunFrames;
			Stunned(dt);
			break;
		case FroggoState::Jump:
			animSource = froggoJumpFrames;
			FollowTarget(movementSpeed);
			break;
		case FroggoState::Climb:
			animSource = froggoLadderFrames;
			FollowTarget(movementSpeed / 2);
			break;
		case FroggoState::Guarding:
			animSource = froggoIdleFrames;
			flipAnimFrame = position.y > target->GetPosition().y;
			break;
		default:
			animSource = froggoIdleFrames;
			break;
	}

	yChange += abs(position.y - prevY);
	hysteresisTimer += dt;

	activeState = DetermineState();

	if (activeState != FroggoState::Jump && falling) {
		useGravity = true;
	}
	else {
		useGravity = false;
	}

	animFrameData = animSource[currentanimFrame];

	falling = true;
	prevY = position.y;

	return active;
}

void NCL::CSC3222::Froggo::OnCollisionEnter(SimObject* col)
{
	if (col->TagEqualTo("Floor") || col->TagEqualTo("LadderTop")) {
		yChange = 0;
	}

	if (col->TagEqualTo("Player") && activeState != FroggoState::Stunned) {
		game->LoseLife();
	}

	if (col->TagEqualTo("Spell") && activeState != FroggoState::Stunned) {
		activeState = FroggoState::Stunned;
		stunTimer = 0;
	}
}

void NCL::CSC3222::Froggo::OnCollisionExit(SimObject* col)
{
	// Ensure collision doesn't return when player is colliding with guard during stun
	if (col->TagEqualTo("Player") && activeState != FroggoState::Stunned && !PhysicalCollisionAllowed("Player")) {
		RemoveNonPhysicalCollision("Player");
	}
}

void NCL::CSC3222::Froggo::OnCollision(SimObject* col)
{
	// Fall and don't stick to tiles above
	if ((col->TagEqualTo("Floor") || col->TagEqualTo("Ledge")) && col->GetPosition().y < position.y) {
		falling = false;
	}

	if (col->TagEqualTo("Ladder") || col->TagEqualTo("LadderTop")) {
		falling = false;
	}
}

void NCL::CSC3222::Froggo::TargetObject(SimObject* object)
{
	if (player == nullptr && object->TagEqualTo("Player")) {
		player = object;
	}

	target = object;
	pathToTarget.clear();
	foundPath = map->GeneratePath(Vector2(position.x, position.y - 16), Vector2(target->GetPosition().x , target->GetPosition().y - 16), pathToTarget);

	if (foundPath) {
		// Set froggo to follow new path

		for (auto& point : pathToTarget) {
			if (point.second == 3) {
				// Add offset to ladder tiles to ensure path is down ladder centre
				point.first = Vector2((point.first.x + 16), (point.first.y + 8));
			}
			else if (point.second == 8) {
				// Add offset to gap tiles to give impression of jumping
				point.first = Vector2((point.first.x + 8), (point.first.y + 24));
			}
			else {
				point.first = Vector2((point.first.x + 8), (point.first.y + 8));
			}
		}

		currentPathIndex = pathToTarget.size() - 1;
		currentPathTarget = pathToTarget[currentPathIndex].first;
		destinationReached = false;
	}
	else {
		pathToTarget.clear();
	}
}

NCL::CSC3222::Froggo::FroggoState NCL::CSC3222::Froggo::DetermineState()
{
	FroggoState state = activeState;

	float distanceFromPlayer = abs(position.x - game->GetPlayerPosition().x) / game->GetTileSize();
	bool playerOnSameLevel = abs(position.y - game->GetPlayerPosition().y) < 8;

	switch (activeState) {
		case FroggoState::Walk:
			// Tile is a traversable gap between platforms
			if (pathToTarget.size() > 0 && pathToTarget[currentPathIndex].second == 8) {
				state = FroggoState::Jump;
			}
			else if (activeState != FroggoState::Jump && yChange > climbingThreshold) {
				state = FroggoState::Climb;
			} 
			else if (distanceFromPlayer < 2 && playerOnSameLevel && hysteresisTimer > 1) {
				state = FroggoState::Attack;
			}
			else if (destinationReached && target->TagEqualTo("PixieDust")) {
				state = FroggoState::Guarding;
				hysteresisTimer = 0;
			}
			break;
		case FroggoState::Climb:
			if (yChange < climbingThreshold && hysteresisTimer > 0.5) {
				state = FroggoState::Walk;
				hysteresisTimer = 0;
			}
			break;
		case FroggoState::Attack:
			if (distanceFromPlayer > 2.5 && hysteresisTimer > 1.5) {
				state = FroggoState::Walk;
				hysteresisTimer = 0;
				TargetObject(target);
			}
			break;
		case FroggoState::Stunned:
			if (stunTimer >= stunLength) {
				state = FroggoState::Walk;
				stunTimer = 0;
				// Ensure collision doesn't return when player is colliding with froggo during stun
				if (CollidingWith("Player")) {
					RemoveNonPhysicalCollision("Player");
				}
				TargetObject(target);
			}
			break;
		case FroggoState::Guarding:
			if (distanceFromPlayer < 2 && playerOnSameLevel && hysteresisTimer > 1) {
				state = FroggoState::Attack;
			} 
			else if (target->TagEqualTo("Player")) {
				state = FroggoState::Walk;
				hysteresisTimer = 0;
			} 
			else if ((position - target->GetPosition()).Length() > 32) {
				TargetObject(target);
				state = FroggoState::Walk;
				hysteresisTimer = 0;
			}
			break;
		case FroggoState::Jump:
			// Tile is a floor tile
			if (pathToTarget.size() > 0 && pathToTarget[currentPathIndex].second == 1) {
				state = FroggoState::Walk;
			}
			break;
	}

	return state;
}

void NCL::CSC3222::Froggo::FollowTarget(float speed)
{
	// If at destination, no current path or froggo is stuck, try and find path to target again
	if (destinationReached || !foundPath) {
		TargetObject(target);
	}

	// If path found, follow path
	if (foundPath) {
		Vector2 movementDirection = (currentPathTarget - position).Normalised();
		AddForce(movementDirection * speed);

		if (activeState == FroggoState::Walk) {
			flipAnimFrame = movementDirection.x > 0;
		}
		else if (activeState == FroggoState::Climb) {
			flipAnimFrame = movementDirection.y < 0;
		}
		else if (activeState == FroggoState::Jump) {
			flipAnimFrame = movementDirection.x < 0;
		}

		// Check if enemy is within threshold distance of path point
		if ((position - currentPathTarget).Length() < distanceThreshold) {
			--currentPathIndex;
			if (currentPathIndex == -1) {
				destinationReached = true;
				++currentPathIndex;
			}
			else {
				currentPathTarget = pathToTarget[currentPathIndex].first;
			}
		}
	}
}

void NCL::CSC3222::Froggo::AttackPlayer()
{
	AddForce(Vector2(movementSpeed, 0) * (game->GetPlayerPosition().x < position.x ? -1 : 1));
	flipAnimFrame = game->GetPlayerPosition().x > position.x;
}

void NCL::CSC3222::Froggo::Stunned(float dt)
{
	if (stunTimer == 0) {
		float knockbackForce = 500;
		float knockback = (game->GetPlayerPosition().x < position.x ? 1 : -1) * knockbackForce;
		AddImpulse(Vector2(knockback, 0));
		AddNonPhysicalCollision("Player");
	}

	if (stunTimer < stunLength) {
		stunTimer += dt;
	}
}
