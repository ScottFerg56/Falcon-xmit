#include "Falcon.h"

TypeItem TypeItems[] =
{
    FT_OnOff,     { {"OFF", 0}, {"ON", 1} },
    FT_OnOffAnim, { {"OFF",  0}, {"ON",   1}, {"ANIMATE", 2} },
    FT_UpDown,    { {"DOWN", 0}, {"UP",   1} },
    FT_Volume,    { {"6db",  0}, {"12db", 1}, {"18db", 2}, {"24db", 3} },
    FT_Sound,     { {"Kessel Run", 1}, {"Captain", 2}, {"Startup", 0}, {"Shutdown", 3}, {"R2D2 Cheerful", 4}, {"R2D2 Sad", 5} },
    FT_Int,       {},
    FT_Float,     {},
    FT_Color,     {},
    FT_Colors,    {},
};
