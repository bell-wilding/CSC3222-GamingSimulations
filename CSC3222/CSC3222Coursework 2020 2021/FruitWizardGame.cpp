#include "FruitWizardGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "Fruit.h"
#include "Guard.h"
#include "Froggo.h"
#include "Pixie.h"
#include "PixieDust.h"
#include "Spell.h"
#include "CollisionCircle.h"
#include "CollisionBox.h"
#include <algorithm>
#include <random>

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

FruitWizardGame::FruitWizardGame()	{
	renderer	= new GameSimsRenderer();
	texManager	= new TextureManager();
	physics		= new GameSimsPhysics();
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

FruitWizardGame::~FruitWizardGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

void FruitWizardGame::Update(float dt) {
	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;

	UpdateGameState(dt);

	renderer->Update(dt);
	physics->FixedUpdate(dt);
	currentMap->DrawMap(*renderer);

	srand((int)(gameTime * 1000.0f));

	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		(*i)->UpdateAnimFrame(dt);
		if (!(*i)->UpdateObject(dt)) { //object has said its finished with
			physics->RemoveCollider((*i)->GetCollider());
			physics->RemoveRigidBody((*i));
			delete (*i);
			 i = gameObjects.erase(i);
		}
		else {
			(*i)->DrawObject(*renderer);
			++i;
		}
	}

	renderer->DrawString("Score:" + std::to_string(currentScore), 
		Vector2(32, 12), Vector4(1,1,1,1), 100.0f);
	renderer->DrawString("Lives:" + std::to_string(lives), 
		Vector2(144, 12), Vector4(1, 0,0, 1), 100.0f);
	renderer->DrawString("Magic:" + std::to_string(magicCount), 
		Vector2(256, 12), Vector4(1, 0, 0.5f, 1), 100.0f);
	renderer->DrawString("Dust:"  + std::to_string(dustCount), 
		Vector2(366, 12), Vector4(0.5f, 0.3f, 0.8f, 1), 100.0f);

	// Remove to stop rendering colliders
	//RenderColliders();
	// Remove to stop rendering Froggo pathfinding
	//RenderPathfinding();

	renderer->Render();

	// Reset game if lives = -1
	if (resetGame) {
		ResetGame();
		resetGame = false;
	}
}

void NCL::CSC3222::FruitWizardGame::CollectDust(PixieDust* dust)
{
	if (froggoActive && dust == froggo->GetTarget()) {
		froggo->TargetObject(player);
	}
}

void NCL::CSC3222::FruitWizardGame::LoseLife()
{
	--lives;
	if (lives == -1) {
		resetGame = true;
	}
	else {
		resetLevel = true;
	}
}

void NCL::CSC3222::FruitWizardGame::RemoveFruit(Fruit* fruit)
{
	activeFruit.erase(std::remove(activeFruit.begin(), activeFruit.end(), fruit), activeFruit.end());
}

void NCL::CSC3222::FruitWizardGame::RemovePixie(Pixie* pixie)
{
	activePixies.erase(std::remove(activePixies.begin(), activePixies.end(), pixie), activePixies.end());
}

void FruitWizardGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	currentMap = new GameMap("FruitWizardMap.txt", gameObjects, *texManager, physics);

	tileSize = 16;
	renderer->SetScreenProperties(tileSize, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter();
	player->SetPosition(Vector2(32, 32));
	AddNewObject(player);

	spawnPositions.emplace_back(Vector2(240, 8), 216);
	spawnPositions.emplace_back(Vector2(24, 72), 96);
	spawnPositions.emplace_back(Vector2(24, 200), 176);
	spawnPositions.emplace_back(Vector2(24, 264), 80);
	spawnPositions.emplace_back(Vector2(200, 72), 256);
	spawnPositions.emplace_back(Vector2(200, 136), 256);
	spawnPositions.emplace_back(Vector2(232, 264), 224);
	spawnPositions.emplace_back(Vector2(308, 200), 36);

	resetLevel = false;
	resetGame = false;
	pixieDustTimer = 0;

	SpawnGuards(4);
	SpawnFruit(16);

	gameTime		= 0;
	currentScore	= 0;
	magicCount		= 4;
	dustCount		= 0;
	lives			= 3;
}

