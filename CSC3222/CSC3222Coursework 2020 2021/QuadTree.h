#pragma once

#include "CollisionVolume.h"
#include "CollisionBox.h"
#include <vector>

namespace NCL {
	namespace CSC3222 {
		class QuadTree {
		public:
			QuadTree(int level, CollisionBox bounds);
			~QuadTree();
			void Insert(CollisionVolume* item);
			void GetLeafNodes(std::vector<QuadTree*>* leafNodes);
			std::vector <CollisionVolume*> GetContents() { return contents; };

		protected:
			int MAX_DEPTH = 5;
			int MAX_ITEMS = 10;

			int depth;
			std::vector <CollisionVolume*> contents;
			QuadTree* nodes[4];
			CollisionBox bounds;

			void Split();
			QuadTree* GetNodes() { return *nodes; };
			bool IsLeaf() { return nodes[0]; }
		};
	}
}