#pragma once

#include "Geometry/AABB.h"
#include <list>

class GameObject;

class OctreeNode
{
private:
	AABB box;
	std::list<GameObject*> objects;
	OctreeNode* children[8];
	OctreeNode* parent = nullptr;
public:
	OctreeNode(const AABB& box);
	~OctreeNode();

	void Insert(GameObject* go);
	bool Erase(GameObject* go);
	void OnChildrenErase();
	void CreateChildren();
	void ForwardToChildren();
	void Draw();

	const AABB& GetBox() const { return box; }
};

class Octree
{
private:
	OctreeNode* root = nullptr;
public:
	Octree();
	~Octree();
	void SetBoundaries(const AABB& box);

	void Insert(GameObject* go);
	void Erase(GameObject* go);
	void UpdateGO(GameObject* go);
	void CleanUp();
	void Draw();

	const OctreeNode* GetRoot() const { return root; }
};
