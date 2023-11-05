#pragma once
#include <vector>
#include "PlayerCharacter.h"
#include "Guard.h"
#include "Fruit.h"
#include "Froggo.h"
#include "Pixie.h"
#include "PixieDust.h"

namespace NCL::CSC3222 {
	class GameMap;
	class SimObject;
	class GameSimsRenderer;
	class GameSimsPhysics;
	class TextureBase;
	class TextureManager;
	class PlayerCharacter;

	class FruitWizardGame {
	public:
		FruitWizardGame();
		~FruitWizardGame();

		void Update(float dt);

		void AddNewObject(SimObject* object);

		Vector2 GetPlayerPosition() { return player->GetPosition();	}

		float GetTileSize() { return tileSize; }

		void IncrementScore(int amount) { currentScore += amount; };
		void IncrementSpells(int amount) { magicCount += amount; };
		void IncrementDust(int amount) { dustCount += amount; };

		void CollectDust(PixieDust* dust);

		void LoseLife();

		void RemoveFruit(Fruit* fruit);
		void RemovePixie(Pixie* pixie);

		void SetSpellCount(int newCount) { magicCount = newCount; };
		int GetSpellCount() { return magicCount; };

		std::vector<SimObject*> GetActiveEnemies() { return activeEnemies; };

	protected:
		void InitialiseGame();
		void ResetGame();
		void ResetLevel();
		void UpdateGameState(float dt);

		void RenderColliders();
		void RenderPathfinding();

		void ActivatePixieMode();

		void SpawnGuards(int amount);
		void SpawnFruit(int amount);
		void SpawnPixieDust();
		void SpawnFroggo();

		void RedistributeGuards();

		void RedistributeFruit();		

		TextureManager*		texManager;
		GameSimsRenderer*	renderer;
		GameSimsPhysics*	physics;
		GameMap*			currentMap;

		PlayerCharacter* player;
		Froggo* froggo;

		float gameTime;

		float tileSize;

		int currentScore;
		int magicCount;
		int dustCount;
		int lives;

		bool froggoActive;
		bool resetLevel;
		bool resetGame;

		float pixieDustTimer;

		std::vector<std::pair<Vector2, int>> spawnPositions;

		std::vector<SimObject*> activeEnemies;
		std::vector<Fruit*> activeFruit;
		std::vector<PixieDust*> activePixieDust;
		std::vector<Pixie*> activePixies;

		std::vector<SimObject*> gameObjects;
		std::vector<SimObject*> newObjects;
	};
}