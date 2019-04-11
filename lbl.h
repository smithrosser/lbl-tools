#ifndef LBL_H
#define LBL_H

// legacy
#define END 0
#define FILL 1
#define WASH 2
#define DRY 3

#define STAGE_FILL	0
#define STAGE_WASH	1
#define STAGE_DRY	2

class Stage {
private:
    int type;
    int pump;
    int dur;

public:
    Stage();
    Stage( int _type, int _dur, int _pump );
    Stage( int _type, int _dur );
    Stage( const Stage& s );
    ~Stage();
    void initStage();

    int getType();
    int getPump();
    int getDur();
    void setType( int _type );
    void setPump( int _pump );
    void setDur( int _dur );
};

#endif // LBL_H