void NCL::CSC3222::FruitWizardGame::ResetGame()
{
	player->Reset();

	for (auto enemy : activeEnemies) {
		enemy->Remove();
		enemy = nullptr;
	}
	activeEnemies.clear();

	for (auto fruit : activeFruit) {
		fruit->Remove();
		fruit = nullptr;
	}
	activeFruit.clear();

	for (auto dust : activePixieDust) {
		dust->Remove();
		dust = nullptr;
	}
	activePixieDust.clear();

	for (auto pixie : activePixies) {
		pixie->Remove();
		pixie = nullptr;
	}
	activePixies.clear();

	SpawnGuards(4);
	SpawnFruit(16);
	
	froggoActive	= false;
	pixieDustTimer	= 0;
	currentScore	= 0;
	magicCount		= 4;
	dustCount		= 0;
	lives			= 3;
}

void NCL::CSC3222::FruitWizardGame::ResetLevel()
{
	player->Reset();

	if (froggoActive) {
		froggo->SetPosition(Vector2(420, 285));
		froggo->SetVelocity(Vector2(0, 0));
		if (activePixieDust.size() > 0) {
			froggo->TargetObject(activePixieDust[0]);
		}
		else {
			froggo->TargetObject(player);
		}
	}

	for (auto pixie : activePixies) {
		pixie->Remove();
		pixie = nullptr;
	}
	activePixies.clear();

	RedistributeGuards();
	RedistributeFruit();
}

void NCL::CSC3222::FruitWizardGame::UpdateGameState(float dt)
{
	// Reset level if touching an enemy
	if (resetLevel) {
		ResetLevel();
		resetLevel = false;
	}

	// Spawn new fruit, enemy and froggo if all fruit collected
	if (activeFruit.size() == 0) {
		IncrementScore(5000);
		SpawnFruit(16);
		SpawnGuards(1);
		if (!froggoActive) {
			SpawnFroggo();
		}
	}

	// Potentially spawn pixie dust every 30 seconds
	pixieDustTimer += dt;
	if (pixieDustTimer >= 30) {
		// Get non-pseudo-random number
		std::random_device rd;
		std::mt19937 e2(rd());
		std::uniform_real_distribution<> dist(0, 1);
		if (activePixieDust.size() < 4 && dustCount < 4 && dist(e2) < 0.8) {
			SpawnPixieDust();
		}
		pixieDustTimer = 0;
	}

	// Initiate Pixie Mode if player has 4 pixie dust
	if (dustCount == 4 && Window::GetKeyboard()->KeyDown(KeyboardKeys::CONTROL)) {
		ActivatePixieMode();
		dustCount = 0;
	}
}

void FruitWizardGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}

void NCL::CSC3222::FruitWizardGame::RenderColliders()
{
	for (auto& collider : physics->AllColliders()) {
		std::string tag = collider->GetSimObject()->GetTag();

		if (tag == "Wall" || tag == "Floor" || tag == "Ladder" || tag == "Ledge" || tag == "LadderTop") {
			CollisionBox* box = static_cast<CollisionBox*>(collider);
			renderer->DrawBox(collider->GetPosition(), box->GetExtents(), Vector4(1, 1, 1, 1));
		}
		else if (tag == "Spell") {
			renderer->DrawCircle(collider->GetPosition(), 7, Vector4(1, 0, 0, 1));
		}
		else if (tag == "Guard") {
			renderer->DrawBox(collider->GetPosition(), Vector2(12, 15), Vector4(1, 0, 0, 1));
		}
		else if (tag == "Froggo") {
			renderer->DrawCircle(collider->GetPosition(), 12, Vector4(1, 0, 0, 1));
		}
		else if (tag == "Fruit") {
			renderer->DrawCircle(collider->GetPosition(), 6, Vector4(1, 0, 0, 1));
		}
		else if (tag == "PixieDust") {
			renderer->DrawCircle(collider->GetPosition(), 10, Vector4(1, 0, 0, 1));
		}
		else if (tag == "Player") {
			renderer->DrawCircle(collider->GetPosition(), 10.5, Vector4(1, 0, 0, 1));
		}
	}
}

void NCL::CSC3222::FruitWizardGame::RenderPathfinding()
{
	if (froggoActive) {
		std::vector<std::pair<Vector2, char>> pathPoints = froggo->GetPath();
		for (int i = 0; i < pathPoints.size(); ++i) {
			if (i < pathPoints.size() - 2) {
				renderer->DrawLine(pathPoints[i].first, pathPoints[i + 1].first, Vector4(1, 1, 1, 1));
			}
		}
	}
}

