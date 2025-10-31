//
// Created by slq on 10/31/2025.
//

#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <MinHook.h>
#include <filesystem>
#include <string>
#include <ShlObj.h>

#include "Module/ModuleRegistry.hpp"
#include "Util/Config.hpp"
#include "Util/Console.hpp"