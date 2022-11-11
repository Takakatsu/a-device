#include "def.h"

void Main()
{
	//ウィンドウサイズ等の初期設定
	Window::Resize(1280, 720);

	Array<MyWindow*> my_wins;
	MyWindow win = MyWindow(Vec2(0, 0), Vec2(100, 100));
	Calculator cal = Calculator(Vec2(150, 0), Vec2(100, 100));
	my_wins.push_back(&win);
	my_wins.push_back(&cal);

	while (System::Update())
	{
		Scene::SetBackground(Color(255));
		for (int i = 0; i < my_wins.size(); i++)
		{
			my_wins[i]->update();
			my_wins[i]->draw();
		}
	}
}