void NCL::CSC3222::FruitWizardGame::ActivatePixieMode()
{
	for (auto dust : activePixieDust) {
		dust->Remove();
	}
	activePixieDust.clear();

	player->RemoveAllDust();

	for (int i = 0; i < 8; i++) {
		Pixie* pixie = new Pixie();
		Vector2 randomPos = Vector2(rand() % 450, rand() % 300);
		pixie->SetPosition(randomPos);
		AddNewObject(pixie);
		activePixies.emplace_back(pixie);
	}
}

void NCL::CSC3222::FruitWizardGame::SpawnGuards(int amount)
{
	int heightOffset = 24;
	std::vector<std::pair<Vector2, int>> spawnPosTemp = spawnPositions;

	for (int i = 0; i < amount; ++i) {
		int spawnPosIndex = rand() % spawnPosTemp.size();
		Guard* guard = new Guard(Vector2(spawnPosTemp[spawnPosIndex].first.x + (rand() % spawnPosTemp[spawnPosIndex].second), spawnPosTemp[spawnPosIndex].first.y + heightOffset));
		AddNewObject(guard);
		if (spawnPosTemp.size() > (4 - i)) {
			spawnPosTemp.erase(spawnPosTemp.begin() + spawnPosIndex);
		}
		activeEnemies.emplace_back(guard);
	}
}

void NCL::CSC3222::FruitWizardGame::SpawnFruit(int amount)
{
	int heightOffset = 14;

	for (int i = 0; i < amount; ++i) {
		int spawnPosIndex = rand() % spawnPositions.size();
		Fruit* fruit = new Fruit();
		fruit->SetPosition(Vector2(spawnPositions[spawnPosIndex].first.x + (rand() % spawnPositions[spawnPosIndex].second), spawnPositions[spawnPosIndex].first.y + heightOffset));
		AddNewObject(fruit);
		activeFruit.emplace_back(fruit);
	}
}

void NCL::CSC3222::FruitWizardGame::SpawnPixieDust()
{
	int heightOffset = 28;

	int spawnPosIndex = rand() % spawnPositions.size();
	PixieDust* dust = new PixieDust();
	dust->SetPosition(Vector2(spawnPositions[spawnPosIndex].first.x + (rand() % spawnPositions[spawnPosIndex].second), spawnPositions[spawnPosIndex].first.y + heightOffset));
	AddNewObject(dust);
	activePixieDust.emplace_back(dust);
	if (froggoActive) {
		froggo->TargetObject(dust);
	}
}

void NCL::CSC3222::FruitWizardGame::SpawnFroggo()
{
	froggo = new Froggo(currentMap);
	froggo->SetPosition(Vector2(420, 285));
	froggo->TargetObject(player);
	AddNewObject(froggo);
	activeEnemies.emplace_back(froggo);
	froggoActive = true;
}

void NCL::CSC3222::FruitWizardGame::RedistributeGuards()
{
	int heightOffset = 28;
	std::vector<std::pair<Vector2, int>> spawnPosTemp = spawnPositions;

	for (int i = 0; i < activeEnemies.size(); ++i) {
		if (activeEnemies[i]->TagEqualTo("Guard")) {
			int spawnPosIndex = rand() % spawnPosTemp.size();
			activeEnemies[i]->SetPosition(Vector2(spawnPosTemp[spawnPosIndex].first.x + (rand() % spawnPosTemp[spawnPosIndex].second), spawnPosTemp[spawnPosIndex].first.y + heightOffset));
			if (spawnPosTemp.size() > (4 - i)) {
				spawnPosTemp.erase(spawnPosTemp.begin() + spawnPosIndex);
			}
			static_cast<Guard*>(activeEnemies[i])->Reset();
		}
	}
}

void NCL::CSC3222::FruitWizardGame::RedistributeFruit()
{
	int heightOffset = 14;

	for (auto fruit : activeFruit) {
		int spawnPosIndex = rand() % spawnPositions.size();
		fruit->SetPosition(Vector2(spawnPositions[spawnPosIndex].first.x + (rand() % spawnPositions[spawnPosIndex].second), spawnPositions[spawnPosIndex].first.y + heightOffset));
	}
}


