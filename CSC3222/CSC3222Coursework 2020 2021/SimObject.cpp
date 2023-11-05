#include "SimObject.h"
#include "CollisionVolume.h"
#include "GameSimsRenderer.h"
#include <algorithm>

using namespace NCL;
using namespace CSC3222;

TextureManager*  SimObject::texManager	= nullptr;
FruitWizardGame* SimObject::game		= nullptr;

SimObject::SimObject(std::string tag) : RigidBody(tag)	{
	texture		= nullptr;
	collider	= nullptr;

	animFrameCount		= 1;
	currentanimFrame	= 0;
	frameTime			= 0;
	frameRate			= 10;
	flipAnimFrame		= true;
	active				= true;
}

SimObject::~SimObject()	{
	delete collider;
}

void SimObject::InitObjects(FruitWizardGame* game, TextureManager* texManager) {
	SimObject::game			= game;
	SimObject::texManager	= texManager;
}

void SimObject::UpdateAnimFrame(float dt) {
	frameTime -= dt;

	if (frameTime < 0) {
		currentanimFrame = (currentanimFrame + 1) % animFrameCount;
		frameTime += 1.0f / frameRate;
	}
}

void NCL::CSC3222::SimObject::UpdateCollision(SimObject* collidingObject)
{
	currentFrameCollisions.insert(collidingObject);
	if (prevFrameCollisions.find(collidingObject) == prevFrameCollisions.end()) {
		OnCollisionEnter(collidingObject);
	}
	else 
	{
		OnCollision(collidingObject);
	}
}

void NCL::CSC3222::SimObject::UpdateCollisions()
{
	std::set<SimObject*> prevColliders;
	std::set_difference(prevFrameCollisions.begin(), prevFrameCollisions.end(), currentFrameCollisions.begin(), currentFrameCollisions.end(),
		std::inserter(prevColliders, prevColliders.end()));

	std::set<SimObject*>::iterator it = prevColliders.begin();
	while (it != prevColliders.end()) {
		OnCollisionExit(*it);
		it++;
	}

	prevFrameCollisions = currentFrameCollisions;
	currentFrameCollisions.clear();
}

void SimObject::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos		= Vector2(animFrameData.x, animFrameData.y);
	Vector2 texSize		= Vector2(animFrameData.z, animFrameData.w);
	Vector2 screenPos	= position;

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, flipAnimFrame);
}

void NCL::CSC3222::SimObject::OnCollisionEnter(SimObject* col)
{
}

void NCL::CSC3222::SimObject::OnCollisionExit(SimObject* col)
{
}

void NCL::CSC3222::SimObject::OnCollision(SimObject* col)
{
}

bool NCL::CSC3222::SimObject::CollidingWith(std::string tag)
{
	std::set<SimObject*>::iterator it = currentFrameCollisions.begin();
	while (it != currentFrameCollisions.end())
	{
		if ((*it)->TagEqualTo(tag)) {
			return true;
		}
		it++;
	}
	return false;
}
