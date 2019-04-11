#include "parser.h"

int parseStageFromFile( QString str, Stage& s ) {

    QRegExp rxSpace("(\\ )");
    QStringList tokens = str.split(rxSpace);
    qDebug() << tokens;

    if( tokens.size() > 3 )		// catches commands that are too long
        return PARSE_ERROR;

    if( parseType(tokens.at(0), s) != PARSE_SUCCESS )	return PARSE_ERROR;
    if( parseParam(tokens.at(1), s) != PARSE_SUCCESS ) return PARSE_ERROR;
    if( s.getType() == STAGE_FILL )
        if( parseParam(tokens.at(2), s) != PARSE_SUCCESS ) return PARSE_ERROR;

    return PARSE_SUCCESS;
}

int parseType( QString typeStr, Stage& s ) {

    if		( typeStr.compare("fill", Qt::CaseInsensitive) == 0 )  s.setType(STAGE_FILL);
    else if ( typeStr.compare("wash", Qt::CaseInsensitive) == 0 )  s.setType(STAGE_WASH);
    else if ( typeStr.compare("dry", Qt::CaseInsensitive) == 0 )   s.setType(STAGE_DRY);
    else return PARSE_ERROR;

    return PARSE_SUCCESS;
}

int parseParam( QString paramStr, Stage& s ) {

    QRegExp rxColon("(\\:)"), rxNumeric("\\d*");
    QStringList paramTokens = paramStr.split(rxColon);

    if( paramTokens.size() > 2 )		// no more than 2 key:value pairs
        return PARSE_ERROR;
    else if ( !rxNumeric.exactMatch(paramTokens.at(1)) )	// literal after : must be an int
        return PARSE_ERROR;

    if ( paramTokens.at(0).compare("p", Qt::CaseInsensitive) == 0 && s.getType() == STAGE_FILL )
        s.setPump( paramTokens.at(1).toInt() );
    else if ( paramTokens.at(0).compare("t", Qt::CaseInsensitive) == 0 )
        s.setDur( paramTokens.at(1).toInt() );
    else
        return PARSE_ERROR;

    return PARSE_SUCCESS;
}
