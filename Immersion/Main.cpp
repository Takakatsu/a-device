#include "def.h"

void Main()
{
	//ウィンドウサイズ等の初期設定
	Window::Resize(1280, 720);
	Scene::SetBackground(Color(255));

	Array<MyWindow*> my_wins;
	MyWindow win = MyWindow(Vec2(0, 0), Vec2(200, 200));
	Calculator cal = Calculator(Vec2(250, 0), Vec2(200, 200));
	my_wins.push_back(&win);
	my_wins.push_back(&cal);

	MyWindow* win_active = nullptr;

	while (System::Update())
	{
		//クリック時
		if (MouseL.down())
		{
			Vec2 pos = Cursor::PosF();
			win_active = nullptr;
			for (int i = 0; i < my_wins.size(); i++)
			{
				//ウィンドウをクリックしてた時
				if (my_wins[i]->getRectF().contains(pos))
				{
					win_active = my_wins[i];
					my_wins.remove_at(i);
					my_wins.push_front(win_active);
					break;
				}
			}
		}
		//ドラッグ時
		else if (MouseL.pressed())
		{
			//ウィンドウをドラッグしているとき
			if (win_active != nullptr)
			{
				win_active->setPos(win_active->getPos() + Cursor::DeltaF());
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
