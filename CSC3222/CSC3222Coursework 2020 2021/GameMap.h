#pragma once
#include <string>
#include <vector>
#include <set>

#include "GameSimsPhysics.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;

		struct MapNode {		
			int		x;
			int		y;
			bool	closed;
			bool	open;
			float	g;
			float	h;
			float	f;		
			char	type;
			MapNode* bestParent;
		};

		struct CompareMapNode {
			bool operator()(MapNode* const& n1, MapNode* const& n2) {
				return n1->f > n2->f;
			}
		};

		class GameMap	{
		public:
			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics* physics);
			~GameMap();

			void DrawMap(GameSimsRenderer & r);

			int GetMapWidth() const {
				return mapWidth;
			}

			int GetMapHeight() const {
				return mapHeight;
			}

			Vector2 WorldToTilePos(const Vector2& worldPos) const;
			Vector2 TileToWorldPos(const Vector2& tilePos) const;

			int GetTileIndexAtPos(const Vector2& tilePos) const;
			Vector2 GetTilePosFromIndex(const int& index) const;

			bool GeneratePath(const Vector2& worldFrom, const Vector2& worldTo, std::vector<std::pair<Vector2, char>>& path) const;

		protected:
			void BuildMapMesh(GameSimsPhysics* physics);
			void AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize = Vector2(16, 16));
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize = Vector2(16,16));
			void AddNewTileSim(int x, int y, int tileType, GameSimsPhysics* physics, Vector2 colliderSize = Vector2(16, 16));
			void AddFloorCollisionVol(int x, int y, GameSimsPhysics* physics, Vector2 colliderSize = Vector2(16, 16));

			float heuristic(MapNode* a, MapNode* b) const { return abs(a->x - b->x) + abs(a->y - b->y); };

			void ResetNodeData() const;

			std::vector<MapNode*> GetActiveNeighbours(MapNode* node) const;

			int mapWidth;
			int mapHeight;
			int structureCount;

			MapNode* nodeData;

			Rendering::TextureBase*	tileTexture;
			char*			mapData;
			int*			mapCosts;
			Vector2*		mapTexCoords;

			MeshGeometry* mapMesh;
		};	
	}
}

