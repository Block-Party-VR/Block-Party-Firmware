/**
 * @file GlobalPrint.h
 * @brief a method of printing serial data to all outgoing communication methods
 */

#pragma once

#include <Arduino.h>

namespace GlobalPrint{
    static void Print(const char * s){
        Serial.print(s);
    }

    static void Print(const String &s){
        GlobalPrint::Print(s.c_str());
    }

    static void Println(const char * s){
        GlobalPrint::Print(s);
        GlobalPrint::Print("\n");
    }

    static void Println(const String &s){
        GlobalPrint::Println(s.c_str());
    }

}