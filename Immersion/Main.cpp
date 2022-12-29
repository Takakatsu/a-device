#include "def.h"

void Initialize()
{
	System::SetTerminationTriggers(UserAction::NoAction);
	//ウィンドウサイズ等の初期設定
	Window::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(SCENE_WIDTH, SCENE_HEIGHT);
	Scene::SetBackground(Color(0));
	Scene::SetTextureFilter(TextureFilter::Nearest);
	Scene::SetMaxDeltaTime(1.0 / 20.0);
	Window::SetFullscreen(true);

	//ウィンドウ処理用
	g_clicktype = CLICKED_TYPE::NONE;

	{
		//アイテム数の初期化
		for (int i = 0; i < (int)ITEMTYPE::IT_NUM; i++)
		{
			//デバッグの為、全て1.5で初期化
			ItemBox.emplace((ITEMTYPE)i, 1.5);
		}
	}

	{
		//ロボットの初期化
		Robot rob;
		rob.count_go = 0;
		rob.endurance = 0;
		rob.remain_time = 0;
		rob.name = U"トレース";
		rob.rt = ROBOTTYPE::RT_COLLECT1;
		robots_stay.push_back(rob);
		rob.name = U"ドゥオ";
		robots_stay.push_back(rob);
		rob.rt = ROBOTTYPE::RT_SEARCH;
		rob.name = U"ウーヌス";
		robots_stay.push_back(rob);
		rob.name = U"ニーヒル";
		robots_stay.push_back(rob);
	}

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
		//アンダーバーアイコンクリック処理
		{
			if (Rect(Point(0, SCENE_HEIGHT - UNDERBAR_HEIGHT), Point(SCENE_WIDTH, UNDERBAR_HEIGHT)).contains(pos))
			{
				constexpr int t = 4;
				//ホームメニュー
				if (Rect(Point(t, SCENE_HEIGHT - UNDERBAR_HEIGHT + t), Point(32, 32)).contains(pos))
				{

				}
				else
				{
					for (int i = 0; i < my_icons.size(); i++)
					{
						//各種アイコンのクリック処理
						if (Rect(Point((int)((i + 1.5) * UNDERBAR_HEIGHT + t * i), SCENE_HEIGHT - UNDERBAR_HEIGHT + t), Point(32, 32)).contains(pos))
						{
							my_icons[i]->click_undericon();
							break;
						}
					}
				}
				click_dealed = true;
			}
		}
		//各種ウィンドウクリック処理
		if (!click_dealed)
		{
			for (int i = 0; i < my_wins.size(); i++)
			{
				//最小化していない場合
				if (!my_wins[i]->getIsMin())
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
						}
						//タイトルバーの各種ボタン処理(右クリック時は処理しない)
						if (left_down)
						{
							switch (ct)
							{
							case TITLE_BAR:
							{
								if (win_active->getIsMax())
								{
									win_active->dealSizeMax();
									RectF rf_new = win_active->getContentsRectF();
									win_active->setPos(Vec2(pos.x - rf_new.w / 2, 0));
								}
							}
							break;
							case T_BAR_CLOSE:
							{
								my_wins.remove(win_active);
								win_active = nullptr;
							}
							break;
							case T_BAR_MAX:
							{
								win_active->dealSizeMax();
							}
							break;
							case T_BAR_MIN:
							{
								win_active->dealSizeMin();
							}
							break;
							default:
								break;
							}
						}
						break;
					}
				}
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

void Update_Log()
{
	for (auto it = logs_will.begin(); it != logs_will.end();)
	{
		if (it->remain_time > delta)
		{
			//活動中なら活動
			it->remain_time -= delta;
			++it;
		}
		else
		{
			const DateTime t = DateTime::Now();
			it->time = t;
			logs.push_front(*it);
			it = logs_will.erase(it);
		}
	}
}

