#include "pch.h"
#include "Obstacle.h"
#include "Player.h"

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

	//Variables
	m_obstacleTimer = 0.0f;
	m_spawnTime = 1.0f;

	m_collisionForce = 5.0f;
	m_gravity = 9.81f;

	m_cameraShakeTimer = 0.0f;
	m_lastShake = 0.0f;
	m_isCameraShaking = false;

	m_score = 0;
	m_bestScore = 0;
	m_isPlaying = true;

	//Mesh
	m_cylinderMesh.CreateCylinder(0.5f, 0.5f, 15, true, true, CPU_BLUE);
	m_largeCircleMesh.CreateCircle(5.0f, 40, CPU_BLACK);
	m_smallCircleMesh.CreateCircle(4.0f, 40, CPU_WHITE);
	m_holeMesh.CreateCircle(1.5f, 20, XMFLOAT3(0, 0, 0.33));
	m_sphereMesh.CreateSphere(0.5f, 5, 5, CPU_RED, CPU_GREEN);

	//Entities
	m_pLargeCircle = cpuEngine.CreateEntity();
	m_pLargeCircle->pMesh = &m_largeCircleMesh;
	m_pLargeCircle->transform.pos.x = 0.0f;
	m_pLargeCircle->transform.pos.y = -0.5f;
	m_pLargeCircle->transform.pos.z = 0.0f;

	m_pSmallCircle = cpuEngine.CreateEntity();
	m_pSmallCircle->pMesh = &m_smallCircleMesh;
	m_pSmallCircle->transform.pos.x = 0.0f;
	m_pSmallCircle->transform.pos.y = -0.45f;
	m_pSmallCircle->transform.pos.z = 0.0f;

	m_pHole = cpuEngine.CreateEntity();
	m_pHole->pMesh = &m_holeMesh;
	m_pHole->transform.pos.x = 0.0f;
	m_pHole->transform.pos.y = -0.3f;
	m_pHole->transform.pos.z = 0.0f;

	//Player
	m_pPlayer = new Player;
	m_pPlayer->Create(&m_cylinderMesh, XMFLOAT3{ 0.0f, 0.0f, -5.0f });

	// Camera
	cpuEngine.GetCamera()->transform.pos.z = -15.0f;
	cpuEngine.GetCamera()->transform.pos.y = 3.0f;
	cpuEngine.GetCamera()->transform.AddYPR(0.0f, 0.3f, 0.0f);
}

void App::OnUpdate()
{
	// YOUR CODE HERE

	float dt = cpuTime.delta;
	float time = cpuTime.total;

	if (m_isPlaying)
	{
		m_obstacleTimer += dt;

		//Move Player
		m_pPlayer->Update(dt);

		m_pPlayer->SetX(m_pLargeCircle->transform.pos.x + cosf(m_pPlayer->GetPlayerStats()->rotationAngle) * 4.5f);
		m_pPlayer->SetZ(m_pLargeCircle->transform.pos.z + sinf(m_pPlayer->GetPlayerStats()->rotationAngle) * 4.5f);

		//Spawn Obstacles
		SpawnObstacle();

		//Obstacles
		for (auto it = m_pObstacles.begin(); it != m_pObstacles.end();)
		{
			bool nextIt = true;
			Obstacle* obstacle = *it;

			if (obstacle->HasBounced())
				obstacle->GetEntity()->transform.dir.y -= m_gravity * dt * obstacle->GetSpeed();

			obstacle->GetEntity()->transform.Move(dt * obstacle->GetSpeed());

			XMVECTOR obstaclePos = XMLoadFloat3(&obstacle->GetEntity()->transform.pos);
			XMVECTOR playerPos = XMLoadFloat3(&m_pPlayer->GetEntity()->transform.pos);

			//Collisions
			bool isColliding = IsSphereColliding(obstacle->GetEntity(), m_pPlayer->GetEntity());

			float obstacleY = obstacle->GetEntity()->transform.pos.y;

			//Destruction
			if (obstacleY <= 0.0f)
			{
				
				if (obstacle->HasBounced())
				{
					obstacle->GetEntity()->transform.dir.x = 0;
					obstacle->GetEntity()->transform.dir.z = 0;

					if (obstacleY <= -10.0f)
					{
						m_score += 1;

						cpuEngine.Release(obstacle->GetEntity());
						it = m_pObstacles.erase(it);
						delete obstacle;
						nextIt = false;
					}
					
				}
				else
				{
					m_score = std::max(0, m_score - 3);

					cpuEngine.Release(obstacle->GetEntity());
					it = m_pObstacles.erase(it);
					delete obstacle;
					nextIt = false;

					//Camera Shake
					m_isCameraShaking = true;
					m_yawShake = RandomFLOAT(0.01f, 0.015f);
					m_lastShake = m_cameraShake + 0.05f;
				}
			}
			else if (isColliding)
			{
				XMVECTOR to_centerVector = XMVECTOR{ 0, 0, 0 } - obstaclePos;

				XMFLOAT3 to_center;
				XMStoreFloat3(&to_center, to_centerVector);
				to_center.y = m_collisionForce;

				obstacle->GetEntity()->transform.dir = to_center;

				obstacle->Bounce();
			}

			if (nextIt)
				it++;
		}
		m_bestScore = std::max(m_score, m_bestScore);
	}

	if (m_isCameraShaking)
	{
		m_cameraShakeTimer += dt;
		CameraShake();
		
	}	
	
	// Move Camera
	if (cpuInput.IsUp())
		cpuEngine.GetCamera()->transform.Move(dt * 2.0f);
	if (cpuInput.IsDown())
		cpuEngine.GetCamera()->transform.Move(-dt * 2.0f);
	if (cpuInput.vi.IsKey(VK_SPACE))
		cpuEngine.GetCamera()->transform.pos.y += 0.3f;
	if (cpuInput.vi.IsKey(VK_LCONTROL))
		cpuEngine.GetCamera()->transform.pos.y -= 0.3f;

	// Quit
	if (cpuInput.IsBackPressed())
	{
		cpuEngine.Quit();
	}
}

