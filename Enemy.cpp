#include "Enemy.h"
#include "time.h"
#include "Stage.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	static float prog_timer = 0.5f;
	float dt = Time::DeltaTime();
	prog_timer = prog_timer - dt;

	if (prog_timer < 0.0f)
	{
		Point newPos = pos_;

		// 1. 現在の進行方向に応じて、次の移動先を計算
		switch (dir_)
		{
		case UP:    newPos.y -= ENEMY_DRAW_SIZE; break;
		case DOWN:  newPos.y += ENEMY_DRAW_SIZE; break;
		case LEFT:  newPos.x -= ENEMY_DRAW_SIZE; break;
		case RIGHT: newPos.x += ENEMY_DRAW_SIZE; break;
		default: break;
		}

		// 2. 移動先が「壁（1）」であるか、または配列の範囲外（安全対策）かをチェック
		// ※x, y を CHA_SIZE で割ってマップのインデックスを取得します
		int mapX = newPos.x / CHA_SIZE;
		int mapY = newPos.y / CHA_SIZE;

		bool isWall = false;
		if (mapX < 0 || mapX >= STAGE_WIDTH || mapY < 0 || mapY >= STAGE_HEIGHT)
		{
			isWall = true; // 配列外は壁扱い
		}
		else if (FindGameObject<Stage>()->GetMap(mapX, mapY) == 1)
		{
			isWall = true; // マップデータが1（壁）なら壁扱い
		}

		// 3. 判定に応じた処理
		if (isWall)
		{
			// 次のマスが壁なら、移動せずにその場で「時計回り」に向きを変える
			switch (dir_)
			{
			case RIGHT: dir_ = DOWN;  break;
			case DOWN:  dir_ = LEFT;  break;
			case LEFT:  dir_ = UP;    break;
			case UP:    dir_ = RIGHT; break;
			default: break;
			}
		}
		else
		{
			// 次のマスが通路（0）なら進む
			pos_ = newPos;
		}

		prog_timer = 0.5f + prog_timer;
	}

}

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
}
