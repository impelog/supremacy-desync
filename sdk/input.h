#pragma once

constexpr int MULTIPLAYER_BACKUP = 150;

enum button_flags_t : int {
	IN_ATTACK    = ( 1 << 0 ),
	IN_JUMP      = ( 1 << 1 ),
	IN_DUCK      = ( 1 << 2 ),
	IN_FORWARD   = ( 1 << 3 ),
	IN_BACK      = ( 1 << 4 ),
	IN_USE       = ( 1 << 5 ),
	IN_CANCEL    = ( 1 << 6 ),
	IN_LEFT      = ( 1 << 7 ),
	IN_RIGHT     = ( 1 << 8 ),
	IN_MOVELEFT  = ( 1 << 9 ),
	IN_MOVERIGHT = ( 1 << 10 ),
	IN_ATTACK2   = ( 1 << 11 ),
	IN_RUN       = ( 1 << 12 ),
	IN_RELOAD    = ( 1 << 13 ),
	IN_ALT1      = ( 1 << 14 ),
	IN_ALT2      = ( 1 << 15 ),
	IN_SCORE     = ( 1 << 16 ),
	IN_SPEED     = ( 1 << 17 ),
	IN_WALK      = ( 1 << 18 ),
	IN_ZOOM      = ( 1 << 19 ),
	IN_WEAPON1   = ( 1 << 20 ),
	IN_WEAPON2   = ( 1 << 21 ),
	IN_BULLRUSH  = ( 1 << 22 ),
	IN_GRENADE1  = ( 1 << 23 ),
	IN_GRENADE2  = ( 1 << 24 ),
	IN_ATTACK3   = ( 1 << 25 ),
};


class CCSUsrMsg_ServerRankRevealAll {
public:
	__forceinline CCSUsrMsg_ServerRankRevealAll( ) : seconds_till_shutdown{} {}
public:
	PAD( 0x8 );
	int seconds_till_shutdown;
};

class CUserCmd {
public:
	__forceinline CUserCmd( ) { reset( ); }

	__forceinline void reset( ) {
		m_command_number   = 0;
		m_tick       = 0;
		m_view_angles.clear( );
		m_aimdirection.clear( );
		m_forward_move     = 0.f;
		m_side_move        = 0.f;
		m_up_move          = 0.f;
		m_buttons          = 0;
		m_impulse          = 0;
		m_weapon_select    = 0;
		m_weapon_subtype   = 0;
		m_random_seed      = 0;
		m_mousedx          = 0;
		m_mousedy          = 0;
		m_predicted        = false;
		m_head_angles.clear( );
		m_head_offset.clear( );
	}

	__forceinline CUserCmd& operator =( const CUserCmd& src ) {
		if( this == &src )
			return *this;

		m_command_number   = src.m_command_number;
		m_tick       = src.m_tick;
		m_view_angles      = src.m_view_angles;
		m_aimdirection     = src.m_aimdirection;
		m_forward_move     = src.m_forward_move;
		m_side_move        = src.m_side_move;
		m_up_move          = src.m_up_move;
		m_buttons          = src.m_buttons;
		m_impulse          = src.m_impulse;
		m_weapon_select    = src.m_weapon_select;
		m_weapon_subtype   = src.m_weapon_subtype;
		m_random_seed      = src.m_random_seed;
		m_mousedx          = src.m_mousedx;
		m_mousedy          = src.m_mousedy;
		m_predicted        = src.m_predicted;
		m_head_angles      = src.m_head_angles;
		m_head_offset      = src.m_head_offset;

		return *this;
	}

	__forceinline CUserCmd( const CUserCmd& src ) {
		*this = src;
	}

	// virtual dtor.
	PAD( 0x4 )

public:
	int     m_command_number;
	int     m_tick;
	ang_t   m_view_angles;
	vec3_t  m_aimdirection;
	float   m_forward_move;
	float   m_side_move;
	float   m_up_move;
	int     m_buttons;
	uint8_t m_impulse;
	int     m_weapon_select;
	int     m_weapon_subtype;
	int     m_random_seed;
	short   m_mousedx;
	short   m_mousedy;
	bool    m_predicted;
	vec3_t  m_head_angles;
	vec3_t  m_head_offset;
};

