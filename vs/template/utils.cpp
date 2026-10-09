#include "pch.h"
#include "utils.h"

bool utils::IsSphereColliding(cpu_entity* pEntity1, cpu_entity* pEntity2)
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
