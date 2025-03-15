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
int totalProg = 0;
int lastProgUpdate = 0;
int r = 0;
int c = 0;
int ln = 0;
int totalTracksToExtract = 0;

void guiExtractInit(FileList *fl) {
    int row, col;
    getmaxyx(stdscr, row, col);
    r = row;
    c = col;
    totalProg = 0;
    totalTracksToExtract = 0;
    lastProgUpdate = 0;

    extractWin = newwin(row - 6, col - 4, 2, 3);
    box(extractWin, 0, 0);
    mvwprintw(extractWin, 0, 1, " Extracting ESC to abort | Backspace to go back ");
    wrefresh(extractWin);

    extractWinPad = newpad(100, col - 4);
    guiExtractUpdate(fl, 0);
    
    for(int f = 0 ; f < (int)fl->size ; f++) {
        for(int i = 0 ; i < fl->files[f].trackCount ; i++) {
            if(!fl->files[f].tracks[i].Extract) continue;
            totalTracksToExtract++;
        }
    }
}

void drawProgressBarAsci(int y, int x, int width, int progress) {
    if (width < 4) width = 4;
    if (width <= 0 || progress < 0 || progress > 100) return;
    lastProgUpdate = progress;
    char progressBar[width + 1];
    memset(progressBar, ' ', width - 2);
    progressBar[width - 2] = '\0';
    int filled = (progress * (width - 2)) / 100;
    if (filled > width - 2) filled = width - 2;
    memset(progressBar, '=', filled);
    char percentageText[10];
    snprintf(percentageText, sizeof(percentageText), "%3d%%", progress);
    int percentagePos = (width - 2 - strlen(percentageText)) / 2;
    if (percentagePos < 0) percentagePos = 0;
    memcpy(progressBar + percentagePos, percentageText, strlen(percentageText));
    mvwprintw(extractWin, y, x, "[%s]", progressBar);
    wrefresh(extractWin);
}
void drawProgressBar(int y, int x, int width, int progress) {
    if (width < 2) width = 2;
    if (width <= 0 || progress < 0 || progress > 100) return;

    int filled = (progress * (width - 2)) / 100;
    if (filled > width - 2) filled = width - 2;

    wattron(extractWin, COLOR_PAIR(5));
    for (int i = 0; i < filled; i++) {
        mvwaddch(extractWin, y, x + 1 + i, ' ');
    }
    wattroff(extractWin, COLOR_PAIR(5));

    wattron(extractWin, COLOR_PAIR(6));
    for (int i = filled; i < width - 2; i++) {
        mvwaddch(extractWin, y, x + 1 + i, ' ');
    }
    wattroff(extractWin, COLOR_PAIR(6));

    char percentageText[10];
    snprintf(percentageText, sizeof(percentageText), "%3d%%", progress);
    int percentageLength = strlen(percentageText);
    int percentagePos = (width - 2 - percentageLength) / 2;

    if (percentagePos < filled) {
        wattron(extractWin, COLOR_PAIR(5));
        wattron(extractWin, A_BOLD);
    } else {
        wattron(extractWin, COLOR_PAIR(6));
        wattron(extractWin, A_BOLD);
    }

    mvwprintw(extractWin, y, x + 1 + percentagePos, "%s", percentageText);

    wattroff(extractWin, A_BOLD);
    wattroff(extractWin, COLOR_PAIR(5));
    wattroff(extractWin, COLOR_PAIR(6));

    wrefresh(extractWin);
}



void guiExtractUpdate(FileList *fl, const int aborted) {
    totalProg = 0;
    int row, col;
    getmaxyx(stdscr, row, col);
    // TODO move this elsewhere
    ln = 0;
    for(int i = 0 ; i < fl->size ; i++) {
        FileInfo *fi = &fl->files[i];
        for(int t = 0 ; t < fi->trackCount ; t++) {
            Track track = fi->tracks[t];
            if(strlen(track.NewName) > ln) ln = strlen(track.NewName);
        }
    }

    for(int i = 0 ; i < fl->size ; i++) {
        FileInfo *fi = &fl->files[i];
        int idx = 0;
        for(int ii = 0 ; ii < fi->trackCount ; ii++) {
            Track track = fi->tracks[ii];
            if(!track.Extract) continue;
            totalProg += track.ExtractProgress;
            if(track.ExtractProgress > 0 && track.ExtractProgress < 100) {
                wattron(extractWinPad, A_BLINK | A_BOLD);
            } else if(track.ExtractProgress <= 0) {
                wattron(extractWinPad, A_DIM);
            } else {
                wattron(extractWinPad, A_BOLD);
            }
            mvwprintw(extractWinPad, idx, 1, "%s %*s | %3d%c", 
                track.NewName, 
                ln - (int)strlen(track.NewName) + 1, " ",
                track.ExtractProgress, '%');
            wattroff(extractWinPad, A_BLINK | A_BOLD | A_DIM);
            idx++;
        }
    }

    prefresh(extractWinPad, extractPadPos, 0, 3, 4, row - 6, col - 6);
    drawProgressBar(r - 8, 2, c - 9, totalProg);
    wrefresh(extractWin);
}

void guiExtractUpdateAt(const int at, FileInfo *fi, Track *track, const int aborted) {
    if(totalTracksToExtract <= 0) return; // This should never be called with 0 tracks but might as well.

    // TODO maybe a better way to update total progress.
    totalProg = 0;
    for(int i = 0 ; i < fi->trackCount ; i++) {
        if(!fi->tracks[i].Extract) continue;
        totalProg += fi->tracks[i].ExtractProgress;
    }
    totalProg /= totalTracksToExtract;

    if(track->ExtractProgress > 0 && track->ExtractProgress < 100) {
        wattron(extractWinPad, A_BLINK | A_BOLD);
    } else if(track->ExtractProgress <= 0) {
        wattron(extractWinPad, A_DIM);
    } else {
        wattron(extractWinPad, A_BOLD);
    }
    if(aborted == 0 || track->ExtractProgress >= 100) {
        mvwprintw(extractWinPad, at, 1, "%s %*s | %3d%c", 
            track->NewName, 
            ln - (int)strlen(track->NewName) + 1, " ",
            track->ExtractProgress, '%');
    } else {
        mvwprintw(extractWinPad, at, 1, "%s %*s | %3d%c - ABORTED", 
            track->NewName, 
            ln - (int)strlen(track->NewName) + 1, " ",
            track->ExtractProgress, '%');
    }
    wattroff(extractWinPad, A_BLINK | A_BOLD | A_DIM);
    prefresh(extractWinPad, extractPadPos, 0, 3, 4, r - 7, c - 6);

    // Update total progress only when there's enough progress
    if(totalProg >= 100 || totalProg >= lastProgUpdate + 15) {
        drawProgressBar(r - 8, 2, c - 9, totalProg);
        wrefresh(extractWin);
    }
}

void guiExtractClean() {
    if(extractWinPad != NULL) {
        wclear(extractWinPad);
        delwin(extractWinPad);
        extractWinPad = NULL;
    }
    if(extractWin != NULL) {
        wclear(extractWin);
        delwin(extractWin);
        extractWin = NULL;
    }
    totalProg = 0;
    totalTracksToExtract = 0;
    lastProgUpdate = 0;
}