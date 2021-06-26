#include "navigator.h"

// #include <SoftwareSerial.h>
#include "debugger.h"

extern entity_get_t entity_get ;
extern entity_changed_t change_entity ;

// SoftwareSerial mySerial_D1(D2, D1) ; // RX, TX

enum ALARM_STATE {
    ALARM_STATE_NA = -1,
    ALARM_STATE_DISARMED = 0,
    ALARM_STATE_ARMED_HOME,
    ALARM_STATE_ARMED_AWAY,
    ALARM_STATE_ARMED_NIGHT,
    ALARM_STATE_PENDING,
    ALARM_STATE_TRIGGERED
}  ;

const char* ALARM_STATES[] = {
    "disarmed",
    "armed_home",
    "armed_away",
    "armed_night",
    "pending",
    "triggered"
} ;

const char* ALARM_COMMANDS[] = {
    "DISARM",
    "ARM_HOME",
    "ARM_AWAY",
    "ARM_NIGHT",
    "PENDING",
    "TRIGGER"
} ;

struct navigation_t {
    Navigator *navigator ;
    int navValue ;
    bool forceChange;
    int arg ;

    navigation_t(Navigator *navigator, int navValue, bool forceChange = false, int arg = -1) {
        this->navigator = navigator ;
        this->navValue = navValue ;
        this->forceChange = forceChange ;
        this->arg = arg ;
    }
} ;

void navigationCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->navigate(nt->navValue) ;
}

void subnavigationCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->subnavigate(nt->navValue) ;
}

void itemClickCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->itemDidClick(nt->navValue, nt->forceChange) ;
}

void pageCloseCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->closePage() ;
}

void brightnessPlusCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->changeBrightness(nt->navValue, 10) ;
}

void brightnessMinusCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->changeBrightness(nt->navValue, -10) ;
}

void alarmStateCallback(void *ptr) {
    navigation_t *nt = (navigation_t *)ptr ;
    nt->navigator->changeAlarm(nt->navValue, nt->arg) ;
}

navigation_t *blonCallback, *blpCallback , *blmCallback,
             *alarmHomeCallback, *alarmNightCallback, *alarmAwayCallback, *alarmDisarmCallback ;

