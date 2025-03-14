/* guiextract.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <ncurses.h>
#include "guiextract.h"


WINDOW *extractWin;
WINDOW *extractWinPad;
int extractPadPos = 0;

void guiExtractInit() {
    int row, col;
    getmaxyx(stdscr, row, col);
    extractWin = newwin(row - 6, col - 4, 2, 3);
    box(extractWin, 0, 0);
    mvwprintw(extractWin, 0, 1, " Extracting ESC to abort ");
    wrefresh(extractWin);

    extractWinPad = newpad(100, col - 4);
    for(int i = 0 ; i < 100 ; i++) {
        mvwprintw(extractWinPad, i, 1, "%s - %d", "Test", i);
    }

    prefresh(extractWinPad, extractPadPos, 0, 3, 4, row - 6, col - 6);

}
void guiExtractClean() {
    if(extractWinPad != NULL) {
        delwin(extractWinPad);
        extractWinPad = NULL;
    }
    if(extractWin != NULL) {
        delwin(extractWin);
        extractWin = NULL;
    }
}