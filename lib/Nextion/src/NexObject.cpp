/**
 * @file NexObject.cpp
 *
 * The implementation of class NexObject. 
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
 **/
#include "NexObject.h"
#include "NexHardware.h"

NexObject::NexObject(Nextion *nextion, uint8_t pid, uint8_t cid, const char *name, const NexObject* page):
NextionIf(nextion),
_pid{pid},_cid{cid},_name{name}, _page{page}
{
}

uint8_t NexObject::getObjPid(void)
{
    return _pid;
}

uint8_t NexObject::getObjCid(void)
{
    return _cid;
}

const char* NexObject::getObjName(void) const
{
    return _name;
}

const char* NexObject::getObjPageName(void)
{
    if(_page)
    {
        return _page->getObjName();
    }
    return nullptr;
}

void NexObject::getObjGlobalPageName(String &gName)
{
    if(_page)
    {
        gName += _page->getObjName();
        gName += ".";
    }
    gName +=_name;
}

bool NexObject::GetObjectWidth( uint32_t &width)
{
    String cmd;
    cmd = "get ";
    getObjGlobalPageName(cmd);
    cmd += ".w";
    sendCommand(cmd.c_str());
    return recvRetNumber(&width);
}

bool NexObject::GetObjectHeight( uint32_t &height)
{
    String cmd;
    cmd = "get ";
    getObjGlobalPageName(cmd);
    cmd += ".h";
    sendCommand(cmd.c_str());
    return recvRetNumber(&height);
}

void NexObject::printObjInfo(void)
{
    dbSerialPrint("[");
    dbSerialPrint((uint32_t)this);
    dbSerialPrint(":");
    dbSerialPrint(_pid);
    dbSerialPrint(",");
    dbSerialPrint(_cid);
    dbSerialPrint(",");
    if(_page)
    {
        dbSerialPrint(_page->getObjName());
        dbSerialPrint(".");
    }
    else
    {
        dbSerialPrint("(null).");
    }    
    if(_name)
    {
        dbSerialPrint(_name);
    }
    else
    {
        dbSerialPrint("(null)");
    }
    dbSerialPrintln("]");
}

bool NexObject::setVisible(bool visible)
{
    String cmd = String("vis ");
    cmd += _name;
    cmd += ",";
    if(visible)
    {
        cmd += "1";
    }
    else
    {
        cmd += "0";
    }
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexObject::refresh()
{
    String cmd = String("ref ");
    cmd += _name;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}