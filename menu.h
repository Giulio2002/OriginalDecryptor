#include "OriginalDecryptor.h"

extern char key[500];
int button_file_event(original_button *button_file, SDL_Event *e, int *draw);
void button_ok_event(original_button *button_ok, SDL_Event *e,int *quit, int *draw);
int menu();