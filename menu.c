#include "menu.h"

void button_ok_event(original_button *button_ok, SDL_Event *e,
                            int *quit, int *draw)
{
    if (original_button_event(button_ok, e, draw)) *quit = 1;
}

int button_file_event(original_button *button_file, SDL_Event *e, int *draw){
    if (original_button_event(button_file, e, draw)) return 1;
    return 0;
}

int menu()
{
    SDL_Renderer *renderer;
    SDL_Event e;
    original_array objects;
    original_window window;
    original_label label1 = {0};
    original_button button_ok = {0};
    original_button button_file = {0};
    original_entry entry = {0};
    char stext[original_MAX_LENGTH];
    int entry_width;
    int first, draw, quit, i;

    quit = 0;
    draw = 1;
    first = 0;
    strcpy(stext, "");
    entry_width = 250;
    renderer = original_init("Original Decryptor", &objects, 640, 480);
    if (!renderer) return 1;

    /* Arrange the widgets nicely relative to each other */
    original_window_new(&window, NULL, 1, 0, 0, original_screen_width,original_screen_height);
    original_label_new(&label1, &window, "Key", 300, 175);
    original_entry_new(&entry, &window, 1, "",
                   original_screen_width / 2 - entry_width / 2 + original_edge,
                   200, entry_width);

    original_button_new(&button_ok, &window, "OK", 383, 269);
    printf("%d %d", button_ok.rect.x, button_ok.rect.y);
    original_button_new(&button_file, &window, "File",383, 239);

    window.visible = 1;

    while (!quit) {

        SDL_Delay(10);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;
            original_entry_event(&entry, &e, &draw);
            original_window_event(&window, &e, &draw);
            button_ok_event(&button_ok, &e, &quit, &draw);
            if(button_file_event(&button_file, &e, &draw)) {
                original_clean(&objects);
                return 1; //explorer
            }

        }

        if (!draw) continue;
        SDL_RenderClear(renderer);

        original_window_draw(&window, renderer);
        original_button_draw(&button_ok, renderer);
        original_button_draw(&button_file, renderer);
        original_entry_draw(&entry, renderer);
        original_label_draw(&label1, renderer);

        SDL_RenderPresent(renderer);
        draw = 0;
    }
    original_clean(&objects);
    return 0;
}
