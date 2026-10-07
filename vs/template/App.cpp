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
	m_obstacleTimer = 0.0f;
	m_cameraShakeTimer = 0.0f;
	m_lastShake = 0.0f;

	m_score = 0;
	m_bestScore = 0;
	m_isPlaying = true;
	m_isCameraShaking = false;

	m_playerSpeed = 0.0f;
	m_playerMaxSpeed = 5.0f;
	m_playerRotationAngle = 5.0f;
	m_playerAcceleration = 1.05f;
	m_playerDeceleration = 5.00f;
	m_playerBrake = 1.5f;

	m_obstacleSpeed = 1.0f;
	m_spawnTime = 1.0f;

	m_gravity = 9.81f;

	//Mesh
	m_cylinderMesh.CreateCylinder(0.5f, 0.5f, 15, true, true, CPU_BLUE);
	m_largeCircleMesh.CreateCircle(5.0f, 40, CPU_BLACK);
	m_smallCircleMesh.CreateCircle(4.0f, 40, CPU_WHITE);
	m_holeMesh.CreateCircle(1.5f, 20, XMFLOAT3(0, 0, 0.33));
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
	m_smallCircle->transform.pos.y = -0.45f;
	m_smallCircle->transform.pos.z = 0.0f;

	m_hole = cpuEngine.CreateEntity();
	m_hole->pMesh = &m_holeMesh;
	m_hole->transform.pos.x = 0.0f;
	m_hole->transform.pos.y = -0.3f;
	m_hole->transform.pos.z = 0.0f;

	//Even smaller circle in the center

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
		m_obstacleTimer += dt;

		//Move Player
		
		if (cpuInput.vi.IsKey(VK_LEFT))
			if (m_playerSpeed < 0.0f)
				m_playerSpeed = std::min(m_playerMaxSpeed, -m_playerSpeed * 0.5f  + m_playerAcceleration * dt * XM_PI);
			else
				m_playerSpeed = std::min(m_playerMaxSpeed, m_playerSpeed + m_playerAcceleration * dt * XM_PI);
		else if (cpuInput.vi.IsKey(VK_RIGHT))
			if (m_playerSpeed > 0.0f)
				m_playerSpeed = std::max(-m_playerMaxSpeed, -m_playerSpeed * 0.5f + m_playerAcceleration * dt * -XM_PI );
			else
				m_playerSpeed = std::max(-m_playerMaxSpeed, m_playerSpeed + m_playerAcceleration * dt * -XM_PI);
		else if (m_playerSpeed > 0.f)
			m_playerSpeed = std::max(0.f, m_playerSpeed - m_playerDeceleration * dt);
		else
			m_playerSpeed = std::min(0.f, m_playerSpeed + m_playerDeceleration * dt);

		m_playerRotationAngle += dt * m_playerSpeed;
		m_player->transform.pos.x = m_largeCircle->transform.pos.x + cosf(m_playerRotationAngle) * 4.5f;
		m_player->transform.pos.z = m_largeCircle->transform.pos.z + sinf(m_playerRotationAngle) * 4.5f;

		//Spawn Obstacles
		SpawnObstacle();

		//Obstacles
		for (auto it = m_obstacles.begin(); it != m_obstacles.end();)
		{
			bool nextIt = true;
			Obstacle* obstacle = *it;

			if (obstacle->HasBounced())
				obstacle->GetEntity()->transform.dir.y -= m_gravity * dt * obstacle->GetSpeed();

			obstacle->GetEntity()->transform.Move(dt * obstacle->GetSpeed());

			bool isColliding = false;

			XMVECTOR obstaclePos = XMLoadFloat3(&obstacle->GetEntity()->transform.pos);
			XMVECTOR playerPos = XMLoadFloat3(&m_player->transform.pos);

			//Collisions
			if (obstacle->HasBounced() == false)
			{
				XMFLOAT3 plObVect;
				XMStoreFloat3(&plObVect, obstaclePos - playerPos);
				float norm = sqrt(pow(plObVect.x, 2) + pow(plObVect.y, 2) + pow(plObVect.z, 2));

				isColliding = norm <= obstacle->GetEntity()->sphere.radius + m_player->sphere.radius * 0.33f;
			}

			//Destruction
			float obstacleY = obstacle->GetEntity()->transform.pos.y;

			if (obstacleY <= 0.0f)
			{
				OutputDebugStringA(std::to_string(obstacle->GetEntity()->transform.pos.x).c_str());
				OutputDebugStringA(", ");
				OutputDebugStringA(std::to_string(obstacleY).c_str());
				OutputDebugStringA(", ");
				OutputDebugStringA(std::to_string(obstacle->GetEntity()->transform.pos.z).c_str());
				OutputDebugStringA("\n");
				if (obstacle->HasBounced())
				{
					obstacle->GetEntity()->transform.dir.x = 0;
					obstacle->GetEntity()->transform.dir.z = 0;

					if (obstacleY <= -10.0f)
					{
						m_score += 1;

						cpuEngine.Release(obstacle->GetEntity());
						it = m_obstacles.erase(it);
						delete obstacle;
						nextIt = false;
					}
					
				}
				else
				{
					m_score = std::max(0, m_score - 3);

					cpuEngine.Release(obstacle->GetEntity());
					it = m_obstacles.erase(it);
					delete obstacle;
					nextIt = false;

					//Camera Shake
					m_isCameraShaking = true;
					m_yawShake = RandomFLOAT(0.01f, 0.2f);
				}
			}
			else if (isColliding)
			{
				XMVECTOR to_centerVector = XMVECTOR{ 0, 0, 0 } - obstaclePos;

				XMFLOAT3 to_center;
				XMStoreFloat3(&to_center, to_centerVector);
				to_center.y = 5.0f;

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
	{
		cpuEngine.Quit();
	}
		
}

void App::OnExit()
{
	// YOUR CODE HERE
	for (auto it = m_obstacles.begin(); it != m_obstacles.end();)
	{
		Obstacle* obstacle = *it;

		cpuEngine.Release(obstacle->GetEntity());
		it = m_obstacles.erase(it);
		delete obstacle;
	}

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


void App::SpawnObstacle()
{
	if (m_obstacleTimer < m_spawnTime)
		return;

	float randomAngle = RandomFLOAT(0.0f, XM_2PI);
	//OutputDebugStringA(std::to_string(randomAngle).c_str());
	//OutputDebugStringA("\n");

	float posX = m_largeCircle->transform.pos.x + cosf(randomAngle) * 4.5f;
	float posZ = m_largeCircle->transform.pos.z + sinf(randomAngle) * 4.5f;
	float posY = 4.0f;

	Obstacle* obstacle = new Obstacle();
	obstacle->Create(&m_sphereMesh, XMFLOAT3(posX, posY, posZ));
	obstacle->SetSpeed(std::min(4.0f, std::max(1.5f, (m_score / 10.0f))));
	m_obstacles.push_back(obstacle);

	m_obstacleTimer = 0.f;
	m_spawnTime = RandomFLOAT(std::max(0.5f, 4.f - (m_score + 1.f) / 4.0f), std::max(1.0f, 4.0f - ((m_score + 1) / 10.0f)));
}

void App::CameraShake()
{
	//Random YPR each 0.2s for 1.0s
	if (m_cameraShakeTimer > m_lastShake + 0.2f)
	{
		m_lastShake = m_cameraShakeTimer;
		cpuEngine.GetCamera()->transform.SetYPR(0);
	}
	else
	{
		cpuEngine.GetCamera()->transform.SetYPR(m_yawShake, m_yawShake);
	}
	
	

	if (m_cameraShakeTimer > 1.0f)
		m_isCameraShaking = false;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Obstacle::Create(cpu_mesh* mesh, XMFLOAT3 pos)
{
	m_entity = cpuEngine.CreateEntity();
	m_entity->pMesh = mesh;
	m_entity->transform.pos = pos;
	m_entity->transform.dir = XMFLOAT3(0, -1, 0);
}