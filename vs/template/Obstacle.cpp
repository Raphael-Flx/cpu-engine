#include "pch.h"
#include "Obstacle.h"
#include "Player.h"
#include "utils.h"

Obstacle::Obstacle()
{
	m_pEntity = nullptr;
	m_pFsm = nullptr;
}

Obstacle::~Obstacle()
{
	Destroy();
}

void Obstacle::Create(cpu_mesh* mesh, XMFLOAT3 pos)
{
	m_pEntity = cpuEngine.CreateEntity();
	m_pEntity->pMesh = mesh;
	m_pEntity->transform.pos = pos;
	m_pEntity->transform.dir = XMFLOAT3(0, -1, 0);

	m_pFsm = cpuEngine.CreateFSM<Obstacle>(this);
	m_pFsm->SetPostGlobal<StateObstacleGlobal>();
	m_pFsm->Add<StateObstacleIdle>();
	m_pFsm->Add<StateObstacleBounce>();
}

void Obstacle::Destroy()
{
	cpuEngine.Release(m_pEntity);
	cpuEngine.Release(m_pFsm);
}

void Obstacle::Update()
{
	float dt = cpuTime.delta;

	if (m_hasBounced)
		m_pEntity->transform.dir.y -= gravity * dt * m_speed;

	m_pEntity->transform.Move(dt * m_speed);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateObstacleGlobal::OnEnter(Obstacle& cur, int from, void* pParam)
{
}

void StateObstacleGlobal::OnExecute(Obstacle& cur)
{
	cur.Update();
}

void StateObstacleGlobal::OnExit(Obstacle& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateObstacleIdle::OnEnter(Obstacle& cur, int from, void* pParam)
{
}

void StateObstacleIdle::OnExecute(Obstacle& cur)
{
	App app = App::GetInstance();
	float obstacleY = cur.GetEntity()->transform.pos.y;

	if (obstacleY <= 0.0f)
	{
		//Destroy instance "cur"
		//Use an instance of cpu_manager for obstacles and destroy cur with it
		cpu_manager<Obstacle> obstacleManager;
		obstacleManager.Release(&cur);
		app.ChangeScore(-3);
		app.StartShake();

		return;
	}
	
	//Check collision
	Player* player = app.GetPlayer();

	bool isColliding = app.IsSphereColliding(player->GetEntity(), cur.GetEntity());
	if (isColliding)
	{
		XMVECTOR to_centerVector = XMVECTOR{ 0, 0, 0 } - XMLoadFloat3(&cur.GetEntity()->transform.pos);

		XMFLOAT3 to_center;
		XMStoreFloat3(&to_center, to_centerVector);
		to_center.y = cur.GetBounceForce();

		cur.GetEntity()->transform.dir = to_center;

		cur.Bounce();
		cur.GetFSM()->ToState(CPU_ID(StateObstacleBounce));
	}
}

void StateObstacleIdle::OnExit(Obstacle& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateObstacleBounce::OnEnter(Obstacle& cur, int from, void* pParam)
{
}

void StateObstacleBounce::OnExecute(Obstacle& cur)
{
	float obstacleY = cur.GetEntity()->transform.pos.y;

	if (obstacleY <= 0.0f)
	{
		cur.GetEntity()->transform.dir.x = 0;
		cur.GetEntity()->transform.dir.z = 0;
	}
	
	if (obstacleY <= -10.0f)
	{
		App app = App::GetInstance();

		app.ChangeScore(1);

		//Destroy instance "cur"
		//Use an instance of cpu_manager for obstacles and destroy cur with it
		cpu_manager<Obstacle> obstacleManager;
		obstacleManager.Release(&cur);

		return;

		//it = m_pObstacles.erase(it);
		//nextIt = false;
	}


}

void StateObstacleBounce::OnExit(Obstacle& cur, int to)
{
}