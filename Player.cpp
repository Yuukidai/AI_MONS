#include "Player.h"
#include "global.h"
#include "Input.h"

namespace
{
	Point PLAYER_INIT_POS = { 2 * CHA_SIZE, 2 * CHA_SIZE }; //プレイヤーの初期位置
	const int PLAYER_IMAGE_SIZE = 48; //プレイヤーのサイズ 32*32
	const int PLAYER_DRAW_SIZE = CHA_SIZE; //プレイヤーの描画サイズ
	const DIR PLAYER_INIT_DIR = { DOWN }; //プレイヤーの初期方向
	const int animFrame[4]{ 0, 1, 2, 1 }; //アニメーションのコマ番号
	const float ANIM_INTERVAL = 0.2f; //アニメーションの1コマのインターバル

}


Player::Player()
	: GameObject(), pos_(PLAYER_INIT_POS)//32はブロックの位置
{
	hImage_ = LoadGraph("Assets/mogu.png");
	dir_ = PLAYER_INIT_DIR;
}

Player::~Player()
{
}

void Player::Update()
{
	
		
	if (Input::IsKeyDown(KEY_INPUT_UP))
	{
		dir_ = UP;
		pos_.y -= PLAYER_DRAW_SIZE;
	}
	else if (Input::IsKeyDown(KEY_INPUT_DOWN))
	{
		dir_ = DOWN;
		pos_.y += PLAYER_DRAW_SIZE;
	}
	else if (Input::IsKeyDown(KEY_INPUT_LEFT))
	{
		dir_ = LEFT;
		pos_.x -= PLAYER_DRAW_SIZE;
	}
	else if (Input::IsKeyDown(KEY_INPUT_RIGHT))
	{
		dir_ = RIGHT;
		pos_.x += PLAYER_DRAW_SIZE;
	}

}

void Player::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * PLAYER_IMAGE_SIZE, 3 * PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE},
		{  nowFrame * PLAYER_IMAGE_SIZE, 0 * PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE},
		{  nowFrame * PLAYER_IMAGE_SIZE, 1 * PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE},
		{  nowFrame * PLAYER_IMAGE_SIZE, 2 * PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE, PLAYER_IMAGE_SIZE}
	};

	DrawBox(pos_.x, pos_.y, pos_.x + PLAYER_DRAW_SIZE, pos_.y + PLAYER_DRAW_SIZE,
		 GetColor(0, 255, 0), FALSE, 1);
	DrawRectExtendGraph(pos_.x, pos_.y, pos_.x + PLAYER_DRAW_SIZE, pos_.y + PLAYER_DRAW_SIZE,
		iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);

	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
}
