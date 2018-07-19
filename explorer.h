#include "OriginalDecryptor.h"

extern char * path;

void text_reset(original_textbox *textbox, original_vscrollbar *vscrollbar);
void dirent_read(original_textbox *textbox1, original_vscrollbar *vscrollbar1,
                        original_textbox *textbox2,	original_vscrollbar *vscrollbar2,
                        original_label *label_sel);

void textbox1_event(original_textbox *textbox, SDL_Event *e,
                           original_vscrollbar *vscrollbar1, original_textbox *textbox2,
                           original_vscrollbar *vscrollbar2, original_label *label_sel, int *draw);

void vscrollbar1_event(original_vscrollbar *vscrollbar, SDL_Event *e, original_textbox *textbox1, int *draw);
void vscrollbar2_event(original_vscrollbar *vscrollbar, SDL_Event *e, original_textbox *textbox2, int *draw);
int button_ok1_event(original_button *button, SDL_Event *e ,int *draw,original_label label_res);
void button_cancel_event(original_button *button, SDL_Event *e,int *quit, int *draw);
int explorer();

