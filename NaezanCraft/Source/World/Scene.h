#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Player;
class Camera;
class SkyBox;

class Scene
{
public:
	Scene();
	~Scene();

	void Update();
	void Render();

public:
	static const glm::mat4* ViewProjectionMatrix;
private:
	std::unique_ptr<Player> player;//���� �÷��̾�� 1��
	std::shared_ptr<Camera> playerCamera;
	std::unique_ptr<SkyBox> sky;
};