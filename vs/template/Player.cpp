#include "pch.h"
#include "Player.h"

Player::Player()
{
	m_pEntity = nullptr;
	m_pFsm = nullptr;
}

Player::~Player()
{
}

void Player::Create(cpu_mesh* mesh, XMFLOAT3 pos)
{
	m_pEntity = cpuEngine.CreateEntity();
	m_pEntity->pMesh = mesh;
	SetPosition(pos);

	m_pFsm = cpuEngine.CreateFSM(this);
	m_pFsm->SetPostGlobal<StatePlayerGlobal>();
	m_pFsm->Add<StatePlayerIdle>();
	m_pFsm->Add<StatePlayerMove>();
}

void Player::Destroy()
{
	cpuEngine.Release(m_pEntity);
	cpuEngine.Release(m_pFsm);
}

void Player::Update()
{
	float dt = cpuTime.delta;

	m_playerStats.rotationAngle += dt * m_playerStats.speed;
	SetX(cosf(m_playerStats.rotationAngle) * 4.5f);
	SetZ(sinf(m_playerStats.rotationAngle) * 4.5f);
}

void Player::Accelerate(int direction, bool reverseSpeed)
{
	float dt = cpuTime.delta;

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

void Player::Decelerate()
{
	float dt = cpuTime.delta;

	if (m_playerStats.speed > 0.f)
		m_playerStats.speed = std::max(0.f, m_playerStats.speed - m_playerStats.deceleration * dt);
	else
		m_playerStats.speed = std::min(0.f, m_playerStats.speed + m_playerStats.deceleration * dt);
}

void Player::SetPosition(XMFLOAT3 newPosition)
{
	m_pEntity->transform.pos = newPosition;
}

//States
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StatePlayerGlobal::OnEnter(Player& cur, int from, void* pParam)
{
}

void StatePlayerGlobal::OnExecute(Player& cur)
{
	cur.Update();
}

void StatePlayerGlobal::OnExit(Player& cur, int to)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StatePlayerIdle::OnEnter(Player& cur, int from, void* pParam)
{
}

void StatePlayerIdle::OnExecute(Player& cur)
{
	cur.Decelerate();

	if (cpuInput.vi.IsKey(VK_LEFT) || cpuInput.vi.IsKey(VK_RIGHT))
		cur.GetFSM()->ToState(CPU_ID(StatePlayerMove));
}

void StatePlayerIdle::OnExit(Player& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StatePlayerMove::OnEnter(Player& cur, int from, void* pParam)
{
}

void StatePlayerMove::OnExecute(Player& cur)
{
	float playerSpeed = cur.GetPlayerStats()->speed;

	if (cpuInput.vi.IsKey(VK_LEFT))
		if (playerSpeed < 0.0f)
			cur.Accelerate(1, true);
		else
			cur.Accelerate(1);
	else if (cpuInput.vi.IsKey(VK_RIGHT))
		if (playerSpeed > 0.0f)
			cur.Accelerate(-1, true);
		else
			cur.Accelerate(-1);
	else
		cur.GetFSM()->ToState(CPU_ID(StatePlayerIdle));
}

void StatePlayerMove::OnExit(Player& cur, int to)
{
}