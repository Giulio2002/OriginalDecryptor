/*
  original_sdl widget toolkit
  Copyright (c) 2016 Tarvo Korrovits <tkorrovi@mail.com>
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would
     be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

  original_sdl version 1.2.0
*/

#ifndef _original_sdl_h
#define _original_sdl_h

#ifndef RESDIR
#define RESDIR ""
#endif

#if defined(_MSC_VER)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#elif defined(__APPLE__)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#endif
#include <sys/types.h>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#define original_MAX_LENGTH 200
#define original_MIN_LENGTH 10
#define original_MAX_LABEL 500
#define original_MAGIC 12345

enum {OTHER_TYPE, WINDOW_TYPE, RENDERER_TYPE, TEXTURE_TYPE, SURFACE_TYPE,
	FONT_TYPE, STRING_TYPE, ARRAY_TYPE};

#ifdef _MSC_VER
#ifdef _WIN32
typedef struct _stat32 original_stat;
#else
typedef struct _stat64 original_stat;
#endif

typedef struct original_dirent {
	char *d_name;
} original_dirent;

typedef struct original_dir {
	ptrdiff_t fhandle;
	struct _finddata_t fdata;
	original_dirent ent;
	char name[original_MAX_LENGTH];
} original_dir;
#else
typedef struct stat original_stat;
typedef struct dirent original_dirent;
typedef DIR original_dir;
#endif

/* Length is number of elements, size is allocated size */
typedef struct original_array {
	void **data;
	int *id;
	int length;
	int size;
	int ref;
} original_array;

typedef struct original_image {
	SDL_Texture *image;
	int magic;
	int w;
	int h;
} original_image;

typedef struct original_font {
	TTF_Font *font;
	int magic;
	int fontheight;
	int spacing;
	int lineheight;
	int advance;
	int ascent;
} original_font;

typedef struct original_window {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	SDL_Color bg;
	struct original_window *wdw;
} original_window;

typedef struct original_label {
	int visible;
	SDL_Rect rect;
	char text[original_MAX_LABEL];
	SDL_Color textcolor;
	original_font font;
	original_window *wdw;
} original_label;

typedef struct original_button {
	int visible;
	int focus;
	SDL_Rect rect;
	int textx;
	int texty;
	char text[original_MAX_LENGTH];
	int active;
	int prelight;
	SDL_Color textcolor;
	original_font font;
	original_image normalimg;
	original_image activeimg;
	original_image prelightimg;
	original_window *wdw;
} original_button;

typedef struct original_selectbutton {
	int visible;
	int focus;
	SDL_Rect rect;
	int selected;
	original_image selectedimg;
	original_image unselectedimg;
	original_window *wdw;
} original_selectbutton;

typedef struct original_vscrollbar {
	int visible;
	int focus;
	SDL_Rect uprect;
	SDL_Rect downrect;
	SDL_Rect sliderrect;
	int maxpos;
	double fraction;
	double step;
	unsigned int lasttick;
	int downclicked;
	int upclicked;
	int sliderclicked;
	original_image up;
	original_image down;
	original_image vslider;
	original_window *wdw;
} original_vscrollbar;

typedef struct original_hscrollbar {
	int visible;
	int focus;
	SDL_Rect leftrect;
	SDL_Rect rightrect;
	SDL_Rect sliderrect;
	int maxpos;
	double fraction;
	double step;
	unsigned int lasttick;
	int leftclicked;
	int rightclicked;
	int sliderclicked;
	original_image left;
	original_image right;
	original_image hslider;
	original_window *wdw;
} original_hscrollbar;

typedef struct original_progressbar {
	int visible;
	SDL_Rect rect;
	SDL_Rect barrect;
	int width;
	double fraction;
	double step;
	SDL_Color bg;
	unsigned int lasttick;
	int run;
	original_image bar;
	original_window *wdw;
} original_progressbar;

typedef struct original_entry {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	int textx;
	int texty;
	char text[original_MAX_LENGTH];
	int active;
	int textwidth;
	int selection[4];
	int cursor[2];
	SDL_Color normalcolor;
	SDL_Color activecolor;
	SDL_Color bg;
	original_font font;
	original_window *wdw;
} original_entry;

typedef struct original_textbox {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	original_array *array;
	SDL_Rect textrect;
	int firstline;
	int maxlines;
	int textwidth;
	int highlightline;
	int selectedline;
	int selection[4];
	int cursor[2];
	SDL_Color textcolor;
	SDL_Color hlcolor;
	SDL_Color bg;
	original_font font;
	original_window *wdw;
} original_textbox;

typedef struct original_combobox {
	int visible;
	char text[original_MAX_LENGTH];
	original_entry entry;
	original_window window;
	original_vscrollbar vscrollbar;
	original_textbox textbox;
	original_image combo;
	original_window *wdw;
} original_combobox;

extern SDL_Color original_white, original_black, original_green, original_blue,
		original_lightblue;
extern original_font original_textfont, original_buttonfont;
extern original_image original_normal, original_prelight, original_active, original_bar,
	original_up, original_down, original_left, original_right, original_vslider,
	original_hslider, original_selected, original_unselected, original_combo;
