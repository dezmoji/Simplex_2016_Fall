#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList.clear();
}

void MyEntityManager::Release(void)
{
	for (uint i = 0; i < m_uEntityCount; i++) {
		SafeDelete(m_entityList[i]);
	}

	m_uEntityCount = 0;
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	SafeDelete(m_pInstance);
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// loop through the list to check the ID's
	for (uint i = 0; i < m_uEntityCount; i++) {

		// if the strings are equal, return the current index
		if (m_entityList[i]->GetUniqueID == a_sUniqueID) {
			return i;
		}
	}

	// if nothing is found
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// avoid index out of bounds error
	if (a_uIndex > m_uEntityCount - 1) return nullptr;

	// return the model
	return m_entityList[a_uIndex]->GetModel;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	uint index = GetEntityIndex(a_sUniqueID);

	// if nothing is found
	if (index == -1) return nullptr;

	// otherwise return the model
	return m_entityList[index]->GetModel();

}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// avoid index out of bounds error
	if (a_uIndex > m_uEntityCount - 1) return nullptr;

	// return the rigid body
	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	uint index = GetEntityIndex(a_sUniqueID);

	// if nothing is found
	if (index == -1) return nullptr;

	return m_entityList[index]->GetRigidBody();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (a_uIndex > m_uEntityCount - 1) return IDENTITY_M4;

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	uint index = GetEntityIndex(a_sUniqueID);

	// if nothing is found
	if (index == -1) return IDENTITY_M4;

	return m_entityList[index]->GetModelMatrix();
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	uint index = GetEntityIndex(a_sUniqueID);

	m_entityList[index]->SetModelMatrix(a_m4ToWorld);
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{

}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* temp = new MyEntity(a_sFileName, a_sUniqueID);

	// make sure it is initialized
	if (temp->IsInitialized()) {
		m_entityList.push_back(temp);
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{

}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{

}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	return m_entityList[a_uIndex]->GetEntity;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{

}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{

}