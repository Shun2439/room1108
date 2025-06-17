# include <Siv3D.hpp> // OpenSiv3D v0.6.

class GameObj {
private:

public:

	/* 座標 */
	Vec2 pos;

	/* テクスチャ */
	Texture texture;

	/* 動く速さ */
	double speed = 200;

	/* 姿を表すかどうか */
	bool manifestFlag;

	int16 moveMode;

	GameObj() {}

	/* 衝突判定 */
	bool circleCollision(Vec2 b, double r);

	/* 十字キーでの操作 */
	void handling();

	/* 勝手に動く */
	void autoMove(int16 select);
};

bool GameObj::circleCollision(Vec2 b, double r) {
	if ((pow(pos.x - b.x, 2) + pow(pos.y - b.y, 2)) <= r)
		return true;
	else
		return false;
}

void GameObj::handling() {
	// 左キーが押されていたら
	if (KeyLeft.pressed())
		// プレイヤーが左に移動する
		pos.x = Max((pos.x - speed * Scene::DeltaTime()), 55.0);

	// 右キーが押されていたら
	if (KeyRight.pressed())
		// プレイヤーが右に移動する
		pos.x = Min((pos.x + speed * Scene::DeltaTime()), Window::GetState().frameBufferSize.x - 10.0);

	if (KeyUp.pressed())
		// プレイヤーが上に移動する
		pos.y = Max((pos.y - speed * Scene::DeltaTime()), 55.0);

	if (KeyDown.pressed())
		// プレイヤーが下に移動する
		pos.y = Min((pos.y + speed * Scene::DeltaTime()), Window::GetState().frameBufferSize.y - 10.0);
}

void GameObj::autoMove(int16 select) {

	switch (select) {
	case 1:
		/** 縦移動 **/

		pos.y += speed;

		if (pos.y >= (Window::GetState().frameBufferSize.y - 25))
			speed *= -1;
		else if (pos.y <= 25)
			speed *= -1;

		break;

	case 2:
		/** 横移動 **/

		pos.x += speed;

		if (pos.x >= (Window::GetState().frameBufferSize.x - 25))
			speed *= -1;
		else if (pos.x <= 25)
			speed *= -1;

		break;

	case 3:

		break;
	}
}

enum class State {
	Title,
	Game,
	GameOver,
};

/* シーン間での共有データ */
struct GameData {
	double score = 0;
};

using App = SceneManager<State, GameData>;

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

class Game : public App::Scene {
private:

	/* 絵文字からテクスチャを作成する */
	const Texture eye{ U"👀"_emoji };
	const Texture light{ U"🔦"_emoji };
	const Texture treasure{ U"💻"_emoji };
	const Texture ghost{ U"👻"_emoji };

	GameObj playerObj;

	GameObj lightObj;

	GameObj rayObj;

	GameObj treasureObj;

	GameObj contactAlertObj;

	Array<GameObj> enemyObj;

	Stopwatch hitplayer;

	int16 gameOverFlag = 0;

	bool treasureFound = 0;

	int16 treasureCount = 0;

	/** BGM **/
	const Audio mainSound{ Audio::Stream, U"example/main.mp3", Loop::Yes };
	const Audio ghostContactSound{ Audio::Stream, U"example/ghost.mp3" };
	const Audio hearGhostSound{ Audio::Stream, U"example/n.mp3" };
	const Audio treasureSound{ Audio::Stream, U"example/treasure.mp3" };
	const Audio ghostAttackSound{ Audio::Stream, U"example/ghostAttack.mp3" };

	/* 音声の再生制御 */
	Array<bool> played1Flag;

	/*キャラクタの出現範囲*/
	const RectF effectiveRange{ 100, 100, Window::GetState().frameBufferSize.x - 200.0, Window::GetState().frameBufferSize.y - 200.0 };

	double t, s;

	bool isHitPlayer;

	int16 playerFound = 0;

	const Font onryou10{ 10, U"example/font/onryou.TTF" };
	const Font onryou50{ 50, U"example/font/onryou.TTF" };
	const Font onryou100{ 100, U"example/font/onryou.TTF" };

public:

