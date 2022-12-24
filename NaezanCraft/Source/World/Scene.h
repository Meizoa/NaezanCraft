#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Player;
class Camera;

extern float worldTime;
extern float sunIntensity;

class Scene
{
public:
	Scene();
	~Scene();

	void Update();
	void Render();

	inline std::unique_ptr<Player>& GetPlayer() { return player; }
	glm::vec3& GetPlayerPosition();
	inline std::weak_ptr<Camera>& GetCamera() { return playerCamera; }

private:
	std::unique_ptr<Player> player;//���� �÷��̾�� 1��
	std::weak_ptr<Camera> playerCamera;
};