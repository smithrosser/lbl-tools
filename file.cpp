#include "file.h"

int openSessionFile( QString fileName, QVector<Stage>& session ) {

    if( fileName.isEmpty() || fileName.isNull() )
        return OPEN_FILE_ERROR;

    QFile inputFile(fileName);
    if( inputFile.open(QIODevice::ReadOnly) ) {

        QTextStream in(&inputFile);
        QString line;
        Stage parsed;

        int i = 0;
        while( !in.atEnd() ) {
           i++;
           parsed.initStage();
           line = in.readLine();
           if( parseStageFromFile( line, parsed ) == PARSE_SUCCESS ) {
               session.push_back( Stage(parsed) );
           }
           else return i; // return line number of PARSE_ERROR
        }
    }
    inputFile.close();
    return OPEN_SUCCESS;
}

int saveSessionFile( QString fileName, QVector<Stage> session ) {

    if(fileName.isEmpty() && fileName.isNull())
         return SAVE_FILE_ERROR;

    QString line;
    QFile outputFile(fileName);

    if( !outputFile.open(QFile::WriteOnly | QFile::Text) )
        return SAVE_FILE_ERROR;

    outputFile.resize(0);
    QTextStream out(&outputFile);
    for(int i=0; i<session.size(); i++) {
        switch(session[i].getType()) {
        case STAGE_FILL : line = "fill p:" + QString::number(session[i].getPump()) + " t:" + QString::number(session[i].getDur()); break;
        case STAGE_WASH : line = "wash t:" + QString::number(session[i].getDur()); break;
        case STAGE_DRY :  line = "dry t:" + QString::number(session[i].getDur()); break;
        default : break;
        }
        out << line << endl;
    }
    outputFile.close();
    return SAVE_SUCCESS;
}
