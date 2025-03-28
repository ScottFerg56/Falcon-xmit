#pragma once
#include "Arduino.h"
#include <vector>

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

extern void SendCmd(String cmd);
