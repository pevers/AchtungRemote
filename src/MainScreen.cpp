#include "MainScreen.h"

Rect boxes[6] = { {150, MARGIN_Y, boxes[0].x + BOX_WIDTH, BOX_HEIGHT + MARGIN_Y}, {boxes[0].x2 + MARGIN_X, MARGIN_Y, boxes[1].x + BOX_WIDTH, BOX_HEIGHT + MARGIN_Y}, {boxes[1].x2 + MARGIN_X, MARGIN_Y, boxes[2].x + BOX_WIDTH, BOX_HEIGHT + MARGIN_Y},
{150, SCREEN_HEIGHT - MARGIN_Y - BOX_HEIGHT, boxes[3].x + BOX_WIDTH, SCREEN_HEIGHT - MARGIN_Y}, {boxes[3].x2 + MARGIN_X, SCREEN_HEIGHT - MARGIN_Y - BOX_HEIGHT, boxes[4].x + BOX_WIDTH, SCREEN_HEIGHT - MARGIN_Y},
{boxes[4].x2 + MARGIN_X, SCREEN_HEIGHT - MARGIN_Y - BOX_HEIGHT, boxes[5].x + BOX_WIDTH, SCREEN_HEIGHT - MARGIN_Y}
};

MainScreen::MainScreen(boost::shared_ptr<Game> &game)
{
	m_game = game;
	m_titleFont = al_load_ttf_font_f(load_alfaslabone(), TITLE_FONT, 72, 0);
	m_playerFont = al_load_ttf_font_f(load_alfaslabone(), TITLE_FONT, 20, 0);
	m_instrFont = al_load_ttf_font_f(load_alfaslabone(), TITLE_FONT, 20, 0);

	m_playerBitmaps[0] = al_load_bitmap_f(load_red(), ".png");
	m_playerBitmaps[1] = al_load_bitmap_f(load_blue(), ".png");
	m_playerBitmaps[2] = al_load_bitmap_f(load_green(), ".png");
	m_playerBitmaps[3] = al_load_bitmap_f(load_orange(), ".png");
	m_playerBitmaps[4] = al_load_bitmap_f(load_purple(), ".png");
	m_playerBitmaps[5] = al_load_bitmap_f(load_yellow(), ".png");
}

MainScreen::~MainScreen()
{
	al_destroy_font(m_titleFont);
	al_destroy_font(m_playerFont);
	al_destroy_font(m_instrFont);

	for(int i = 0; i < 6; i++)
	{
		if(m_playerBitmaps[i])
			al_destroy_bitmap(m_playerBitmaps[i]);
	}
}

int MainScreen::Update()
{
	// check input, if the user hits SPACE we start the game if enough players joined
	// TODO: Remove!
	if(Input.keyboard.GetKeyPressed(ALLEGRO_KEY_SPACE))
	{
		if(m_game->GetNumberOfPlayersInQueue() >= 1)
		{
			m_game->StartGame();
			return SCREEN_GAME;
		}
	}

	if(m_game->GetServer()->GetServerState() == SERVER_REQUEST_START)
	{
		// start the game if there are enough players
		if(m_game->GetNumberOfPlayersInQueue() >= 2)
		{
			m_game->StartGame();
			return SCREEN_GAME;
		} 
		else
		{
			m_game->GetServer()->SetServerState(SERVER_ACCEPT);
		}
	}

	return SCREEN_HOME;
}

/**
 * Draw this display
 */
void MainScreen::Draw(ALLEGRO_DISPLAY *display)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(m_titleFont, al_map_rgb(220, 10, 10), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 50, ALLEGRO_ALIGN_CENTER, "ACHTUNG, DIE KURVE!");

	//al_draw_rectangle(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 72 + m_titleFont->height, SCREEN_WIDTH/2+100, SCREEN_HEIGHT/2 - 22 + m_titleFont->height, al_map_rgb(200, 10, 10), 4);
	//al_draw_text(m_ipFont, al_map_rgb(200, 10, 10), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 72 + m_titleFont->height + 10, ALLEGRO_ALIGN_CENTER, m_game->GetServer()->GetHostAddress().c_str());

	// draw the 8 rectangles
	DrawPlayerBoxes();

	// draw instruction
	al_draw_text(m_instrFont, al_map_rgb(160, 160, 160), 0, 0, ALLEGRO_ALIGN_LEFT, "SPACE - START GAME");
}

/**
 * Draw the box of all the players
 */
void MainScreen::DrawPlayerBoxes()
{
	for(int i = 0; i < 6; i++)
	{
		al_draw_rectangle(boxes[i].x, boxes[i].y, boxes[i].x2, boxes[i].y2, al_map_rgb(255, 255, 255), 4);

		// check connections
		boost::shared_ptr<GameServer> server = m_game->GetServer();
		std::vector<PlayerController> *controllers = server->GetPlayerControllers();
		for(int i = 0; i < (int)controllers->size(); i++)
		{
			DrawPlayerName(controllers->at(i).name,
				boxes[i].x + BOX_WIDTH/2,
				boxes[i].y + BOX_HEIGHT - m_playerFont->height - 10);

			// draw circle
			if(m_playerBitmaps[i])
			{
				al_draw_bitmap(m_playerBitmaps[i], boxes[i].x + BOX_WIDTH / 2 - 70, boxes[i].y + BOX_HEIGHT / 2 - 70, 0);
			}
		}
	}
}

void MainScreen::DrawPlayerName(std::string name, float x, float y)
{
	al_draw_text(m_playerFont, al_map_rgb(160, 160, 160), x, y, ALLEGRO_ALIGN_CENTER, name.c_str());
}

