#pragma once
#include ".\Library\GameObject.h"
#include "global.h"



class Enemy :
    public GameObject
{

	enum STATE///1
	{
		PATROL,
		CHASE,
		ATTACK,
		SEARCH
	};


	STATE state_;      // 現在のステート2

	float searchTimer_; // Search状態の時間3

	int hImage_;//画像ID
	Point pos_;//位置
	DIR dir_;//移動方向

	void Patrol();
	void Chase();
	void Attack();
	void Search();

	bool IsPlayerInSight();
	bool IsPlayerInAttackRange();

	// 各ステート用のタイマー
	float patrolTimer_;
	float chaseTimer_;
	float attackTimer_;
	float searchMoveTimer_;


public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
};