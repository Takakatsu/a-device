#include "def.h"

void Initialize()
{
	//ウィンドウサイズ等の初期設定
	Window::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(960, 540);
	Scene::SetBackground(Color(255));
	Window::SetFullscreen(true);

	//ウィンドウ処理用
	g_clicktype = CLICKED_TYPE::NONE;

	//データベースの初期化
	initialize_lib();
}

void Mouse_Operation()
{
	bool left_down = MouseL.down();
	bool right_down = MouseR.down();
	bool left_press = MouseL.pressed();
	bool right_press = MouseR.pressed();
	//クリック時
	if (left_down || right_down)
	{
		bool click_dealed = false;
		Vec2 pos = Cursor::PosF();
		win_active = nullptr;
		for (int i = 0; i < my_wins.size(); i++)
		{
			//ウィンドウをクリックしてた時
			CLICKED_TYPE ct = my_wins[i]->getPosType(pos);
			if (ct != CLICKED_TYPE::NONE)
			{
				click_dealed = true;
				win_active = my_wins[i];
				g_clicktype = ct;
				my_wins.remove_at(i);
				my_wins.push_front(win_active);
				//コンテンツクリック時にクリック処理呼び出し
				if (ct == CLICKED_TYPE::CONTENTS)
				{
					win_active->click(pos - win_active->getContentsRectF().pos, left_down);
					break;
				}
				//タイトルバーの各種ボタン処理(右クリック時は処理しない)
				if (left_down)
				{
					if (ct == CLICKED_TYPE::T_BAR_CLOSE)
					{
						my_wins.remove(win_active);
						win_active = nullptr;
						break;
					}
					if (ct == CLICKED_TYPE::T_BAR_MAX)
					{
						win_active->dealSizeMax();
					}
					if (ct == CLICKED_TYPE::T_BAR_MIN)
					{
						win_active->dealSizeMin();
					}
				}
				break;
			}
		}
		if (!click_dealed)
		{
			for (int i = 0; i < my_icons.size(); i++)
			{
				//アイコンクリック処理
				if (my_icons[i]->getRect().contains(pos))
				{
					my_icons[i]->click();
					click_dealed = true;
					break;
				}
			}
		}
	}
	//ドラッグ時
	else if (left_press || right_press)
	{
		Vec2 drag = Cursor::DeltaF();
		//ウィンドウをドラッグしているとき
		if (win_active != nullptr)
		{
			//両方で処理するもの(メインコンテンツのみ)
			if (g_clicktype == CLICKED_TYPE::CONTENTS)
			{
				win_active->drag(drag, left_press);
			}
			//左ドラッグ時(基本は移動と変形のみ)
			if (left_press)
			{
				switch (g_clicktype)
				{
					//////タイトルバー系統//////
				case TITLE_BAR://タイトルドラッグつまり移動
				{
					win_active->setPos(win_active->getPos() + drag);
				}
				break;
				//////以下は変形系統//////
				case FLAME_UP://上方向に変形
				{
					Vec2 v = Vec2(0, drag.y);
					win_active->setPos(win_active->getPos() + v);
					win_active->setSize(win_active->getSize() - v);
				}
				break;
				case FLAME_DOWN://下方向に変形
				{
					Vec2 v = Vec2(0, drag.y);
					win_active->setSize(win_active->getSize() + v);
				}
				break;
				case FLAME_RIGHT://右方向に変形
				{
					Vec2 v = Vec2(drag.x, 0);
					win_active->setSize(win_active->getSize() + v);
				}
				break;
				case FLAME_LEFT://左方向に変形
				{
					Vec2 v = Vec2(drag.x, 0);
					win_active->setPos(win_active->getPos() + v);
					win_active->setSize(win_active->getSize() - v);
				}
				break;
				case FLAME_UP_RIGHT://右上方向に変形
				{
					win_active->setPos(win_active->getPos() + Vec2(0, drag.y));
					win_active->setSize(win_active->getSize() + Vec2(drag.x, -drag.y));
				}
				break;
				case FLAME_UP_LEFT://左上方向に変形
				{
					win_active->setPos(win_active->getPos() + drag);
					win_active->setSize(win_active->getSize() - drag);
				}
				break;
				case FLAME_DOWN_RIGHT://右下方向に変形
				{
					win_active->setSize(win_active->getSize() + drag);
				}
				break;
				case FLAME_DOWN_LEFT://左下方向に変形
				{
					win_active->setPos(win_active->getPos() + Vec2(drag.x, 0));
					win_active->setSize(win_active->getSize() + Vec2(-drag.x, drag.y));
				}
				break;
				default:
					break;
				}
			}
		}
	}
}

void Update_Robot()
{
	for (auto it = robots_active.begin(); it != robots_active.end();)
	{
		if (it->remain_time > delta)
		{
			//活動中なら活動
			it->remain_time -= delta;
			++it;
		}
		else
		{
			//終了時に処理
			robots_stay.push_back(*it);
			it = robots_active.erase(it);
		}
	}
}

void Main()
{
	Initialize();

	//以下はグローバル変数として扱う物とその処理
	//ウィンドウ系
	MyWindow s_tmp = MyWindow(Vec2(300, 300), Vec2(200, 200));
	my_wins.push_back(&s_tmp);
	Calculator s_cal = Calculator(Vec2(250, 0), Vec2(200, 200));
	my_wins.push_back(&s_cal);
	MailSoft s_mail = MailSoft(Vec2(250, 200), Vec2(200, 200));
	my_wins.push_back(&s_mail);
	MAPViewer s_map = MAPViewer(Vec2(250, 200), Vec2(200, 200));
	my_wins.push_back(&s_map);
	//アイコン系
	MyIcon ic_tmp = MyIcon(Point(0, 0), &s_tmp);
	my_icons.push_back(&ic_tmp);
	MyIcon ic_cal = MyIcon(Point(0, 1), &s_cal);
	my_icons.push_back(&ic_cal);
	MyIcon ic_mail = MyIcon(Point(0, 2), &s_mail);
	my_icons.push_back(&ic_mail);
	MyIcon ic_map = MyIcon(Point(0, 3), &s_map);
	my_icons.push_back(&ic_map);

	while (System::Update())
	{
		ClearPrint();
		delta = Scene::DeltaTime();
		cursor_pos = Cursor::PosF();

		//マウス操作
		Mouse_Operation();

		//各種処理
		for (int i = 0; i < my_wins.size(); i++)
		{
			my_wins[i]->update();
		}

		//////描画//////
		TextureFilter::Nearest();

		for (int i = 0; i < my_icons.size(); i++)
		{
			my_icons[i]->draw();
		}

		//描画は配列で後ろから
		for (int i = my_wins.size() - 1; i >= 0; i--)
		{
			my_wins[i]->draw();
		}
	}
}
