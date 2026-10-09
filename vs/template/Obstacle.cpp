#include "pch.h"
#include "Obstacle.h"

void Obstacle::Create(cpu_mesh* mesh, XMFLOAT3 pos)
{
	m_pEntity = cpuEngine.CreateEntity();
	m_pEntity->pMesh = mesh;
	m_pEntity->transform.pos = pos;
	m_pEntity->transform.dir = XMFLOAT3(0, -1, 0);
}