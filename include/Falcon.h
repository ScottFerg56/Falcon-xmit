#pragma once
#include "Arduino.h"
#include <vector>

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

enum FalconValues
{
    FV_System_Voltage,
    FV_Light_Interior,
    FV_Light_Cockpit,
    FV_Light_Engine,
    FV_Light_Landing,
    FV_Light_Warning,
    FV_Light_Headlight,
    FV_Light_Gunwell,
    FV_Sound_Play,
    FV_Sound_Volume,
    FV_Colors,
    FV_Color_Engine,
    FV_Color_Landing,
    FV_Color_Warning,
    FV_Color_Headlight,
    FV_Color_Ramp,
    FV_Color_Sconce,
    FV_Color_Floor,
    FV_Color_Hatch,
    FV_Color_HoldBay,
    FV_Color_Gunwell,
    FV_Color_Bulkhead,
    FV_Color_Hold_Red,
    FV_Color_Hold_Green,
    FV_Color_Hold_Blue,
    FV_Color_Hold_Yellow,
    FV_Color_Cockpit_Red,
    FV_Color_Cockpit_Green,
    FV_Color_Cockpit_Blue,
    FV_Color_Cockpit_Yellow,
    FV_Rectenna_Sweep,
    FV_Rectenna_Speed,
    FV_Ramp_Position,
    FV_Ramp_Speed,
    FV_Rectenna_Position,
};

enum FalconTypes
{
    FT_OnOff,
    FT_OnOffAnim,
    FT_UpDown,
    FT_Volume,
    FT_Sound,
    FT_Int,
    FT_Float,
    FT_Color,
    FT_Colors,
};

enum FalconSections
{
    FS_Rectenna,
    FS_Ramp,
    FS_Lights,
    FS_System,
    FS_Colors,
};

struct NV
{
    const char* Name;
    uint8_t Value;
};

struct TypeItem
{
    FalconTypes TypeIndex;
    std::vector<NV> Values;
};
