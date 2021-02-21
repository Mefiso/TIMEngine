#pragma once

#include "Geometry/AABB.h"
#include "Geometry/LineSegment.h"
#include <list>

class GameObject;

// ******************************************* //
// *************** OCTREE NODE *************** //
// ******************************************* //
class OctreeNode
{
private:
	AABB					box;							// Bounding Box that limits the area of the Node (Collision detection is made against it)
	std::list<GameObject*>	objects;						// List of GameObjects that are held by this node
	OctreeNode*				children[8];					// List of child Nodes in the octree
	OctreeNode*				parent			= nullptr;		// Parent Node in the octree

public:
	OctreeNode(const AABB& _box);							// Constructor
	~OctreeNode();											// Destructor

	void	Insert(GameObject* _go);						// Inserts a GameObject to be held by this node. If the capacity is full opens a new level in the octree and forwards the GameObject list
	void	CreateChildren();								// Subfunction of Insert. If the parent capacity is full, open a new level of Nodes
	void	ForwardToChildren();							// Subfunction of Insert. Forwards the GameObject list to the newly created level and distributes the GameObjects between them. Note: If a GameObject Intersects with all the child nodes, it stays at the parent!
	bool	Erase(GameObject* _go);							// Removes the GameObject from this Node
	void	OnErase(std::list<GameObject*>& _childObjects);	// Subfunction of Erase. When erasing objects, if the parent Node has enough capacity, push the objects back to the parent and remove the octree level.
	void	Draw();											// Draws the lines of the AABB

	void		CollectFrustumIntersections(std::vector<GameObject*>& _intersectedObj, const float4 _frustumPlanes[6], const float3 _frustumPoints[8]);	// Checks for an intersection of all the GameObjects AABB in this Node against a Frustum. All the intesected GameObject are returned in '_intersectedObj' as a reference
	const void	CollectLineIntersections(LineSegment _ray, std::list<GameObject*> &_intersectedObj) const;												// Checks for an intersection of all the GameObjects AABB in this Node against a LineSegment. All the intesected GameObject are returned in '_intersectedObj' as a reference

	// ---------- Getters ---------- //
	const AABB& GetBox() const { return box; }

private:
	bool BoxInFrustum(const AABB& _box, const float4 _frustumPlanes[6], const float3 _frustumPoints[8]);	// Subfunction of CollectFrustumIntersections(). Manually check for intesections against each plane of the frustum
};

// ******************************************* //
// ************* OCTREE TREE (xd) ************ //
// ******************************************* //
class Octree
{
private:
	OctreeNode* root = nullptr;		// Node root of the Octree. AKA Level 0

public:
	Octree();						// Constructor
	~Octree();						// Destructor

	void CleanUp();					// Deletes all memory storage of the octree. Called in the Destructor
	void Insert(GameObject* _go);	// Inserts a new GameObject into the tree. Starts the Insert recursion in the Nodes
	void Erase(GameObject* _go);	// Removes a GameObject in the tree. Starts the Erase recursion in the Nodes
	void UpdateGO(GameObject* _go);	// Calls Erase(go) and Insert(go). Reinserts a GameObject into the tree when the given GameObjects changes its position in the world
	void Draw();					// Draws the lines that define the edges of all the tree Nodes. Starts the Draw recursion in the Nodes

	void		CollectFrustumIntersections(std::vector<GameObject*>& _intersectedObj, const float4 _frustumPlanes[6], const float3 _frustumPoints[8]);	// Checks for an intersection of all the GameObjects AABB in this Node against a Frustum. Starts the CollectFrustumIntersections recursion in the Nodes
	const void	CollectLineIntersections(LineSegment _ray, std::list<GameObject*>& _intersectedObj) const;												// Checks for an intersection of all the GameObjects AABB in this Node against a LineSegment. Starts the CollectLineIntersections recursion in the Nodes

	// ---------- Getters ---------- //
	const OctreeNode* GetRoot() const { return root; }

	// ---------- Setters ---------- //
	void SetBoundaries(const AABB& _box);	// Sets the dimensions of the Octree Level 0. ATTENTION. It resets the octree, and all the Node hierarchy will be lost. Use this only at the start of the compilation, before inserting any GameObjects

};
