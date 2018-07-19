#include "OriginalDecryptor.h"

int original_window_new(original_window *window, original_window *wdw, int decorate,
	int x, int y, int w, int h)
{
	if (!window) return -1;
	window->bg = original_white;
	original_makerect(&window->rect, x, y, w, h);
	window->decorate = decorate;
	window->visible = 0;
	window->focus = 1;
	window->wdw = wdw;
	return 0;
}

int original_window_event(original_window *window, SDL_Event *event, int *draw)
{
	if (!window || !window->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!window->focus && (!window->wdw ||
		(window->wdw && !window->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&window->rect))
		return 1;
	return 0;
}

int original_window_draw(original_window *window, SDL_Renderer *renderer)
{
	if (window && window->wdw) window->visible = window->wdw->visible;
	if (!window || !window->visible || !renderer) return 0;
	original_fillrect(renderer, &window->rect, window->bg);
	if (window->decorate)
		original_decorate(renderer, &window->rect, original_blue, original_edge);
	return 1;
}

int original_label_new(original_label *label, original_window *wdw, char *text,
	int x, int y)
{
	if (!label || !text) return -1;
	if (label->font.magic != original_MAGIC) label->font = original_textfont;
	label->textcolor = original_black;
	original_makerect(&label->rect, x, y, 0, 0);
	original_string_copy(label->text, original_MAX_LABEL, text, NULL);
	label->visible = 0;
	label->wdw = wdw;
	return 0;
}

int original_label_draw(original_label *label, SDL_Renderer *renderer)
{
	char buf[original_MAX_LABEL], *p;
	int len, y;

	if (label && label->wdw) label->visible = label->wdw->visible;
	if (!label || !label->visible || !renderer) return 0;
	y = label->rect.y + label->font.spacing / 2;
	len = strlen(label->text);
	if (len > original_MAX_LABEL - 2)
		label->text[len - 1] = '\n';
	else
		strcat(label->text, "\n");
	for (p = label->text; *p; p = strchr(p, '\n') + 1) {
		original_string_copy(buf, strcspn(p, "\n") + 1, p, NULL);
		original_rendertext(renderer, buf, label->rect.x, y,
			label->font, label->textcolor);
		y += label->font.lineheight;
	}
	label->text[len] = 0;
	return 1;
}

int original_button_new(original_button *button, original_window *wdw, char *text,
	int x, int y)
{
	if (!button || !text) return -1;
	if (button->font.magic != original_MAGIC) button->font = original_buttonfont;
	if (button->normalimg.magic != original_MAGIC)
		button->normalimg = original_normal;
	if (button->activeimg.magic != original_MAGIC)
		button->activeimg = original_active;
	if (button->prelightimg.magic != original_MAGIC)
		button->prelightimg = original_prelight;
	original_makerect(&button->rect, x, y, button->normalimg.w,
		button->normalimg.h);
	button->textcolor = original_white;
	original_string_copy(button->text, original_MAX_LENGTH, text, NULL);
	button->textx = x + button->normalimg.w / 2 -
		original_textwidth(button->font, text, NULL) / 2;
	button->texty = y + button->normalimg.h / 2 -
		button->font.fontheight / 2;
	button->active = 0;
	button->prelight = 0;
	button->visible = 0;
	button->focus = 0;
	button->wdw = wdw;
	return 0;
}

int original_button_event(original_button *button, SDL_Event *event, int *draw)
{
	if (!button || !button->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!button->focus && (!button->wdw ||
		(button->wdw && !button->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&button->rect)) {
		button->active = 1;
		*draw = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP &&
		original_pointinrect(event->button.x, event->button.y,
		&button->rect) && button->active) {
		button->active = 0;
		*draw = 1;
		return 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		original_pointinrect(event->motion.x, event->motion.y,
		&button->rect)) {
		button->prelight = 1;
		*draw = 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		!original_pointinrect(event->motion.x, event->motion.y,
		&button->rect)) {
		button->prelight = 0;
		*draw = 1;
		if (button->active) {
			button->active = 0;
			return 1;
		}
	}
	return 0;
}

int original_button_draw(original_button *button, SDL_Renderer *renderer)
{
	if (button && button->wdw) button->visible = button->wdw->visible;
	if (!button || !button->visible || !renderer) return 0;
	if (button->active)
		original_renderimage(renderer, button->activeimg, button->rect.x,
			button->rect.y, NULL);
	else if (button->prelight && !button->active)
		original_renderimage(renderer, button->prelightimg,
			button->rect.x, button->rect.y, NULL);
	else
		original_renderimage(renderer, button->normalimg, button->rect.x,
			button->rect.y, NULL);
	original_rendertext(renderer, button->text, button->textx, button->texty,
		button->font, button->textcolor);
	return 1;
}

int original_selectbutton_new(original_selectbutton *selectbutton, original_window *wdw,
	int x, int y)
{
	if (!selectbutton) return -1;
	if (selectbutton->selectedimg.magic != original_MAGIC)
		selectbutton->selectedimg = original_selected;
	if (selectbutton->unselectedimg.magic != original_MAGIC)
		selectbutton->unselectedimg = original_unselected;
	original_makerect(&selectbutton->rect, x, y, selectbutton->selectedimg.w,
		selectbutton->selectedimg.h);
	selectbutton->selected = 0;
	selectbutton->focus = 0;
	selectbutton->wdw = wdw;
	return 0;
}

int original_selectbutton_event(original_selectbutton *selectbutton,
	SDL_Event *event, int *draw)
{
	if (!selectbutton || !selectbutton->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!selectbutton->focus && (!selectbutton->wdw ||
		(selectbutton->wdw && !selectbutton->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&selectbutton->rect)) {
		selectbutton->selected ^= 1;
		*draw = 1;
		return 1;
	}
	return 0;
}

int original_selectbutton_draw(original_selectbutton *selectbutton,
	SDL_Renderer *renderer)
{
	if (selectbutton && selectbutton->wdw)
		selectbutton->visible = selectbutton->wdw->visible;
	if (!selectbutton || !selectbutton->visible || !renderer) return 0;
	if (selectbutton->selected)
		original_renderimage(renderer, selectbutton->selectedimg,
			selectbutton->rect.x, selectbutton->rect.y, NULL);
	else
		original_renderimage(renderer, selectbutton->unselectedimg,
			selectbutton->rect.x, selectbutton->rect.y, NULL);
	return 1;
}

int original_vscrollbar_new(original_vscrollbar *vscrollbar, original_window *wdw,
	int x, int y, int h)
{
	if (!vscrollbar) return -1;
	if (vscrollbar->up.magic != original_MAGIC)
		vscrollbar->up = original_up;
	if (vscrollbar->down.magic != original_MAGIC)
		vscrollbar->down = original_down;
	if (vscrollbar->vslider.magic != original_MAGIC)
		vscrollbar->vslider = original_vslider;
	if (vscrollbar->up.h + vscrollbar->down.h + 2 * original_edge +
		2 * original_slider_padding + vscrollbar->vslider.h > h)
		return -1;
	original_makerect(&vscrollbar->uprect, x, y + original_edge,
		vscrollbar->up.w, vscrollbar->up.h + original_slider_padding);
	original_makerect(&vscrollbar->downrect, x, y + h - vscrollbar->down.h -
		original_slider_padding - original_edge, vscrollbar->down.w,
		vscrollbar->down.h + original_slider_padding);
	original_makerect(&vscrollbar->sliderrect, x, y + vscrollbar->uprect.h +
		original_edge, vscrollbar->vslider.w, vscrollbar->vslider.h);
	vscrollbar->maxpos = h - 2 * original_slider_padding - 2 * original_edge -
		vscrollbar->up.h - vscrollbar->down.h -
		vscrollbar->vslider.h;
	vscrollbar->fraction = 0.;
	vscrollbar->step = 0.1;
	vscrollbar->upclicked = 0;
	vscrollbar->downclicked = 0;
	vscrollbar->sliderclicked = 0;
	vscrollbar->lasttick = 0;
	vscrollbar->visible = 0;
	vscrollbar->focus = 0;
	vscrollbar->wdw = wdw;
	return 0;
}

static void vnewpos(original_vscrollbar *vscrollbar, double step, int *draw)
{
	*draw = 1;
	vscrollbar->fraction += step;
	vscrollbar->lasttick = original_getticks();
	if (vscrollbar->fraction < -0.000001) vscrollbar->fraction = 0.;
	if (vscrollbar->fraction > 0.999999) vscrollbar->fraction = 1.;
	vscrollbar->sliderrect.y = vscrollbar->uprect.y +
		vscrollbar->uprect.h + (int) (vscrollbar->fraction *
		vscrollbar->maxpos + 0.5);
	if (vscrollbar->fraction > 0.000001 &&
		vscrollbar->fraction < 0.999999)
		return;
	vscrollbar->upclicked = 0;
	vscrollbar->downclicked = 0;
}

int original_vscrollbar_event(original_vscrollbar *vscrollbar, SDL_Event *event,
	int *draw)
{
	if (!vscrollbar || !vscrollbar->visible) return 0;
	if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) {
		vscrollbar->upclicked = 0;
		vscrollbar->downclicked = 0;
		vscrollbar->lasttick = 0;
	} else if (vscrollbar->upclicked && original_getticks() >
		vscrollbar->lasttick + original_click_interval) {
		vnewpos(vscrollbar, -vscrollbar->step, draw);
		return 1;
	} else if (vscrollbar->downclicked && original_getticks() >
		vscrollbar->lasttick + original_click_interval) {
		vnewpos(vscrollbar, vscrollbar->step, draw);
		return 1;
	}
	if (!event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!vscrollbar->focus && (!vscrollbar->wdw ||
		(vscrollbar->wdw && !vscrollbar->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&vscrollbar->uprect) && vscrollbar->step > 0.000001) {
		if (vscrollbar->fraction > 0.000001) {
			vscrollbar->upclicked = 1;
			if (vscrollbar->wdw) vscrollbar->wdw->focus = 0;
			vscrollbar->focus = 1;
		}
		vscrollbar->lasttick = original_getticks() -
			original_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&vscrollbar->downrect) && vscrollbar->step > 0.000001) {
		if (vscrollbar->fraction < 0.999999) {
			vscrollbar->downclicked = 1;
			if (vscrollbar->wdw) vscrollbar->wdw->focus = 0;
			vscrollbar->focus = 1;
		}
		vscrollbar->lasttick = original_getticks() -
			original_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&vscrollbar->sliderrect) && vscrollbar->step > 0.000001) {
		if (vscrollbar->wdw) vscrollbar->wdw->focus = 0;
		vscrollbar->focus = 1;
		vscrollbar->sliderclicked = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		vscrollbar->upclicked = 0;
		vscrollbar->downclicked = 0;
		vscrollbar->lasttick = 0;
		if (vscrollbar->wdw) vscrollbar->wdw->focus = 1;
		vscrollbar->focus = 0;
		vscrollbar->sliderclicked = 0;
	} else if (event->type == SDL_MOUSEMOTION &&
		(event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
		vscrollbar->sliderclicked) {
		vnewpos(vscrollbar, 1. * event->motion.yrel /
			vscrollbar->maxpos, draw);
		return 1;
	}
	return 0;
}

int original_vscrollbar_draw(original_vscrollbar *vscrollbar, SDL_Renderer *renderer)
{
	if (vscrollbar && vscrollbar->wdw)
		vscrollbar->visible = vscrollbar->wdw->visible;
	if (!vscrollbar || !vscrollbar->visible || !renderer) return 0;
	vscrollbar->sliderrect.y = vscrollbar->uprect.y +
		vscrollbar->uprect.h + (int) (vscrollbar->fraction *
		vscrollbar->maxpos);
	original_renderimage(renderer, vscrollbar->up, vscrollbar->uprect.x,
		vscrollbar->uprect.y, NULL);
	original_renderimage(renderer, vscrollbar->down, vscrollbar->downrect.x,
		vscrollbar->downrect.y + original_slider_padding, NULL);
	original_renderimage(renderer, vscrollbar->vslider,
		vscrollbar->sliderrect.x, vscrollbar->sliderrect.y, NULL);
	return 1;
}

int original_hscrollbar_new(original_hscrollbar *hscrollbar, original_window *wdw,
	int x, int y, int w)
{
	if (!hscrollbar) return -1;
	if (hscrollbar->left.magic != original_MAGIC)
		hscrollbar->left = original_left;
	if (hscrollbar->right.magic != original_MAGIC)
		hscrollbar->right = original_right;
	if (hscrollbar->hslider.magic != original_MAGIC)
		hscrollbar->hslider = original_hslider;
	if (hscrollbar->left.w + hscrollbar->right.w + 2 * original_edge +
		2 * original_slider_padding + hscrollbar->hslider.w > w)
		return -1;
	original_makerect(&hscrollbar->leftrect, x + original_edge, y,
		hscrollbar->left.w + original_slider_padding,
		hscrollbar->left.h);
	original_makerect(&hscrollbar->rightrect, x + w - hscrollbar->right.w -
		original_slider_padding - original_edge, y,
		hscrollbar->right.w + original_slider_padding,
		hscrollbar->right.h);
	original_makerect(&hscrollbar->sliderrect, x + hscrollbar->leftrect.w +
		original_edge, y, hscrollbar->hslider.w, hscrollbar->hslider.h);
	hscrollbar->maxpos = w - 2 * original_slider_padding - 2 * original_edge -
		hscrollbar->left.w - hscrollbar->right.w - 
		hscrollbar->hslider.w;
	hscrollbar->fraction = 0.;
	hscrollbar->step = 0.1;
	hscrollbar->leftclicked = 0;
	hscrollbar->rightclicked = 0;
	hscrollbar->sliderclicked = 0;
	hscrollbar->lasttick = 0;
	hscrollbar->visible = 0;
	hscrollbar->focus = 0;
	hscrollbar->wdw = wdw;
	return 0;
}

static void hnewpos(original_hscrollbar *hscrollbar, double step, int *draw)
{
	*draw = 1;
	hscrollbar->fraction += step;
	hscrollbar->lasttick = original_getticks();
	if (hscrollbar->fraction < -0.000001) hscrollbar->fraction = 0.;
	if (hscrollbar->fraction > 0.999999) hscrollbar->fraction = 1.;
	hscrollbar->sliderrect.x = hscrollbar->leftrect.x +
		hscrollbar->leftrect.w + (int) (hscrollbar->fraction *
		hscrollbar->maxpos + 0.5);
	if (hscrollbar->fraction > 0.000001 &&
		hscrollbar->fraction < 0.999999)
		return;
	hscrollbar->leftclicked = 0;
	hscrollbar->rightclicked = 0;
}

int original_hscrollbar_event(original_hscrollbar *hscrollbar, SDL_Event *event,
	int *draw)
{
	if (!hscrollbar || !hscrollbar->visible) return 0;
	if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) {
		hscrollbar->leftclicked = 0;
		hscrollbar->rightclicked = 0;
		hscrollbar->lasttick = 0;
	} else if (hscrollbar->leftclicked && original_getticks() >
		hscrollbar->lasttick + original_click_interval) {
		hnewpos(hscrollbar, -hscrollbar->step, draw);
		return 1;
	} else if (hscrollbar->rightclicked && original_getticks() >
		hscrollbar->lasttick + original_click_interval) {
		hnewpos(hscrollbar, hscrollbar->step, draw);
		return 1;
	}
	if (!event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!hscrollbar->focus && (!hscrollbar->wdw ||
		(hscrollbar->wdw && !hscrollbar->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&hscrollbar->leftrect)) {
		if (hscrollbar->fraction > 0.000001) {
			hscrollbar->leftclicked = 1;
			if (hscrollbar->wdw) hscrollbar->wdw->focus = 0;
			hscrollbar->focus = 1;
		}
		hscrollbar->lasttick = original_getticks() -
			original_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&hscrollbar->rightrect) && hscrollbar->step > 0.000001) {
		if (hscrollbar->fraction < 0.999999) {
			hscrollbar->rightclicked = 1;
			if (hscrollbar->wdw) hscrollbar->wdw->focus = 0;
			hscrollbar->focus = 1;
		}
		hscrollbar->lasttick = original_getticks() -
			original_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&hscrollbar->sliderrect) && hscrollbar->step > 0.000001) {
		if (hscrollbar->wdw) hscrollbar->wdw->focus = 0;
		hscrollbar->focus = 1;
		hscrollbar->sliderclicked = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		hscrollbar->leftclicked = 0;
		hscrollbar->rightclicked = 0;
		hscrollbar->lasttick = 0;
		if (hscrollbar->wdw) hscrollbar->wdw->focus = 1;
		hscrollbar->focus = 0;
		hscrollbar->sliderclicked = 0;
	} else if (event->type == SDL_MOUSEMOTION &&
		(event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
		hscrollbar->sliderclicked) {
		hnewpos(hscrollbar, 1. * event->motion.xrel /
			hscrollbar->maxpos, draw);
		return 1;
	}
	return 0;
}

int original_hscrollbar_draw(original_hscrollbar *hscrollbar, SDL_Renderer *renderer)
{
	if (hscrollbar && hscrollbar->wdw)
		hscrollbar->visible = hscrollbar->wdw->visible;
	if (!hscrollbar || !hscrollbar->visible || !renderer) return 0;
	hscrollbar->sliderrect.x = hscrollbar->leftrect.x +
		hscrollbar->leftrect.w + (int) (hscrollbar->fraction *
		hscrollbar->maxpos);
	original_renderimage(renderer, hscrollbar->left, hscrollbar->leftrect.x,
		hscrollbar->leftrect.y, NULL);
	original_renderimage(renderer, hscrollbar->right,
		hscrollbar->rightrect.x + original_slider_padding,
		hscrollbar->rightrect.y, NULL);
	original_renderimage(renderer, hscrollbar->hslider,
		hscrollbar->sliderrect.x, hscrollbar->sliderrect.y, NULL);
	return 1;
}

int original_progressbar_new(original_progressbar *progressbar, original_window *wdw,
	int x, int y, int w)
{
	if (!progressbar || w < 2 * original_border + 1) return -1;
	if (progressbar->bar.magic != original_MAGIC)
		progressbar->bar = original_bar;
	progressbar->bg = original_white;
	original_makerect(&progressbar->rect, x, y, w,
		progressbar->bar.h + 2 * original_border);
	original_makerect(&progressbar->barrect, x + original_border,
		y + original_border, 0, progressbar->bar.h);
	progressbar->width = w - 2 * original_border;
	progressbar->fraction = 0.;
	progressbar->step = 0.02;
	progressbar->lasttick = 0;
	progressbar->run = 0;
	progressbar->visible = 0;
	progressbar->wdw = wdw;
	return 0;
}

int original_progressbar_event(original_progressbar *progressbar, SDL_Event *event,
	int *draw)
{
	if (!progressbar || !progressbar->visible) return 0;
	if (progressbar->run && original_getticks() > progressbar->lasttick +
		original_progress_interval) {
		progressbar->fraction += progressbar->step;
		if (progressbar->fraction > 0.999999) {
			progressbar->run = 0;
			progressbar->fraction = 1.;
		}
		progressbar->lasttick = original_getticks();
		*draw = 1;
	}
	return 1;
}

int original_progressbar_draw(original_progressbar *progressbar,
	SDL_Renderer *renderer)
{
	SDL_Rect clip;

	if (progressbar && progressbar->wdw)
		progressbar->visible = progressbar->wdw->visible;
	if (!progressbar || !progressbar->visible || !renderer)
		return 0;
	original_fillrect(renderer, &progressbar->rect, progressbar->bg);
	original_decorate(renderer, &progressbar->rect, original_blue, original_edge);
	progressbar->barrect.w = (int) (progressbar->width *
		progressbar->fraction + 0.5);
	original_makerect(&clip, 0, 0, progressbar->barrect.w,
		progressbar->barrect.h);
	original_renderimage(renderer, progressbar->bar, progressbar->barrect.x,
		progressbar->barrect.y, &clip);
	return 1;
}

int original_entry_new(original_entry *entry, original_window *wdw, int decorate,
	char *text, int x, int y, int w)
{
	if (!entry || !text) return -1;
	if (entry->font.magic != original_MAGIC) entry->font = original_textfont;
	if (w < 2 * original_border + entry->font.advance) return -1;
	entry->bg = original_white;
	entry->normalcolor = original_black;
	entry->activecolor = original_blue;
	entry->textwidth = w - 2 * original_border;
	original_string_copy(entry->text, original_maxlength(entry->font,
		entry->textwidth, text, NULL), text, NULL);
	original_makerect(&entry->rect, x, y, w, entry->font.fontheight +
		2 * original_border);
	entry->decorate = decorate;
	entry->textx = x + original_border;
	entry->texty = y + original_border;
	entry->active = 0;
	entry->visible = 0;
	entry->focus = 0;
	entry->wdw = wdw;
	return 0;
}

int original_entry_event(original_entry *entry, SDL_Event *event, int *draw)
{
	if (!entry || !entry->visible || !event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!entry->focus && (!entry->wdw ||
		(entry->wdw && !entry->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN && !entry->active &&
		original_pointinrect(event->button.x, event->button.y,
		&entry->rect)) {
		entry->active = 1;
		SDL_StartTextInput();
		if (entry->wdw) entry->wdw->focus = 0;
		entry->focus = 1;
		*draw = 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
		entry->active = 0;
		SDL_StopTextInput();
		if (entry->wdw) entry->wdw->focus = 1;
		entry->focus = 0;
		*draw = 1;
		return 1;
	} else if (event->type == SDL_TEXTINPUT && entry->active) {
		if (original_textwidth(entry->font, entry->text,
			event->text.text) < entry->textwidth &&
			strlen(entry->text) + strlen(event->text.text) <
			original_MAX_LENGTH)
			strcat(entry->text, event->text.text);
		*draw = 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
		original_backspace(entry->text);
		*draw = 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		(event->key.keysym.mod & KMOD_CTRL) &&
		event->key.keysym.scancode == SDL_SCANCODE_U) {
		strcpy(entry->text, "");
		*draw = 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN && entry->active &&
		original_pointinrect(event->button.x, event->button.y,
		&entry->rect)) {
		strcpy(entry->text, "");
		*draw = 1;
	}
	return 0;
}

int original_entry_draw(original_entry *entry, SDL_Renderer *renderer)
{
	SDL_Color color;

	if (entry && entry->wdw) entry->visible = entry->wdw->visible;
	if (!entry || !entry->visible || !renderer) return 0;
	original_fillrect(renderer, &entry->rect, entry->bg);
	color = original_blue;
	if (entry->active) color = original_green;
	if (entry->decorate)
		original_decorate(renderer, &entry->rect, color, original_edge);
	color = entry->normalcolor;
	if (entry->active) color = entry->activecolor;
	original_rendertext(renderer, entry->text, entry->textx, entry->texty,
		entry->font, color);
	return 1;
}

int original_textbox_new(original_textbox *textbox, original_window *wdw, int decorate,
	original_array *a, int x, int y, int w, int h)
{
	if (!textbox || !a) return -1;
	if (textbox->font.magic != original_MAGIC) textbox->font = original_textfont;
	if (h - 2 * original_border < textbox->font.lineheight) return -1;
	textbox->bg = original_white;
	textbox->textcolor = original_black;
	textbox->hlcolor = original_lightblue;
	original_makerect(&textbox->rect, x, y, w, h);
	original_makerect(&textbox->textrect, x + original_border,
		y + original_border, w - 2 * original_border, h - 2 * original_border);
	textbox->decorate = decorate;
	textbox->array = a;
	textbox->firstline = 0;
	textbox->maxlines = (h - 2 * original_border) / textbox->font.lineheight;
	textbox->textwidth = w - 2 * original_border;
	textbox->highlightline = -1;
	textbox->selectedline = -1;
	textbox->visible = 0;
	textbox->focus = 0;
	textbox->wdw = wdw;
	return 0;
}

static int textbox_numoflines(original_textbox *textbox)
{
	int numoflines;

	numoflines = textbox->maxlines;
	if (textbox->array->length - textbox->firstline < textbox->maxlines)
		numoflines = textbox->array->length - textbox->firstline;
	return numoflines;
}

int original_textbox_event(original_textbox *textbox, SDL_Event *event, int *draw)
{
	int texty, textmaxy, numoflines;

	if (!textbox || !textbox->visible || !event || !textbox->array ||
		!textbox->array->length)
		return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (!textbox->focus && (!textbox->wdw ||
		(textbox->wdw && !textbox->wdw->focus)))
		return 0;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&textbox->textrect)) {
		numoflines = textbox_numoflines(textbox);
		texty = event->button.y - textbox->textrect.y;
		textmaxy = numoflines * textbox->font.lineheight;
		if (texty < textmaxy) {
			textbox->selectedline =
				texty / textbox->font.lineheight;
			return 1;
		}
	} else if (event->type == SDL_MOUSEMOTION &&
		original_pointinrect(event->motion.x, event->motion.y,
		&textbox->textrect)) {
		numoflines = textbox_numoflines(textbox);
		texty = event->motion.y - textbox->textrect.y;
		textmaxy = numoflines * textbox->font.lineheight;
		textbox->highlightline = -1;
		if (texty < textmaxy)
			textbox->highlightline =
				texty / textbox->font.lineheight;
		*draw = 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		!original_pointinrect(event->motion.x, event->motion.y,
		&textbox->textrect)) {
		textbox->highlightline = -1;
		*draw = 1;
	}
	return 0;
}

int original_textbox_draw(original_textbox *textbox, SDL_Renderer *renderer)
{
	SDL_Rect highlightrect;
	char buf[original_MAX_LENGTH];
	int numoflines, i;

	if (textbox && textbox->wdw)
		textbox->visible = textbox->wdw->visible;
	if (!textbox || !textbox->visible || !renderer) return 0;
	original_fillrect(renderer, &textbox->rect, textbox->bg);
	if (textbox->decorate)
		original_decorate(renderer, &textbox->rect, original_blue,
			original_edge);
	if (textbox->highlightline >= 0) {
		original_makerect(&highlightrect, textbox->textrect.x,
			textbox->textrect.y +
			textbox->highlightline * textbox->font.lineheight,
			textbox->textrect.w, textbox->font.lineheight);
		original_fillrect(renderer, &highlightrect, textbox->hlcolor);
	}
	if (!textbox->array || !textbox->array->length) return 0;
	numoflines = textbox_numoflines(textbox);
	for (i = 0; i < numoflines; i++) {
		original_string_copy(buf, original_maxlength(textbox->font,
			textbox->textwidth,
			(char *) original_array_data(textbox->array,
			textbox->firstline + i), NULL),
			(char *) original_array_data(textbox->array,
			textbox->firstline + i), NULL);
		original_rendertext(renderer, buf, textbox->textrect.x,
			textbox->textrect.y + i * textbox->font.lineheight +
			textbox->font.spacing / 2, textbox->font,
			textbox->textcolor);
	}
	return 1;
}

int original_combobox_new(original_combobox *combobox, original_window *wdw,
	char *text, original_array *a, int x, int y, int w, int h)
{
	if (!combobox || !a || !text) return -1;
	if (combobox->combo.magic != original_MAGIC)
		combobox->combo = original_combo;
	original_entry_new(&combobox->entry, wdw, 1, text, x, y, w);
	strcpy(combobox->text, combobox->entry.text);
	original_window_new(&combobox->window, NULL, 0, x,
		y + combobox->entry.rect.h, w +
		combobox->vscrollbar.up.w, h);
	if (original_textbox_new(&combobox->textbox, &combobox->window, 1,
		a, x, y + combobox->entry.rect.h, w, h) == -1)
		return -1;
	if (original_vscrollbar_new(&combobox->vscrollbar, &combobox->window,
		x + combobox->textbox.rect.w, combobox->textbox.rect.y,
		combobox->textbox.rect.h) == -1)
		return -1;
	combobox->visible = 0;
	combobox->wdw = wdw;
	combobox->vscrollbar.step = 0.;
	if (combobox->textbox.array->length - combobox->textbox.maxlines > 0)
		combobox->vscrollbar.step = 1. /
			(combobox->textbox.array->length -
			combobox->textbox.maxlines);
	return 0;
}

int original_combobox_event(original_combobox *combobox, SDL_Event *event, int *draw)
{
	int firstline, index;

	if (!combobox || !combobox->visible) return 0;
	if (original_vscrollbar_event(&combobox->vscrollbar, event,
		draw) && combobox->textbox.array->length -
		combobox->textbox.maxlines >= 0) {
		combobox->vscrollbar.step = 0.;
		if (combobox->textbox.array->length -
			combobox->textbox.maxlines > 0)
			combobox->vscrollbar.step = 1. /
				(combobox->textbox.array->length -
				combobox->textbox.maxlines);
		firstline = (int) ((combobox->textbox.array->length - 
			combobox->textbox.maxlines) *
			combobox->vscrollbar.fraction + 0.5);
		if (firstline >= 0) combobox->textbox.firstline = firstline;
		*draw = 1;
	}
	if (!event) return 0;
	if (event->type == SDL_WINDOWEVENT &&
		event->window.event == SDL_WINDOWEVENT_EXPOSED)
		*draw = 1;
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		original_pointinrect(event->button.x, event->button.y,
		&combobox->entry.rect)) {
		combobox->window.visible = 1;
		*draw = 1;
	}
	if (original_entry_event(&combobox->entry, event, draw)) {
		combobox->window.visible = 0;
		strcpy(combobox->text, combobox->entry.text);
		*draw = 1;
		SDL_StopTextInput();
		return 1;
	} else if (original_textbox_event(&combobox->textbox, event, draw)) {
		combobox->window.visible = 0;
		combobox->entry.active = 0;
		if (combobox->entry.wdw) combobox->entry.wdw->focus = 1;
		combobox->entry.focus = 0;
		index = combobox->textbox.firstline +
			combobox->textbox.selectedline;
		original_string_copy(combobox->entry.text,
			original_maxlength(combobox->textbox.font,
			combobox->entry.textwidth,
			(char *) original_array_data(combobox->textbox.array,
			index), NULL),
			(char *) original_array_data(combobox->textbox.array,
			index), NULL);
		*draw = 1;
		SDL_StopTextInput();
		return 1;
	}
	return 0;
}

int original_combobox_draw(original_combobox *combobox, SDL_Renderer *renderer)
{
	if (combobox && combobox->wdw)
		combobox->visible = combobox->wdw->visible;
	if (!combobox || !combobox->visible || !renderer) return 0;
	original_renderimage(renderer, combobox->combo,
		combobox->entry.rect.x + combobox->entry.rect.w,
		combobox->entry.rect.y + combobox->entry.rect.h -
		combobox->combo.h - original_edge, NULL);
	original_entry_draw(&combobox->entry, renderer);
	original_window_draw(&combobox->window, renderer);
	original_vscrollbar_draw(&combobox->vscrollbar, renderer);
	original_textbox_draw(&combobox->textbox, renderer);
	return 1;
}