Navigator::Navigator() {
    this->nextion = Nextion::GetInstance(Serial) ;
    
    this->pageBoot = new NexPage(this->nextion, 0, "efi") ;
    this->pbBoot = new NexProgressBar(this->nextion, 0, 2, "pbar", this->pageBoot) ;
    
    this->pageHome = new NexPage(this->nextion, 1, "home") ;

    this->bhome = new NexButton(this->nextion, 1, 9, "bhome", this->pageHome) ;
    this->bcab = new NexButton(this->nextion, 1, 11, "bcab", this->pageHome) ;
    this->bhall = new NexButton(this->nextion, 1, 12, "bhall", this->pageHome) ;
    this->bcorr = new NexButton(this->nextion, 1, 13, "bcorr", this->pageHome) ;
    this->bbath = new NexButton(this->nextion, 1, 14, "bbath", this->pageHome) ;
    this->bkitchen = new NexButton(this->nextion, 1, 15, "bkitchen", this->pageHome) ;
    this->blady = new NexButton(this->nextion, 1, 16, "blady", this->pageHome) ;

    this->bhome->attachPop(navigationCallback, new navigation_t(this, 0)) ;
    this->bcab->attachPop(navigationCallback, new navigation_t(this, 1)) ;
    this->bhall->attachPop(navigationCallback, new navigation_t(this, 2)) ;
    this->bcorr->attachPop(navigationCallback, new navigation_t(this, 3)) ;
    this->bbath->attachPop(navigationCallback, new navigation_t(this, 4)) ;
    this->bkitchen->attachPop(navigationCallback, new navigation_t(this, 5)) ;
    this->blady->attachPop(navigationCallback, new navigation_t(this, 6)) ;


    this->bsn1 = new NexButton(this->nextion, 1, 10, "bsn1", this->pageHome) ;
    this->bsn2 = new NexButton(this->nextion, 1, 17, "bsn2", this->pageHome) ;
    this->bsn3 = new NexButton(this->nextion, 1, 18, "bsn3", this->pageHome) ;

    this->bsn1->attachPop(subnavigationCallback, new navigation_t(this, 0)) ;
    this->bsn2->attachPop(subnavigationCallback, new navigation_t(this, 1)) ;
    this->bsn3->attachPop(subnavigationCallback, new navigation_t(this, 2)) ;

    this->b0 = new NexButton(this->nextion, 1, 19, "b0", this->pageHome) ;
    this->b1 = new NexButton(this->nextion, 1, 20, "b2", this->pageHome) ;
    this->b2 = new NexButton(this->nextion, 1, 21, "b4", this->pageHome) ;
    this->b3 = new NexButton(this->nextion, 1, 22, "b1", this->pageHome) ;
    this->b4 = new NexButton(this->nextion, 1, 23, "b3", this->pageHome) ;
    this->b5 = new NexButton(this->nextion, 1, 24, "b5", this->pageHome) ;

    this->b0->attachPop(itemClickCallback, new navigation_t(this, 0)) ;
    this->b1->attachPop(itemClickCallback, new navigation_t(this, 1)) ;
    this->b2->attachPop(itemClickCallback, new navigation_t(this, 2)) ;
    this->b3->attachPop(itemClickCallback, new navigation_t(this, 3)) ;
    this->b4->attachPop(itemClickCallback, new navigation_t(this, 4)) ;
    this->b5->attachPop(itemClickCallback, new navigation_t(this, 5)) ;

    this->z0 = new NexGauge(this->nextion, 1, 34, "z0", this->pageHome) ;
    this->z1 = new NexGauge(this->nextion, 1, 38, "z2", this->pageHome) ;
    this->z2 = new NexGauge(this->nextion, 1, 42, "z4", this->pageHome) ;
    this->z3 = new NexGauge(this->nextion, 1, 36, "z1", this->pageHome) ;
    this->z4 = new NexGauge(this->nextion, 1, 40, "z3", this->pageHome) ;
    this->z5 = new NexGauge(this->nextion, 1, 43, "z5", this->pageHome) ;

    this->t0 = new NexText(this->nextion, 1, 35, "t0", this->pageHome) ;
    this->t1 = new NexText(this->nextion, 1, 39, "t2", this->pageHome) ;
    this->t2 = new NexText(this->nextion, 1, 44, "t4", this->pageHome) ;
    this->t3 = new NexText(this->nextion, 1, 37, "t1", this->pageHome) ;
    this->t4 = new NexText(this->nextion, 1, 41, "t3", this->pageHome) ;
    this->t5 = new NexText(this->nextion, 1, 45, "t5", this->pageHome) ;

    this->pageLight = new NexPage(this->nextion, 2, "light") ;
    this->pbLight = new NexProgressBar(this->nextion, 2, 3, "pblight", this->pageLight) ;
    this->blon = new NexButton(this->nextion, 2, 4, "blon", this->pageLight) ;
    this->blclose = new NexButton(this->nextion, 2, 5, "blclose", this->pageLight) ;
    this->blp = new NexButton(this->nextion, 2, 1, "blp", this->pageLight) ;
    this->blm = new NexButton(this->nextion, 2, 2, "blm", this->pageLight) ;

    blonCallback = new navigation_t(this, -1, true) ;
    blpCallback = new navigation_t(this, -1, false) ;
    blmCallback = new navigation_t(this, -1, false) ;
    this->blclose->attachPop(pageCloseCallback, new navigation_t(this, 0)) ;


    this->pageAlarm = new NexPage(this->nextion, 3, "alarm") ;
    this->baclose = new NexButton(this->nextion, 3, 1, "baclose", this->pageAlarm) ;
    this->banight = new NexButton(this->nextion, 3, 2, "banight", this->pageAlarm) ;
    this->bahome = new NexButton(this->nextion, 3, 3, "bahome", this->pageAlarm) ;
    this->baaway = new NexButton(this->nextion, 3, 4, "baaway", this->pageAlarm) ;
    this->badisarm = new NexButton(this->nextion, 3, 5, "b2", this->pageAlarm) ;

    this->baclose->attachPop(pageCloseCallback, new navigation_t(this, 0)) ;
    alarmHomeCallback = new navigation_t(this, -1, false, ALARM_STATE_ARMED_HOME) ;
    alarmNightCallback = new navigation_t(this, -1, false, ALARM_STATE_ARMED_NIGHT) ;
    alarmAwayCallback = new navigation_t(this, -1, false, ALARM_STATE_ARMED_AWAY) ;
    alarmDisarmCallback = new navigation_t(this, -1, false, ALARM_STATE_DISARMED) ;

    this->ntlist = new NexTouch*[26] ;
    this->ntlist[0] = this->bhome ;
    this->ntlist[1] = this->bcab ;
    this->ntlist[2] = this->bhall ;
    this->ntlist[3] = this->bcorr ;
    this->ntlist[4] = this->bbath ;
    this->ntlist[5] = this->bkitchen ;
    this->ntlist[6] = this->blady ;

    this->ntlist[7] = this->bsn1 ;
    this->ntlist[8] = this->bsn2 ;
    this->ntlist[9] = this->bsn3 ;

    this->ntlist[10] = this->b0 ;
    this->ntlist[11] = this->b1 ;
    this->ntlist[12] = this->b2 ;
    this->ntlist[13] = this->b3 ;
    this->ntlist[14] = this->b4 ;
    this->ntlist[15] = this->b5 ;

    this->ntlist[16] = this->blclose ;
    this->ntlist[17] = this->blon ;
    this->ntlist[18] = this->blp ;
    this->ntlist[19] = this->blm ;

    this->ntlist[20] = this->baclose ;
    this->ntlist[21] = this->bahome ;
    this->ntlist[22] = this->baaway ;
    this->ntlist[23] = this->banight ;
    this->ntlist[24] = this->badisarm ;

    this->ntlist[25] = nullptr ;

    this->gaugeitem( 0, 0, 0, 21, "Temp, C", "%2.1f", 10, 40, 42) ;
    this->switchitem(0, 0, 1, 5, "Телевизор", 34, 35) ;
    this->switchitem(0, 0, 2, 11, "Прихожая", 18, 19) ;
    this->switchitem(0, 0, 3, 6, "Диван", 14, 15) ;
    this->switchitem(0, 0, 4, 8, "Малый свет", 26, 27) ;
    this->switchitem(0, 0, 5, 9, "Большой свет", 26, 27) ;

    this->switchitem(1, 0, 0, 1, "Верхний свет", 12, 13) ;
    this->switchitem(1, 0, 1, 2, "Настоль. лампа", 14, 15) ;
    this->switchitem(1, 0, 2, 3, "Розетка", 30, 31) ;
    this->switchitem(1, 0, 3, 4, "Стол", 20, 21) ;

    this->switchitem(2, 0, 0, 5, "Телевизор", 34, 35) ;
    this->switchitem(2, 0, 1, 6, "Диван", 14, 15) ;
    this->switchitem(2, 0, 2, 7, "Ночник", 43, 44) ;
    this->gaugeitem(2, 0, 3, 21, "Temp, C", "%2.1f", 10, 40, 42) ;
    this->switchitem(2, 0, 4, 8, "Малый свет", 26, 27) ;
    this->switchitem(2, 0, 5, 9, "Большой свет", 26, 27) ;

    this->switchitem(3, 0, 0, 11, "Прихожая", 18, 19) ;
    this->switchitem(3, 0, 1, 12, "Шкаф", 36, 37) ;
    this->switchitem(3, 0, 2, 10, "Проход", 28, 29) ;
    this->switchitem(3, 0, 3, 13, "Зарядка", 30, 31) ;
    this->alarmitem( 3, 0, 5, 25, "СБУ") ;

    this->switchitem(4, 0, 0, 17, "Свет", 32, 33) ;
    this->switchitem(4, 0, 1, 18, "Ветиляция", 24, 25) ;
    this->gaugeitem(4, 0, 3, 22, "Напряжение, V", "%3.0f", 110, 380, 46) ;
    this->gaugeitem(4, 0, 4, 23, "Бойлер, W", "%4.0f", 0, 2400, 45) ;
    this->gaugeitem(4, 0, 5, 24, "Стиралка, W", "%4.0f", 0, 2000, 45) ;
    this->switchitem(4, 1, 0, 19, "Блокировка", 54, 56) ;
    this->switchitem(4, 1, 1, 20, "Подписка", 54, 53) ;

    this->switchitem(5, 0, 0, 14, "Люстра", 26, 27) ;
    this->switchitem(5, 0, 1, 15, "Свет", 12, 13) ;
    this->switchitem(5, 0, 2, 16, "Раб. зона", 49, 50) ;
}

