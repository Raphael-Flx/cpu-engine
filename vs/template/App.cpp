#include "pch.h"

App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);
}

App::~App()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void App::OnStart()
{
	// YOUR CODE HERE

	srand(time(NULL));

	m_font.Create(cpuDevice.GetHeight() <= 512 ? 14 : 28);

	m_playerRotationAngle = 0.0f;
	m_timer = 0.0f;

	m_score = 0;
	m_lives = 3;

	m_isPlaying = true;

	//Mesh
	m_cylinderMesh.CreateCylinder(0.5f, 0.5f, 100, true, true, CPU_BLUE);
	m_largeCircleMesh.CreateCircle(5.0f, 500, CPU_BLACK);
	m_smallCircleMesh.CreateCircle(4.0f, 500, CPU_WHITE);
	m_sphereMesh.CreateSphere(0.5f, 5, 5, CPU_RED, CPU_GREEN);

	//Entities
	m_largeCircle = cpuEngine.CreateEntity();
	m_largeCircle->pMesh = &m_largeCircleMesh;
	m_largeCircle->transform.pos.x = 0.0f;
	m_largeCircle->transform.pos.y = -0.5f;
	m_largeCircle->transform.pos.z = 0.0f;

	m_axisCenter = m_largeCircle->transform.pos;
	m_axisCenter.y = 0.2f;

	m_smallCircle = cpuEngine.CreateEntity();
	m_smallCircle->pMesh = &m_smallCircleMesh;
	m_smallCircle->transform.pos.x = 0.0f;
	m_smallCircle->transform.pos.y = -0.4f;
	m_smallCircle->transform.pos.z = 0.0f;

	m_player = cpuEngine.CreateEntity();
	m_player->pMesh = &m_cylinderMesh;
	m_player->transform.pos.x = 0.0f;
	m_player->transform.pos.y = 0.0f;
	m_player->transform.pos.z = 5.0f;

	// Camera
	cpuEngine.GetCamera()->transform.pos.z = -15.0f;
	cpuEngine.GetCamera()->transform.pos.y = 3.0f;

	// Turn camera
	cpuEngine.GetCamera()->transform.AddYPR(0.0f, 0.3f, 0.0f);
}

void App::OnUpdate()
{
	// YOUR CODE HERE

	float dt = cpuTime.delta;
	float time = cpuTime.total;

	if (m_isPlaying)
	{
		m_timer += dt;

		//Move Player
		float speed = 0.0f;
		if (cpuInput.vi.IsKey(VK_LEFT))
			speed = XM_PI;
		if (cpuInput.vi.IsKey(VK_RIGHT))
			speed = -XM_PI;

		m_playerRotationAngle += dt * speed;
		m_player->transform.pos.x = m_largeCircle->transform.pos.x + cosf(m_playerRotationAngle) * 4.5f;
		m_player->transform.pos.z = m_largeCircle->transform.pos.z + sinf(m_playerRotationAngle) * 4.5f;

		//Spawn Obstacles
		SpawnObstacle();

		//Obstacles
		//Collisions ?
		// Vérifier la distance entre le centre du joueur et le centre de chaque obstacle 
		// moins la somme du rayon de la sphere collider joueur et obstacle
		for (auto it = m_obstacles.begin(); it != m_obstacles.end();)
		{
			cpu_entity* obstacle = *it;
			obstacle->transform.Move(dt);

			XMFLOAT3 obstaclePos = obstacle->transform.pos;
			XMFLOAT3 playerPos = m_player->transform.pos;

			XMFLOAT3 plObVect = XMFLOAT3(obstaclePos.x - playerPos.x, obstaclePos.y - playerPos.y, obstaclePos.z - playerPos.z);
			float norm = sqrt(pow(plObVect.x, 2) + pow(plObVect.y, 2) + pow(plObVect.z, 2));

			bool isColliding = norm <= obstacle->sphere.radius + m_player->sphere.radius;

			if (obstacle->transform.pos.y <= -0.0f)
			{
				cpuEngine.Release(obstacle);
				it = m_obstacles.erase(it);
				m_lives = std::max(0, m_lives -1);
			}
			else if (isColliding)
			{
				cpuEngine.Release(obstacle);
				it = m_obstacles.erase(it);
				m_score += 1;
			}
			else
				it++;
		}

		if (m_lives == 0)
			m_isPlaying = false;
	}
	
	// Move Camera
	if (cpuInput.IsUp())
		cpuEngine.GetCamera()->transform.Move(dt * 2.0f);
	if (cpuInput.IsDown())
		cpuEngine.GetCamera()->transform.Move(-dt * 3.0f);
	/*if (cpuInput.IsLeft())
		cpuEngine.GetCamera()->transform.AddYPR((- dt * XM_PI) *0.5f);
	if (cpuInput.IsRight())
		cpuEngine.GetCamera()->transform.AddYPR((dt * XM_PI) *0.5f);*/
	if (cpuInput.vi.IsKey(VK_SPACE))
		cpuEngine.GetCamera()->transform.pos.y += 0.3f;
	if (cpuInput.vi.IsKey(VK_LCONTROL))
		cpuEngine.GetCamera()->transform.pos.y -= 0.3f;

	// Quit
	if (cpuInput.IsBackPressed())
		cpuEngine.Quit();
}

void App::OnExit()
{
	// YOUR CODE HERE
}

void App::OnRender(int pass)
{
	// YOUR CODE HERE
	switch(pass)
	case CPU_PASS_UI_END:
	{
		// Debug
		std::string info = "Score : " + std::to_string(m_score);
		info += "\nLives : " + std::to_string(m_lives);

		XMFLOAT3 tint = { 1.0f, 1.0f, 0.8f };
		cpuDevice.DrawText(&m_font, info.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), 10, CPU_TEXT_CENTER, &tint);
		break;
	}
}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void App::SpawnObstacle()
{
	if (m_timer < 1.0f)
		return;

	float randomAngle = RandomFLOAT(0.0f, XM_2PI);
	OutputDebugStringA(std::to_string(randomAngle).c_str());
	OutputDebugStringA("\n");

	float posX = m_largeCircle->transform.pos.x + cosf(randomAngle) * 4.5f;
	float posZ = m_largeCircle->transform.pos.z + sinf(randomAngle) * 4.5f;
	float posY = 4.0f;

	cpu_entity* obstacle = cpuEngine.CreateEntity();
	obstacle->pMesh = &m_sphereMesh;
	obstacle->transform.pos = XMFLOAT3(posX, posY, posZ);
	obstacle->transform.dir = XMFLOAT3(0, -1, 0);
	m_obstacles.push_back(obstacle);

	m_timer = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int App::RandomUINT()
{
	unsigned int randomNumber = 0;
	for (int i = 0; i < 4; ++i)
	{
		unsigned int byte = rand() % 256;
		randomNumber |= byte << (i*8);
	}
	return randomNumber;
}

int App::RandomINT(int min, int max)
{
	unsigned int r = RandomUINT();
	return r % (max - min) + min;
}

float App::RandomFLOAT(float min, float max)
{
	unsigned int rnd = RandomUINT();
	double ratio = (double)rnd / (double)0xFFFFFFFF;
	return (max - min) * float(ratio) + min;
}