#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Dezmon Gilbert - dog6487@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	// create the required amount of cubes for the invader
	for (uint i = 0; i < m_uMeshCount; i++) {
		MyMesh* pMesh = new MyMesh();
		m_pMeshList.push_back(pMesh);
		m_pMeshList[i]->GenerateCube(1.0f, C_BLACK);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	static float value = 0.0f;

	int currentMesh = 0;	// controls which mesh in the list is being referenced
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(5.0, 5.0f, 5.0f));

	// loop through the mesh layout array
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			// when a mesh is present, translate it to the right place and render
			if (m_pMeshLayout[i][j] == 1) {
				
				// translate
				matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(-1*((float)j - 2.0f) + value,-1 * ((float)i - 2.0f) + sin(value), -25.0f));
				matrix4 m4Model = m4Scale * m4Translate;

				// render the current mesh
				m_pMeshList[currentMesh]->Render(m4Projection, m4View, m4Model);
				
				// increment to the next mesh
				currentMesh++;
			}
		}
	}

	// increment the movement value
	value += 0.05f;

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	// delete all mesh pointers
	for (uint i = 0; i < m_uMeshCount; i++) {
		SafeDelete(m_pMeshList[i]);
	}

	// clear the vector
	m_pMeshList.clear();

	//release GUI
	ShutdownGUI();
}