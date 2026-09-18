#pragma once

#pragma warning( disable : 4307 ) // '*': integral constant overflow
#pragma warning( disable : 4244 ) // possible loss of data
#pragma warning( disable : 4800 ) // forcing value to bool 'true' or 'false'
#pragma warning( disable : 4838 ) // conversion from '::size_t' to 'int' requires a narrowing conversion

// You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

using ulong_t = unsigned long;

// windows / stl includes.
#include <Windows.h>
#include <cstdint>
#include <intrin.h>
#include <xmmintrin.h>
#include <array>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <deque>
#include <functional>
#include <map>
#include <shlobj.h>
#include <filesystem>
#include <streambuf>

// our custom wrapper.
#include "util/unique_vector.h"
#include "util/tinyformat.h"

// other includes.
#include "util/hash.h"
#include "util/xorstr.h"
#include "util/pe.h"
#include "util/winapir.h"
#include "util/address.h"
#include "util/util.h"
#include "util/modules.h"
#include "util/pattern.h"
#include "util/vmt.h"
#include "util/stack.h"
#include "util/nt.h"
#include "util/x86.h"
#include "util/syscall.h"
#include "util/exceptions.h"

// hack includes.
#include "sdk/interfaces.h"
#include "sdk/sdk.h"
#include "sdk/csgo.h"
#include "features/penetration.h"
#include "sdk/netvars.h"
#include "sdk/entoffsets.h"
#include "sdk/entity.h"
#include "core/client.h"
#include "sdk/gamerules.h"
#include "core/hooks.h"
#include "util/render.h"
#include "features/pred.h"
#include "features/lagrecord.h"
#include "features/visuals.h"
#include "features/movement.h"
#include "features/bonesetup.h"
#include "features/hvh.h"
#include "features/lagcomp.h"
#include "features/aimbot.h"
#include "features/netdata.h"
#include "features/chams.h"
#include "util/notify.h"
#include "features/resolver.h"
#include "features/animations.h"
#include "features/grenades.h"
#include "features/skins.h"
#include "detours.h"
#include "sdk/movemsg.h"
#include "features/exploits.h"
#include "features/events.h"
#include "features/shots.h"

// gui includes.
#include "menu/json.h"
#include "menu/base64.h"
#include "menu/element.h"
#include "menu/checkbox.h"
#include "menu/dropdown.h"
#include "menu/multidropdown.h"
#include "menu/slider.h"
#include "menu/colorpicker.h"
#include "menu/edit.h"
#include "menu/keybind.h"
#include "menu/button.h"
#include "menu/tab.h"
#include "menu/form.h"
#include "menu/gui.h"
#include "menu/callbacks.h"
#include "menu/menu.h"
#include "menu/config.h"