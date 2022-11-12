#pragma once

#include<Siv3D.hpp> // OpenSiv3D v0.6.5

//////定数//////

//アイコン系
#define MARGIN_ICON 50//アイコンの間隔
#define ICON_SIZE 40//アイコンのサイズ
//ウィンドウ系
#define MARGIN_FLAME 4//フレームの幅
#define MARGIN_TITLEBAR 30//タイトルバーの太さ
#define MARGIN_TITLEBAR_BUTTON 50//タイトルバーのボタンの太さ

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

typedef struct MailData
{
	String from;
	String title;
	String text;
};

extern Array<MailData> MailLib;

#include "MyWindow.h"

extern Array<MyWindow*> my_wins;
extern MyWindow* win_active;

#include "MyIcon.h"

extern Array<MyIcon*> my_icons;

#include "func.h"
