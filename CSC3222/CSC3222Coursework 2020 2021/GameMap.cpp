#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "CollisionBox.h"
#include "Ladder.h"
#include "Wall.h"
#include "Floor.h"
#include "Ledge.h"
#include "LadderTop.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <math.h> 
#include <queue>

using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics* physics)	{
	tileTexture = texManager.GetTexture("FruitWizard//arcade_platformerV2.png");

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData		= new char[mapWidth * mapHeight];
	mapCosts	= new int[mapWidth * mapHeight];
	nodeData	= new MapNode[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';		

			MapNode node { x, mapHeight - 1 - y, false, false };
			node.type = c - '0';
			nodeData[tileIndex] = node;
		}
	}

	BuildMapMesh(physics);
}

GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] nodeData;
	delete[] mapTexCoords;

	delete mapMesh;
}

void GameMap::DrawMap(GameSimsRenderer & r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
}

Vector2 NCL::CSC3222::GameMap::WorldToTilePos(const Vector2& worldPos) const
{
	return Vector2(round(worldPos.x / 16), round(worldPos.y / 16));
}

Vector2 NCL::CSC3222::GameMap::TileToWorldPos(const Vector2& tilePos) const
{
	return Vector2((tilePos.x * 16), (tilePos.y * 16));
}

int NCL::CSC3222::GameMap::GetTileIndexAtPos(const Vector2& tilePos) const
{
	if (tilePos.x < 0 || tilePos.y < 0 || tilePos.x > mapWidth - 1 || tilePos.y > mapHeight - 1) {
		return -1;
	}

	return ((mapHeight - 1 - tilePos.y) * mapWidth) + tilePos.x;
}

Vector2 NCL::CSC3222::GameMap::GetTilePosFromIndex(const int& index) const
{
	if (index < 0 || index > mapWidth * mapHeight) {
		return Vector2(-1, -1);
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			if (tileIndex == index) {
				return Vector2(x, y);
			}
		}
	}
}

bool NCL::CSC3222::GameMap::GeneratePath(const Vector2& worldFrom, const Vector2& worldTo, std::vector<std::pair<Vector2, char>>& path) const
{
	ResetNodeData();
	MapNode* start = &nodeData[GetTileIndexAtPos(WorldToTilePos(worldFrom))];
	MapNode* end = &nodeData[GetTileIndexAtPos(WorldToTilePos(worldTo))];

	std::priority_queue<MapNode*, std::vector<MapNode*>, CompareMapNode> openList;

	openList.push(start);

	start->g = 0;
	start->h = heuristic(start, end);
	start->f = start->g + start->h;
	start->bestParent = nullptr;
	start->open = true;

	MapNode* currentBest = nullptr;

	while (!openList.empty()) {
		currentBest = openList.top();
		openList.pop();

		if (currentBest == end) {
			MapNode* node = end;
			while (node != nullptr) {
				// Include vector and tile type
				path.push_back(std::make_pair(TileToWorldPos(Vector2(node->x, node->y)), node->type));
				node = node->bestParent;
			}
			return true;
		}
		else {
			std::vector<MapNode*> activeNeighbours = GetActiveNeighbours(currentBest);

			for (int i = 0; i < activeNeighbours.size(); ++i) {

				MapNode* neighbour = activeNeighbours[i];

				if (neighbour->closed) {
					continue;
				}

				float h = heuristic(neighbour, end);
				float g = currentBest->g + neighbour->type;
				float f = h + g;

				if (neighbour->open && f > neighbour->f) {
					continue;
				}

				neighbour->g = g;
				neighbour->h = h;
				neighbour->f = f;
				neighbour->bestParent = currentBest;

				if (!neighbour->open) {
					openList.push(neighbour);
					neighbour->open = true;
				}
			}
			currentBest->open = false;
			currentBest->closed = true;
		}
	}
	return false;
}