void Navigator::switchitem(int pageNumber, int subpageNumber, int itemNumber, int entityId, const char *title, int iconOff, int iconOn) {
    Item *item = &this->items[pageNumber][subpageNumber][itemNumber] ;
    item->itemNumber = itemNumber ;
    item->entityId = entityId ;
    item->title = (char *)title ;
    item->iconOff = iconOff ;
    item->iconOn = iconOn ;
    item->gauge = false ;
}

void Navigator::gaugeitem(int pageNumber, int subpageNumber, int itemNumber, int entityId, const char *title, const char *format, double minValue, double maxValue, int icon) {
    Item *item = &this->items[pageNumber][subpageNumber][itemNumber] ;
    item->itemNumber = itemNumber ;
    item->entityId = entityId ;
    item->title = (char *)title ;
    item->gauge = true ;
    item->iconOff = icon ;
    item->iconOn = icon ;
    item->format = (char *)format ;
    item->minValue = minValue ;
    item->maxValue = maxValue ;
}

void Navigator::alarmitem(int pageNumber, int subpageNumber, int itemNumber, int entityId, const char *title) {
    Item *item = &this->items[pageNumber][subpageNumber][itemNumber] ;
    item->itemNumber = itemNumber ;
    item->entityId = entityId ;
    item->title = (char *)title ;
    item->iconOff = 56 ;
    item->iconOn = 54 ;
    item->alarm = true ;
}

