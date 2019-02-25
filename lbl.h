#ifndef LBL_H
#define LBL_H

// Stage objects are one of three types:
// 1 = Fill, 2 = Wash, 3 = Dry, 0 = End (not implemented)

#define END 0
#define FILL 1
#define WASH 2
#define DRY 3

class Stage {
private:
    int type, dur, pump;
public:
    Stage() {
        type = -1;
        dur = -1;
        pump = -1;
    }
    Stage( int _type, int _dur ) {
        type = _type;
        dur = _dur;
        pump = -1;
    }
    Stage( int _type, int _dur, int _pump ) {
        type = _type;
        dur = _dur;
        pump = _pump;
    }
    ~Stage() { }

    int getType() { return type; }
    int getDur() { return dur; }
    int getPump() { return pump; }
    void setType( int _type ) { type = _type; }
    void setDur( int _dur ) { dur = _dur; }
    void setPump( int _pump ) { pump = _pump; }
};

#endif // LBL_H