extern double original_spacing;
extern int original_textfont_size, original_buttonfont_size;
extern int original_click_interval, original_progress_interval;
extern int original_slider_padding;
extern int original_border, original_edge;
extern int original_screen_width, original_screen_height;

#ifdef __cplusplus
extern "C" {
#endif

char *original_getcwd(char *buf, int size);
int original_chdir(char *path);
int original_getstat(char *pathname, original_stat *buf);
original_dir *original_opendir(char *name);
original_dirent *original_readdir(original_dir *dirp);
int original_closedir(original_dir *dirp);
int original_isdir(original_stat s);
int original_isreg(original_stat s);
int original_makerect(SDL_Rect *rect, int x, int y, int h, int w);
int original_pointinrect(int x, int y, SDL_Rect *rect);
int original_utf8next(char *str, int index);
int original_utf8prev(char *str, int index);
int original_utf8fix(char *str);
char *original_string_copy(char *dest, size_t size, char *str1, char *str2);
int original_string_compare(const void *a, const void *b);
char *original_backspace(char *str);
int original_array_new(original_array *a);
void *original_array_data(original_array *a, int index);
int original_array_id(original_array *a, int index);
int original_array_assign(original_array *a, int index, int id, void *data);
int original_array_append(original_array *a, int id, void *data);
int original_array_appendstring(original_array *a, int id, char *text1, char *text2);
int original_array_insert(original_array *a, int index, int id, void *data);
int original_array_remove(original_array *a, int index);
int original_array_free(original_array *a);
unsigned int original_getticks(void);
int original_maxlength(original_font font, int width, char *str1, char *str2);
int original_textwidth(original_font font, char *str1, char *str2);
int original_renderimage(SDL_Renderer *renderer, original_image image,
	int x, int y, SDL_Rect *clip);
int original_rendertext(SDL_Renderer *renderer, char *text, int x, int y,
	original_font font, SDL_Color color);
int original_fillrect(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color);
int original_decorate(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color,
	int edge);
int original_image_new(original_image *image, char *fname, original_array *a,
	SDL_Renderer* renderer);
int original_font_new(original_font *font, char *fname, original_array *a, int size);
SDL_Renderer* original_init(char* title, original_array *a, int w, int h);
int original_clean(original_array *a);
int original_window_new(original_window *window, original_window *wdw, int decorate,
	int x, int y, int w, int h);
int original_window_event(original_window *window, SDL_Event *event, int *draw);
int original_window_draw(original_window *window, SDL_Renderer *renderer);
int original_label_new(original_label *label, original_window *wdw, char *text,
	int x, int y);
int original_label_draw(original_label *label, SDL_Renderer *renderer);
int original_button_new(original_button *button, original_window *wdw, char *text,
	int x, int y);
int original_button_event(original_button *button, SDL_Event *event, int *draw);
int original_button_draw(original_button *button, SDL_Renderer *renderer);
int original_selectbutton_new(original_selectbutton *selectbutton, original_window *wdw,
	int x, int y);
int original_selectbutton_event(original_selectbutton *selectbutton,
	SDL_Event *event, int *draw);
int original_selectbutton_draw(original_selectbutton *selectbutton,
	SDL_Renderer *renderer);
int original_vscrollbar_new(original_vscrollbar *vscrollbar, original_window *wdw,
	int x, int y, int h);
int original_vscrollbar_event(original_vscrollbar *vscrollbar, SDL_Event *event,
	int *draw);
int original_vscrollbar_draw(original_vscrollbar *vscrollbar,
	SDL_Renderer *renderer);
int original_hscrollbar_new(original_hscrollbar *hscrollbar, original_window *wdw,
	int x, int y, int w);
int original_hscrollbar_event(original_hscrollbar *hscrollbar, SDL_Event *event,
	int *draw);
int original_hscrollbar_draw(original_hscrollbar *hscrollbar,
	SDL_Renderer *renderer);
int original_progressbar_new(original_progressbar *progressbar, original_window *wdw,
	int x, int y, int w);
int original_progressbar_event(original_progressbar *progressbar, SDL_Event *event,
	int *draw);
int original_progressbar_draw(original_progressbar *progressbar,
	SDL_Renderer *renderer);
int original_entry_new(original_entry *entry, original_window *wdw, int decorate,
	char *text, int x, int y, int w);
int original_entry_event(original_entry *entry, SDL_Event *event, int *draw);
int original_entry_draw(original_entry *entry, SDL_Renderer *renderer);
int original_textbox_new(original_textbox *textbox, original_window *wdw, int decorate,
	original_array *a, int x, int y, int w, int h);
int original_textbox_event(original_textbox *textbox, SDL_Event *event, int *draw);
int original_textbox_draw(original_textbox *textbox, SDL_Renderer *renderer);
int original_combobox_new(original_combobox *combobox, original_window *wdw,
	char *text, original_array *a, int x, int y, int w, int h);
int original_combobox_event(original_combobox *combobox, SDL_Event *event,
	int *draw);
int original_combobox_draw(original_combobox *combobox, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif /* _original_sdl_h */

