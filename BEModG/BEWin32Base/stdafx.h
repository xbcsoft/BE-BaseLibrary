#pragma once

#include "BEMod/BEMod.h"
#include <winsock2.h>

extern HashTbe<UINT_PTR, be::function<void()>> g_DelayCallMap;