void GameMap::BuildMapMesh(GameSimsPhysics* physics) {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 primaryTile;

	AddNewTilePos(0, 0, positions, Vector2(mapWidth * 16.0f, mapHeight * 16.0f));
	AddNewTileTex(0, 176, texCoords, Vector2(160, 128));

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
	
			switch (tileType) {
			case '0': {
				continue;
			}break; //Top of ladder
				//the normal tile types first
				case 'T':primaryTile = Vector2(320, 0); break; //Top of ladder
				case 'L':primaryTile = Vector2(320, 16); break; //Ladder
				case 'B':primaryTile = Vector2(320, 32); break; //Bottom of ladder

				case 't':primaryTile = Vector2(336, 0); break; //Top of ladder
				case 'l':primaryTile = Vector2(336, 16); break; //Ladder
				case 'b':primaryTile = Vector2(336, 32); break; //Bottom of ladder

				case 'G': {
					primaryTile = Vector2(224, 48);
				}break;//Random grass tiles
				case 'F': { //Left End
					primaryTile = Vector2(208, 48);
				}break;//Random grass tiles
				case 'H': { //Right End
					primaryTile = Vector2(240, 48);
				}break;//Random grass tiles

				case 'W': {//wall
					primaryTile = Vector2(256, 112);
				}break;//Random grass tiles
				case 'w': {//wall top
					primaryTile = Vector2(256, 96);
				}break;//Random grass tiles
			}			
			AddNewTilePos(x, y, positions);			
			AddNewTileSim(x, y, tileType, physics);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize) {
	Vector3 topLeft		= Vector3((x + 0) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 topRight	= Vector3((x + 1) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 bottomLeft	= Vector3((x + 0) * tileSize.x, (y + 1) * tileSize.y, 0);
	Vector3 bottomRight = Vector3((x + 1) * tileSize.x, (y + 1) * tileSize.y, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize) {
	Vector2 topLeft		= Vector2((x + 0.0f) , (y + texSize.y));
	Vector2 topRight	= Vector2((x + texSize.x), (y + texSize.y));
	Vector2 bottomLeft	= Vector2((x + 0.0f) , (y + 0.0f));
	Vector2 bottomRight = Vector2((x + texSize.x), (y + 0.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft); 
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}

void NCL::CSC3222::GameMap::AddNewTileSim(int x, int y, int tileType, GameSimsPhysics* physics, Vector2 colliderSize)
{	
	if (tileType == 'T') {

		float ladderHeight = 0;

		for (int curY = y - 1; curY >= 0; --curY) {
			if (mapData[(curY * mapWidth) + x] == 'L' || mapData[(curY * mapWidth) + x] == 'B') {
				ladderHeight++;
			}
			else {
				break;
			}
		}	

		LadderTop* ladderTop = new LadderTop();
		ladderTop->SetPosition(Vector2(x * (colliderSize.x) + colliderSize.x, ((y + 1) * colliderSize.y) - colliderSize.y / 2));
		ladderTop->SetCollider(new CollisionBox(Vector2(colliderSize.x, colliderSize.y * 0.5), ladderTop, Vector2(0, 0), false));
		physics->AddCollider(ladderTop->GetCollider());

		Ladder* ladder = new Ladder(ladderTop);
		ladder->SetPosition(Vector2((x * (colliderSize.x)) + colliderSize.x, (y * colliderSize.y) - ((ladderHeight / 2) * colliderSize.y)));
		ladder->SetCollider(new CollisionBox(Vector2(colliderSize.x * 0.2, ladderHeight / 2 * colliderSize.y), ladder, Vector2(0, 0), true));
		physics->AddCollider(ladder->GetCollider());
	}

	if (tileType == 'w') {

		float wallCount = 1;

		for (int curY = y-1; curY >= 0; --curY) {
			if (mapData[(curY * mapWidth) + x] == 'W' || mapData[(curY * mapWidth) + x] == 'G') {
				wallCount++;
			}
			else {
				break;
			}
		}

		Wall* wall = new Wall();
		wall->SetPosition(Vector2((x * colliderSize.x) + colliderSize.x / 2, wallCount/2 * colliderSize.y));
		wall->SetCollider(new CollisionBox(Vector2(colliderSize.x/2, wallCount * colliderSize.y/2), wall));
		physics->AddCollider(wall->GetCollider());
	}

	if (x == 0 && tileType == 'G') {
		AddFloorCollisionVol(x, y, physics);
	}

	if (tileType == 'F') {
		Ledge* ledge = new Ledge();
		ledge->SetPosition(Vector2(x * colliderSize.x + (colliderSize.x/2), (y * colliderSize.y) + colliderSize.y / 2));
		ledge->SetCollider(new CollisionBox(Vector2(colliderSize.x / 2, colliderSize.y / 2), ledge));
		physics->AddCollider(ledge->GetCollider());

		AddFloorCollisionVol(x+1, y, physics);
	}
}

void NCL::CSC3222::GameMap::AddFloorCollisionVol(int x, int y, GameSimsPhysics* physics, Vector2 colliderSize)
{
	bool ladderBreak = false;
	bool ledge = false;
	int breakX = 0;

	float floorCount = 0;

	for (int curX = x; curX < mapWidth; ++curX) {

		int tile = mapData[(y * mapWidth) + curX];
		if (tile == 'G') {
			floorCount++;
		}
		else {
			ledge = tile == 'H';
			ladderBreak = tile == 'T' || tile == 't';
			breakX = curX + 2;
			break;
		}
	}

	if (floorCount > 0) {
		Floor* floor = new Floor();
		floor->SetPosition(Vector2(x * colliderSize.x + (floorCount / 2 * colliderSize.x), (y * colliderSize.y) + colliderSize.y / 2));
		floor->SetCollider(new CollisionBox(Vector2(floorCount * colliderSize.x / 2, colliderSize.y / 2), floor));
		physics->AddCollider(floor->GetCollider());
	}

	if (ladderBreak) {
		AddFloorCollisionVol(breakX, y, physics);
	}

	if (ledge) {
		Ledge* ledge = new Ledge();
		ledge->SetPosition(Vector2((breakX-2) * colliderSize.x + (colliderSize.x/2), (y * colliderSize.y) + colliderSize.y / 2));
		ledge->SetCollider(new CollisionBox(Vector2(colliderSize.x / 2, colliderSize.y / 2), ledge));
		physics->AddCollider(ledge->GetCollider());

		AddFloorCollisionVol(breakX-1, y, physics);
	} 
}

void NCL::CSC3222::GameMap::ResetNodeData() const
{
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			nodeData[tileIndex].closed = false;
			nodeData[tileIndex].open = false;
			nodeData[tileIndex].g = 0;
			nodeData[tileIndex].h = 0;
			nodeData[tileIndex].f = 0;
		}
	}
}

std::vector<MapNode*> NCL::CSC3222::GameMap::GetActiveNeighbours(MapNode* node) const
{
	std::vector<MapNode*> activeNeighbours;

	if (node->x < mapWidth - 2) {
		int tileType = mapCosts[(((mapHeight - 1 - node->y)) * mapWidth) + (node->x + 1)];
		if (tileType != 0) {
			MapNode* neighbour = &nodeData[((mapHeight - 1 - node->y) * mapWidth) + (node->x + 1)];
			activeNeighbours.emplace_back(neighbour);
		}
	}

	if (node->x > 0) {
		int tileType = mapCosts[((mapHeight - 1 - node->y) * mapWidth) + (node->x - 1)];
		if (tileType != 0) {
			MapNode* neighbour = &nodeData[((mapHeight - 1 - node->y) * mapWidth) + (node->x - 1)];
			activeNeighbours.emplace_back(neighbour);
		}
	}

	if (node->y < mapHeight - 2) {
		int tileType = mapCosts[((mapHeight - node->y) * mapWidth) + node->x];
		if (tileType != 0) {
			MapNode* neighbour = &nodeData[((mapHeight - node->y) * mapWidth) + node->x];
			activeNeighbours.emplace_back(neighbour);
		}
	}

	if (node->y > 0) {
		int tileType = mapCosts[((mapHeight - 2 - node->y) * mapWidth) + node->x];
		if (tileType != 0) {
			MapNode* neighbour = &nodeData[((mapHeight - 2 - node->y) * mapWidth) + node->x];
			activeNeighbours.emplace_back(neighbour);
		}
	}
	
	return activeNeighbours;
}