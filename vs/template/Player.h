#pragma once

struct playerStats
{
	float rotationAngle = 5.0f;
	float speed = 0.0f;
	float maxSpeed = 5.0f;
	float acceleration = 1.05f;
	float deceleration = 5.0f;
};

class Player
{
public:
	cpu_entity* m_pEntity = nullptr;

	playerStats m_playerStats;

public:
	void Create(cpu_mesh* mesh, XMFLOAT3 pos);

	void Update(float dt);

	void UpdateSpeed(float dt);

	void Accelerate(float dt, int direction, bool reverseSpeed = false);
	void Decelerate(float dt);

	void SetPosition(XMFLOAT3 newPosition);
	void SetX(float x) { m_pEntity->transform.pos.x = x; }
	void SetY(float y) { m_pEntity->transform.pos.y = y; }
	void SetZ(float z) { m_pEntity->transform.pos.z = z; }

	cpu_entity* GetEntity() { return m_pEntity; }

	playerStats* GetPlayerStats() { return &m_playerStats; }
};