/* guiexctract.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef GUIEXTRACT_H
#define GUIEXTRACT_H

#include "filesys.h"

void guiExtractInit(FileList *fl);
void guiExtractClean();
void guiExtractUpdate(FileList *fl);

#endif // GUIEXTRACT_H