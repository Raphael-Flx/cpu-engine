#pragma once

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

	std::list<cpu_entity*> m_obstacles;

	//Mesh
	cpu_mesh m_largeCircleMesh;
	cpu_mesh m_smallCircleMesh;
	cpu_mesh m_cylinderMesh;
	cpu_mesh m_sphereMesh;

	cpu_font m_font;

	XMFLOAT3 m_axisCenter;
	float m_playerRotationAngle;

	float m_timer;

	int m_score;
	int m_lives;

	bool m_isPlaying;

private:
	inline static App* s_pApp = nullptr;
};
