#include "OriginalDecryptor.h"

static void text_reset(original_textbox *textbox, original_vscrollbar *vscrollbar)
{
    qsort(textbox->array->data, textbox->array->length, sizeof(void *),
          original_string_compare);
    vscrollbar->step = 0.;
    if (textbox->array->length - textbox->maxlines > 0)
        vscrollbar->step = 1. / (textbox->array->length -
                                 textbox->maxlines);
    textbox->firstline = 0;
    textbox->highlightline = -1;
    vscrollbar->fraction = 0.;
}

/* Read directory entries into the textboxes */
static void dirent_read(original_textbox *textbox1, original_vscrollbar *vscrollbar1,
                        original_textbox *textbox2,	original_vscrollbar *vscrollbar2,
                        original_label *label_sel)
{
    original_dirent *ent;
    original_stat s;
    original_dir *dir;
    char buf[original_MAX_LENGTH], ending[2];

    original_array_free(textbox1->array);
    original_array_free(textbox2->array);
    original_array_new(textbox1->array);
    original_array_new(textbox2->array);
    original_getcwd(buf, original_MAX_LENGTH);
    strcpy(ending, "/");
    if (buf[0] == 'C') strcpy(ending, "\\");
    if (!strcmp(buf, "/") || !strcmp(buf, "C:\\")) strcpy(ending, "");
    original_string_copy(label_sel->text, (2 * textbox1->rect.w +
                                       2 * original_up.w) / original_textfont.advance, buf, ending);
#ifdef _MSC_VER
    dir = original_opendir("*");
#else
    dir = original_opendir(".");
#endif
    while ((ent = original_readdir(dir))) {
        if (!ent->d_name) continue;
        original_getstat(ent->d_name, &s);
        if (original_isdir(s))
            original_array_appendstring(textbox1->array, 0,
                                    ent->d_name, "/");
        else if (original_isreg(s))
            original_array_appendstring(textbox2->array, 0,
                                    ent->d_name, NULL);
    }
    original_closedir(dir);
    text_reset(textbox1, vscrollbar1);
    text_reset(textbox2, vscrollbar2);
}

/* The widget arguments are widgets that this widget talks with */
static void textbox1_event(original_textbox *textbox, SDL_Event *e,
                           original_vscrollbar *vscrollbar1, original_textbox *textbox2,
                           original_vscrollbar *vscrollbar2, original_label *label_sel, int *draw)
{
    int index;

    if (original_textbox_event(textbox, e, draw)) {
        index = textbox->firstline + textbox->selectedline;
        if (strcmp((char *) original_array_data(textbox->array, index),
                   "")) {
            textbox->selectedline = -1;
            original_chdir((char *) original_array_data(textbox->array,
                                                index));
            dirent_read(textbox, vscrollbar1, textbox2,
                        vscrollbar2, label_sel);
            *draw = 1;
        }
    }
}

static void vscrollbar1_event(original_vscrollbar *vscrollbar, SDL_Event *e,
                              original_textbox *textbox1, int *draw)
{
    int firstline;

    if (original_vscrollbar_event(vscrollbar, e, draw) &&
        textbox1->array->length - textbox1->maxlines > 0) {
        firstline = (int) ((textbox1->array->length -
                            textbox1->maxlines) * vscrollbar->fraction + 0.5);
        if (firstline >= 0) textbox1->firstline = firstline;
        *draw = 1;
    }
}

static void textbox2_event(original_textbox *textbox, SDL_Event *e,
                           original_vscrollbar *vscrollbar2, original_entry *entry, int *draw)
{
    int index;

    if (original_textbox_event(textbox, e, draw)) {
        index = textbox->firstline + textbox->selectedline;
        if (strcmp((char *) original_array_data(textbox->array, index),
                   "")) {
            original_string_copy(entry->text,
                             entry->textwidth / original_textfont.advance,
                             (char *) original_array_data(textbox->array,
                                                      index), NULL);
            *draw = 1;
        }
    }
}

