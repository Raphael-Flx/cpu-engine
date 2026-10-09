#include "pch.h"
#include "Player.h"

void Player::Create(cpu_mesh* mesh, XMFLOAT3 pos)
{
	m_pEntity = cpuEngine.CreateEntity();
	m_pEntity->pMesh = mesh;
	SetPosition(pos);
}

void Player::Update(float dt)
{
	//Move
	UpdateSpeed(dt);

	
}

void Player::UpdateSpeed(float dt)
{
	if (cpuInput.vi.IsKey(VK_LEFT))
		if (m_playerStats.speed < 0.0f)
			Accelerate(dt, 1, true);
		else
			Accelerate(dt, 1);
	else if (cpuInput.vi.IsKey(VK_RIGHT))
		if (m_playerStats.speed > 0.0f)
			Accelerate(dt, -1, true);
		else
			Accelerate(dt, -1);
	else
		Decelerate(dt);

	m_playerStats.rotationAngle += dt * m_playerStats.speed;
}

void Player::Accelerate(float dt, int direction, bool reverseSpeed)
{
	if (reverseSpeed)
		float a = 1;

	float acceleratedSpeed = m_playerStats.speed * (1.0f - 0.5f * reverseSpeed) + m_playerStats.acceleration * dt * (direction * XM_PI);

	float maxSpeed = direction * m_playerStats.maxSpeed;

	if (direction == -1)
		m_playerStats.speed = std::max(maxSpeed, acceleratedSpeed);
	else if (direction == 1)
		m_playerStats.speed = std::min(maxSpeed, acceleratedSpeed);

	OutputDebugStringA(std::to_string(m_playerStats.speed).c_str());
	OutputDebugStringA("\n");
}

void Player::Decelerate(float dt)
{
	if (m_playerStats.speed > 0.f)
		m_playerStats.speed = std::max(0.f, m_playerStats.speed - m_playerStats.deceleration * dt);
	else
		m_playerStats.speed = std::min(0.f, m_playerStats.speed + m_playerStats.deceleration * dt);
}

void Player::SetPosition(XMFLOAT3 newPosition)
{
	m_pEntity->transform.pos = newPosition;
}