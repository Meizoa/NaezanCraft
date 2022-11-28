#pragma once

class Renderer;
class Player;

class World
{
public:
	World();
	~World();

	void Update();
	void Shutdown();

	static inline std::unique_ptr<World> CreateCraftWorld()
	{
		return std::make_unique<World>();
	}

private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Player> player;//���� �÷��̾�� 1��
};