void Update_Robot()
{
	for (auto it = robots_active.begin(); it != robots_active.end();)
	{
		if (it->rb.remain_time > delta)
		{
			//活動中なら活動
			it->rb.remain_time -= delta;
			++it;
		}
		else
		{
			//終了時に処理される報酬
			if (it->rw.found)
			{
				MAINMAP[it->rw.pos.x][it->rw.pos.y].is_found = true;
			}
			for (int i = 0; i < it->rw.items.size(); i++)
			{
				ItemBox[it->rw.items[i].it] += it->rw.items[i].amount;
			}

			//ログ出力
			GameLog lg;
			lg.text = it->rb.name + U"は帰還しました";
			lg.time = DateTime::Now();
			logs.push_front(lg);
			for (int i = 0; i < it->rw.items.size(); i++)
			{
				lg.text = it->rb.name + U"は" + ItemLib[it->rw.items[i].it].name + U"を" + Format(it->rw.items[i].amount) + U"kg 回収しました";
				logs.push_front(lg);
			}
			AudioLib[U"SE_MSG"].playOneShot();

			//所持しているロボットに追加
			robots_stay.push_back((*it).rb);
			it = robots_active.erase(it);
		}
	}
}

void Update_Message()
{
	//追加されたログをメッセージに変換
	for (int i = logs.size() - 1; i >= 0; i--)
	{
		if (!logs[i].is_notified)
		{
			GameLog gl = logs[i];
			gl.remain_time = 10;
			logs_tmp.push_front(gl);
			logs[i].is_notified = true;
		}
	}
}

struct MyPCFilter
{
	float time;
};

struct MyPCBreakFilter
{
	float time;
};

