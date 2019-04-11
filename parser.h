#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include "lbl.h"

#define PARSE_SUCCESS 0
#define PARSE_ERROR 1

int parseStageFromFile ( QString str, Stage& s );
int parseType( QString str, Stage& s );
int parseParam( QString paramStr, Stage& s );

bool isInteger( QString str );

#endif // PARSER_H
