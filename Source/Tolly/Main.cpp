#include "Main.h"

void HandleInputState(ALLEGRO_EVENT &ev);
Settings LoadSettings();

int main(int argc, char **argv)
{
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_TIMER *timer = NULL;
   ALLEGRO_EVENT ev;
 
   if(!al_init()) {
      al_show_native_message_box(NULL, NULL, NULL, "Could not initialize Allegro 5", NULL, NULL);
      return -1;
   }
 
   al_set_new_display_flags(ALLEGRO_WINDOWED);
   display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
   al_set_window_title(display, "AchtungApp");

   if(!display)
   {
	   al_show_native_message_box(display, "Tolly", "Display Settings", "Display Window was not created successfully", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	   return -1;
   }

   al_init_primitives_addon();
   al_init_font_addon();
   al_init_ttf_addon();
   al_init_image_addon();
   al_install_keyboard();
   al_install_mouse();
   al_install_audio();   
   al_init_acodec_addon();

   event_queue = al_create_event_queue();
   timer = al_create_timer(1.0 / GAME_TICKS);

   // register events
   al_register_event_source(event_queue, al_get_keyboard_event_source());
   al_register_event_source(event_queue, al_get_mouse_event_source());
   al_register_event_source(event_queue, al_get_display_event_source(display));
   al_register_event_source(event_queue, al_get_timer_event_source(timer));

   // load settings
   Settings settings = LoadSettings();

   // create the game
   boost::shared_ptr<Game> game = boost::shared_ptr<Game>(new Game(settings));

   bool done = false, redraw = false;
   ScreenManager screenManager;
   screenManager.AddScreen(SCREEN_WIN, new WinScreen(game));
   screenManager.AddScreen(SCREEN_GAME, new GameScreen(game));
   screenManager.AddScreen(SCREEN_HOME, new MainScreen(game));
   
   al_start_timer(timer);
   while(!done)
   {
	   al_wait_for_event(event_queue, &ev);

	   HandleInputState(ev);

	   if(ev.type == ALLEGRO_EVENT_TIMER)
	   {
		   screenManager.Update();
		   redraw = true;
	   }
	   else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
	   {
		   done = true;
	   }
	   else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
	   {
		   done = ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE;
	   }

	   if(redraw && al_event_queue_is_empty(event_queue))
	   {
		   redraw = false;
		   screenManager.Draw(display);
		   al_flip_display();
	   }
   }

   al_destroy_display(display);
   al_destroy_timer(timer);
   al_destroy_event_queue(event_queue);
   return 0;
}

void HandleInputState(ALLEGRO_EVENT &ev)
{
	if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		Input.keyboard.SetKey(ev.keyboard.keycode, true);
	} else if(ev.type == ALLEGRO_EVENT_KEY_UP)
	{
		Input.keyboard.SetKey(ev.keyboard.keycode, false);
	}
}

/**
 * Load the settings for this game from the configuration file.
 */
Settings LoadSettings()
{
	std::ifstream file;
	file.open(CONFIG_FILE);

	Settings settings;
	settings.gamespeed = SPEED_MEDIUM;	// don't change the gamespeed yet, not sure if I should include this or not
	settings.name = "Default Tolly Server";

	if(!file.is_open())
	{
		std::cout << "[Warning]: Can't open " << CONFIG_FILE << " to read the settings. Using default settings." << std::endl;
		return settings;
	}

	std::string serverName;
	std::getline(file, serverName);
	if(serverName.length() == 0)
	{
		std::cout << "[Warning]: Invalid server name found in " << CONFIG_FILE << std::endl;
		return settings;	// invalid, return default
	}

	settings.name = serverName.substr(0, 30);
	return settings;
}