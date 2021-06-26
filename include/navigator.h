#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <Nextion.h>
#include "entity.h"

struct Item {
    int itemNumber, entityId ;
    char *title ;
    int32 iconOff, iconOn ;
    
    bool gauge, alarm ;
    char *format ;
    double minValue, maxValue ;

    Item() {
        itemNumber = -1 ;
        this->entityId = -1 ;
        this->iconOff = -1 ;
        this->iconOn = -1 ;
        title = NULL ;
        gauge = false ;
        alarm = false ;
        format = (char *)"%3.0f" ;
        minValue = 0 ;
        maxValue = 100 ;
    }
} ;

class Navigator {
    public:
        Navigator() ;
        void setup() ;
        void loop() ;
        void setBootProgress(uint32) ;
        void showHomePage() ;
        void navigate(int) ;
        void subnavigate(int) ;
        void onEntityChanged(Entity *) ;
        void itemDidClick(int itemNo, bool forceChange = false) ;
        void closePage() ;
        void changeBrightness(int itemNo, int step) ;
        void changeAlarm(int itemNo, int state) ;

    private:
        Nextion *nextion ;
        NexPage *pageBoot, *pageHome, *pageLight, *pageAlarm ;
        NexProgressBar *pbBoot, *pbLight ;
        NexButton *bhome, *bcab, *bhall, *bcorr, *bbath, *bkitchen, *blady,
                  *bsn1, *bsn2, *bsn3,
                  *b0, *b1, *b2, *b3, *b4, *b5,
                  *blon, *blclose, *blp, *blm,
                  *baclose, *bahome, *badisarm, *baaway, *banight ;
        NexGauge *z0, *z1, *z2, *z3, *z4, *z5 ;
        NexText  *t0, *t1, *t2, *t3, *t4, *t5 ;
        NexTouch **ntlist ;
        int currentPage = -1, currentSubPage = -1,
            prevPage = -1, prevSubPage = -1 ;
        Item items[7][3][6] ;

        void switchitem(int pageNumber, int subpageNumber, int itemNumber, int entityId, const char *title, int iconOff, int iconOn) ;
        void gaugeitem(int pageNumber, int subpageNumber, int itemNumber, int entityId, const char *title, const char *format, double minValue, double maxValue, int icon = 42) ;
        void alarmitem(int pageNumber, int subpageNumber, int itemNumber, int entityId, const char *title) ;
        void showLightPage(Item) ;
        void showAlarmPage(Item) ;
} ;



#endif
