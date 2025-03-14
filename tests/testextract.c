/* testfs.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <ncurses.h>
#include "../src/guiextract.h"

int main(int argc, char *argv[]) {
    // Since this is isolated let's setup ncurses here
    initscr();              // Init screen
    start_color();          // Enable colors
    noecho();               // Don't echo inputs
    cbreak();               // Disables line buffering and erase/kill character-processing 
    curs_set(0);            // Disable cursor
    keypad(stdscr, TRUE);   // Enable keyboard input
    refresh();

    guiExtractInit();

    while(1) {
        int ch = getch();
        if(ch == 'q') break;
    }

    guiExtractClean();
    endwin();
}