void Navigator::setup() {
    // if (this->nextion->nexInit(19200)) {
    if (this->nextion->nexInit(115200)) {
        Debug_println("Nextion init OK") ;
    } else {
        Debug_println("Nextion init failed") ;
        return ;
    }

    this->pageBoot->show() ;
    this->pbBoot->setValue(0) ;
}

void Navigator::setBootProgress(uint32 progress) {
   this->pbBoot->setValue(progress) ;
}

void Navigator::showHomePage() {
    this->pageHome->show() ;
    this->navigate(0) ;
}

int gauge_value(double minValue, double value, double maxValue) {
    const int maxDeg = 225 ;
    const int minDeg = 315 ;
    Debug_print("input value ") ;
    Debug_println(value) ;
    double val = min(maxValue, max(value, minValue)) ;
    Debug_print("calc value ") ;
    Debug_println(val) ;

    int result = minDeg + (maxDeg + (360 - minDeg)) * ((val - minValue) / (maxValue - minValue)) ;
    while (result > 360) {
        result -= 360 ;
    }

    Debug_print("degree value ") ;
    Debug_println(result) ;

    return result ;
}

int alarm_icon(char *state) {
    int result = ALARM_STATE_NA ;
    
    for (int i = ALARM_STATE_DISARMED; i <= ALARM_STATE_TRIGGERED; i++) {
        if (strcmp(state, ALARM_STATES[i]) == 0) {
            result = i ;
            break ;
        }
    }

    switch (result) {
        case ALARM_STATE_NA:
            return 56 ;
        case ALARM_STATE_DISARMED:
            return 56 ;
        case ALARM_STATE_ARMED_HOME:
            return 54 ;
        case ALARM_STATE_ARMED_AWAY:
            return 53 ;
        case ALARM_STATE_ARMED_NIGHT:
            return 55 ;
        default:
            return 56 ;
    }

}