static void vscrollbar2_event(original_vscrollbar *vscrollbar, SDL_Event *e,
                              original_textbox *textbox2, int *draw)
{
    int firstline;

    if (original_vscrollbar_event(vscrollbar, e, draw) &&
        textbox2->array->length) {
        firstline = (int) ((textbox2->array->length -
                            textbox2->maxlines) * vscrollbar->fraction + 0.5);
        if (firstline >= 0) textbox2->firstline = firstline;
        *draw = 1;
    }
}

static void button_ok1_event(original_button *button, SDL_Event *e,
                             original_window *window1, original_window *window2, original_label *label_sel,
                             original_entry *entry, original_label *label_res,
                             original_progressbar *progressbar, int *draw)
{
    char buf[original_MAX_LENGTH];

    if (original_button_event(button, e, draw)) {
        original_string_copy(buf, original_maxlength(original_textfont,
                                             window2->rect.w - 2 * original_vslider.w,
                                             label_sel->text, entry->text),
                         label_sel->text, entry->text);
        original_string_copy(label_res->text, original_MAX_LABEL,
                         "The following path was selected:\n", buf);
        window2->visible = 1;
        window2->focus = 1;
        window1->focus = 0;
        button->prelight = 0;
        progressbar->fraction = 0.;
        progressbar->run = 1;
        *draw = 1;
    }
}

static void button_cancel_event(original_button *button, SDL_Event *e,
                                int *quit, int *draw)
{
    if (original_button_event(button, e, draw)) *quit = 1;
}

static void button_ok2_event(original_button *button, SDL_Event *e,
                             original_window *window1, original_window *window2,
                             original_progressbar *progressbar, int *draw)
{
    if (original_button_event(button, e, draw)) {
        window2->visible = 0;
        window1->focus = 1;
        button->prelight = 0;
        progressbar->fraction = 0.;
        progressbar->run = 0;
        *draw = 1;
    }
}

