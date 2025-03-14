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
int r;
int c;
int ln;

void guiExtractInit(FileList *fl) {
    int row, col;
    getmaxyx(stdscr, row, col);
    r = row;
    c = col;
    extractWin = newwin(row - 6, col - 4, 2, 3);
    box(extractWin, 0, 0);
    mvwprintw(extractWin, 0, 1, " Extracting ESC to abort ");
    wrefresh(extractWin);

    extractWinPad = newpad(100, col - 4);
    guiExtractUpdate(fl);
}

void guiExtractUpdate(FileList *fl) {
    int row, col;
    getmaxyx(stdscr, row, col);
    // TODO move this elsewhere
    ln = 0;
    for(int i = 0 ; i < fl->size ; i++) {
        FileInfo *fi = &fl->files[i];
        for(int t = 0 ; t < fi->trackCount ; t++) {
            Track track = fi->tracks[t];
            char newName[256];
            trackResolveNewName(fi->name, &track, newName);
            if(strlen(newName) > ln) ln = strlen(newName);
        }
    }

    for(int i = 0 ; i < fl->size ; i++) {
        FileInfo *fi = &fl->files[i];
        int idx = 0;
        for(int ii = 0 ; ii < fi->trackCount ; ii++) {
            Track track = fi->tracks[ii];
            if(!track.Extract) continue;
            char newName[256];
            trackResolveNewName(fi->name, &track, newName);
            if(track.ExtractProgress > 0 && track.ExtractProgress < 100) {
                wattron(extractWinPad, A_BLINK | A_BOLD);
            } else if(track.ExtractProgress <= 0) {
                wattron(extractWinPad, A_DIM);
            } else {
                wattron(extractWinPad, A_BOLD);
            }
            mvwprintw(extractWinPad, idx, 1, "%s %*s | %3d%c", 
                newName, 
                ln - (int)strlen(newName) + 1, " ",
                track.ExtractProgress, '%');
            wattroff(extractWinPad, A_BLINK | A_BOLD | A_DIM);
            idx++;
        }
    }

    prefresh(extractWinPad, extractPadPos, 0, 3, 4, row - 6, col - 6);
}

void guiExtractUpdateAt(const int at, FileInfo *fi, Track *track) {
    char newName[256];
    trackResolveNewName(fi->name, &track, newName);
    if(track->ExtractProgress > 0 && track->ExtractProgress < 100) {
        wattron(extractWinPad, A_BLINK | A_BOLD);
    } else if(track->ExtractProgress <= 0) {
        wattron(extractWinPad, A_DIM);
    } else {
        wattron(extractWinPad, A_BOLD);
    }
    mvwprintw(extractWinPad, at, 1, "%s %*s | %3d%c", 
        newName, 
        ln - (int)strlen(newName) + 1, " ",
        track->ExtractProgress, '%');
    wattroff(extractWinPad, A_BLINK | A_BOLD | A_DIM);
    prefresh(extractWinPad, extractPadPos, 0, 3, 4, r - 6, c - 6);
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