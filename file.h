#ifndef FILE_H
#define FILE_H

#include "lbl.h"
#include "parser.h"
#include <QVector>
#include <QFile>
#include <QTextStream>

#define OPEN_SUCCESS 0
#define OPEN_FILE_ERROR -1

#define SAVE_SUCCESS 0
#define SAVE_FILE_ERROR -1

int openSessionFile( QString fileName, QVector<Stage>& session );
int saveSessionFile( QString fileName, QVector<Stage> session );

#endif // FILE_H