int main(int argc, char **argv)
{
    SDL_Renderer *renderer;
    SDL_Event e;
    original_array objects, a1, a2;
    original_window window1, window2;
    original_label label1 = {0}, label2 = {0}, label_sel = {0},
            label_res = {0};
    original_button button_ok1 = {0}, button_ok2 = {0}, button_cancel = {0};
    original_textbox textbox1 = {0}, textbox2 = {0};
    original_vscrollbar vscrollbar1 = {0}, vscrollbar2 = {0};
    original_progressbar progressbar = {0};
    original_entry entry = {0};
    int textbox_width, textbox_height, window2_width, window2_height,
            draw, quit;

    quit = 0;
    draw = 1;
    textbox_width = 250;
    textbox_height = 250;
    window2_width = 400;
    window2_height = 168;
    renderer = original_init("Original Decryptor", &objects, 640, 480);
    if (!renderer) return 1;
    original_array_new(&a1);
    original_array_append(&objects, ARRAY_TYPE, &a1);
    original_array_new(&a2);
    original_array_append(&objects, ARRAY_TYPE, &a2);

    /* Arrange the widgets nicely relative to each other */
    original_window_new(&window1, NULL, 1, 0, 0, original_screen_width,
                    original_screen_height);
    original_textbox_new(&textbox1, &window1, 1, &a1, original_screen_width / 2 -
                                                  (2 * textbox_width + 2 * original_up.w - original_edge) / 2,
                     3 * original_normal.h, textbox_width, textbox_height);
    original_vscrollbar_new(&vscrollbar1, &window1, textbox1.rect.x +
                                                textbox_width, textbox1.rect.y, textbox_height);
    original_textbox_new(&textbox2, &window1, 1, &a2,
                     vscrollbar1.uprect.x + original_up.w, textbox1.rect.y,
                     textbox_width, textbox_height);
    original_vscrollbar_new(&vscrollbar2, &window1, textbox2.rect.x +
                                                textbox_width, vscrollbar1.uprect.y, textbox_height);
    original_label_new(&label1, &window1, "Folders", textbox1.rect.x +
                                                 original_edge, textbox1.rect.y - original_textfont.lineheight);
    original_label_new(&label2, &window1, "Files", textbox2.rect.x +
                                               original_edge, textbox1.rect.y - original_textfont.lineheight);
    original_label_new(&label_sel, &window1, "", textbox1.rect.x +
                                             original_edge, textbox1.rect.y + textbox_height +
                                                        original_normal.h);
    original_entry_new(&entry, &window1, 1, "", textbox1.rect.x,
                   label_sel.rect.y + original_textfont.lineheight,
                   2 * textbox_width + 2 * original_up.w + original_edge);
    original_button_new(&button_cancel, &window1, "Cancel",
                    entry.rect.x + entry.rect.w - original_edge - original_normal.w,
                    entry.rect.y + entry.rect.h + original_normal.h);
    original_button_new(&button_ok1, &window1, "OK", button_cancel.rect.x -
                                                 2 * original_normal.w, button_cancel.rect.y);
    original_window_new(&window2, NULL, 1, original_screen_width / 2 -
                                       window2_width / 2, original_screen_height / 2 -
                                                          window2_height / 2, window2_width, window2_height);
    original_label_new(&label_res, &window2, "", window2.rect.x +
                                             original_up.w, window2.rect.y + original_vslider.h);
    label_res.textcolor = original_blue;
    original_progressbar_new(&progressbar, &window2, window2.rect.x +
                                                 original_up.w - original_edge, window2.rect.y + window2.rect.h / 2 -
                                                                        original_bar.h / 2 - original_border,
                         window2.rect.w - 2 * original_up.w + 2 * original_edge);
    original_button_new(&button_ok2, &window2, "OK", window2.rect.x +
                                                 window2.rect.w / 2 - original_normal.w / 2,
                    progressbar.rect.y + progressbar.rect.h +
                    2 * original_vslider.h);

    dirent_read(&textbox1, &vscrollbar1, &textbox2, &vscrollbar2,
                &label_sel);
    /* Do that, and all widgets associated with the window will show */
    window1.visible = 1;

    while (!quit) {

        /* Some code may be written here */

        SDL_Delay(10);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            original_window_event(&window2, &e, &draw);
            original_window_event(&window1, &e, &draw);
            textbox1_event(&textbox1, &e, &vscrollbar1,
                           &textbox2, &vscrollbar2, &label_sel, &draw);
            vscrollbar1_event(&vscrollbar1, &e, &textbox1,
                              &draw);
            textbox2_event(&textbox2, &e, &vscrollbar2, &entry,
                           &draw);
            vscrollbar2_event(&vscrollbar2, &e, &textbox2, &draw);
            button_ok1_event(&button_ok1, &e, &window1, &window2,
                             &label_sel, &entry, &label_res,	&progressbar,
                             &draw);
            button_cancel_event(&button_cancel, &e, &quit,
                                &draw);
            original_entry_event(&entry, &e, &draw);
            button_ok2_event(&button_ok2, &e, &window1, &window2,
                             &progressbar, &draw);
        }

        vscrollbar1_event(&vscrollbar1, NULL, &textbox1, &draw);
        vscrollbar2_event(&vscrollbar2, NULL, &textbox2, &draw);
        original_progressbar_event(&progressbar, NULL, &draw);

        if (!draw) continue;
        SDL_RenderClear(renderer);

        original_window_draw(&window1, renderer);
        original_label_draw(&label1, renderer);
        original_label_draw(&label2, renderer);
        original_textbox_draw(&textbox1, renderer);
        original_vscrollbar_draw(&vscrollbar1, renderer);
        original_textbox_draw(&textbox2, renderer);
        original_vscrollbar_draw(&vscrollbar2, renderer);
        original_label_draw(&label_sel, renderer);
        original_entry_draw(&entry, renderer);
        original_button_draw(&button_ok1, renderer);
        original_button_draw(&button_cancel, renderer);
        original_window_draw(&window2, renderer);
        original_label_draw(&label_res, renderer);
        original_progressbar_draw(&progressbar, renderer);
        original_button_draw(&button_ok2, renderer);

        SDL_RenderPresent(renderer);
        draw = 0;
    }
    original_clean(&objects);
    return 0;
}