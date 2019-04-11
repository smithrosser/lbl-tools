#include "lbl.h"

Stage::Stage() {
    initStage();
}
Stage::Stage( int _type, int _dur, int _pump ) {
    type = _type;
    pump = _pump;
    dur = _dur;
}
Stage::Stage( int _type, int _dur ) {
    initStage();
    type = _type;
    dur = _dur;
}
Stage::Stage( const Stage& s ) {
    type = s.type;
    pump = s.pump;
    dur = s.dur;
}
Stage::~Stage() { }

void Stage::initStage() {
    type = pump = dur = -1;
}
int Stage::getType() { return type; }
int Stage::getPump() { return pump; }
int Stage::getDur() { return dur; }
void Stage::setType( int _type ) { type = _type; }
void Stage::setPump( int _pump ) { pump = _pump; }
void Stage::setDur( int _dur ) { dur = _dur; }