void Navigator::subnavigate(int subPageNumber) {
    this->currentSubPage = subPageNumber ;
    NexButton *bsns[] = {this->bsn1, this->bsn2, this->bsn3} ;
    for (int i = 0 ; i < 3; i++) {
        NexButton *bsn = bsns[i] ;
        bsn->Set_background_image_pic(i == subPageNumber ? 3 : 2) ;
    }

    NexButton *buttons[] = {this->b0, this->b1, this->b2, this->b3, this->b4, this->b5} ;
    NexGauge  *gauges[]  = {this->z0, this->z1, this->z2, this->z3, this->z4, this->z5} ;
    NexText   *texts[]   = {this->t0, this->t1, this->t2, this->t3, this->t4, this->t5} ;

    for (int i = 0 ; i < 6; i++) {
        Item item = this->items[this->currentPage][this->currentSubPage][i] ;
        if (item.entityId < 0) {
            buttons[i]->setVisible(false) ;
            gauges[i]->setVisible(false) ;
            texts[i]->setVisible(false) ;
            continue ;
        } 

        Entity *entity = NULL ;

        if (entity_get != NULL) {
            entity = entity_get(item.entityId) ;
        }

        if (item.alarm) {
            gauges[i]->setVisible(false) ;
            texts[i]->setVisible(false) ;

            buttons[i]->setText(item.title) ;
            buttons[i]->Set_press_background_image_pic2(56) ;

            if (entity) {
                buttons[i]->Set_background_image_pic(alarm_icon(entity->state)) ;
            } else {
                buttons[i]->Set_background_image_pic(56) ;
            }

            buttons[i]->setVisible(true) ;
        } else if (item.gauge) {
            buttons[i]->Set_background_image_pic(16) ;
            buttons[i]->Set_press_background_image_pic2(16) ;
            buttons[i]->setText(item.title) ;
            buttons[i]->setVisible(true) ;

            if (entity) {
                gauges[i]->Set_background_image_pic(item.iconOff) ;
                gauges[i]->setValue(gauge_value(item.minValue, entity->value, item.maxValue)) ;
                gauges[i]->setVisible(true) ;
                char buffer[32] ;
                sprintf(buffer, item.format, entity->value) ;
                texts[i]->setText(buffer) ;
            } else {
                gauges[i]->setVisible(false) ;
                texts[i]->setText("-") ;
            }
            texts[i]->setVisible(true) ;

        } else {
            buttons[i]->setText(item.title) ;
            buttons[i]->Set_press_background_image_pic2(item.iconOn) ;

            if (entity == NULL) {
                buttons[i]->Set_background_image_pic(item.iconOff) ;
            } else {
                buttons[i]->Set_background_image_pic(entity->onff ? item.iconOn : item.iconOff) ;
            }

            gauges[i]->setVisible(false) ;
            texts[i]->setVisible(false) ;
            buttons[i]->setVisible(true) ;
        }

    }
}

void Navigator::navigate(int pageNumber) {
    this->currentPage = pageNumber ;

    NexButton *buttons[] = {this->bhome, this->bcab, this->bhall, this->bcorr, this->bbath, this->bkitchen, this->blady} ;
    for (int i = 0; i < 7; i++) {
        NexButton *button = buttons[i] ;
        button->Set_background_image_pic(i == this->currentPage ? 1 : 0) ;
    }
    
    this->subnavigate(0) ;
}

void Navigator::onEntityChanged(Entity *entity) {
    NexButton *buttons[] = {this->b0, this->b1, this->b2, this->b3, this->b4, this->b5} ;
    NexGauge  *gauges[]  = {this->z0, this->z1, this->z2, this->z3, this->z4, this->z5} ;
    NexText   *texts[]   = {this->t0, this->t1, this->t2, this->t3, this->t4, this->t5} ;

    for (int i = 0 ; i < 6; i++) {
        Item item = this->items[this->currentPage][this->currentSubPage][i] ;
        if (item.entityId != entity->id) {
            continue ;
        }

        if (item.alarm) {
            buttons[i]->Set_background_image_pic(alarm_icon(entity->state)) ;
        } else if (item.gauge) {
            gauges[i]->setValue(gauge_value(item.minValue, entity->value, item.maxValue)) ;
            char buffer[32] ;
            sprintf(buffer, item.format, entity->value) ;
            texts[i]->setText(buffer) ;
        } else {
            buttons[i]->Set_background_image_pic(entity->onff ? item.iconOn : item.iconOff) ;
        }

    }

    if (blonCallback->navValue >= 0) {
        Item item = this->items[this->currentPage][this->currentSubPage][blonCallback->navValue] ;
        if (item.entityId != entity->id) {
            return ;
        }
        this->blon->Set_background_image_pic(entity->onff ? item.iconOn : item.iconOff) ;
        this->pbLight->setValue(entity->brightness) ;
    }
}

void Navigator::itemDidClick(int itemNo, bool forceChange) {
    Item item = this->items[this->currentPage][this->currentSubPage][itemNo] ;
    if (item.entityId < 0) {
        return ;
    }

    Entity *entity = NULL ;

    if (entity_get) {
        entity = entity_get(item.entityId) ;
    }

    if (!entity) {
        return ;
    }

    if (item.alarm) {
        return this->showAlarmPage(item) ;
    }

    if (entity->dimmable && !forceChange) {
        return this->showLightPage(item) ;
    }

    entity->onff = !entity->onff ;

    if (change_entity) {
        change_entity(entity) ;
    }

}

