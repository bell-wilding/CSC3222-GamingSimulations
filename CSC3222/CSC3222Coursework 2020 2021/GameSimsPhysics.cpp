#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "../../Common/Vector2.h"

using namespace NCL;
using namespace CSC3222;

GameSimsPhysics::GameSimsPhysics()	{
	timeRemaining = 0;
}

GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::Update(float dt) {
	for (auto& body : allBodies) {
		if (body->useGravity) {
			body->AddForce(Vector2(0, -400));
		}
	}

	IntegrateAcceleration(dt);
	CollisionDetection(dt);
	CollisionResolution();
	IntegrateVelocity(dt);	

	for (auto& body : allBodies) {
		body->force = Vector2(0, 0);
	}

	for (auto& collision : collisionPairs) {
		delete collision;
	}
	collisionPairs.clear();
}

void NCL::CSC3222::GameSimsPhysics::FixedUpdate(float dt)
{
	timeRemaining += dt;

	const float subTimeDelta = 1.0 / 120;

	while (timeRemaining > subTimeDelta) {
		Update(subTimeDelta);
		timeRemaining -= subTimeDelta;
	}
}

void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}

void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
}

void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}

void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}

void GameSimsPhysics::Integration(float dt) {
	IntegrateAcceleration(dt);
	IntegrateVelocity(dt);
}

void GameSimsPhysics::CollisionDetection(float dt) {

	QuadTree* root = new QuadTree(0, CollisionBox(Vector2(240, 160), Vector2(240, 160)));

	for (int i = 0; i < allColliders.size(); ++i) {
		root->Insert(allColliders[i]);		
	}

	std::vector <QuadTree*> leafNodes;
	root->GetLeafNodes(&leafNodes);

	for (int i = 0; i < leafNodes.size(); ++i) {
		NarrowPhase(leafNodes.at(i));
	}

	delete root;
	root = nullptr;
}

void NCL::CSC3222::GameSimsPhysics::CollisionResolution()
{
	for (auto& collision : collisionPairs) {

		// Determine if collision if physical (i.e. should actually collide)
		bool physicalCollision = collision->firstVol->PhysicalCollisionAllowed(collision->secondVol->GetSimObjectTag())
			&& collision->secondVol->PhysicalCollisionAllowed(collision->firstVol->GetSimObjectTag());

		if ((!collision->firstVol->IsTrigger() && !collision->secondVol->IsTrigger()) && physicalCollision) {
			// Perform Projection
			Projection(collision);
			
			// Determine if collision should be impulse-based
			bool impulseCollision = collision->firstVol->ImpulseCollisionAllowed(collision->secondVol->GetSimObjectTag())
				&& collision->secondVol->ImpulseCollisionAllowed(collision->firstVol->GetSimObjectTag());

			// Perform impulse collision
			if (impulseCollision) {
				Impulse(collision);
			}
		}
		collision->firstVol->GetSimObject()->UpdateCollision(collision->secondVol->GetSimObject());
		collision->secondVol->GetSimObject()->UpdateCollision(collision->firstVol->GetSimObject());
	}

	for (auto& collider : allColliders) {
		collider->GetSimObject()->UpdateCollisions();
	}
}

void NCL::CSC3222::GameSimsPhysics::Projection(CollisionPair* collision)
{
	float objMassA = collision->firstVol->GetSimObject()->GetInverseMass();
	float objMassB = collision->secondVol->GetSimObject()->GetInverseMass();
	float massTotal = objMassA + objMassB;
	collision->firstVol->GetSimObject()->SetPosition(collision->firstVol->GetSimObject()->GetPosition() - collision->normal * collision->penetration * (objMassA / massTotal));
	collision->secondVol->GetSimObject()->SetPosition(collision->secondVol->GetSimObject()->GetPosition() + collision->normal * collision->penetration * (objMassB / massTotal));
}

void NCL::CSC3222::GameSimsPhysics::Impulse(CollisionPair* collision)
{
	float e = 1;
	float objMassA = collision->firstVol->GetSimObject()->GetInverseMass();
	float objMassB = collision->secondVol->GetSimObject()->GetInverseMass();

	Vector2 relativeVelocity = collision->secondVol->GetSimObject()->GetVelocity() - collision->firstVol->GetSimObject()->GetVelocity();
	float J = -(1 + e) * Vector2::Dot(relativeVelocity, collision->normal)/(objMassA + objMassB);

	collision->firstVol->GetSimObject()->AddImpulse(-collision->normal * J);
	collision->secondVol->GetSimObject()->AddImpulse(collision->normal * J);
}

void GameSimsPhysics::IntegrateAcceleration(float dt) {
	for (auto& body : allBodies) {
		Vector2 acceleration = body->force * body->inverseMass;
		body->velocity += acceleration * dt;
	}
}

void GameSimsPhysics::IntegrateVelocity(float dt) {
	for (auto& body : allBodies) {	
		if (body->velocity.x > body->maxVelocity.x) body->velocity.x = body->maxVelocity.x;
		if (body->velocity.x < -body->maxVelocity.x) body->velocity.x = -body->maxVelocity.x;
		if (body->velocity.y > body->maxVelocity.y) body->velocity.y = body->maxVelocity.y;
		if (body->velocity.y < -body->maxVelocity.y) body->velocity.y = -body->maxVelocity.y;

		body->position = body->position + body->velocity * dt;
		body->velocity *= body->dampingValue;		
	}
}

void NCL::CSC3222::GameSimsPhysics::NarrowPhase(QuadTree* leafNode)
{
	std::vector<CollisionVolume*> colliders = leafNode->GetContents();

	for (int i = 0; i < colliders.size(); ++i) {
		for (int j = i + 1; j < colliders.size(); ++j) {

			// Two static bodies cannot collide
			if (!colliders.at(i)->BodyIsStatic() || !colliders.at(j)->BodyIsStatic()) {

				CollisionPair* collision = colliders.at(i)->Collides(*colliders.at(j));

				if (collision) {
					collisionPairs.emplace_back(collision);
				}
			}		
		}
	}
}

