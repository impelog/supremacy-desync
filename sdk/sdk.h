#pragma once

// padding macro, please use, counts pads in class automaticly
#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define PAD( size ) uint8_t MACRO_CONCAT( _pad, __COUNTER__ )[ size ];

// pre-declares.
class Player;
class Weapon;
class Entity;
class WeaponInfo;
class matrix3x4_t;
class vec3_t;
class ang_t;

struct BoxTraceInfo_t;
struct CBaseTrace;

#include "util/game.h"
#include "util/math.h"
#include "util/vector2.h"
#include "util/vector3.h"
#include "util/angle.h"
#include "util/matrix.h"
#include "sdk/memalloc.h"
#include "sdk/basehandle.h"
#include "util/color.h"
#include "sdk/utlvector.h"
#include "sdk/clientclass.h"
#include "sdk/datamap.h"
#include "sdk/entlist.h"
#include "core/clientdll.h"
#include "core/clientmode.h"
#include "sdk/engineclient.h"
#include "sdk/cvar.h"
#include "sdk/globalvars.h"
#include "sdk/input.h"
#include "sdk/surface.h"
#include "sdk/glow.h"
#include "sdk/studio.h"
#include "features/prediction.h"
#include "sdk/trace.h"
#include "sdk/studiorender.h"
#include "sdk/gameevents.h"
#include "sdk/matchframework.h"
#include "sdk/hud.h"
#include "sdk/localize.h"
#include "util/beams.h"
#include "sdk/networkstringtable.h"
#include "sdk/sound.h"
#include "sdk/effects.h"