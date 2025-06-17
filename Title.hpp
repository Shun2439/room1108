#include "Character.hpp"

class Title : public App::Scene {
private:
	const Audio mainSound{ Audio::Stream, U"example/main.mp3", Loop::Yes };

	const Font onryou25{ 25, U"example/font/onryou.TTF" };
	const Font onryou50{ 50, U"example/font/onryou.TTF" };
	const Font onryou100{ 100, U"example/font/onryou.TTF" };

	const Texture ghost{ U"👻"_emoji };
	GameObj ghostObj;

	double spawnTime = 2.0;
	double accumulator = 0.0;

	Rect shape{ 100, 100, Window::GetState().frameBufferSize.x - 200, Window::GetState().frameBufferSize.y - 200 };

public:

	Title(const InitData& init) : IScene(init) {
		ghostObj.texture = ghost;
		ghostObj.speed = 5;
		ghostObj.pos = RandomVec2(shape);
		ghostObj.moveMode = Random(1, 2);

		getData().score = 0;
	}

	void update() override {
		if (KeyH.down())
			System::LaunchBrowser(U"https://github.com/Shun2439/room1108/blob/main/README.md");

		if (KeySpace.down())
			changeScene(State::Game);

		ghostObj.autoMove(ghostObj.moveMode);

		accumulator += Scene::DeltaTime();

		if (spawnTime <= accumulator) {
			accumulator -= spawnTime;
			ghostObj.moveMode = Random(1, 2);
			ghostObj.pos = RandomVec2(shape);
			Circle{ ghostObj.pos, 70 }.draw(Color{ 255, 0, 0 });
		}
	}

	void draw() const override {
		Scene::SetBackground(ColorF{ 0, 0, 0 });

		mainSound.play();

		onryou25(U"ヘルプ:hキーを押してください").draw(Vec2{ 10, 10 }, ColorF{ 255, 0, 0 });
		onryou100(U"1108号室").drawAt(Scene::Center(), ColorF{ 255, 0, 0 });
		//onryou50(U"\n\n\n\n<Spaceキーを押してください>").drawAt(Scene::Center(), ColorF{255, 0, 0});
		onryou50(U"<Spaceキーを押してください>").drawAt(Scene::Center().x, Scene::Center().y + 100, ColorF{ 255, 0, 0 });

		ghostObj.texture.scaled(0.5).drawAt(ghostObj.pos);
	}
};