	Game(const InitData& init) : IScene(init) {
		playerObj.texture = eye;
		playerObj.pos = RandomVec2(effectiveRange);
		playerObj.manifestFlag = true;

		lightObj.texture = light;
		lightObj.manifestFlag = true;

		rayObj.manifestFlag = true;

		treasureObj.texture = treasure;
		treasureObj.pos = RandomVec2(effectiveRange);

		enemyObj.resize(10);

		for (auto i : step(enemyObj.size())) {
			enemyObj[i].pos = RandomVec2(effectiveRange);
			enemyObj[i].texture = ghost;
			enemyObj[i].moveMode = Random<int16>(1, 2);
			enemyObj[i].speed = 5;
		}

		played1Flag.resize(5, false);

		isHitPlayer = false;
	}

	void update() override
	{
		/* ライトの位置 */
		lightObj.pos.x = playerObj.pos.x - 30.0;
		lightObj.pos.y = playerObj.pos.y;

		/* 光線の位置 */
		rayObj.pos.x = lightObj.pos.x - 30.0;
		rayObj.pos.y = lightObj.pos.y - 30.0;

		/*
		お宝をとる

		OK
		*/
		if (treasureObj.circleCollision(rayObj.pos, 80.0)) {
			Circle{ rayObj.pos, 60 }.draw(ColorF{ 255 });
			treasureObj.manifestFlag = false;
			treasureSound.play();
			treasureObj.pos = RandomVec2(effectiveRange);
			treasureCount++;
		}
		else if (treasureObj.circleCollision(rayObj.pos, 1500.0))
			treasureObj.manifestFlag = true;
		else
			treasureObj.manifestFlag = false;

		/* 敵との判定 */
		for (auto i : step(enemyObj.size())) {
			/*
			敵の表示

			OK
			*/
			if (enemyObj[i].circleCollision(rayObj.pos, 1500.0)) {
				enemyObj[i].manifestFlag = true;
				ghostContactSound.play();

				/* お宝の場所のヒント表示 */
				treasureObj.manifestFlag = true;
			}
			else
				enemyObj[i].manifestFlag = false;

			/*
			プレイヤーとの当たり判定

			?
			*/
			if (enemyObj[i].circleCollision(playerObj.pos, 512.0)) {
				playerFound++;
				ghostAttackSound.play();
			}

			/*
			警告表示

			OK
			*/
			if (enemyObj[i].circleCollision(playerObj.pos, 10000.0)) {
				hearGhostSound.play();
				contactAlertObj.manifestFlag = true;
				Circle(playerObj.pos, 100).draw(ColorF{ 1, 0, 0, 0.3 });
			}
			else
				contactAlertObj.manifestFlag = false;

			/* 敵の動き */
			enemyObj[i].autoMove(enemyObj[i].moveMode);
		}

		playerObj.handling();

		/* シーン遷移 */
		if (100 <= playerFound || KeyQ.down()) {
			getData().score = treasureCount * Math::Pi;
			changeScene(State::GameOver);
		}
	}

	void draw() const override {
		mainSound.play();

		onryou50(U"見つけたお宝:{} ライフ:{}%"_fmt(treasureCount, (100 - playerFound))).draw(10, Window::GetState().frameBufferSize.y - 50, ColorF{ 255, 0, 0 });

		/* お宝 */
		if (treasureObj.manifestFlag)
			treasureObj.texture.scaled(0.5).drawAt(treasureObj.pos);

		/* キャラクターの表示 */

		/* 敵 */
		for (auto i : step(enemyObj.size()))
			if (enemyObj[i].manifestFlag)
				enemyObj[i].texture.scaled(0.5).drawAt(enemyObj[i].pos);

		if (playerObj.manifestFlag)
			playerObj.texture.scaled(0.2).drawAt(playerObj.pos);

		if (lightObj.manifestFlag)
			lightObj.texture.scaled(0.2).drawAt(lightObj.pos);

		if (rayObj.manifestFlag)
			Circle{ rayObj.pos, 30 }.draw(ColorF{ 100, 100, 100, 0.5 });

		Scene::SetBackground(ColorF{ 0, 0, 0 });
	}
};

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
