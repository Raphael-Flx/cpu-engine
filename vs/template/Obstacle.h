#pragma once

class Obstacle : public cpu_entity
{
private:
	cpu_entity* m_pEntity;
	cpu_fsm<Obstacle>* m_pFsm;

	bool m_hasBounced = false;
	float m_speed = 1.0f;
	float m_bounceForce = 4.0f;
public:
	Obstacle();
	~Obstacle();

	void Create(cpu_mesh* mesh, XMFLOAT3 pos);
	void Destroy();

	void Update();
	
	bool HasBounced() { return m_hasBounced; }
	float GetSpeed() { return m_speed; }
	float GetBounceForce() { return m_bounceForce; }
	
	void SetSpeed(float speed) { m_speed = speed; }

	void Bounce() { m_hasBounced = true; }

	cpu_entity* GetEntity() { return m_pEntity; }
	cpu_fsm<Obstacle>* GetFSM() { return m_pFsm; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//States

struct StateObstacleGlobal
{
	void OnEnter(Obstacle& cur, int from, void* pParam);
	void OnExecute(Obstacle& cur);
	void OnExit(Obstacle& cur, int to);
};

struct StateObstacleIdle
{
	void OnEnter(Obstacle& cur, int from, void* pParam);
	void OnExecute(Obstacle& cur);
	void OnExit(Obstacle& cur, int to);
};

struct StateObstacleBounce
{
	void OnEnter(Obstacle& cur, int from, void* pParam);
	void OnExecute(Obstacle& cur);
	void OnExit(Obstacle& cur, int to);
};