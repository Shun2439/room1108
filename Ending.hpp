#include "Game.hpp"

class GameOver : public App::Scene {
private:
	const Audio gameOverSound{ Audio::Stream, U"example/gameover.mp3" };

	const Font onryou50{ 50, U"example/font/onryou.TTF" };
	const Font onryou100{ 100, U"example/font/onryou.TTF" };

public:

	GameOver(const InitData& init) : IScene(init) {}

	void update() override {
		if (KeySpace.down())
			changeScene(State::Title);
	}

	void draw() const override {
		Scene::SetBackground(ColorF{ 0, 0, 0 });

		ClearPrint();

		onryou100(U"ゲームオーバー!").drawAt(Scene::Center(), ColorF{ 255, 0, 0 });
		onryou100(U"あなたの得点は:{:.2f}です"_fmt(getData().score)).drawAt(Scene::Center().x, Scene::Center().y + 100, ColorF{ 255, 0, 0 });
		onryou50(U"<スペースキーを押してください>").drawAt(Scene::Center().x, Scene::Center().y + 200, ColorF{ 255, 0, 0 });

		gameOverSound.play();
	}
};
