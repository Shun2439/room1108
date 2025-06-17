# include <Siv3D.hpp> // OpenSiv3D v0.6.

# include "Game.hpp"

void Main() {
	//最大化
	Window::SetFullscreen(true);

	App manager;

	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);
	manager.add<GameOver>(State::GameOver);

	while (System::Update()) {
		if (not manager.update()) {
			break;
		}
	}
}
