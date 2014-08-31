#ifndef ACHTUNGRESOURCE_H__
#define ACHTUNGRESOURCE_H__

#ifndef _DEBUG
#define ALLEGRO_STATICLINK
#endif

#include <allegro5/allegro_memfile.h>

ALLEGRO_FILE* load_blue();
ALLEGRO_FILE* load_yellow();
ALLEGRO_FILE* load_red();
ALLEGRO_FILE* load_purple();
ALLEGRO_FILE* load_orange();
ALLEGRO_FILE* load_green();
ALLEGRO_FILE* load_alfaslabone();
ALLEGRO_FILE* load_punch();
ALLEGRO_FILE* load_bigbounce();

#endif