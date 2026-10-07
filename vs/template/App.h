#pragma once

class Obstacle;

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

	void SpawnObstacle();

	unsigned int RandomUINT();
	int RandomINT(int min, int max);
	float RandomFLOAT(float min, float max);

private:
	//Entities
	cpu_entity* m_largeCircle;
	cpu_entity* m_smallCircle;
	cpu_entity* m_player;

	std::list<Obstacle*> m_obstacles;

	//Mesh
	cpu_mesh m_largeCircleMesh;
	cpu_mesh m_smallCircleMesh;
	cpu_mesh m_cylinderMesh;
	cpu_mesh m_sphereMesh;

	cpu_font m_font;

	XMFLOAT3 m_axisCenter;
	float m_playerRotationAngle;
	float m_playerSpeed;
	float m_playerMaxSpeed;
	float m_playerAcceleration;
	float m_playerDeceleration;
	float m_playerBrake;

	float m_timer;
	float m_spawnTime;

	float m_obstacleSpeed;
	float m_gravity;
	float m_collisionForce;

	int m_score;
	int m_lives;
	bool m_isPlaying;

private:
	inline static App* s_pApp = nullptr;
};

class Obstacle
{
private:
	cpu_entity* m_entity = nullptr;

	bool m_hasBounced = false;
	float m_speed = 1.0f;
public:
	void Create(cpu_mesh* mesh, XMFLOAT3 pos);

	bool HasBounced() { return m_hasBounced; }

	void Bounce() { m_hasBounced = true; }
	void SetSpeed(float speed) { m_speed = speed; }
	float GetSpeed() { return m_speed; }

	cpu_entity* GetEntity() { return m_entity; }
};
