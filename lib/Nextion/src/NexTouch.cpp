/**
 * @file NexTouch.cpp
 *
 * The implementation of class NexTouch. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/13
 * @author Jyrki Berg 2/17/2019 (https://github.com/jyberg)
 * 
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * @copyright 2020 Jyrki Berg
 *
 */
#include "NexTouch.h"
#include "NexHardware.h"

NexTouch::NexTouch(Nextion *nextion, uint8_t pid, uint8_t cid, const char *name, const NexObject* page)
    :NexObject(nextion, pid, cid, name, page)
{
    this->__cb_push = nullptr;
    this->__cb_pop = nullptr;
    this->__cbpop_ptr = nullptr;
    this->__cbpush_ptr = nullptr;
}

void NexTouch::attachPush(NexTouchEventCb push, void *ptr)
{
    this->__cb_push = push;
    this->__cbpush_ptr = ptr;
}

void NexTouch::detachPush(void)
{
    this->__cb_push = nullptr;
    this->__cbpush_ptr = nullptr;
}

void NexTouch::attachPop(NexTouchEventCb pop, void *ptr)
{
    this->__cb_pop = pop;
    this->__cbpop_ptr = ptr;
}

void NexTouch::detachPop(void)
{
    this->__cb_pop = nullptr;    
    this->__cbpop_ptr = nullptr;
}

void NexTouch::push(void)
{
    if (__cb_push)
    {
        __cb_push(__cbpush_ptr);
    }
}

void NexTouch::pop(void)
{
    if (__cb_pop)
    {
        __cb_pop(__cbpop_ptr);
    }
}

void NexTouch::iterate(NexTouch **list, uint8_t pid, uint8_t cid, uint8_t event)
{
    NexTouch *e = nullptr;
    uint16_t i = 0;

    if (nullptr == list)
    {
        dbSerialPrintln("Nex Touch events not registered/listed");
        return;
    }
    bool found{false};
    for(i = 0; (e = list[i]) != nullptr; i++)
    {
        if (e->getObjPid() == pid && e->getObjCid() == cid)
        {
            e->printObjInfo();
            if (NEX_EVENT_PUSH == event)
            {
                e->push();
                found = true;
            }
            else if (NEX_EVENT_POP == event)
            {
                e->pop();
                found = true;
            }
            break;
        }
    }
    if(!found)
    {
       dbSerialPrint("Nex Touch events not registered Pid: ");
       dbSerialPrint(pid);
       dbSerialPrint(" Cid: ");
       dbSerialPrintln(cid);
    }
}

