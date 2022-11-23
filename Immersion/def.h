#pragma once

#include<Siv3D.hpp> // OpenSiv3D v0.6.5

//////定数//////

//マップ系
#define MAP_CENTER_X 32
#define MAP_CENTER_Y 32

//アイコン系
#define MARGIN_ICON 80//アイコンの間隔
#define ICON_SIZE 64//アイコンのサイズ
//ウィンドウ系
#define MARGIN_FLAME 4//フレームの幅
#define MARGIN_TITLEBAR 30//タイトルバーの太さ
#define MARGIN_TITLEBAR_BUTTON 50//タイトルバーのボタンの太さ

//デルタ秒
extern double delta;
//マウス座標
extern Vec2 cursor_pos;

enum CLICKED_TYPE
{
	NONE,
	TITLE_BAR,//タイトルバー
	T_BAR_CLOSE,//閉じるボタン
	T_BAR_MAX,//最大化orウィンドウ化ボタン
	T_BAR_MIN,//最小化ボタン
	CONTENTS,
	FLAME_UP,
	FLAME_DOWN,
	FLAME_RIGHT,
	FLAME_LEFT,
	FLAME_UP_RIGHT,
	FLAME_UP_LEFT,
	FLAME_DOWN_RIGHT,
	FLAME_DOWN_LEFT,
};
extern CLICKED_TYPE g_clicktype;

//メール系
typedef struct MailData
{
	String from;
	String title;
	String text;
};
extern Array<MailData> MailLib;

//マップ系
enum MAPTILE
{
	SHIP,//宇宙船
	GRASS,//草原
	ROCK,//岩
	SAND,//砂場
	WATER,//水場
};
typedef struct TileData
{
	Color c;
};
extern HashTable<MAPTILE, TileData> TileLib;
enum ENEMYTYPE
{
	ETNONE,
	TYPE1,
	TYPE2,
	WALL,//絶対破壊
};
typedef struct MapTip
{
	MAPTILE tile;
	bool is_found;//発見済みか否か
	ENEMYTYPE et;//敵状態
	int e_life;//敵体力
};
extern Array<Array<MapTip>> MAINMAP;

//ロボット系
enum ROBOTTYPE
{
	SEARCH,
	FIGHT1,
	FIGHT2,
	COLLECT1,
	COLLECT2,
};
typedef struct Robot
{
	ROBOTTYPE rt;
	int count_go;//使用回数
	int endurance;//耐久値的な。使うたびに蓄積して一定以上で破壊
	String name;//ロボットの名前
	double remain_time;//探索に行っている場合の残り時間
};
typedef struct RobotData
{
	int max_endurance;
};
extern HashTable<ROBOTTYPE, RobotData> RobotLib;
extern Array<Robot> robots_stay;
//報酬(探索報酬は戻ってきたときに与えられる)
typedef struct Reward
{
	Point pos;//座標
	bool found;//地形発見
	//アイテム
};
//探索中のロボット
typedef struct Robot_Activated
{
	Robot rb;
	Reward rw;
};
extern Array<Robot_Activated> robots_active;

//ゲームのログ
typedef struct GameLog
{
	double remain_time;//表示されるまでの時間
	DateTime time;//表示された時間？
	String text;//本体
};

extern Array<GameLog> logs;//表示されうるログ
extern Array<GameLog> logs_will;//未だ表示されないログ

#include "MyWindow.h"

extern Array<MyWindow*> my_wins;
extern MyWindow* win_active;

#include "SubWindows.h"

#include "MyIcon.h"

extern Array<MyIcon*> my_icons;

#include "func.h"
