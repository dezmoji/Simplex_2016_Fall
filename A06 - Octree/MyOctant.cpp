#include "MyOctant.h"
using namespace Simplex;

// static variables
uint MyOctant::m_uOctantCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uIdealEntityCount;

// accessor methods
float Simplex::MyOctant::GetSize(void) { return m_fSize; }
vector3 Simplex::MyOctant::GetCenterGlobal(void) { return m_v3Center; }
vector3 Simplex::MyOctant::GetMinGlobal(void) { return m_v3Min; }
vector3 Simplex::MyOctant::GetMaxGlobal(void) { return m_v3Max; }
MyOctant * Simplex::MyOctant::GetChild(uint a_nChild) { return m_pChild[a_nChild]; }
MyOctant * Simplex::MyOctant::GetParent(void) { return m_pParent; }
uint Simplex::MyOctant::GetOctantCount(void) { return m_uOctantCount; }


Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	// initialize variables
	Init();
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uEntities = m_pEntityMngr->GetEntityCount();

	// get a min and max
	m_v3Max = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();
	m_v3Min = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();

	// find the actual min and max
	for (int i = 0; i < m_uEntities; i++) {
		// add this entity
		m_EntityList.push_back(i);

		vector3 currMax = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();
		vector3 currMin = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();

		if (currMax.x > m_v3Max.x) m_v3Max.x = currMax.x;
		if (currMax.y > m_v3Max.y) m_v3Max.y = currMax.y;
		if (currMax.z > m_v3Max.z) m_v3Max.z = currMax.z;

		if (currMin.x < m_v3Min.x) m_v3Min.x = currMin.x;
		if (currMin.y < m_v3Min.y) m_v3Min.y = currMin.y;
		if (currMin.z < m_v3Min.z) m_v3Min.z = currMin.z;
	}

	// caluclate the new center and size
	m_v3Center = (m_v3Max + m_v3Min) *.5f;
	m_fSize = abs(m_v3Max.x) + abs(m_v3Min.x);

	// set this octant to be the root
	m_pRoot = this;

	// construct the tree 
	ConstructTree(m_uMaxLevel);

	// construct the list 
	ConstructList();

}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	// initialize variables
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	// find the min and max
	m_v3Min = m_v3Center - m_fSize * .5f;
	m_v3Max = m_v3Center + m_fSize * .5f;

}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	// initialize and copy variables
	Init();
	m_pEntityMngr = other.m_pEntityMngr;
	m_pMeshMngr = other.m_pMeshMngr;
	m_uLevel = other.m_uLevel;
	m_uMaxLevel = other.m_uMaxLevel;
	m_uOctantCount = other.m_uOctantCount;
	m_uIdealEntityCount = other.m_uIdealEntityCount;
	m_uID = other.m_uID;
	m_fSize = other.m_fSize;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_pParent = other.m_pParent;

	//loops through and recursively copy/create more nodes
	m_uChildren = other.m_uChildren;
	for (uint i = 0; i < m_uChildren; ++i) {
		m_pChild[i] = new MyOctant(*other.m_pChild[i]);
	}

	// find the new amount of entities
	m_uEntities = other.m_uEntities;

	//create a new entity list
	for (uint i = 0; i < m_uEntities; ++i) {
		m_EntityList.push_back(other.m_EntityList[i]);
	}

	//if it's root, copy m_lChildren over
	if (this == m_pRoot) {
		float fChildCount = other.m_lChild.size();

		for (uint i = 0; i < fChildCount; ++i) {
			m_lChild.push_back(other.m_lChild[i]);
		}
	}
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	// return if it is the same octant
	if (this == &other) return *this;

	// release and then reinitialize
	Release();
	Init();

	// swap the values
	MyOctant temp(other);
	Swap(temp);

	// return this
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	// release
	Release();
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
	// swap all values
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uMaxLevel, other.m_uMaxLevel);
	std::swap(m_uOctantCount, other.m_uOctantCount);
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uEntities, other.m_uEntities);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild, other.m_pChild);
	std::swap(m_EntityList, other.m_EntityList);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
	std::swap(m_pEntityMngr, other.m_pEntityMngr);
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
}

