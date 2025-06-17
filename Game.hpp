#include "Character.hpp"

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

