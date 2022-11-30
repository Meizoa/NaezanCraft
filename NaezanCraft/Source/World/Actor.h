#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Actor
{
public:
	Actor() :
		position(glm::vec3(0.0f, 0.0f, 0.0f)),
		rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
		scale(glm::vec3(1.0f, 1.0f, 1.0f)),
		Owner(nullptr)
	{}
	virtual ~Actor() {}

	virtual void Update() = 0;
	void SetOwner(Actor* owner);
	inline Actor*& GetOwner() { return Owner; }
	inline glm::vec3& GetPosition() { return position; }
	inline glm::vec3& GetRotation() { return rotation; }
	inline glm::vec3& GetScale() { return scale; }

	template<typename T>
	static std::unique_ptr<T> CreateUnique()
	{
		return std::make_unique<T>();
	}
	template<typename T>
	static std::shared_ptr<T> CreateShared()
	{
		return std::make_shared<T>();
	}

public:
	static const glm::vec3 UpVector;

protected:
	//TO DO need to transform struct?
	glm::vec3 position, rotation, scale;

private:
	//������ �ڽĸ��� ���� �� �ִ� �ϳ��� �θ��
	Actor* Owner;
};