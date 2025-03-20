/* guisettings.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <ncurses.h>
#include "guisettings.h"
#include "config.h"

WINDOW *settingsWin;
void guiSettingsInit() {
    int row, col;
    getmaxyx(stdscr, row, col);

    settingsWin = newwin(row - 6, col - 4, 2, 3);
    box(settingsWin, 0, 0);
    mvwprintw(settingsWin, 0, 1, " Enter to save | Backspace to go back");
    
    int idx = 0;
    mvwprintw(settingsWin, ++idx, 2, "Auto Check: %s", g_cfg.autoCheck);
    mvwprintw(settingsWin, ++idx, 2, "Pattern:    %s", g_cfg.format);
    mvwprintw(settingsWin, ++idx, 2, "No Gui:     [%c]", g_cfg.noGui ? 'x' : ' ');
    mvwprintw(settingsWin, ++idx, 2, "Quiet:      [%c]", g_cfg.quiet ? 'x' : ' ');
    mvwprintw(settingsWin, ++idx, 2, "Check all:  [%c]", g_cfg.autoCheckAll ? 'x' : ' ');
    mvwprintw(settingsWin, ++idx, 2, "Fast Gui:   [%c]", g_cfg.fastUpdate ? 'x' : ' ');
    wrefresh(settingsWin);
}

void guiSettingsUpdate() {}

void guiSettingsClean() {
    if(settingsWin != NULL) {
        wclear(settingsWin);
        delwin(settingsWin);
        settingsWin = NULL;
    }
}