void App::OnExit()
{
	// YOUR CODE HERE
	for (auto it = m_pObstacles.begin(); it != m_pObstacles.end();)
	{
		Obstacle* obstacle = *it;

		cpuEngine.Release(obstacle->GetEntity());
		it = m_pObstacles.erase(it);
		delete obstacle;
	}

	delete m_pPlayer;

}

void App::OnRender(int pass)
{
	// YOUR CODE HERE
	switch(pass)
	case CPU_PASS_UI_END:
	{
		// Debug
		std::string info = "Score : " + std::to_string(m_score);
		info += "\nBest Score : " + std::to_string(m_bestScore);

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

// Game Functions

bool App::IsSphereColliding(cpu_entity* pEntity1, cpu_entity* pEntity2)
{
	bool isColliding = false;

	XMVECTOR entity1Pos = XMLoadFloat3(&pEntity1->transform.pos);
	XMVECTOR entity2Pos = XMLoadFloat3(&pEntity2->transform.pos);

	XMFLOAT3 plObVect;
	XMStoreFloat3(&plObVect, entity1Pos - entity2Pos);
	float norm = sqrt(pow(plObVect.x, 2) + pow(plObVect.y, 2) + pow(plObVect.z, 2));

	isColliding = norm <= pEntity1->sphere.radius + pEntity2->sphere.radius * 0.33f;

	return isColliding;
}

void App::SpawnObstacle()
{
	if (m_obstacleTimer < m_spawnTime)
		return;

	float randomAngle = RandomFLOAT(0.0f, XM_2PI);
	//OutputDebugStringA(std::to_string(randomAngle).c_str());
	//OutputDebugStringA("\n");

	float posX = m_pLargeCircle->transform.pos.x + cosf(randomAngle) * 4.5f;
	float posZ = m_pLargeCircle->transform.pos.z + sinf(randomAngle) * 4.5f;
	float posY = 4.0f;

	Obstacle* obstacle = new Obstacle();
	obstacle->Create(&m_sphereMesh, XMFLOAT3(posX, posY, posZ));
	obstacle->SetSpeed(std::min(4.0f, std::max(1.5f, (m_score / 10.0f))));
	m_pObstacles.push_back(obstacle);

	m_obstacleTimer = 0.f;
	m_spawnTime = RandomFLOAT(std::max(0.5f, 4.f - (m_score + 1.f) / 4.0f), std::max(1.0f, 4.0f - ((m_score + 1) / 10.0f)));
}

void App::CameraShake()
{
	//Random YPR each 0.2s for 1.0s
	if (m_cameraShakeTimer > m_lastShake + 0.05f)
	{
		m_lastShake = m_cameraShakeTimer;
		cpuEngine.GetCamera()->transform.SetYPR(m_nextYawShake, 0.3f, m_nextYawShake);

		if (m_nextYawShake == 0)
			m_nextYawShake = m_yawShake;
		else
			m_nextYawShake = 0;
	}

	if (m_cameraShakeTimer > 0.5f)
	{
		m_isCameraShaking = false;

		m_cameraShakeTimer = 0.0f;

		cpuEngine.GetCamera()->transform.SetYPR(0, 0.3f);
	}
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Random Functions

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////