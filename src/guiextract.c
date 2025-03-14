/* guiextract.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <ncurses.h>
#include "guiextract.h"


WINDOW *extractWin;

void guiExtractInit() {
    int row, col;
    getmaxyx(stdscr, row, col);
    extractWin = newwin(row - 6, col - 4, 2, 3);
    box(extractWin, 0, 0);
    mvwprintw(extractWin, 0, 1, " Extracting ESC to abort ");
    wrefresh(extractWin);
}
void guiExtractClean() {
    if(extractWin != NULL) {
        delwin(extractWin);
        extractWin= NULL;
    }
}