class CVerifiedUserCmd {
public:
	CUserCmd   m_cmd;
	ulong_t    m_crc;
};

class IInputSystem {
public:
	enum indices : size_t {
		ENABLEINPUT          = 11,
		ISBUTTONDOWN         = 15,
		GETBUTTONPRESSEDTICK = 16,
		GETCURSORPOSITION    = 56,
	};

public:
	__forceinline void EnableInput( bool enable ) {
		return util::get_method< void( __thiscall* )( decltype( this ), bool ) >( this, ENABLEINPUT )( this, enable );
	}

	__forceinline void GetCursorPosition( int* x, int* y ) {
		return util::get_method< void( __thiscall* )( decltype( this ), int*, int* ) >( this, GETCURSORPOSITION )( this, x, y );
	}
};

class CInput {
public:
	enum indices : size_t {
		CREATEMOVE                = 3,
		WRITEUSERCMDDELTATOBUFFER = 5,
		ENCODEUSERCMDTOBUFFER     = 6,
		GETUSERCMD                = 8,
		CAMISTHIRDPERSON          = 32,
		CAMTOTHIRDPERSON          = 35,
		CAMTOFIRSTPERSON          = 36,
	};

public:
	void* vtable; // 0x00
	PAD( 0x8 ); // 0x0004
	bool bTrackIRAvailable; // 0x000C
	bool bMouseInitialized; // 0x000D
	bool bMouseActive; // 0x000E
	PAD( 0x99 ); // 0x000F
	bool bCameraInterceptingMouse; // 0x00A8 // @ida: client.dll -> ["88 86 ? ? ? ? 8B 0D ? ? ? ? 66" + 0x2]
	bool bCameraInThirdPerson; // 0x00A9 // @ida: client.dll -> ["38 86 ? ? ? ? 74 28" + 0x2]
	bool bCameraMovingWithMouse; // 0x00AA // @ida: client.dll -> ["80 BE ? ? ? ? ? F3 0F 10 44" + 0x2]
	vec3_t vecCameraOffset; // 0x00AC // @ida: client.dll -> ["F3 0F 7E 87 ? ? ? ? 89 44" + 0x4]
	bool bCameraDistanceMove; // 0x00B8 // @ida: client.dll -> ["C6 86 ? ? ? ? ? EB 3F" + 0x2]
	int nCameraOldX; // 0x00BC
	int nCameraOldY; // 0x00C0
	int nCameraX; // 0x00C4
	int nCameraY; // 0x00C8
	bool bCameraIsOrthographic; // 0x00CC
	bool bCameraIsThirdPersonOverview; // 0x00CD
	ang_t angPreviousView; // 0x00D0
	ang_t angPreviousViewTilt; // 0x00DC
	float flLastForwardMove; // 0x00E8
	int nClearInputState; // 0x00EC
	CUserCmd *pCommands; // 0x00F0 // @ida: client.dll -> ["8B B7 ? ? ? ? C1 FA" + 0x2]
	CVerifiedUserCmd *pVerifiedCommands; // 0x00F4
	uintptr_t hSelectedWeapon; // 0x00F8 // @ida: client.dll -> ["8B 8F ? ? ? ? C7 05" + 0x2]
	void *pCameraThirdPersonData; // 0x00FC
	int nCameraCommand; // 0x0100
	PAD( 0x24 ); // 0x0104

public:
	__forceinline int CAM_IsThirdPerson( int slot = -1 ) {
		return util::get_method< int( __thiscall* )( decltype( this ), int ) >( this, CAMISTHIRDPERSON )( this, slot );
	}

	__forceinline void CAM_ToThirdPerson( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, CAMTOTHIRDPERSON )( this );
	}

	__forceinline void CAM_ToFirstPerson( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, CAMTOFIRSTPERSON )( this );
	}

	__forceinline CUserCmd* GetUserCmd( int sequence_number ) {
		return util::get_method< CUserCmd*( __thiscall* )( decltype( this ), int, int ) >( this, GETUSERCMD )( this, -1, sequence_number );
	}
};