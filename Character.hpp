#include <Siv3D.hpp>

#include "Game.hpp"

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
