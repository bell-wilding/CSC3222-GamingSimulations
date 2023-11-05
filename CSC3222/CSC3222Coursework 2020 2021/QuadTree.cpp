#include "QuadTree.h"
#include "CollisionCircle.h"
#include "CollisionBox.h"

NCL::CSC3222::QuadTree::QuadTree(int depth, CollisionBox bounds) : depth(depth), bounds(bounds), nodes{} {}

NCL::CSC3222::QuadTree::~QuadTree()
{
	contents.clear();

	for (int i = 0; i < 4; i++) {
		if (nodes[i] != nullptr) {
			nodes[i]->~QuadTree();
			nodes[i] = nullptr;
		}
	}	
}

void NCL::CSC3222::QuadTree::Split()
{
	Vector2 extents = Vector2((int)bounds.GetExtents().x / 2, (int)bounds.GetExtents().y / 2);
	nodes[0] = new QuadTree(depth + 1, CollisionBox(Vector2(bounds.GetX() - extents.x, bounds.GetY() + extents.y), extents));
	nodes[1] = new QuadTree(depth + 1, CollisionBox(Vector2(bounds.GetX() + extents.x, bounds.GetY() + extents.y), extents));
	nodes[2] = new QuadTree(depth + 1, CollisionBox(Vector2(bounds.GetX() - extents.x, bounds.GetY() - extents.y), extents));
	nodes[3] = new QuadTree(depth + 1, CollisionBox(Vector2(bounds.GetX() + extents.x, bounds.GetY() - extents.y), extents));
}

void NCL::CSC3222::QuadTree::Insert(CollisionVolume* item)
{
	CollisionBox* boxRepresentation = CollisionVolume::VolumeToBox(*item);

	if (bounds.CollidesWith(*boxRepresentation)) {

		if (nodes[0] != nullptr) {
			for (int i = 0; i < 4; ++i) {
				nodes[i]->Insert(item);
			}
		}
		else {
			if (contents.size() < MAX_ITEMS) {
				contents.emplace_back(item);
			}
			else if (depth < MAX_DEPTH) {
				contents.emplace_back(item);
				Split();
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < contents.size(); ++j) {
						nodes[i]->Insert(contents[j]);
					}
				}
				contents.clear();
			}
			else {
				contents.emplace_back(item);
			}
		}
	}

	delete boxRepresentation;
	boxRepresentation = nullptr;
}

void NCL::CSC3222::QuadTree::GetLeafNodes(std::vector<QuadTree*>* leafNodes)
{
	if (nodes[0] == nullptr) {
		leafNodes->emplace_back(this);
	}
	else {
		for (int i = 0; i < 4; i++) {
			nodes[i]->GetLeafNodes(leafNodes);
		}
	}
}
