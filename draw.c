#include "OriginalDecryptor.h"

original_font original_textfont, original_buttonfont;
original_image original_normal, original_prelight, original_active, original_bar,
	original_up, original_down, original_left, original_right, original_vslider,
	original_hslider, original_selected, original_unselected, original_combo;
int original_screen_width, original_screen_height;
int original_textfont_size = 15;
int original_buttonfont_size = 12;
int original_click_interval = 140;
int original_progress_interval = 50;
int original_slider_padding = 2;
int original_edge = 2;
int original_border = 6;
double original_spacing = 0.5;
SDL_Color original_white = {255, 255, 255, 255};
SDL_Color original_black = {0, 0, 0, 255};
SDL_Color original_green = {0, 150, 0, 255};
SDL_Color original_blue = {0, 0, 255, 255};
SDL_Color original_lightblue = {200, 225, 255, 255};

unsigned int original_getticks(void)
{
	return SDL_GetTicks();
}

/* Can be rewritten for proportional fonts */
int original_maxlength(original_font font, int width, char *str1, char *str2)
{
	char buf[original_MAX_LENGTH];
	int n, i;

	n = 0;
	if (!str1 && !str2) return -1;
	original_string_copy(buf, original_MAX_LENGTH, str1, str2);
	/* Maximum length + 1 for '\0', by the rule */
	for (i = 0; buf[i]; i += original_utf8next(buf, i))
		if (++n * font.advance > width)
			return i + 1;
	return i + 1;
}

/* Works also with proportional fonts */
int original_textwidth(original_font font, char *str1, char *str2)
{
	char buf[original_MAX_LENGTH];
	int width;

	if (!str1 && !str2) return -1;
	original_string_copy(buf, original_MAX_LENGTH, str1, str2);
	TTF_SizeUTF8(font.font, buf, &width, NULL);
	return width;
}

int original_renderimage(SDL_Renderer *renderer, original_image image,
	int x, int y, SDL_Rect *clip)
{
	SDL_Rect dst;

	if (!renderer || !image.image) return -1;
	original_makerect(&dst, x, y, image.w, image.h);
	if (clip) dst.w = clip->w;
	if (clip) dst.h = clip->h;
	SDL_RenderCopy(renderer, image.image, clip, &dst);
	return 0;
}

int original_rendertext(SDL_Renderer *renderer, char *text, int x, int y,
	original_font font, SDL_Color color)
{
	SDL_Surface *surface;
	original_image image;

	if (!text || !renderer || !font.font) return -1;
	surface = TTF_RenderUTF8_Blended(font.font, text, color);
	image.image = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(image.image, NULL, NULL, &image.w, &image.h);
	if (surface) SDL_FreeSurface(surface);
	original_renderimage(renderer, image, x, y, NULL);
	SDL_DestroyTexture(image.image);
	return 0;
}

int original_fillrect(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color)
{
	if (!renderer || !rect) return -1;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, rect);
	return 0;
}

int original_decorate(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color,
	int edge)
{
	SDL_Rect outlinerect;
	int d, i;

	d = 2 * edge;
	if (!renderer || !rect || rect->w < d + 6 || rect->h < d + 6)
		return -1;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (i = 0; i < 2; i++) {
		original_makerect(&outlinerect, rect->x + edge + i,
			rect->y + edge + i, rect->w - d - i - i,
			rect->h - d - i - i);
		SDL_RenderDrawRect(renderer, &outlinerect);
	}
	return 0;
}

int original_image_new(original_image *image, char *fname, original_array *a,
	SDL_Renderer* renderer)
{
	char buf[original_MAX_LENGTH];

	if (!image || !fname) return -1;
	original_string_copy(buf, original_MAX_LENGTH, RESDIR, fname);
	if (!(image->image = IMG_LoadTexture(renderer, buf))) {
		fprintf(stderr, "Cannot load image %s\n", fname);
		return -1;
	}
	if (a) original_array_append(a, TEXTURE_TYPE, image->image);
	SDL_QueryTexture(image->image, NULL, NULL, &image->w, &image->h);
	image->magic = original_MAGIC;
	return 0;
}

