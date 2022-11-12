#include "def.h"

void Main()
{
	//ウィンドウサイズ等の初期設定
	Window::Resize(1280, 720);
	Scene::SetBackground(Color(255));

	Array<MyWindow*> my_wins;
	MyWindow win = MyWindow(Vec2(300, 300), Vec2(200, 200));
	Calculator cal = Calculator(Vec2(250, 0), Vec2(200, 200));
	my_wins.push_back(&win);
	my_wins.push_back(&cal);

	//ウィンドウ処理用
	MyWindow* g_win_active = nullptr;
	CLICKED_TYPE g_clicktype = CLICKED_TYPE::NONE;

	while (System::Update())
	{
		ClearPrint();
		//クリック時
		if (MouseL.down())
		{
			Vec2 pos = Cursor::PosF();
			g_win_active = nullptr;
			for (int i = 0; i < my_wins.size(); i++)
			{
				//ウィンドウをクリックしてた時
				CLICKED_TYPE ct = my_wins[i]->getPosType(pos);
				if (ct != CLICKED_TYPE::NONE)
				{
					g_win_active = my_wins[i];
					g_clicktype = ct;
					my_wins.remove_at(i);
					my_wins.push_front(g_win_active);
					break;
				}
			}
		}
		//ドラッグ時
		else if (MouseL.pressed())
		{
			Vec2 drag = Cursor::DeltaF();
			//ウィンドウをドラッグしているとき
			if (g_win_active != nullptr)
			{
				switch (g_clicktype)
				{
				case NONE://エラー？
					break;
				case TITLE_BAR://タイトルドラッグつまり移動
				{
					g_win_active->setPos(g_win_active->getPos() + drag);
				}
				break;
				case T_BAR_CLOSE://ボタンをドラッグしても…ねぇ？
					break;
				case T_BAR_MAX://ボタンをドラッグしても…ねぇ？
					break;
				case T_BAR_MIN://ボタンをドラッグしても…ねぇ？
					break;
				case CONTENTS://コンテンツドラッグ処理
					break;

					//////以下は変形系統//////
				case FLAME_UP://上方向に変形
				{
					Vec2 v = Vec2(0, drag.y);
					g_win_active->setPos(g_win_active->getPos() + v);
					g_win_active->setSize(g_win_active->getSize() - v);
				}
				break;
				case FLAME_DOWN://下方向に変形
				{
					Vec2 v = Vec2(0, drag.y);
					g_win_active->setSize(g_win_active->getSize() + v);
				}
				break;
				case FLAME_RIGHT://右方向に変形
				{
					Vec2 v = Vec2(drag.x, 0);
					g_win_active->setSize(g_win_active->getSize() + v);
				}
				break;
				case FLAME_LEFT://左方向に変形
				{
					Vec2 v = Vec2(drag.x, 0);
					g_win_active->setPos(g_win_active->getPos() + v);
					g_win_active->setSize(g_win_active->getSize() - v);
				}
				break;
				case FLAME_UP_RIGHT://右上方向に変形
				{
					g_win_active->setPos(g_win_active->getPos() + Vec2(0, drag.y));
					g_win_active->setSize(g_win_active->getSize() + Vec2(drag.x, -drag.y));
				}
				break;
				case FLAME_UP_LEFT://左上方向に変形
				{
					g_win_active->setPos(g_win_active->getPos() + drag);
					g_win_active->setSize(g_win_active->getSize() - drag);
				}
				break;
				case FLAME_DOWN_RIGHT://右下方向に変形
				{
					g_win_active->setSize(g_win_active->getSize() + drag);
				}
				break;
				case FLAME_DOWN_LEFT://左下方向に変形
				{
					g_win_active->setPos(g_win_active->getPos() + Vec2(drag.x, 0));
					g_win_active->setSize(g_win_active->getSize() + Vec2(-drag.x, drag.y));
				}
				break;
				default:
					break;
				}
			}
		}

		//各種処理
		for (int i = 0; i < my_wins.size(); i++)
		{
			my_wins[i]->update();
		}

		//描画は配列で後ろから
		for (int i = my_wins.size() - 1; i >= 0; i--)
		{
			my_wins[i]->draw();
		}
	}
}
