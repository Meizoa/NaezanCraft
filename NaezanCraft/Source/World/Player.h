#pragma once

#include "Actor.h"
#include "../../Collision/Collision.h"

class Camera;
class Event;

enum Dir
{
	DIR_X, DIR_Y, DIR_Z
};

class Player : public Actor
{
public:
	Player(glm::vec3 pos, glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f), glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f), glm::vec3 dir = glm::vec3(0.f, 0.f, -1.f));
	~Player();

	virtual void Update() override;
	void Collision(const glm::vec3& dir);

	inline glm::vec3& GetVelocity() { return velocity; }
	inline glm::vec3& GetAcceleration() { return acceleration; }
	inline std::shared_ptr<Camera>& GetCamera() { return mainCamera; }

	void OnCursorPos(const Event& event);

private:
	glm::vec3 velocity, acceleration, forwardDirection;
	std::shared_ptr<Camera> mainCamera;//single play
	AABox playerBox;
};