#pragma once
#include "Arduino.h"
#include <vector>
#include "Agent.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

#define SEND(x) Agent::GetInstance().SendCmd((x))
