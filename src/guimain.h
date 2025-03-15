/* guimain.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef GUIMAIN_H
#define GUIMAIN_H

#include "filesys.h"

typedef void (*ExtractFinished)();

/// @brief Initialize main gui
/// @param title  Title for main gui
/// @param fileList Filelist to display
void guiMainInit(const char *title, FileList *fileList);

/// @brief Gui cleanup. Deletes all windows and ends ncurses.
void guiMainClean();

/// @brief Main gui update
void guiMainUpdate();

/// @brief Sidebar update
void guiMainUpdateSideBar();

/// @brief Main area update
void guiMainUpdateMain();

/// @brief Select next/previous index in sidebar
/// @param dir Up = -1, Down = 1
void guiSidebarSelect(int dir);

/// @brief Select next/previous index in main area
/// @param dir Up = -1, Down = 1
void guiMainSelect(int dir);

/// @brief Get the current sidebar selected item index
/// @return Sidebar selected index
int guiGetSidebarIndex();

/// @brief Get the current main container selected item index
/// @return Main container selected index
int guiGetMainIndex();

/// @brief Check selected track
void guiMainCheck(int mode);

void guiMainExtract(ExtractFinished cb);
void guiMainAbortExtract(ExtractFinished cb);

void guiMainBackSpace();

#endif // GUIMAIN_H