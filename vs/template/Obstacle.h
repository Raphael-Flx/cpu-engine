#pragma once

class Obstacle
{
private:
	cpu_entity* m_pEntity = nullptr;
	cpu_fsm<Obstacle>* m_pFsm;

	bool m_hasBounced = false;
	float m_speed = 1.0f;
public:
	void Create(cpu_mesh* mesh, XMFLOAT3 pos);

	bool HasBounced() { return m_hasBounced; }

	void Bounce() { m_hasBounced = true; }
	void SetSpeed(float speed) { m_speed = speed; }
	float GetSpeed() { return m_speed; }

	cpu_entity* GetEntity() { return m_pEntity; }
};