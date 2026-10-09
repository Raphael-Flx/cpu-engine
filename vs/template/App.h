#pragma once

class Obstacle;
class Player;

#define gravity 9.81f

class App
{
public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	static void MyPixelShader(cpu_ps_io& io);

	void ChangeScore(int amount) { m_score += amount; }


	bool IsSphereColliding(cpu_entity* pEntity1, cpu_entity* pEntity2);

	void SpawnObstacle();

	void StartShake();
	void CameraShake();

	Player* GetPlayer() { return m_pPlayer; }

	unsigned int RandomUINT();
	int RandomINT(int min, int max);
	float RandomFLOAT(float min, float max);

private:
	//Entities
	cpu_entity* m_pLargeCircle = nullptr;
	cpu_entity* m_pSmallCircle = nullptr;
	cpu_entity* m_pHole = nullptr;
	Player* m_pPlayer = nullptr;

	std::list<Obstacle*> m_pObstacles;

	//Mesh
	cpu_mesh m_largeCircleMesh;
	cpu_mesh m_smallCircleMesh;
	cpu_mesh m_holeMesh;
	cpu_mesh m_cylinderMesh;
	cpu_mesh m_sphereMesh;

	cpu_font m_font;

	float m_obstacleTimer;
	float m_spawnTime;

	int m_score;
	int m_bestScore;
	bool m_isPlaying;

	bool m_isCameraShaking;
	float m_cameraShake;
	float m_cameraShakeTimer;
	float m_lastShake;
	float m_yawShake;
	float m_nextYawShake;

private:
	inline static App* s_pApp = nullptr;
};
