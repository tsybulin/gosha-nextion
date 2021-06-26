/**
 * @file NexUpload.cpp
 *
 * The implementation of download tft file for nextion. 
 *
 * @author  Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date    2016/3/29
 * @author Jyrki Berg 2/24/2019 (https://github.com/jyberg)
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

#ifdef NEX_ENABLE_TFT_UPLOAD

#include "NexConfig.h"
#include "NexUpload.h"
#include "NextionIf.h"

NexUpload::NexUpload(Nextion *nextion)
    :NextionIf(nextion)
{
}

NexUpload::~NexUpload()
{}

bool NexUpload::upload(File  &tftFile)
{
    Reconnect();
    
    if(!InitUpload(tftFile))
    {
        dbSerialPrintln("Initialize upload error");
        return false;
    }
    if(!_downloadTftFile(tftFile))
    {
        dbSerialPrintln("download file error");
        return false;
    }
    dbSerialPrintln("download ok\r\n");
    return true;
}

void NexUpload::Reconnect()
{
    sendCommand("DRAKJHSUYDGBNCJHGJKSHBDN");
    delay(500);
    sendCommand("connect");
    delay(50);
    sendCommand("ÿÿconnect");
    delay(200);
}


bool NexUpload::InitUpload(File  &tftFile)
{
    String string = String(""); 
    String cmd = String("");
    
    String filesize_str = String(tftFile.size(),10);
    String baudrate_str = String(GetCurrentBaud(),10);
    cmd = "whmi-wri " + filesize_str + "," + baudrate_str + ",0";
    
    sendCommand(cmd.c_str());
    delay(50);
    if(!recvCommand(0x05, 500))
    { 
        return false;
    } 
    return true;
}

bool NexUpload::_downloadTftFile(File  &tftFile)
{
    uint8_t c;
    uint16_t send_timer = 0;
    uint16_t last_send_num = 0;
    send_timer = tftFile.size() / 4096 + 1;
    last_send_num = tftFile.size() % 4096;
    tftFile.seek(0);

    while(send_timer)
    {

        if(send_timer == 1)
        {
           for(uint16_t j = 1; j <= 4096; j++)
            {
                if(j <= last_send_num)
                {
                    c = tftFile.read();
                    sendRawByte(c);
                }
                else
                {
                    break;
                }
            }
        }

        else
        {
            for(uint16_t i = 1; i <= 4096; i++)
            {
                c = tftFile.read();
                sendRawByte(c);
            }
        }
        if(!recvCommand(0x05, 500))
        {
            return false;
        }
         --send_timer;
    } 
    return true;
}
#endif
