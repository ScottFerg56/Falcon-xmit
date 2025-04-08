#pragma once
#include "Arduino.h"
#include <vector>
#include "ESPNAgent.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

#define SEND(x) ESPNAgent::PrimaryAgent()->SendCmd((x))
