#pragma once

#include<Siv3D.hpp> // OpenSiv3D v0.6.5

//////定数//////

//#define SCENE_WIDTH 960
//#define SCENE_HEIGHT 540
#define SCENE_WIDTH 1920
#define SCENE_HEIGHT 1080

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
#define UNDERBAR_HEIGHT 40//下のバーの太さ

extern bool is_game_exit;

//時間
extern double passed_time;
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

//アイテム系
enum ITEMTYPE
{
	IT_LIQUID1,
	IT_LIQUID2,
	IT_WOOD1,
	IT_WOOD2,
	IT_ROCK1,
	IT_ROCK2,
	IT_ROCK3,
	IT_ORE1,
	IT_ORE2,
	IT_ORE3,
	IT_ORE4,
	IT_NUM,
};
typedef struct ItemData
{
	String name;//名称
};
extern HashTable<ITEMTYPE, ItemData> ItemLib;
extern HashTable<ITEMTYPE, double> ItemBox;
//報酬用アイテムの実体
typedef struct Item
{
	ITEMTYPE it;
	double amount;
};
//マップに於けるドロップ指定用構造体
typedef struct ItemRate
{
	ITEMTYPE it;
	double min, max;
};

//マップ系
enum MAPTILE
{
	MT_SHIP,//宇宙船
	MT_GRASS1,//草原1
	MT_GRASS2,//草原2
	MT_ROCK1,//岩1
	MT_ROCK2,//岩2
	MT_ROCK3,//岩3
	MT_SAND,//砂場
	MT_WATER,//水場
};
typedef struct TileData
{
	Color c;
	Array<ItemRate> irs_1, irs_2;
};
extern HashTable<MAPTILE, TileData> TileLib;
enum ENEMYTYPE
{
	ET_NONE,
	ET_TYPE1,
	ET_TYPE2,
	ET_WALL,//絶対破壊
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
	RT_SEARCH,
	RT_COLLECT1,
	RT_COLLECT2,
	RT_FIGHT1,
	RT_FIGHT2,
	RT_NUM
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
	String name;
	int max_endurance;
	String texture_name;//テクスチャの名前
};
extern HashTable<ROBOTTYPE, RobotData> RobotLib;
extern Array<std::pair<ROBOTTYPE, Array<Item>>> Recipes;
extern Array<Robot> robots_stay;
//報酬(探索報酬は戻ってきたときに与えられる)
typedef struct Reward
{
	Point pos;//座標
	bool found;//地形発見
	Array<Item> items;//アイテム
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
	bool is_notified = false;//通知されたか否か
};

extern Array<GameLog> logs;//表示されうるログ
extern Array<GameLog> logs_will;//未だ表示されないログ
extern Array<GameLog> logs_tmp;//一時的に表示されるログ(メッセージ？)

extern HashTable<String, Audio> AudioLib;
extern HashTable<String, Texture> TextureLib;

#include "MyWindow.h"

extern Array<MyWindow*> my_wins;
extern MyWindow* win_active;

#include "SubWindows.h"

#include "MyIcon.h"

extern Array<MyIcon*> my_icons;

#include "func.h"
