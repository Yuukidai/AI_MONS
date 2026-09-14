#include "Enemy.h"
#include "Player.h"
#include "time.h"
#include "Stage.h"
#include <cstdlib>


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
	state_ = PATROL;
	searchTimer_ = 0.0f;

	patrolTimer_ = 0.5f;
	chaseTimer_ = 0.3f;
	attackTimer_ = 0.5f;
	searchMoveTimer_ = 0.5f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{

	switch (state_)
	{
	case PATROL:
		Patrol();
		break;

	case CHASE:
		Chase();
		break;

	case ATTACK:
		Attack();
		break;

	case SEARCH:
		Search();
		break;
	}
	

}

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{ nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE },
		{ nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE },
		{ nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE },
		{ nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE }
	};

	

	// 索敵範囲の可視化
	const int VIEW_DISTANCE = 5 * CHA_SIZE;

	int startX = pos_.x;
	int startY = pos_.y;

	int endX = pos_.x;
	int endY = pos_.y;

	switch (dir_)
	{
	case UP:
		startY = pos_.y - VIEW_DISTANCE;
		endY = pos_.y;
		break;

	case DOWN:
		startY = pos_.y;
		endY = pos_.y + VIEW_DISTANCE;
		break;

	case LEFT:
		startX = pos_.x - VIEW_DISTANCE;
		endX = pos_.x;
		break;

	case RIGHT:
		startX = pos_.x;
		endX = pos_.x + VIEW_DISTANCE;
		break;
	}

	// 索敵範囲を黄色で表示
	DrawBox(startX,startY,endX + ENEMY_DRAW_SIZE,endY + ENEMY_DRAW_SIZE,GetColor(255, 255, 0),FALSE,2);

	
	// 敵本体
	
	DrawBox(pos_.x,pos_.y,
				pos_.x + ENEMY_DRAW_SIZE,pos_.y + ENEMY_DRAW_SIZE,
				GetColor(255, 255, 0),FALSE,2);	
	// パンダの攻撃範囲
	if (state_ == ATTACK)
	{
		DrawBox(pos_.x - CHA_SIZE * 2,pos_.y - CHA_SIZE * 2,
			pos_.x + ENEMY_DRAW_SIZE + CHA_SIZE * 2,pos_.y + ENEMY_DRAW_SIZE + CHA_SIZE * 2,
			GetColor(255, 0, 0),FALSE,3);
	}

	DrawRectExtendGraph(pos_.x,pos_.y,pos_.x + ENEMY_DRAW_SIZE,pos_.y + ENEMY_DRAW_SIZE,
		iRect[dir_].x,iRect[dir_].y,iRect[dir_].w,iRect[dir_].h,hImage_,TRUE
	);

	if (animTimer < 0)
	{
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}

	animTimer = animTimer - Time::DeltaTime();

	// ステータス確認
	DrawFormatString(20,20,GetColor(255, 255, 255),"Enemy State : %d",state_);
}

void Enemy::Patrol()
{
	int random = rand() % 2;

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

		// 移動先が「壁」であるか、または配列の範囲外（安全対策）かをチェック
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
			if (random == 0)
			{
				// 左に曲がる
				switch (dir_)
				{
				case UP:    dir_ = LEFT;  break;
				case DOWN:  dir_ = RIGHT; break;
				case LEFT:  dir_ = DOWN;  break;
				case RIGHT: dir_ = UP;    break;
				}
			}
			else
			{
				// 右に曲がる
				switch (dir_)
				{
				case UP:    dir_ = RIGHT; break;
				case DOWN:  dir_ = LEFT;  break;
				case LEFT:  dir_ = UP;    break;
				case RIGHT: dir_ = DOWN;  break;
				}
			}
		}
		else
		{
			// 次のマスが通路（0）なら進む
			pos_ = newPos;
		}

		prog_timer = 0.5f + prog_timer;
	}
	if (IsPlayerInSight())
	{
		state_ = CHASE;
	}
}

void Enemy::Chase()
{
	float dt = Time::DeltaTime();

	chaseTimer_ -= dt;

	if (chaseTimer_ > 0.0f)
	{
		return;
	}

	Point playerPos = FindGameObject<Player>()->GetPlayerPos();

	
	// プレイヤーとの距離をチェック
	int dx = abs(playerPos.x - pos_.x);
	int dy = abs(playerPos.y - pos_.y);

	const int LOSE_DISTANCE = 10 * CHA_SIZE;

	// 一定距離以上離れたら見失う
	if (dx + dy > LOSE_DISTANCE)
	{
		state_ = SEARCH;
		searchTimer_ = 0.0f;
		searchMoveTimer_ = 0.5f;

		return;
	}

	
	// 攻撃範囲ならATTACK
	if (IsPlayerInAttackRange())
	{
		state_ = ATTACK;
		attackTimer_ = 0.5f;
		return;
	}

	
	// プレイヤーの方向を向く
	if (playerPos.x > pos_.x)
	{
		dir_ = RIGHT;
	}
	else if (playerPos.x < pos_.x)
	{
		dir_ = LEFT;
	}
	else if (playerPos.y > pos_.y)
	{
		dir_ = DOWN;
	}
	else if (playerPos.y < pos_.y)
	{
		dir_ = UP;
	}

	
	// 移動
	Point newPos = pos_;

	switch (dir_)
	{
	case UP:
		newPos.y -= ENEMY_DRAW_SIZE;
		break;

	case DOWN:
		newPos.y += ENEMY_DRAW_SIZE;
		break;

	case LEFT:
		newPos.x -= ENEMY_DRAW_SIZE;
		break;

	case RIGHT:
		newPos.x += ENEMY_DRAW_SIZE;
		break;
	}

	int mapX = newPos.x / CHA_SIZE;
	int mapY = newPos.y / CHA_SIZE;

	bool isWall = false;

	if (mapX < 0 || mapX >= STAGE_WIDTH ||
		mapY < 0 || mapY >= STAGE_HEIGHT)
	{
		isWall = true;
	}
	else if (FindGameObject<Stage>()->GetMap(mapX, mapY) == 1)
	{
		isWall = true;
	}

	if (!isWall)
	{
		pos_ = newPos;
	}

	chaseTimer_ = 0.5f;
}

