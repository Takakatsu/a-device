#pragma once

#include<Siv3D.hpp> // OpenSiv3D v0.6.5

struct Manipulation
{
	bool is_click;//クリック時のみtrue
	bool is_drag;//ドラッグ中のみtrue
	Vec2 cursor_pos;//クリック座標
	bool is_used;//クリックが処理されたか

};

#include "MyWindow.h"
