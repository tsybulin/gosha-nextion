/**
 * @file NexCheckbox.cpp
 *
 * The implementation of class NexCheckbox. 
 *
 * @author  huang xiaoming (email:<xiaoming.huang@itead.cc>)
 * @date    2016/9/13
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
#include "NexCheckbox.h"
#include "NexHardware.h"

void NexCheckbox::attachPush(NexTouchEventCb push, void *ptr)
{}


NexCheckbox::NexCheckbox(Nextion *nextion, uint8_t pid, uint8_t cid, const char *name, const NexObject* page)
    :NexTouch(nextion, pid, cid, name, page)
{
}

bool NexCheckbox::getValue(uint32_t *number)
{
    String cmd = String("get ");
    getObjGlobalPageName(cmd);
    cmd += ".val";
    sendCommand(cmd.c_str());
    return recvRetNumber(number);
}

bool NexCheckbox::setValue(uint32_t number)
{
    char buf[10] = {0};
    String cmd;
    
    utoa(number, buf, 10);
    getObjGlobalPageName(cmd);
    cmd += ".val=";
    cmd += buf;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexCheckbox::Get_background_color_bco(uint32_t *number)
{
    String cmd;
    cmd += "get ";
    getObjGlobalPageName(cmd);
    cmd += ".bco";
    sendCommand(cmd.c_str());
    return recvRetNumber(number);
}

bool NexCheckbox::Set_background_color_bco(uint32_t number)
{
    char buf[10] = {0};
    String cmd;
    utoa(number, buf, 10);
    getObjGlobalPageName(cmd);
    cmd += ".bco=";
    cmd += buf;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexCheckbox::Get_font_color_pco(uint32_t *number)
{
    String cmd;
    cmd += "get ";
    getObjGlobalPageName(cmd);
    cmd += ".pco";
    sendCommand(cmd.c_str());
    return recvRetNumber(number);
}

bool NexCheckbox::Set_font_color_pco(uint32_t number)
{
    char buf[10] = {0};
    String cmd;
    utoa(number, buf, 10);
    getObjGlobalPageName(cmd);
    cmd += ".pco=";
    cmd += buf;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}