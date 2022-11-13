#include "def.h"

void Main()
{
	//ウィンドウサイズ等の初期設定
	Window::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(960, 540);
	Scene::SetBackground(Color(255));
	Window::SetFullscreen(true);

	//データの初期化
	initialize_lib();

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

	//ウィンドウ処理用
	CLICKED_TYPE g_clicktype = CLICKED_TYPE::NONE;

	while (System::Update())
	{
		ClearPrint();
		//クリック時
		if (MouseL.down())
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
					//タイトルバーの各種ボタン処理
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
					//コンテンツクリック時にクリック処理呼び出し
					if (ct == CLICKED_TYPE::CONTENTS)
					{
						win_active->click(pos - win_active->getContentsRectF().pos);
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
		else if (MouseL.pressed())
		{
			Vec2 drag = Cursor::DeltaF();
			//ウィンドウをドラッグしているとき
			if (win_active != nullptr)
			{
				switch (g_clicktype)
				{
				case NONE://エラー？
					break;

					//////タイトルバー系統//////
				case TITLE_BAR://タイトルドラッグつまり移動
				{
					win_active->setPos(win_active->getPos() + drag);
				}
				break;
				case T_BAR_CLOSE://ボタンをドラッグしても…ねぇ？
					break;
				case T_BAR_MAX://ボタンをドラッグしても…ねぇ？
					break;
				case T_BAR_MIN://ボタンをドラッグしても…ねぇ？
					break;

					//////メイン処理//////
				case CONTENTS://コンテンツドラッグ処理
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