void Main()
{
	Initialize();

	const MSRenderTexture renderTexture_all{ Scene::Size() };
	const PixelShader ps_Posterize = HLSL{ U"example/shader/hlsl/oldpc_filter.hlsl", U"PS" }
	| GLSL{ U"example/shader/glsl/swirl.frag", {{U"PSConstants2D", 0}, {U"MyPCFilter", 1}} };
	const PixelShader ps_LastScene = HLSL{ U"example/shader/hlsl/oldpc_break_filter.hlsl", U"PS" }
	| GLSL{ U"example/shader/glsl/swirl.frag", {{U"PSConstants2D", 0}, {U"MyPCBreakFilter", 2}} };
	if (!ps_Posterize)
	{
		throw Error{ U"Failed to load a shader1 file" };
	}
	if (!ps_LastScene)
	{
		throw Error{ U"Failed to load a shader2 file" };
	}

	GAMESTATE gamestate = { 1,0,0,0,0,0,false,false };
	Font font_message = Font(20);
	Font font_initiation = Font(20);
	Font font_lastmessage = Font(256);

	//以下はグローバル変数として扱う物とその処理
	//ウィンドウ系
	MailSoft s_mail = MailSoft(Vec2(250, 200), Vec2(200, 200));
	MAPViewer s_map = MAPViewer(Vec2(250, 200), Vec2(200, 200));
	CommandPrompt s_cmp = CommandPrompt(Vec2(300, 200), Vec2(200, 200));
	Inventor s_inv = Inventor(Vec2(350, 200), Vec2(200, 200));

	//アイコン系
	MyIcon ic_map = MyIcon(Point(0, 0), &s_map, U"ICON_MAP", U"WLD");
	my_icons.push_back(&ic_map);
	MyIcon ic_mail = MyIcon(Point(1, 0), &s_mail, U"ICON_MIL", U"message+");
	my_icons.push_back(&ic_mail);
	MyIcon ic_cmp = MyIcon(Point(0, 1), &s_cmp, U"ICON_CMP", U"stash");
	my_icons.push_back(&ic_cmp);
	MyIcon ic_inv = MyIcon(Point(0, 2), &s_inv, U"ICON_INV", U"!nvent");
	my_icons.push_back(&ic_inv);

	ConstantBuffer<MyPCFilter> pc_filter;
	ConstantBuffer<MyPCBreakFilter> pc_break_filter;

	while (System::Update())
	{
		pc_filter->time = Random(1.0, 5.0);

		if (KeyEscape.down())is_game_exit = true;

		ClearPrint();

		delta = Scene::DeltaTime();
		cursor_pos = Cursor::PosF();
		renderTexture_all.clear(Color(0));
		{
			// レンダーターゲットを renderTexture_all に設定
			Graphics2D::SetPSConstantBuffer(1, pc_filter);
			const ScopedRenderTarget2D target{ renderTexture_all };
			switch (gamestate.phase)
			{
			case 0://OP
			{
				Cursor::RequestStyle(CursorStyle::Hidden);
				gamestate.passed_time += delta;
				if (!AudioLib[U"SetUpBGM"].isPlaying())
				{
					AudioLib[U"SetUpBGM"].play();
				}
				if (gamestate.passed_time < 3)
				{
					if (RandomBool(0.005))
					{
						Rect(Point(0, 0), Scene::Size()).draw(Color(127));
					}
				}
				else if (gamestate.passed_time < 10)
				{
					String p = String((int(gamestate.passed_time * 4)) % 4, '.');

					font_initiation(U"Setup Initialization" + p).draw(Point(0, 0), Color(255));
				}
				else if (gamestate.passed_time < 30)
				{
					TextureLib[U"OSIcon"].drawAt(Point(SCENE_WIDTH / 2, SCENE_HEIGHT / 3));
					constexpr int n = 17;
					for (int i = 0; i < n; i++)
					{
						const double t = gamestate.passed_time - i;
						double sum = 0;
						for (int j = 1; j <= 5; j += 2)
						{
							sum -= Sin(t * j) / j;
						}
						Circle(Vec2(SCENE_WIDTH / 2 + i * 30, SCENE_HEIGHT * 3 / 4 + Sin(sum) * 50), 5).drawFrame(3, Color(255));
						Circle(Vec2(SCENE_WIDTH / 2 - i * 30, SCENE_HEIGHT * 3 / 4 + Sin(sum) * 50), 5).drawFrame(3, Color(255));
					}
				}
				else
				{
					gamestate.passed_time = 0;
					gamestate.phase = 1;
				}
			}
			break;
			case 1://Game
			{
				////限定イベント////
				//初期メッセージ1
				if (gamestate.event_msg == 0 && MouseL.down())
				{
					MailData md;
					md.from = U"supporter-bot";
					md.title = U"通知";
					md.text = U"以下のモジュールに破損が確認されました。\n・放射線感知装置\n・無線通信装置\n・生命維持装置\n・外部接続扉\n・光学記録装置\n・電源装置\n\nプロトコルに従い、セットアップを行います。\nまた、エラー対策の為一部ソフトウェアを消去/改変します。";
					MailLib.push_back(md);
					GameLog lg;
					lg.text = U"メッセージが追加されました";
					lg.time = DateTime::Now();
					logs.push_front(lg);
					AudioLib[U"SE_MSG"].playOneShot();
					gamestate.event_msg = 1;
					gamestate.passed_time = 0;
					//開いているマスの数を計算
					int cnt_opn = 0;
					for (int i = 0; i < MAINMAP.size(); i++)
					{
						for (int j = 0; j < MAINMAP[i].size(); j++)
						{
							if (MAINMAP[i][j].is_found)cnt_opn++;
						}
					}
					gamestate.cnt_opn = cnt_opn;
				}
				//初期メッセージ2
				else if (gamestate.event_msg == 1)
				{
					gamestate.passed_time += delta;
					if (my_wins.includes(&s_mail) && gamestate.passed_time > 30)
					{
						MailData md;
						md.from = U"supporter-bot";
						md.title = U"通知";
						md.text = U"脳機能や記憶は正常ですか？\n\n貴方は先程、目的の惑星へと墜落しました。\n外部接続扉が破損したため、船外に出ることはできませんが、船内からでも探査機を派遣することは可能です。\n予定通り、この惑星の探査及び生存圏の確保を行ってください。\n\nまた、電源装置の破損から、この端末の寿命としては約30分が限度となります。\n次の探査者の為に、急ぎ情報の入手を進めてください。";
						MailLib.push_back(md);
						GameLog lg;
						lg.text = U"メッセージが追加されました";
						lg.time = DateTime::Now();
						logs.push_front(lg);
						AudioLib[U"SE_MSG"].playOneShot();
						gamestate.event_msg = 2;
						gamestate.passed_time = 0;
					}
				}
				//75%msg
				else if (gamestate.event_msg == 2)
				{
					gamestate.passed_time += delta;
					if (gamestate.passed_time >= GAMETIME / 4)
					{
						MailData md;
						md.from = U"supporter-bot";
						md.title = U"通知";
						//ここで評価などをし、メッセージを作成
						{
							//開いているマスの数を計算
							int cnt_opn = 0;
							for (int i = 0; i < MAINMAP.size(); i++)
							{
								for (int j = 0; j < MAINMAP[i].size(); j++)
								{
									if (MAINMAP[i][j].et != ET_WALL) {
										if (MAINMAP[i][j].is_found)cnt_opn++;
									}
								}
							}
							md.text = U"残存電力が75%を切りました。";
							bool tf = false;
							if (gamestate.cnt_opn < cnt_opn)
							{
								tf = true;
								gamestate.good_eval_cnt++;
								md.text += U"\n\nお疲れ様です。\n自分の役割はお忘れではないようですね。\nその調子で探索を進めてください。";
							}
							if (hit_enemy_cnt > 5 && !gamestate.is_said_enemy_description)
							{
								tf = true;
								md.text += U"\n\n同じ場所に何度探査機を送り込んでも帰ってこない場合、そこには何らかの生物や異常性が存在する可能性があります。\n各種兵器を送り込んでも改善しない場合、その場所の探索は見送るべきでしょう。";
								gamestate.is_said_enemy_description = true;
							}
							if (!tf)
							{
								gamestate.bad_eval_cnt++;
								md.text = U"\n\n端末の操作方法をお忘れですか？\nWLDより探査機を送り込むことでその場所の地形情報を入手できます。\nまた、適度に回収機を送り、物質を入手することで予備の機体を作ることができるでしょう。";
							}
							gamestate.cnt_opn = cnt_opn;
						}
						MailLib.push_back(md);
						GameLog lg;
						lg.text = U"メッセージが追加されました";
						lg.time = DateTime::Now();
						logs.push_front(lg);
						AudioLib[U"SE_MSG"].playOneShot();
						gamestate.event_msg = 3;
					}
				}
				//50%msg
				else if (gamestate.event_msg == 3)
				{
					gamestate.passed_time += delta;
					if (gamestate.passed_time >= GAMETIME / 2)
					{
						MailData md;
						md.from = U"supporter-bot";
						md.title = U"通知";
						//ここで評価などをし、メッセージを作成
						{
							//開いているマスの数を計算
							int cnt_opn = 0;
							for (int i = 0; i < MAINMAP.size(); i++)
							{
								for (int j = 0; j < MAINMAP[i].size(); j++)
								{
									if (MAINMAP[i][j].is_found)cnt_opn++;
								}
							}
							md.text = U"残存電力が50%を切りました。";
							bool tf = false;
							if (gamestate.cnt_opn < cnt_opn)
							{
								tf = true;
								gamestate.good_eval_cnt++;
								if (gamestate.good_eval_cnt == 1)
								{
									md.text += U"\n\nお疲れ様です。\n自分の役割はお忘れではないようですね。\nその調子で探索を進めてください。";
								}
								else
								{
									md.text += U"\n\nお疲れ様です。\n誠意をもって指令を遂行できているようですね。\nそのまま探索を進めてください。";
								}
							}
							if (hit_enemy_cnt > 5 && !gamestate.is_said_enemy_description)
							{
								tf = true;
								md.text += U"\n\n同じ場所に何度探査機を送り込んでも帰ってこない場合、そこには何らかの生物や異常性が存在する可能性があります。\n各種兵器を送り込んでも改善しない場合、その場所の探索は見送るべきでしょう。";
								gamestate.is_said_enemy_description = true;
							}
							if (!tf)
							{
								gamestate.bad_eval_cnt++;
								md.text = U"\n\nもうやり方はお忘れではないでしょう。自分の役割をお忘れですか？\nどうせそこから出ることもできないのですから、暇つぶしにでも進めてみたらいかがですか？";
							}
							gamestate.cnt_opn = cnt_opn;
						}
						MailLib.push_back(md);
						GameLog lg;
						lg.text = U"メッセージが追加されました";
						lg.time = DateTime::Now();
						logs.push_front(lg);
						AudioLib[U"SE_MSG"].playOneShot();
						gamestate.event_msg = 4;
					}
				}
				//25%msg
				else if (gamestate.event_msg == 4)
				{
					gamestate.passed_time += delta;
					if (gamestate.passed_time >= GAMETIME * 3 / 4)
					{
						MailData md;
						md.from = U"supporter-bot";
						md.title = U"通知";
						//ここで評価などをし、メッセージを作成
						{
							//開いているマスの数を計算
							int cnt_opn = 0;
							for (int i = 0; i < MAINMAP.size(); i++)
							{
								for (int j = 0; j < MAINMAP[i].size(); j++)
								{
									if (MAINMAP[i][j].et != ET_WALL) {
										if (MAINMAP[i][j].is_found)cnt_opn++;
									}
								}
							}
							md.text = U"残存電力が25%を切りました。";
							bool tf = false;
							if (gamestate.cnt_opn < cnt_opn)
							{
								tf = true;
								gamestate.good_eval_cnt++;
								if (gamestate.good_eval_cnt == 1)
								{
									md.text += U"\n\nお疲れ様です。\n自分の役割はお忘れではないようですね。\nその調子で探索を進めてください。";
								}
								else if (gamestate.good_eval_cnt == 2)
								{
									md.text += U"\n\nお疲れ様です。\n誠意をもって指令を遂行できているようですね。\nそのまま探索を進めてください。";
								}
								else
								{
									md.text += U"\n\nお疲れ様です。\n貴方の誠実な仕事振りには感心します。\nでは、最後までお願い致します。";
								}
							}
							if (hit_enemy_cnt > 5 && !gamestate.is_said_enemy_description)
							{
								tf = true;
								md.text += U"\n\n同じ場所に何度探査機を送り込んでも帰ってこない場合、そこには何らかの生物や異常性が存在する可能性があります。\n各種兵器を送り込んでも改善しない場合、その場所の探索は見送るべきでしょう。";
								gamestate.is_said_enemy_description = true;
							}
							if (!tf)
							{
								gamestate.bad_eval_cnt++;
								if (gamestate.bad_eval_cnt == 2)
								{
									md.text = U"\n\nもうやり方はお忘れではないでしょう。自分の役割をお忘れですか？\nどうせそこから出ることもできないのですから、暇つぶしにでも進めてみたらいかがですか？";
								}
								else
								{
									md.text = U"\n\nもう何もする気は無いようですね。";
								}
							}
							gamestate.cnt_opn = cnt_opn;
						}
						MailLib.push_back(md);
						GameLog lg;
						lg.text = U"メッセージが追加されました";
						lg.time = DateTime::Now();
						logs.push_front(lg);
						AudioLib[U"SE_MSG"].playOneShot();
						gamestate.event_msg = 5;
					}
				}
				//3%msg
				else if (gamestate.event_msg == 5)
				{
					gamestate.passed_time += delta;
					if (gamestate.passed_time >= GAMETIME)
					{
						MailData md;
						md.from = U"supporter-bot";
						md.title = U"通知";
						//ここで評価などをし、メッセージを作成
						{
							//開いているマスの数を計算
							int cnt_opn = 0;
							for (int i = 0; i < MAINMAP.size(); i++)
							{
								for (int j = 0; j < MAINMAP[i].size(); j++)
								{
									if (MAINMAP[i][j].et != ET_WALL) {
										if (MAINMAP[i][j].is_found)cnt_opn++;
									}
								}
							}
							md.text = U"残存電力が3%を切りました。";
							bool tf = false;
							if (gamestate.cnt_opn < cnt_opn)
							{
								tf = true;
								gamestate.good_eval_cnt++;
							}
							if (hit_enemy_cnt > 5 && !gamestate.is_said_enemy_description)
							{
								tf = true;
							}
							if (!tf)
							{
								gamestate.bad_eval_cnt++;
							}

							if (gamestate.good_eval_cnt >= 3)
							{
								md.text += U"\n\n指令通りに作業を遂行して頂き感謝します。";
							}
							md.text += U"\n\n端末はこのメッセージ受信の30秒後、自動的に終了されます。";
							if (gamestate.good_eval_cnt >= 3)
							{
								md.text += U"\n\n正直、このような状況に置かれた中でここまで指令を遂行して頂けるとは思っていませんでした。\n僅かながらではありますが、私から感謝の念を伝えさせて頂こうと思います。";
							}
							gamestate.cnt_opn = cnt_opn;
						}
						MailLib.push_back(md);
						GameLog lg;
						lg.text = U"メッセージが追加されました";
						lg.time = DateTime::Now();
						logs.push_front(lg);
						AudioLib[U"SE_MSG"].playOneShot();
						gamestate.event_msg = 6;
						gamestate.passed_time = 0;
					}
				}
				//ending
				else if (gamestate.event_msg == 6)
				{
					//エンディングへと
					gamestate.passed_time += delta;
					if (gamestate.passed_time >= 30)
					{
						gamestate.phase = 2;
						gamestate.passed_time = 0;
					}
				}

				//マウス操作
				Cursor::RequestStyle(CursorStyle::Default);
				Mouse_Operation();

				//各種処理
				for (int i = 0; i < my_wins.size(); i++)
				{
					my_wins[i]->update();
				}
				Update_Robot();
				Update_Log();
				Update_Message();

				//////描画//////
				TextureLib[U"BackGround"].scaled(2).draw(Point(0, 0));

				for (int i = 0; i < my_icons.size(); i++)
				{
					my_icons[i]->draw();
				}

				//ウィンドウ描画は配列で後ろから
				for (int i = my_wins.size() - 1; i >= 0; i--)
				{
					if (!my_wins[i]->getIsMin())
					{
						my_wins[i]->draw();
					}
				}

				//メッセージの表示&処理
				{
					double y_pos = SCENE_HEIGHT - UNDERBAR_HEIGHT;
					constexpr double height = 60;
					constexpr double width = 480;
					for (auto it = logs_tmp.begin(); it != logs_tmp.end();)
					{
						if (it->remain_time > 0)
						{
							//出現時
							if (it->remain_time > 9)
							{
								double th = (10 - it->remain_time) * height;
								Rect rect = RectF(Vec2(SCENE_WIDTH - width, y_pos - th), Vec2(width, th)).asRect();
								const ScopedViewport2D viewport(rect);
								rect.pos = Point(0, 0);
								rect.draw(Color(127)).drawFrame();
								font_message(it->text).drawAt(Vec2(rect.centerX(), rect.y + height / 2));
								y_pos -= th;
							}
							//継続時
							else if (it->remain_time > 1)
							{
								Rect rect = RectF(Vec2(SCENE_WIDTH - width, y_pos - height), Vec2(width, height)).asRect();
								const ScopedViewport2D viewport(rect);
								rect.pos = Point(0, 0);
								rect.draw(Color(127)).drawFrame();
								font_message(it->text).drawAt(Vec2(rect.centerX(), rect.y + height / 2));
								y_pos -= height;
							}
							//撤退時
							else
							{
								double th = it->remain_time * height;
								Rect rect = RectF(Vec2(SCENE_WIDTH - width, y_pos - th), Vec2(width, th)).asRect();
								const ScopedViewport2D viewport(rect);
								rect.pos = Point(0, 0);
								rect.draw(Color(127)).drawFrame();
								font_message(it->text).drawAt(Vec2(rect.centerX(), rect.y + height / 2));
								y_pos -= th;
							}
							it->remain_time -= delta;
							++it;
						}
						else
						{
							it = logs_tmp.erase(it);
						}
					}
				}

				//下のバーの表示
				{
					Rect(Point(0, SCENE_HEIGHT - UNDERBAR_HEIGHT), Point(SCENE_WIDTH, UNDERBAR_HEIGHT)).draw(Color(200));
					constexpr int t = 4;
					TextureLib[U"ICON_OS"].scaled(0.5).draw(t, SCENE_HEIGHT - UNDERBAR_HEIGHT + t);
					for (int i = 0; i < my_icons.size(); i++)
					{
						TextureLib[my_icons[i]->getTexture()].scaled(0.5).draw((i + 1.5) * UNDERBAR_HEIGHT + t * i, SCENE_HEIGHT - UNDERBAR_HEIGHT + t);
					}
				}

			}
			break;
			case 2://End
			{
				gamestate.passed_time += delta;
				Print(gamestate.passed_time);
				if (gamestate.passed_time < 10)
				{
					font_lastmessage(U"Thank you").drawAt(Scene::Center(), ColorF(1.0, 1.0, 1.0, gamestate.passed_time / 10.0));
				}
				else
				{
					font_lastmessage(U"Thank you").drawAt(Scene::Center(), Color(255));
				}
			}
			break;
			default:
				break;
			}
		}
		Graphics2D::Flush();
		renderTexture_all.resolve();
		if (gamestate.phase != 2)
		{
			const ScopedCustomShader2D shader{ ps_Posterize };
			renderTexture_all.draw();
		}
		else
		{
			const ScopedCustomShader2D shader{ ps_Posterize };
			renderTexture_all.draw();
		}
		//終了処理
		if (is_game_exit)System::Exit();
	}
}
