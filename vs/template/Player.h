#pragma once

struct playerStats
{
	float rotationAngle = 5.0f;
	float speed = 0.0f;
	float maxSpeed = 5.0f;
	float acceleration = 1.05f;
	float deceleration = 5.0f;
};

class Player : cpu_object
{
public:
	cpu_entity* m_pEntity;
	cpu_fsm<Player>* m_pFsm;

	playerStats m_playerStats;

public:
	Player();
	~Player();

	void Create(cpu_mesh* mesh, XMFLOAT3 pos);
	void Destroy();

	void Update();

	void UpdateSpeed(float dt);

	void Accelerate(int direction, bool reverseSpeed = false);
	void Decelerate();

	void SetPosition(XMFLOAT3 newPosition);
	void SetX(float x) { m_pEntity->transform.pos.x = x; }
	void SetY(float y) { m_pEntity->transform.pos.y = y; }
	void SetZ(float z) { m_pEntity->transform.pos.z = z; }

	cpu_entity* GetEntity() { return m_pEntity; }
	cpu_fsm<Player>* GetFSM() { return m_pFsm; }

	playerStats* GetPlayerStats() { return &m_playerStats; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//States

struct StatePlayerGlobal
{
	void OnEnter(Player& cur, int from, void* pParam);
	void OnExecute(Player& cur);
	void OnExit(Player& cur, int to);
};

struct StatePlayerIdle
{
	void OnEnter(Player& cur, int from, void* pParam);
	void OnExecute(Player& cur);
	void OnExit(Player& cur, int to);
};

struct StatePlayerMove
{
	void OnEnter(Player& cur, int from, void* pParam);
	void OnExecute(Player& cur);
	void OnExit(Player& cur, int to);
};