void Navigator::changeBrightness(int itemNo, int step) {
    Item item = this->items[this->currentPage][this->currentSubPage][itemNo] ;
    if (item.entityId < 0) {
        return ;
    }

    Entity *entity = NULL ;

    if (entity_get) {
        entity = entity_get(item.entityId) ;
    }

    if (!entity) {
        return ;
    }

    if (!entity->dimmable) {
        return ;
    }

    entity->brightness += step ;
    
    if (entity->brightness > 100) {
        entity->brightness = 100 ;
    } else if (entity->brightness < 0) {
        entity->brightness = 0;
    }

    if (change_entity) {
        change_entity(entity) ;
    }
}

void Navigator::changeAlarm(int itemNo, int state) {
    Item item = this->items[this->currentPage][this->currentSubPage][itemNo] ;
    if (item.entityId < 0) {
        return ;
    }

    Entity *entity = NULL ;

    if (entity_get) {
        entity = entity_get(item.entityId) ;
    }

    if (!entity) {
        return ;
    }

    strcpy(entity->state, ALARM_COMMANDS[state]) ;
    Debug_print("Alarm state change command ") ;
    Debug_println(entity->state) ;

    if (change_entity) {
        change_entity(entity) ;
    }
}

void Navigator::showLightPage(Item item) {
    this->prevPage = this->currentPage ;
    this->prevSubPage = this->currentSubPage ;
    this->pageLight->show() ;

    Entity *entity = NULL ;

    if (entity_get) {
        entity = entity_get(item.entityId) ;
    }

    if (!entity) {
        return ;
    }

    this->blon->setText(item.title) ;
    this->blon->Set_background_image_pic(entity->onff ? item.iconOn : item.iconOff) ;
    this->blon->Set_press_background_image_pic2(item.iconOn) ;
    this->pbLight->setValue(entity->brightness) ;

    blonCallback->navValue = item.itemNumber ;
    this->blon->attachPop(itemClickCallback, blonCallback) ;

    blpCallback->navValue = item.itemNumber ;
    this->blp->attachPop(brightnessPlusCallback, blpCallback) ;
    blmCallback->navValue = item.itemNumber ;
    this->blm->attachPop(brightnessMinusCallback, blmCallback) ;
}

void Navigator::showAlarmPage(Item item) {
    this->prevPage = this->currentPage ;
    this->prevSubPage = this->currentSubPage ;
    this->pageAlarm->show() ;

    Entity *entity = NULL ;

    if (entity_get) {
        entity = entity_get(item.entityId) ;
    }

    if (!entity) {
        return ;
    }

    alarmHomeCallback->navValue = item.itemNumber ;
    alarmNightCallback->navValue = item.itemNumber ;
    alarmAwayCallback->navValue = item.itemNumber ;
    alarmDisarmCallback->navValue = item.itemNumber ;

    this->bahome->attachPop(alarmStateCallback, alarmHomeCallback) ;
    this->badisarm->attachPop(alarmStateCallback, alarmDisarmCallback) ;
    this->baaway->attachPop(alarmStateCallback, alarmAwayCallback) ;
    this->banight->attachPop(alarmStateCallback, alarmNightCallback) ;
}

void Navigator::closePage() {
    this->blon->detachPop() ;
    this->blp->detachPop() ;
    this->blm->detachPop() ;
    this->bahome->detachPop() ;
    this->badisarm->detachPop() ;
    this->baaway->detachPop() ;
    this->banight->detachPop() ;

    blonCallback->navValue = -1 ;
    blpCallback->navValue = -1 ;
    blmCallback->navValue = -1 ;
    alarmHomeCallback->navValue = -1 ;
    alarmNightCallback->navValue = -1 ;
    alarmAwayCallback->navValue = -1 ;
    alarmDisarmCallback->navValue = -1 ;

    if (this->prevPage >= 0) {
        this->currentPage = this->prevPage ;
    }

    NexButton *buttons[] = {this->bhome, this->bcab, this->bhall, this->bcorr, this->bbath, this->bkitchen, this->blady} ;
    for (int i = 0; i < 7; i++) {
        NexButton *button = buttons[i] ;
        button->Set_background_image_pic(i == this->currentPage ? 1 : 0) ;
    }

    if (this->prevSubPage >= 0) {
        this->subnavigate(this->prevSubPage) ;
    }
}

void Navigator::loop() {
    this->nextion->nexLoop(this->ntlist) ;
}