void Simplex::MyOctant::ConstructList(void)
{
	// if this is a leaf, assign an ID
	if (IsLeaf()) {
		AssignIDtoEntity();
	}

	// otherwise construct a list
	else {
		for (uint i = 0; i < m_uChildren; i++) {
			m_pChild[i]->ConstructList();
		}
	}
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	// check for current level vs max level and ideal entity count vs current entity count
	if (a_nMaxLevel <= m_uLevel || !ContainsMoreThan(m_uIdealEntityCount)) {
		m_pRoot->m_lChild.push_back(this);
		return; 
	}

	// don't proceed if there are 8 children
	if (m_uChildren == 8) {
		return;
	}

	// subdivide
	Subdivide();

	// loop through the children
	for (uint i = 0; i < m_uChildren; i++) {
		// set their levels and roots
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pRoot = m_pRoot;
	
		// adds colliding entities to entity list
		for (uint k = 0; k < m_uEntities; k++) {
			if (m_pChild[i]->IsColliding(m_EntityList[k])) m_pChild[i]->m_EntityList.push_back(m_EntityList[k]);
		}

		// find the new entity count
		m_pChild[i]->m_uEntities = m_pChild[i]->m_EntityList.size();

		// construct tree again
		m_pChild[i]->ConstructTree(a_nMaxLevel);
	}

}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	// obtain the rigidbody and it's min and max
	MyRigidBody* tempRB = m_pEntityMngr->GetRigidBody(a_uRBIndex);
	vector3 tempRB_min = tempRB->GetMinGlobal();
	vector3 tempRB_max = tempRB->GetMaxGlobal();

	// test for collisions
	if (tempRB_max.x > m_v3Min.x &&
		tempRB_min.x < m_v3Max.x &&
		tempRB_max.y > m_v3Min.y &&
		tempRB_min.y < m_v3Max.y &&
		tempRB_max.z > m_v3Min.z &&
		tempRB_min.z < m_v3Max.z) {
		return true;
	}

	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	// display leafs if index is greater than octants
	if (a_nIndex >= m_uOctantCount) {
		DisplayLeafs();
		return;
	}

	// display the children
	m_lChild[a_nIndex]->Display(a_v3Color);
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	// adds a cube to be rendered
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(IDENTITY_M4, m_v3Max - m_v3Min), a_v3Color);
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	// display if this is a leaf
	if (IsLeaf()) Display(a_v3Color);

	// find a leaf to display
	else {
		for (uint i = 0; i < m_uChildren; i++) {
			m_pChild[i]->DisplayLeafs();
		}
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	// clear the entities in each child
	for (uint i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ClearEntityList();
	}

	// clear entities here
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	// get a fourth of the size
	float fourth = m_fSize * .25f;

	// create the children
	m_pChild[0] = new MyOctant(m_v3Center + vector3(-fourth, fourth, -fourth), m_fSize * .5f);
	m_pChild[1] = new MyOctant(m_v3Center + vector3(-fourth, fourth, fourth), m_fSize * .5f);
	m_pChild[2] = new MyOctant(m_v3Center + vector3(-fourth, -fourth, -fourth), m_fSize * .5f);
	m_pChild[3] = new MyOctant(m_v3Center + vector3(-fourth, -fourth, fourth), m_fSize * .5f);
	m_pChild[4] = new MyOctant(m_v3Center + vector3(fourth, -fourth, -fourth), m_fSize * .5f);
	m_pChild[5] = new MyOctant(m_v3Center + vector3(fourth, -fourth, fourth), m_fSize * .5f);
	m_pChild[6] = new MyOctant(m_v3Center + vector3(fourth, fourth, -fourth), m_fSize * .5f);
	m_pChild[7] = new MyOctant(m_v3Center + vector3(fourth, fourth, fourth), m_fSize * .5f);

	// parent the children to this
	for (int i = 0; i < 8; i++) {
		m_pChild[i]->m_pParent = this;
		m_uChildren++;
	}
}

bool Simplex::MyOctant::IsLeaf(void)
{
	// if there are children, this is not a leaf
	return m_uChildren == 0;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return m_uEntities > a_nEntities;
}

void Simplex::MyOctant::KillBranches(void)
{
	// check to see if this is a leaf or branch
	if (IsLeaf()) return;

	// if this is a branch, kill the branches in the children
	for (uint i = 0; i < m_uChildren; i++) {
		m_pChild[i]->KillBranches();
		SafeDelete(m_pChild[i]);
	}
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	// add dimensions to entities
	for (uint i = 0; i < m_EntityList.size(); i++) {
		m_pEntityMngr->GetEntity(m_EntityList[i])->AddDimension(m_uID);
	}
}

void Simplex::MyOctant::Init(void)
{
	// get instances of the managers
	m_pEntityMngr = m_pEntityMngr->GetInstance();
	m_pMeshMngr = m_pMeshMngr->GetInstance();

	// new values for ID and OctantCount
	m_uID = m_uOctantCount;
	m_uOctantCount++;
}

void Simplex::MyOctant::Release(void)
{
	// kill all branches 
	if (this == m_pRoot) {
		KillBranches();
	}

	// set octant count to 0
	m_uOctantCount = 0;
}