int original_font_new(original_font *font, char *fname, original_array *a, int size)
{
	char buf[original_MAX_LENGTH];

	if (!font || !fname) return -1;
	original_string_copy(buf, original_MAX_LENGTH, RESDIR, fname);
	if (!(font->font = TTF_OpenFont(buf, size))) {
		fprintf(stderr, "Cannot load font %s\n", fname);
		return -1;
	}
	if (a) original_array_append(a, FONT_TYPE, font->font);
	font->fontheight = TTF_FontHeight(font->font);
	font->spacing = (int) original_spacing * font->fontheight;
	font->lineheight = font->fontheight + font->spacing;
	font->ascent = TTF_FontAscent(font->font);
	TTF_GlyphMetrics(font->font, 'W', NULL, NULL, NULL, NULL,
		&(font->advance));
	font->magic = original_MAGIC;
	return 0;
}

SDL_Renderer* original_init(char* title, original_array *a, int w, int h)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Rect srect;
	int r;

	r = 0;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GetDisplayBounds(0, &srect);
	if (!a || w > srect.w || h > srect.h) {
		SDL_Quit();
		return NULL;
	}
	original_screen_width = w;
	original_screen_height = h;
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	original_array_new(a);
	window = SDL_CreateWindow(title, srect.w / 2 - w / 2,
		srect.h / 2 - h / 2, w, h, SDL_WINDOW_SHOWN);
	if (window) original_array_append(a, WINDOW_TYPE, window);
	renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer) original_array_append(a, RENDERER_TYPE, renderer);
	r += original_font_new(&original_textfont, "font.ttf", a,
		original_textfont_size);
	r += original_font_new(&original_buttonfont, "font.ttf", a,
		original_buttonfont_size);
	r += original_image_new(&original_normal, "normal.png", a, renderer);
	r += original_image_new(&original_prelight, "prelight.png", a, renderer);
	r += original_image_new(&original_active, "active.png", a, renderer);
	r += original_image_new(&original_bar, "bar.png", a, renderer);
	r += original_image_new(&original_vslider, "vslider.png", a, renderer);
	r += original_image_new(&original_hslider, "hslider.png", a, renderer);
	r += original_image_new(&original_up, "up.png", a, renderer);
	r += original_image_new(&original_down, "down.png", a, renderer);
	r += original_image_new(&original_left, "left.png", a, renderer);
	r += original_image_new(&original_right, "right.png", a, renderer);
	r += original_image_new(&original_combo, "combo.png", a, renderer);
	r += original_image_new(&original_selected, "selected.png", a, renderer);
	r += original_image_new(&original_unselected, "unselected.png", a,
		renderer);
	if (r) {
		original_clean(a);
		return NULL;
	}
	return renderer;
}

int original_clean(original_array *a)
{
	int i;

	if (!a) return -1;
	if (a->length)
		for (i = a->length - 1; i >= 0; i--) {
			if (original_array_id(a, i) == FONT_TYPE)
				TTF_CloseFont((TTF_Font *)
					original_array_data(a, i));
			else if (original_array_id(a, i) == TEXTURE_TYPE)
				SDL_DestroyTexture((SDL_Texture *)
					original_array_data(a, i));
			else if (original_array_id(a, i) == RENDERER_TYPE)
				SDL_DestroyRenderer((SDL_Renderer *)
					original_array_data(a, i));
			else if (original_array_id(a, i) == WINDOW_TYPE)
				SDL_DestroyWindow((SDL_Window *)
					original_array_data(a, i));
			else if (original_array_id(a, i) == ARRAY_TYPE)
				original_array_free((original_array *)
					original_array_data(a, i));
			else
				free (a->data[i]);
		}
	a->length = 0;
	original_array_free(a);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}