void Enemy::Attack()
{
	attackTimer_ -= Time::DeltaTime();

	if (attackTimer_ <= 0.0f)
	{
		// 攻撃処理

		attackTimer_ = 0.5f;
	}

	// 攻撃範囲から出た
	if (!IsPlayerInAttackRange())
	{
		state_ = SEARCH;
		searchTimer_ = 0.0f;
		searchMoveTimer_ = 0.5f;
	}
}

void Enemy::Search()
{
	searchTimer_ += Time::DeltaTime();

	// プレイヤーが攻撃範囲まで近づいた
	if (IsPlayerInAttackRange())
	{
		state_ = ATTACK;
		searchTimer_ = 0.0f;
		attackTimer_ = 0.5f;
		return;
	}

	// プレイヤーを視界に入れた
	if (IsPlayerInSight())
	{
		state_ = CHASE;
		searchTimer_ = 0.0f;
		return;
	}

	// 5秒探しても見つからなければPATROL
	if (searchTimer_ >= 5.0f)
	{
		state_ = PATROL;
		searchTimer_ = 0.0f;
		return;
	}

	searchMoveTimer_ -= Time::DeltaTime();

	if (searchMoveTimer_ > 0.0f)
	{
		return;
	}

	Point newPos = pos_;

	switch (dir_)
	{
	case UP:
		newPos.y -= ENEMY_DRAW_SIZE;
		break;

	case DOWN:
		newPos.y += ENEMY_DRAW_SIZE;
		break;

	case LEFT:
		newPos.x -= ENEMY_DRAW_SIZE;
		break;

	case RIGHT:
		newPos.x += ENEMY_DRAW_SIZE;
		break;
	}

	int mapX = newPos.x / CHA_SIZE;
	int mapY = newPos.y / CHA_SIZE;

	bool isWall = false;

	if (mapX < 0 || mapX >= STAGE_WIDTH ||
		mapY < 0 || mapY >= STAGE_HEIGHT)
	{
		isWall = true;
	}
	else if (FindGameObject<Stage>()->GetMap(mapX, mapY) == 1)
	{
		isWall = true;
	}

	if (isWall)
	{
		if (rand() % 2 == 0)
		{
			switch (dir_)
			{
			case UP:    dir_ = LEFT;  break;
			case DOWN:  dir_ = RIGHT; break;
			case LEFT:  dir_ = DOWN;  break;
			case RIGHT: dir_ = UP;    break;
			}
		}
		else
		{
			switch (dir_)
			{
			case UP:    dir_ = RIGHT; break;
			case DOWN:  dir_ = LEFT;  break;
			case LEFT:  dir_ = UP;    break;
			case RIGHT: dir_ = DOWN;  break;
			}
		}
	}
	else
	{
		pos_ = newPos;
	}

	searchMoveTimer_ = 0.5f;
}

bool Enemy::IsPlayerInSight()
{
	Point playerPos = FindGameObject<Player>()->GetPlayerPos();

	int dx = playerPos.x - pos_.x;
	int dy = playerPos.y - pos_.y;

	// 索敵距離
	const int VIEW_DISTANCE = 7 * CHA_SIZE;

	// マンハッタン距離
	if (abs(dx) + abs(dy) > VIEW_DISTANCE)
	{
		return false;
	}

	// 敵とプレイヤーが同じ位置
	if (dx == 0 && dy == 0)
	{
		return false;
	}

	
	// 向いている方向によって扇形の判定
	switch (dir_)
	{
	case UP:
		// 上方向が一番強く、
		// 横方向に行くほど範囲外
		return dy < 0 && abs(dx) <= -dy;

	case DOWN:
		return dy > 0 && abs(dx) <= dy;

	case LEFT:
		return dx < 0 && abs(dy) <= -dx;

	case RIGHT:
		return dx > 0 && abs(dy) <= dx;
	}
	return false;
}


bool Enemy::IsPlayerInAttackRange()
{
	Point playerPos = FindGameObject<Player>()->GetPlayerPos();

	int dx = abs(playerPos.x - pos_.x);
	int dy = abs(playerPos.y - pos_.y);

	return dx + dy <= 2 * CHA_SIZE;
}
