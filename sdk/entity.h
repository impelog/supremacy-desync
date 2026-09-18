#pragma once

#define MAX_WEAPONS	64

enum Hitboxes_t : int {
	HITBOX_HEAD = 0,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_R_THIGH,
	HITBOX_L_THIGH,
	HITBOX_R_CALF,
	HITBOX_L_CALF,
	HITBOX_R_FOOT,
	HITBOX_L_FOOT,
	HITBOX_R_HAND,
	HITBOX_L_HAND,
	HITBOX_R_UPPER_ARM,
	HITBOX_R_FOREARM,
	HITBOX_L_UPPER_ARM,
	HITBOX_L_FOREARM,
	HITBOX_MAX
};

enum RenderFlags_t : uint32_t {
	STUDIO_NONE = 0x00000000,
	STUDIO_RENDER = 0x00000001,
	STUDIO_VIEWXFORMATTACHMENTS = 0x00000002,
	STUDIO_DRAWTRANSLUCENTSUBMODELS = 0x00000004,
	STUDIO_TWOPASS = 0x00000008,
	STUDIO_STATIC_LIGHTING = 0x00000010,
	STUDIO_WIREFRAME = 0x00000020,
	STUDIO_ITEM_BLINK = 0x00000040,
	STUDIO_NOSHADOWS = 0x00000080,
	STUDIO_WIREFRAME_VCOLLIDE = 0x00000100,
	STUDIO_NOLIGHTING_OR_CUBEMAP = 0x00000200,
	STUDIO_SKIP_FLEXES = 0x00000400,
	STUDIO_DONOTMODIFYSTENCILSTATE = 0x00000800,
	STUDIO_TRANSPARENCY = 0x80000000,
	STUDIO_SHADOWDEPTHTEXTURE = 0x40000000,
	STUDIO_SHADOWTEXTURE = 0x20000000,
	STUDIO_SKIP_DECALS = 0x10000000
};

enum BoneFlags_t : int {
	BONE_USED_BY_ANYTHING = 0x0007FF00,
	BONE_USED_BY_HITBOX = 0x00000100, // bone (or child) is used by a hit box
	BONE_USED_BY_ATTACHMENT = 0x00000200, // bone (or child) is used by an attachment point
	BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
	BONE_USED_BY_VERTEX_LOD0 = 0x00000400, // bone (or child) is used by the toplevel model via skinned vertex
	BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
	BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
	BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
	BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
	BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
	BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
	BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
	BONE_USED_BY_BONE_MERGE = 0x00040000
};

enum CSWeaponType : int {
	WEAPONTYPE_UNKNOWN = -1,
	WEAPONTYPE_KNIFE,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_TASER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_HEALTHSHOT = 11,
	WEAPONTYPE_FISTS = 12,
	WEAPONTYPE_BREACHCHARGE = 13,
	WEAPONTYPE_BUMPMINE = 14,
	WEAPONTYPE_TABLET = 15,
	WEAPONTYPE_MELEE = 16
};

enum teams_t : int {
	TEAM_NOTEAM = 0,
	TEAM_SPECTATOR,
	TEAM_TERRORISTS,
	TEAM_COUNTERTERRORISTS
};

enum effects_t : int {
	EF_BONEMERGE = 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT = 0x004,	// player flashlight
	EF_NOINTERP = 0x008,	// don't interpolate the next frame
	EF_NOSHADOW = 0x010,	// Don't cast no shadow
	EF_NODRAW = 0x020,	// don't draw entity
	EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
	EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
	EF_MAX_BITS = 10
};

enum InvalidatePhysicsBits_t : int {
	POSITION_CHANGED = 0x1,
	ANGLES_CHANGED = 0x2,
	VELOCITY_CHANGED = 0x4,
	ANIMATION_CHANGED = 0x8,
};

enum DataUpdateType_t : int {
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

enum LifeStates_t : int {
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

enum PlayerFlags_t : int {
	FL_ONGROUND = ( 1 << 0 ),
	FL_DUCKING = ( 1 << 1 ),
	FL_WATERJUMP = ( 1 << 3 ),
	FL_ONTRAIN = ( 1 << 4 ),
	FL_INRAIN = ( 1 << 5 ),
	FL_FROZEN = ( 1 << 6 ),
	FL_ATCONTROLS = ( 1 << 7 ),
	FL_CLIENT = ( 1 << 8 ),
	FL_FAKECLIENT = ( 1 << 9 ),
	FL_INWATER = ( 1 << 10 ),
};

enum MoveType_t : int {
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4,
};

enum Weapons_t : int {
	WEAPON_NONE = 0,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_ZONE_REPULSOR = 20,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SHIELD = 37,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE_GG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS = 69,
	WEAPON_BREACHCHARGE = 70,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE = 75,
	WEAPON_HAMMER = 76,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB = 81,
	WEAPON_DIVERSION = 82,
	WEAPON_FRAG_GRENADE = 83,
	WEAPON_SNOWBALL = 84,
	WEAPON_BUMPMINE = 85,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	WEAPON_KNIFE_SKELETON = 525,
	GLOVE_STUDDED_BROKENFANG = 4725,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T = 5028,
	GLOVE_CT = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_HANDWRAPS = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_STUDDED_HYDRA = 5035,
	SPECIAL_AGENT_BLUEBERRIES_BUCKSHOT = 4619,
	SPECIAL_AGENT_TWO_TIMES_MCCOY_TACP = 4680,
	SPECIAL_AGENT_COMMANDOR_MAE_JAMISON = 4711,
	SPECIAL_AGENT_1ST_LIEUTENANT_FARLOW,
	SPECIAL_AGENT_JOHN_KASK,
	SPECIAL_AGENT_BIO_HAZ_SPECIALIST,
	SPECIAL_AGENT_SERGEANT_BOMBSON,
	SPECIAL_AGENT_CHEM_HAZ_SPECIALIST,
	SPECIAL_AGENT_REZAN_THE_REDSHIRT = 4718,
	SPECIAL_AGENT_SIR_BLOODY_MIAMI_DARRYL = 4726,
	SPECIAL_AGENT_SAFECRACKER_VOLTZMANN,
	SPECIAL_AGENT_LITTLE_KEV,
	SPECIAL_AGENT_GETAWAY_SALLY = 4730,
	SPECIAL_AGENT_NUMBER_K = 4732,
	SPECIAL_AGENT_SIR_BLOODY_SILENT_DARRYL = 4733,
	SPECIAL_AGENT_SIR_BLOODY_SKULLHEAD_DARRYL,
	SPECIAL_AGENT_SIR_BLOODY_DARRYL_ROYALE,
	SPECIAL_AGENT_SIR_BLOODY_LOUDMOUTH_DARRYL,
	SPECIAL_AGENT_T = 5036,
	SPECIAL_AGENT_CT = 5037,
	SPECIAL_AGENT_GROUND_REBEL = 5105,
	SPECIAL_AGENT_OSIRIS,
	SPECIAL_AGENT_SHAHMAT,
	SPECIAL_AGENT_MUHLIK,
	SPECIAL_AGENT_SOLDIER = 5205,
	SPECIAL_AGENT_ENFORCER,
	SPECIAL_AGENT_SLINGSHOT,
	SPECIAL_AGENT_STREET_SOLDIER,
	SPECIAL_AGENT_OPERATOR = 5305,
	SPECIAL_AGENT_MARKUS_DELROW,
	SPECIAL_AGENT_MICHAEL_SYFERS,
	SPECIAL_AGENT_AVA,
	SPECIAL_AGENT_3RD_COMMANDO_COMPANY = 5400,
	SPECIAL_AGENT_SEAL_TEAM_6_SOLDIER,
	SPECIAL_AGENT_BUCKSHOT,
	SPECIAL_AGENT_TWO_TIMES_MCCOY_USAF,
	SPECIAL_AGENT_RICKSAW,
	SPECIAL_AGENT_DRAGOMIR = 5500,
	SPECIAL_AGENT_MAXIMUS,
	SPECIAL_AGENT_REZAN_THE_READY,
	SPECIAL_AGENT_BLACKWOLF = 5503,
	SPECIAL_AGENT_THE_DOCTOR,
	SPECIAL_AGENT_DRAGOMIR_FOOTSOLDIERS,
	SPECIAL_AGENT_B_SQUADRON_OFFICER = 5601
};

struct RenderableInstance_t {
	uint8_t m_alpha;
	__forceinline RenderableInstance_t( ) : m_alpha{ 255ui8 } {}
};

class Entity {
public:
	// helper methods.
	template< typename t >
	__forceinline t& get( size_t offset ) {
		return *( t* )( ( uintptr_t )this + offset );
	}

	template< typename t >
	__forceinline void set( size_t offset, const t& val ) {
		*( t* )( ( uintptr_t )this + offset ) = val;
	}

	template< typename t >
	__forceinline t as( ) {
		return ( t )this;
	}

public:
	// netvars / etc.
	__forceinline vec3_t& m_vecOrigin( ) {
		return get< vec3_t >( g_entoffsets.m_vecOrigin );
	}

	__forceinline vec3_t& m_vecOldOrigin( ) {
		return get< vec3_t >( g_entoffsets.m_vecOldOrigin );
	}

	__forceinline vec3_t& m_vecVelocity( ) {
		return get< vec3_t >( g_entoffsets.m_vecVelocity );
	}

	__forceinline vec3_t& m_vecMins( ) {
		return get< vec3_t >( g_entoffsets.m_vecMins );
	}

	__forceinline vec3_t& m_vecMaxs( ) {
		return get< vec3_t >( g_entoffsets.m_vecMaxs );
	}

	__forceinline int& m_iTeamNum( ) {
		return get< int >( g_entoffsets.m_iTeamNum );
	}

	__forceinline int& m_nSequence( ) {
		return get< int >( g_entoffsets.m_nSequence );
	}

	__forceinline float& m_flCycle( ) {
		return get< float >( g_entoffsets.m_flCycle );
	}

	__forceinline float& m_flC4Blow( ) {
		return get< float >( g_entoffsets.m_flC4Blow );
	}

	__forceinline bool& m_bBombTicking( ) {
		return get< bool >( g_entoffsets.m_bBombTicking );
	}

	__forceinline int& m_fEffects( ) {
		// todo; netvar.
		return get< int >( g_entoffsets.m_fEffects );
	}

	__forceinline int& m_nModelIndex( ) {
		return get< int >( g_entoffsets.m_nModelIndex );
	}

	__forceinline bool& m_bReadyToDraw( ) {
		return get< bool >( g_entoffsets.m_bReadyToDraw );
	}

public:
	// virtual indices
	enum indices : size_t {
		WORLDSPACECENTER = 79,
		GETMAXHEALTH = 123,
		ISPLAYER = 158,
		ISBASECOMBATWEAPON = 166,
		UPDATECLIENTSIDEANIMATION = 224,
	};

public:
	// virtuals.
	// renderable table.
	__forceinline void* renderable( ) {
		return ( void* )( ( uintptr_t )this + 0x4 );
	}

	__forceinline vec3_t& GetRenderOrigin( ) {
		return util::get_method< vec3_t & ( __thiscall* )( void* ) >( renderable( ), 1 )( renderable( ) );
	}

	__forceinline ang_t& GetRenderAngles( ) {
		return util::get_method< ang_t & ( __thiscall* )( void* ) >( renderable( ), 2 )( renderable( ) );
	}

	__forceinline const model_t* GetModel( ) {
		return util::get_method< const model_t * ( __thiscall* )( void* ) >( renderable( ), 8 )( renderable( ) );
	}

	__forceinline void DrawModel( int flags = STUDIO_RENDER, const RenderableInstance_t& instance = {} ) {
		return util::get_method< void( __thiscall* )( void*, int, const RenderableInstance_t& )>( renderable( ), 9 )( renderable( ), flags, instance );
	}

	__forceinline bool SetupBones( matrix3x4_t* out, int max, int mask, float time ) {
		return util::get_method< bool( __thiscall* )( void*, matrix3x4_t*, int, int, float )>( renderable( ), 13 )( renderable( ), out, max, mask, time );
	}

	// networkable table.
	__forceinline void* networkable( ) {
		return ( void* )( ( uintptr_t )this + 0x8 );
	}

	__forceinline void Release( ) {
		return util::get_method< void( __thiscall* )( void* ) >( networkable( ), 1 )( networkable( ) );
	}

	__forceinline ClientClass* GetClientClass( ) {
		return util::get_method< ClientClass * ( __thiscall* )( void* ) >( networkable( ), 2 )( networkable( ) );
	}

	__forceinline void OnDataChanged( DataUpdateType_t type ) {
		return util::get_method< void( __thiscall* )( void*, DataUpdateType_t ) >( networkable( ), 5 )( networkable( ), type );
	}

	__forceinline void PreDataUpdate( DataUpdateType_t type ) {
		return util::get_method< void( __thiscall* )( void*, DataUpdateType_t ) >( networkable( ), 6 )( networkable( ), type );
	}

	__forceinline void PostDataUpdate( DataUpdateType_t type ) {
		return util::get_method< void( __thiscall* )( void*, DataUpdateType_t ) >( networkable( ), 7 )( networkable( ), type );
	}

	__forceinline bool dormant( ) {
		return util::get_method< bool( __thiscall* )( void* ) >( networkable( ), 9 )( networkable( ) );
	}

	__forceinline int index( ) {
		return util::get_method< int( __thiscall* )( void* ) >( networkable( ), 10 )( networkable( ) );
	}

	__forceinline void SetDestroyedOnRecreateEntities( ) {
		return util::get_method< void( __thiscall* )( void* ) >( networkable( ), 13 )( networkable( ) );
	}

	// normal table.
	__forceinline const vec3_t& GetAbsOrigin( ) {
		return util::get_method< const vec3_t & ( __thiscall* )( void* ) >( this, 10 )( this );
	}

	__forceinline const ang_t& GetAbsAngles( ) {
		return util::get_method< const ang_t & ( __thiscall* )( void* ) >( this, 11 )( this );
	}

	__forceinline bool IsPlayer( ) {
		return util::get_method< bool( __thiscall* )( void* ) >( this, ISPLAYER )( this );
	}

	__forceinline bool IsBaseCombatWeapon( ) {
		return util::get_method< bool( __thiscall* )( void* ) >( this, ISBASECOMBATWEAPON )( this );
	}

	__forceinline std::string GetBombsiteName( ) {
		std::string out;

		// note - dex; bomb_target + 0x150 has a char array for site name... not sure how much memory gets allocated for it.
		out.resize( 32u );

		std::memcpy( &out[ 0 ], ( const void* )( ( uintptr_t )this + 0x150 ), 32u );

		return out;
	}

	__forceinline void InvalidatePhysicsRecursive( InvalidatePhysicsBits_t bits ) {
		using InvalidatePhysicsRecursive_t = void( __thiscall* )( decltype( this ), InvalidatePhysicsBits_t );
		g_csgo.InvalidatePhysicsRecursive.as< InvalidatePhysicsRecursive_t >( )( this, bits );
	}

	__forceinline void SetAbsAngles( const ang_t& angles ) {
		using SetAbsAngles_t = void( __thiscall* )( decltype( this ), const ang_t& );
		g_csgo.SetAbsAngles.as< SetAbsAngles_t >( )( this, angles );
	}

	__forceinline void SetAbsOrigin( const vec3_t& origin ) {
		using SetAbsOrigin_t = void( __thiscall* )( decltype( this ), const vec3_t& );
		g_csgo.SetAbsOrigin.as< SetAbsOrigin_t >( )( this, origin );
	}

	__forceinline void SetAbsVelocity( const vec3_t& velocity ) {
		using SetAbsVelocity_t = void( __thiscall* )( decltype( this ), const vec3_t& );
		g_csgo.SetAbsVelocity.as< SetAbsVelocity_t >( )( this, velocity );
	}

	__forceinline void AddEffect( int effects ) {
		m_fEffects( ) |= effects;
	}

	__forceinline int get_class_id( ) {
		ClientClass* cc{ GetClientClass( ) };

		return ( cc ) ? cc->m_ClassID : -1;
	}

	__forceinline void UpdateClientSideAnimation( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, UPDATECLIENTSIDEANIMATION )( this );
	}

	__forceinline bool is( hash32_t hash ) {
		return g_netvars.GetClientID( hash ) == get_class_id( );
	}
};

class CCSGOPlayerAnimState {
public:
	char    pad0[ 4 ];
	bool    m_first_update;
	char    pad1[ 3 ];
	int     m_tick_count;
	char    pad2[ 84 ];
	Player* m_player;
	Weapon* m_weapon;
	Weapon* m_prev_weapon;
	float   m_prev_update_time;
	int     m_frame;
	float   m_update_delta;
	float   m_eye_yaw;
	float   m_eye_pitch;
	float   m_goal_feet_yaw;
	float   m_cur_feet_yaw;
	float   m_move_yaw;
	float   m_move_yaw_ideal;
	float   m_move_yaw_cur_to_ideal;
	char    pad3[ 4 ];
	float   m_feet_cycle;
	float   m_feet_weight;
	char    pad4[ 4 ];
	float   m_duck_amount;
	float   m_duck_additional;
	char    pad5[ 4 ];
	vec3_t  m_origin;
	vec3_t  m_prev_origin;
	vec3_t  m_velocity;
	vec3_t  m_velocity_normalized;
	vec3_t  m_velocity_normalized_non_zero;
	float   m_speed;
	float   m_up_velocity;
	float   m_speed_as_portion_of_run_speed;
	float   m_speed_as_portion_of_walk_speed;
	float   m_speed_as_portion_of_crouch_speed;
	float   m_time_since_started_moving;
	float   m_time_since_stopped_moving;
	bool    m_ground;
	bool    m_land;
	char    pad6[ 6 ];
	float   m_time_since_in_air;
	float   m_left_ground_height;
	float   m_land_anim_multiplier;
	float   m_walk_to_run_transition;
	char    pad7[ 4 ];
	float   m_in_air_smooth_value;
	bool    m_on_ladder;
	char    pad8[ 47 ];
	float   m_velocity_test_time;
	vec3_t  m_prev_velocity;
	vec3_t  m_dst_acceleration;
	vec3_t  m_acceleration;
	float   m_acceleration_weight;
	char    pad9[ 12 ];
	float   m_strafe_weight;
	char    pad10[ 4 ];
	float   m_strafe_cycle;
	int     m_strafe_sequence;
	char    pad11[ 388 ];
	float   m_camera_shoot_height;
	bool    m_smooth_height_valid;
	char    pad12[ 11 ];
	float   m_min_body_yaw;
	float   m_max_body_yaw;
	float   m_min_pitch;
	float   m_max_pitch;
	int     m_anim_set_version;

	__forceinline float GetMaxDesyncDelta( ) {
		float walk_speed = std::clamp( m_speed_as_portion_of_walk_speed, 0.f, 1.f );
		float run_speed = ( ( 0.8f - ( m_walk_to_run_transition * 0.3f ) ) - 1.f ) * walk_speed;
		float body_yaw_modifier = run_speed + 1.f;

		if ( m_duck_amount > 0.f ) {
			float crouch_walk_speed = std::clamp( m_speed_as_portion_of_crouch_speed, 0.f, 1.f );
			body_yaw_modifier += ( m_duck_amount * crouch_walk_speed ) * ( 0.5f - body_yaw_modifier );
		}

		float max_yaw = ( m_max_body_yaw > 0.f ) ? m_max_body_yaw : 58.f;
		return std::clamp( max_yaw * body_yaw_modifier, 0.f, 58.f );
	}
};

class CStudioHdr {
public:
	class mstudioposeparamdesc_t {
	public:
		int					sznameindex;
		__forceinline char* const name( void ) const { return ( ( char* )this ) + sznameindex; }
		int					flags;	// ????
		float				start;	// starting value
		float				end;	// ending value
		float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
	};

	studiohdr_t* m_pStudioHdr;
	void* m_pVModel;
};

class C_AnimationLayer {
public:
	float   m_anim_time;			// 0x0000
	float   m_fade_out_time;		// 0x0004
	int     m_flags;				// 0x0008
	int     m_activty;				// 0x000C
	int     m_priority;				// 0x0010
	int     m_order;				// 0x0014
	int     m_sequence;				// 0x0018
	float   m_prev_cycle;			// 0x001C
	float   m_weight;				// 0x0020
	float   m_weight_delta_rate;	// 0x0024
	float   m_playback_rate;		// 0x0028
	float   m_cycle;				// 0x002C
	Entity* m_owner;				// 0x0030
	int     m_bits;					// 0x0034
}; // size: 0x0038

class CBoneAccessor {
public:
	void* m_pAnimating;
	BoneArray* m_pBones;
	int        m_ReadableBones;
	int        m_WritableBones;
};

class CBoneCache {
public:
	BoneArray* m_pCachedBones;
	PAD( 0x8 );
	int        m_CachedBoneCount;
};

class Ragdoll : public Entity {
public:
	__forceinline Player* GetPlayer( ) {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Player* >( m_hPlayer( ) );
	}

	__forceinline EHANDLE& m_hPlayer( ) {
		return get< EHANDLE >( g_entoffsets.m_hPlayer );
	}

	__forceinline float& m_flDeathYaw( ) {
		return get< float >( g_entoffsets.m_flDeathYaw );
	}

	__forceinline float& m_flAbsYaw( ) {
		return get< float >( g_entoffsets.m_flAbsYaw );
	}
};

class Player : public Entity {
public:
	// netvars / etc.
	__forceinline vec3_t& m_vecAbsVelocity( ) {
		return get< vec3_t >( g_entoffsets.m_vecAbsVelocity );
	}

	__forceinline int& m_lifeState( ) {
		return get< int >( g_entoffsets.m_lifeState );
	}

	__forceinline int& m_fFlags( ) {
		return get< int >( g_entoffsets.m_fFlags );
	}

	__forceinline int& m_MoveType( ) {
		return get< int >( g_entoffsets.m_MoveType );
	}

	__forceinline int& m_iHealth( ) {
		return get< int >( g_entoffsets.m_iHealth );
	}

	__forceinline int& m_iAccount( ) {
		return get< int >( g_entoffsets.m_iAccount );
	}

	__forceinline bool& m_bHasDefuser( ) {
		return get< bool >( g_entoffsets.m_bHasDefuser );
	}

	__forceinline int& m_nHitboxSet( ) {
		return get< int >( g_entoffsets.m_nHitboxSet );
	}

	__forceinline ang_t& m_angAbsRotation( ) {
		return get< ang_t >( g_entoffsets.m_angAbsRotation );
	}

	__forceinline ang_t& m_angRotation( ) {
		return get< ang_t >( g_entoffsets.m_angRotation );
	}

	__forceinline ang_t& m_angNetworkAngles( ) {
		return get< ang_t >( g_entoffsets.m_angNetworkAngles );
	}

	__forceinline bool m_bIsLocalPlayer( ) {
		// .text:101E0078 674     84 C0				   test    al, al          ; Logical Compare
		// .text:101E007A 674     74 17				   jz      short loc_101E0093; Jump if Zero( ZF = 1 )
		// .text:101E007C 674     8A 83 F8 35 00 00	   mov     al, [ ebx + 35F8h ]
		return get< bool >( g_csgo.IsLocalPlayer );
	}

	__forceinline CCSGOPlayerAnimState* m_PlayerAnimState( ) {
		// .text:1037A5B8 00C     E8 E3 40 E6 FF         call    C_BasePlayer__Spawn ; Call Procedure
		// .text:1037A5BD 00C     80 BE E1 39 00 00 00   cmp     byte ptr[ esi + 39E1h ], 0; Compare Two Operands
		// .text:1037A5C4 00C     74 48                  jz      short loc_1037A60E; Jump if Zero( ZF = 1 )
		// .text:1037A5C6 00C     8B 8E 74 38 00 00      mov     ecx, [ esi + 3874h ]; this
		// .text:1037A5CC 00C     85 C9                  test    ecx, ecx; Logical Compare
		// .text:1037A5CE 00C     74 3E                  jz      short loc_1037A60E; Jump if Zero( ZF = 1 )
		return get< CCSGOPlayerAnimState* >( g_csgo.PlayerAnimState );
	}

	__forceinline CStudioHdr* m_studioHdr( ) {
		// .text:1017E902 08C    8B 86 3C 29 00 00    mov     eax, [ esi + 293Ch ]
		// .text:1017E908 08C    89 44 24 10          mov[ esp + 88h + var_78 ], eax
		return get< CStudioHdr* >( g_csgo.studioHdr );
	}

	__forceinline ulong_t& m_iMostRecentModelBoneCounter( ) {
		// .text:101AC9A9 000    89 81 80 26 00 00    mov[ ecx + 2680h ], eax
		return get< ulong_t >( g_csgo.MostRecentModelBoneCounter );
	}

	__forceinline float& m_flLastBoneSetupTime( ) {
		// .text:101AC99F 000    C7 81 14 29 00 00 FF FF+    mov     dword ptr [ecx+2914h], 0FF7FFFFFh;
		return get< float >( g_csgo.LastBoneSetupTime );
	}

	__forceinline int& m_nTickBase( ) {
		return get< int >( g_entoffsets.m_nTickBase );
	}

	__forceinline float& m_flNextAttack( ) {
		return get< float >( g_entoffsets.m_flNextAttack );
	}

	__forceinline float& m_flDuckAmount( ) {
		return get< float >( g_entoffsets.m_flDuckAmount );
	}

	__forceinline float& m_flSimulationTime( ) {
		return get< float >( g_entoffsets.m_flSimulationTime );
	}

	__forceinline float& m_flOldSimulationTime( ) {
		return get< float >( g_entoffsets.m_flOldSimulationTime );
	}

	__forceinline float& m_flLowerBodyYawTarget( ) {
		return get< float >( g_entoffsets.m_flLowerBodyYawTarget );
	}

	__forceinline float& m_flVelocityModifier( ) {
		return get< float >( g_entoffsets.m_flVelocityModifier );
	}

	__forceinline float& m_fImmuneToGunGameDamageTime( ) {
		return get< float >( g_entoffsets.m_fImmuneToGunGameDamageTime );
	}

	__forceinline bool& m_bHasHelmet( ) {
		return get< bool >( g_entoffsets.m_bHasHelmet );
	}

	__forceinline bool& m_bClientSideAnimation( ) {
		return get< bool >( g_entoffsets.m_bClientSideAnimation );
	}

	__forceinline bool& m_bHasHeavyArmor( ) {
		return get< bool >( g_entoffsets.m_bHasHeavyArmor );
	}

	__forceinline bool& m_bIsScoped( ) {
		return get< bool >( g_entoffsets.m_bIsScoped );
	}

	__forceinline bool& m_bDucking( ) {
		return get< bool >( g_entoffsets.m_bDucking );
	}

	__forceinline bool& m_bSpotted( ) {
		return get< bool >( g_entoffsets.m_bSpotted );
	}

	__forceinline int& m_iObserverMode( ) {
		return get< int >( g_entoffsets.m_iObserverMode );
	}

	__forceinline int& m_ArmorValue( ) {
		return get< int >( g_entoffsets.m_ArmorValue );
	}

	__forceinline float& m_flMaxspeed( ) {
		return get< float >( g_entoffsets.m_flMaxspeed );
	}

	__forceinline float& m_surfaceFriction( ) {
		return get< float >( g_entoffsets.m_surfaceFriction );
	}

	__forceinline float& m_flFlashBangTime( ) {
		return get< float >( g_entoffsets.m_flFlashBangTime );
	}

	__forceinline ang_t& m_angEyeAngles( ) {
		return get< ang_t >( g_entoffsets.m_angEyeAngles );
	}

	__forceinline ang_t& m_aimPunchAngle( ) {
		return get< ang_t >( g_entoffsets.m_aimPunchAngle );
	}

	__forceinline ang_t& m_viewPunchAngle( ) {
		return get< ang_t >( g_entoffsets.m_viewPunchAngle );
	}

	__forceinline ang_t& m_aimPunchAngleVel( ) {
		return get< ang_t >( g_entoffsets.m_aimPunchAngleVel );
	}

	__forceinline vec3_t& m_vecViewOffset( ) {
		return get< vec3_t >( g_entoffsets.m_vecViewOffset );
	}

	__forceinline CUserCmd& m_PlayerCommand( ) {
		return get< CUserCmd >( g_entoffsets.m_PlayerCommand );
	}

	__forceinline CUserCmd*& m_pCurrentCommand( ) {
		return get< CUserCmd* >( g_entoffsets.m_pCurrentCommand );
	}

	__forceinline int& m_iClientEntityMask( ) {
		return get< int >( g_entoffsets.m_iClientEntityMask );
	}

	__forceinline bool& m_bEnableJiggleBones( ) {
		return get< bool >( g_entoffsets.m_bEnableJiggleBones );
	}

	__forceinline bool& m_bUseNewAnimstate( ) {
		return get< bool >( g_entoffsets.m_bUseNewAnimstate );
	}

	__forceinline int& m_iEFlags( ) {
		return get< int >( g_entoffsets.m_iEFlags );
	}

	__forceinline float* m_flPoseParameter( ) {
		return ( float* )( ( uintptr_t )this + g_entoffsets.m_flPoseParameter );
	}

	__forceinline CBaseHandle* m_hMyWearables( ) {
		return ( CBaseHandle* )( ( uintptr_t )this + g_entoffsets.m_hMyWearables );
	}

	__forceinline CBoneCache& m_BoneCache( ) {
		// TODO; sig
		return get< CBoneCache >( g_entoffsets.m_BoneCache );
	}

	__forceinline EHANDLE& m_hObserverTarget( ) {
		return get< EHANDLE >( g_entoffsets.m_hObserverTarget );
	}

	__forceinline EHANDLE& m_hActiveWeapon( ) {
		return get< EHANDLE >( g_entoffsets.m_hActiveWeapon );
	}

	__forceinline EHANDLE& m_hGroundEntity( ) {
		return get< EHANDLE >( g_entoffsets.m_hGroundEntity );
	}

	__forceinline CBaseHandle* m_hMyWeapons( ) {
		return ( CBaseHandle* )( ( uintptr_t )this + g_entoffsets.m_hMyWeapons );
	}

	__forceinline CBaseHandle* m_hViewModel( ) {
		return ( CBaseHandle* )( ( uintptr_t )this + g_entoffsets.m_hViewModel );
	}

	__forceinline C_AnimationLayer* m_AnimOverlay( ) {
		// .text:1017EAB1 08C    8B 47 1C                mov     eax, [edi+1Ch]
		// .text:1017EAB4 08C    8D 0C D5 00 00 00 00    lea     ecx, ds:0[ edx * 8 ]; Load Effective Address
		// .text:1017EABB 08C    2B CA                   sub     ecx, edx; Integer Subtraction
		// .text:1017EABD 08C    8B 80 70 29 00 00       mov     eax, [ eax + 2970h ]
		// .text:1017EAC3 08C    8D 34 C8                lea     esi, [ eax + ecx * 8 ]; Load Effective Address
		// .text:1017EAC6
		return get< C_AnimationLayer* >( g_csgo.AnimOverlay );
	}

	__forceinline float& m_flSpawnTime( ) {
		// .text:10381AB3 00C    F3 0F 10 49 10             movss   xmm1, dword ptr [ecx+10h] ; Move Scalar Single-FP
		// .text:10381AB8 00C    F3 0F 5C 88 90 A2 00 00    subss   xmm1, dword ptr[ eax + 0A290h ]; Scalar Single - FP Subtract
		return get< float >( g_csgo.SpawnTime );
	}

	__forceinline CBoneAccessor& m_BoneAccessor( ) {
		// .text:101A9253 1C4    C7 81 A0 26 00 00 00 FF 0F 00    mov     dword ptr[ ecx + 26A0h ], 0FFF00h
		// .text:101A925D 1C4    C7 81 9C 26 00 00 00 FF 0F 00    mov     dword ptr[ ecx + 269Ch ], 0FFF00h
		// .text:101A9267 1C4    8B 10                            mov     edx, [ eax ]
		// .text:101A9269 1C4    8D 81 94 26 00 00                lea     eax, [ ecx + 2694h ]; Load Effective Address
		// .text:101A926F 1C4    50                               push    eax
		return get< CBoneAccessor >( g_csgo.BoneAccessor );
	}

	__forceinline float& m_flCollisionHeight( ) {
		return get< float >( g_entoffsets.m_flCollisionHeight );
	}

	__forceinline float& m_flCollisionChangeTime( ) {
		return get< float >( g_entoffsets.m_flCollisionChangeTime );
	}

public:
	enum indices : size_t {
		GETREFEHANDLE = 2,
		BUILDTRANSFORMATIONS = 190,
		DOEXTRABONEPROCESSING = 198,
		STANDARDBLENDINGRULES = 206,
		UPDATECLIENTSIDEANIMATION = 224,
		GETACTIVEWEAPON = 268,
		GETEYEPOS = 169,
		GETFOV = 332,
		UPDATECOLLISIONBOUNDS = 340
	};

public:
	// virtuals.
	__forceinline ulong_t GetRefEHandle( ) {
		using GetRefEHandle_t = ulong_t( __thiscall* )( decltype( this ) );
		return util::get_method< GetRefEHandle_t >( this, GETREFEHANDLE )( this );
	}

	__forceinline void BuildTransformations( CStudioHdr* hdr, vec3_t* pos, quaternion_t* q, const matrix3x4_t& transform, int mask, uint8_t* computed ) {
		using BuildTransformations_t = void( __thiscall* )( decltype( this ), CStudioHdr*, vec3_t*, quaternion_t*, matrix3x4_t const&, int, uint8_t* );
		return util::get_method< BuildTransformations_t >( this, BUILDTRANSFORMATIONS )( this, hdr, pos, q, transform, mask, computed );
	}

	__forceinline void StandardBlendingRules( CStudioHdr* hdr, vec3_t* pos, quaternion_t* q, float time, int mask ) {
		using StandardBlendingRules_t = void( __thiscall* )( decltype( this ), CStudioHdr*, vec3_t*, quaternion_t*, float, int );
		return util::get_method< StandardBlendingRules_t >( this, STANDARDBLENDINGRULES )( this, hdr, pos, q, time, mask );
	}

	__forceinline float GetFOV( ) {
		return util::get_method< float( __thiscall* )( decltype( this ) ) >( this, GETFOV )( this );
	}

	__forceinline const vec3_t& WorldSpaceCenter( ) {
		return util::get_method< const vec3_t & ( __thiscall* )( void* ) >( this, WORLDSPACECENTER )( this );
	}

	__forceinline void GetEyePos( vec3_t* pos ) {
		util::get_method< void( __thiscall* )( decltype( this ), vec3_t* ) >( this, GETEYEPOS )( this, pos );
	}

	__forceinline void ModifyEyePosition( CCSGOPlayerAnimState* state, vec3_t* pos ) {
		if ( !state ) {
			return;
		}

		//  if ( *(this + 0x50) && (*(this + 0x100) || *(this + 0x94) != 0.0 || !sub_102C9480(*(this + 0x50))) )
		if ( state->m_player &&
			( state->m_land || state->m_player->m_flDuckAmount( ) != 0.f || !state->m_player->GetGroundEntity( ) ) ) {
			auto v5 = 8;

			if ( v5 != -1 && state->m_player->m_BoneCache( ).m_pCachedBones ) {
				vec3_t head_pos(
					state->m_player->m_BoneCache( ).m_pCachedBones[ 8 ][ 0 ][ 3 ],
					state->m_player->m_BoneCache( ).m_pCachedBones[ 8 ][ 1 ][ 3 ],
					state->m_player->m_BoneCache( ).m_pCachedBones[ 8 ][ 2 ][ 3 ] 
				);

				auto v12 = head_pos;
				auto v7 = v12.z + 1.7;

				auto v8 = pos->z;
				if ( v8 > v7 ) // if (v8 > (v12 + 1.7))
				{
					float v13 = 0.f;
					float v3 = ( *pos ).z - v7;

					float v4 = ( v3 - 4.f ) * 0.16666667;
					if ( v4 >= 0.f )
						v13 = std::fminf( v4, 1.f );

					( *pos ).z = ( ( ( v7 - ( *pos ).z ) ) * ( ( ( v13 * v13 ) * 3.0 ) - ( ( ( v13 * v13 ) * 2.0 ) * v13 ) ) ) + ( *pos ).z;
				}
			}
		}
	}

	__forceinline vec3_t Weapon_ShootPosition( ) {
		vec3_t out{};
		util::get_method< void( __thiscall* )( void*, vec3_t& ) >( this, 285 )( this, out );
		if ( out.x == 0.f && out.y == 0.f && out.z == 0.f )
			return GetShootPosition( );
		return out;
	}

	__forceinline vec3_t GetShootPosition( ) {
		return m_vecOrigin( ) + m_vecViewOffset( );
	}

	__forceinline void UpdateClientSideAnimation( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, UPDATECLIENTSIDEANIMATION )( this );
	}

	__forceinline void UpdateCollisionBounds( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, UPDATECOLLISIONBOUNDS )( this );
	}

	// misc funcs.
	__forceinline void SetCollisionBounds( vec3_t& mins, vec3_t& maxs ) {
		float height, change_time;
		PVOID collideable;

		using Collideable_t = PVOID( __thiscall* )( decltype( this ) );

		// backup collision vars.
		height = m_flCollisionHeight( );
		change_time = m_flCollisionChangeTime( );

		collideable = ( *reinterpret_cast< Collideable_t** >( this ) )[ 3 ]( this );

		g_csgo.SetCollisionBounds( collideable, mins, maxs );

		// restore collision bounds.
		m_flCollisionHeight( ) = height;
		m_flCollisionChangeTime( ) = change_time;
	}

	__forceinline CStudioHdr* GetModelPtr( ) {
		using LockStudioHdr_t = void( __thiscall* )( decltype( this ) );

		if ( !m_studioHdr( ) )
			g_csgo.LockStudioHdr.as< LockStudioHdr_t >( )( this );

		return m_studioHdr( );
	}

	__forceinline Weapon* GetActiveWeapon( ) {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Weapon* >( m_hActiveWeapon( ) );
	}

	__forceinline Entity* GetObserverTarget( ) {
		return g_csgo.m_entlist->GetClientEntityFromHandle( m_hObserverTarget( ) );
	}

	__forceinline Entity* GetGroundEntity( ) {
		return g_csgo.m_entlist->GetClientEntityFromHandle( m_hGroundEntity( ) );
	}

	__forceinline void SetAnimLayers( C_AnimationLayer* layers ) {
		std::memcpy( m_AnimOverlay( ), layers, sizeof( C_AnimationLayer ) * 13 );
	}

	__forceinline void GetAnimLayers( C_AnimationLayer* layers ) {
		std::memcpy( layers, m_AnimOverlay( ), sizeof( C_AnimationLayer ) * 13 );
	}

	__forceinline void SetPoseParameters( float* poses ) {
		std::memcpy( m_flPoseParameter( ), poses, sizeof( float ) * 24 );
	}

	__forceinline void GetPoseParameters( float* poses ) {
		std::memcpy( poses, m_flPoseParameter( ), sizeof( float ) * 24 );
	}

	__forceinline bool ComputeHitboxSurroundingBox( vec3_t* mins, vec3_t* maxs ) {
		using ComputeHitboxSurroundingBox_t = bool( __thiscall* )( void*, vec3_t*, vec3_t* );

		return g_csgo.ComputeHitboxSurroundingBox.as< ComputeHitboxSurroundingBox_t >( )( this, mins, maxs );
	}

	__forceinline int GetSequenceActivity( int sequence ) {
		using GetSequenceActivity_t = int( __fastcall* )( void*, CStudioHdr*, int );

		CStudioHdr* studio_hdr = GetModelPtr( );
		if ( !studio_hdr )
			return -1;

		return g_csgo.GetSequenceActivity.as< GetSequenceActivity_t >( )( this, studio_hdr, sequence );
	}

	__forceinline bool HasC4( ) {
		using HasC4_t = bool( __thiscall* )( decltype( this ) );
		return g_csgo.HasC4.as< HasC4_t >( )( this );
	}

	__forceinline void InvalidateBoneCache( ) {
		CBoneAccessor* accessor = &m_BoneAccessor( );
		if ( !accessor )
			return;

		accessor->m_WritableBones = 0;
		accessor->m_ReadableBones = 0;

		m_iMostRecentModelBoneCounter( ) = 0;
		m_flLastBoneSetupTime( ) = std::numeric_limits< float >::lowest( );
	}

	__forceinline bool alive( ) {
		return m_lifeState( ) == LIFE_ALIVE;
	}

	__forceinline bool enemy( Player* from ) {
		if ( !from )
			return false;

		if ( m_iTeamNum( ) != from->m_iTeamNum( ) )
			return true;

		else if ( g_csgo.mp_teammates_are_enemies && g_csgo.mp_teammates_are_enemies->GetInt( ) )
			return true;

		return false;
	}
};

class WeaponInfo {
public:
	PAD( 0x14 );			// 0x0000
	int iMaxClip1;					// 0x0014
	int iMaxClip2;					// 0x0018
	int iDefaultClip1;				// 0x001C
	int iDefaultClip2;				// 0x0020
	int iPrimaryMaxReserveAmmo;		// 0x0024
	int iSecondaryMaxReserveAmmo;	// 0x0028
	const char* szWorldModel;		// 0x002C
	const char* szViewModel;		// 0x0030
	const char* szDroppedModel;		// 0x0034
	PAD( 0x50 );			// 0x0038
	const char* szHudName;			// 0x0088
	const char* szWeaponName;		// 0x008C
	PAD( 0x2 );			// 0x0090
	bool bIsMeleeWeapon;			// 0x0092
	PAD( 0x9 );			// 0x0093
	float flWeaponWeight;			// 0x009C
	PAD( 0x4 );			// 0x00A0
	int iSlot;						// 0x00A4
	int iPosition;					// 0x00A8
	PAD( 0x1C );			// 0x00AC
	int nWeaponType;				// 0x00C8
	PAD( 0x4 );			// 0x00CC
	int iWeaponPrice;				// 0x00D0
	int iKillAward;					// 0x00D4
	const char* szAnimationPrefix;	// 0x00D8
	float flCycleTime;				// 0x00DC
	float flCycleTimeAlt;			// 0x00E0
	PAD( 0x8 );			// 0x00E4
	bool bFullAuto;					// 0x00EC
	PAD( 0x3 );			// 0x00ED
	int iDamage;					// 0x00F0
	float flHeadShotMultiplier;		// 0x00F4
	float flArmorRatio;				// 0x00F8
	int iBullets;					// 0x00FC
	float flPenetration;			// 0x0100
	PAD( 0x8 );			// 0x0104
	float flRange;					// 0x010C
	float flRangeModifier;			// 0x0110
	float flThrowVelocity;			// 0x0114
	PAD( 0xC );			// 0x0118
	bool bHasSilencer;				// 0x0124
	PAD( 0xF );			// 0x0125
	float flMaxSpeed[ 2 ];			// 0x0134
	PAD( 0x4 );			// 0x013C
	float flSpread[ 2 ];				// 0x0140
	float flInaccuracyCrouch[ 2 ];	// 0x0148
	float flInaccuracyStand[ 2 ];		// 0x0150
	PAD( 0x8 );			// 0x0158
	float flInaccuracyJump[ 2 ];		// 0x0160
	float flInaccuracyLand[ 2 ];		// 0x0168
	float flInaccuracyLadder[ 2 ];	// 0x0170
	float flInaccuracyFire[ 2 ];		// 0x0178
	float flInaccuracyMove[ 2 ];		// 0x0180
	float flInaccuracyReload;		// 0x0188
	int iRecoilSeed;				// 0x018C
	float flRecoilAngle[ 2 ];			// 0x0190
	float flRecoilAngleVariance[ 2 ];	// 0x0198
	float flRecoilMagnitude[ 2 ];		// 0x01A0
	float flRecoilMagnitudeVariance[ 2 ]; // 0x01A8
	int iSpreadSeed;				// 0x01B0

	__forceinline bool IsGun( ) const {
		switch ( this->nWeaponType ) {
			case WEAPONTYPE_PISTOL:
			case WEAPONTYPE_SUBMACHINEGUN:
			case WEAPONTYPE_RIFLE:
			case WEAPONTYPE_SHOTGUN:
			case WEAPONTYPE_SNIPER_RIFLE:
			case WEAPONTYPE_MACHINEGUN:
				return true;
		}

		return false;
	}
};

class IRefCounted {
private:
	volatile long refCount;

public:
	virtual void destructor( char bDelete ) = 0;
	virtual bool OnFinalRelease( ) = 0;

	void unreference( ) {
		if ( InterlockedDecrement( &refCount ) == 0 && OnFinalRelease( ) ) {
			destructor( 1 );
		}
	}
};

class Weapon : public Entity {
public:
	using ref_vec_t = CUtlVector< IRefCounted* >;

	// netvars / etc.
	__forceinline ref_vec_t& m_CustomMaterials( ) {
		return get< ref_vec_t >( g_entoffsets.m_CustomMaterials );
	}

	__forceinline ref_vec_t& m_CustomMaterials2( ) {
		return get< ref_vec_t >( g_entoffsets.m_CustomMaterials2 );
	}

	__forceinline ref_vec_t& m_VisualsDataProcessors( ) {
		return get< ref_vec_t >( g_entoffsets.m_VisualsDataProcessors );
	}

	__forceinline bool& m_bCustomMaterialInitialized( ) {
		return get< bool >( g_entoffsets.m_bCustomMaterialInitialized );
	}

	__forceinline int& m_iItemDefinitionIndex( ) {
		return get< int >( g_entoffsets.m_iItemDefinitionIndex );
	}

	__forceinline int& m_iClip1( ) {
		return get< int >( g_entoffsets.m_iClip1 );
	}

	__forceinline int& m_iPrimaryReserveAmmoCount( ) {
		return get< int >( g_entoffsets.m_iPrimaryReserveAmmoCount );
	}

	__forceinline int& m_Activity( ) {
		return get< int >( g_entoffsets.m_Activity );
	}

	__forceinline float& m_fFireDuration( ) {
		return get< float >( g_entoffsets.m_fFireDuration );
	}

	__forceinline int& m_iBurstShotsRemaining( ) {
		return get< int >( g_entoffsets.m_iBurstShotsRemaining );
	}

	__forceinline float& m_flNextPrimaryAttack( ) {
		return get< float >( g_entoffsets.m_flNextPrimaryAttack );
	}

	__forceinline float& m_flNextSecondaryAttack( ) {
		return get< float >( g_entoffsets.m_flNextSecondaryAttack );
	}

	__forceinline float& m_flThrowStrength( ) {
		return get< float >( g_entoffsets.m_flThrowStrength );
	}

	__forceinline float& m_fNextBurstShot( ) {
		return get< float >( g_entoffsets.m_fNextBurstShot );
	}

	__forceinline int& m_zoomLevel( ) {
		return get< int >( g_entoffsets.m_zoomLevel );
	}

	__forceinline float& m_flRecoilIndex( ) {
		return get< float >( g_entoffsets.m_flRecoilIndex );
	}

	__forceinline int& m_weaponMode( ) {
		return get< int >( g_entoffsets.m_weaponMode );
	}

	__forceinline int& m_nFallbackPaintKit( ) {
		return get< int >( g_entoffsets.m_nFallbackPaintKit );
	}

	__forceinline int& m_nFallbackStatTrak( ) {
		return get< int >( g_entoffsets.m_nFallbackStatTrak );
	}

	__forceinline int& m_nFallbackSeed( ) {
		return get< int >( g_entoffsets.m_nFallbackSeed );
	}

	__forceinline float& m_flFallbackWear( ) {
		return get< float >( g_entoffsets.m_flFallbackWear );
	}

	__forceinline int& m_iViewModelIndex( ) {
		return get< int >( g_entoffsets.m_iViewModelIndex );
	}

	__forceinline int& m_iWorldModelIndex( ) {
		return get< int >( g_entoffsets.m_iWorldModelIndex );
	}

	__forceinline int& m_iAccountID( ) {
		return get< int >( g_entoffsets.m_iAccountID );
	}

	__forceinline int& m_iItemIDHigh( ) {
		return get< int >( g_entoffsets.m_iItemIDHigh );
	}

	__forceinline int& m_iEntityQuality( ) {
		return get< int >( g_entoffsets.m_iEntityQuality );
	}

	__forceinline int& m_OriginalOwnerXuidLow( ) {
		return get< int >( g_entoffsets.m_OriginalOwnerXuidLow );
	}

	__forceinline int& m_OriginalOwnerXuidHigh( ) {
		return get< int >( g_entoffsets.m_OriginalOwnerXuidHigh );
	}

	__forceinline bool& m_bPinPulled( ) {
		return get< bool >( g_entoffsets.m_bPinPulled );
	}

	__forceinline float& m_fThrowTime( ) {
		return get< float >( g_entoffsets.m_fThrowTime );
	}

	__forceinline EHANDLE& m_hWeapon( ) {
		return get< EHANDLE >( g_entoffsets.m_hWeapon );
	}

	__forceinline EHANDLE& m_hWeaponWorldModel( ) {
		return get< EHANDLE >( g_entoffsets.m_hWeaponWorldModel );
	}

	__forceinline EHANDLE& m_hOwnerEntity( ) {
		return get< EHANDLE >( g_entoffsets.m_hOwnerEntity );
	}

	__forceinline float& m_flConstraintRadius( ) {
		return get< float >( g_entoffsets.m_flConstraintRadius );
	}

	__forceinline float& m_fLastShotTime( ) {
		return get< float >( g_entoffsets.m_fLastShotTime );
	}

public:
	enum indices : size_t {
		SETMODELINDEX = 75,
		GETSPREAD = 453,
		GETWPNDATA = 461, // C_WeaponCSBaseGun::GetCSWpnData
		GETINACCURACY = 483,
		UPDATEACCURACYPENALTY = 484,
	};

public:
	// virtuals.
	__forceinline void SetGloveModelIndex( int index ) {
		return util::get_method< void( __thiscall* )( void*, int ) >( this, SETMODELINDEX )( this, index );
	}

	__forceinline WeaponInfo* GetWpnData( ) {
		return util::get_method< WeaponInfo * ( __thiscall* )( void* ) >( this, GETWPNDATA )( this );
	}

	__forceinline float GetInaccuracy( ) {
		return util::get_method< float( __thiscall* )( void* ) >( this, GETINACCURACY )( this );
	}

	__forceinline float GetSpread( ) {
		return util::get_method< float( __thiscall* )( void* ) >( this, GETSPREAD )( this );
	}

	__forceinline void UpdateAccuracyPenalty( ) {
		return util::get_method< void( __thiscall* )( void* ) >( this, UPDATEACCURACYPENALTY )( this );
	}

	// misc funcs.
	__forceinline Weapon* GetWeapon( ) {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Weapon* >( m_hWeapon( ) );
	}

	__forceinline Weapon* GetWeaponWorldModel( ) {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Weapon* >( m_hWeaponWorldModel( ) );
	}

	__forceinline bool IsKnife( ) {
		return ( GetWpnData( )->nWeaponType == WEAPONTYPE_KNIFE && m_iItemDefinitionIndex( ) != WEAPON_TASER );
	}

	__forceinline vec3_t CalculateSpread( int seed, float inaccuracy, float spread, bool revolver2 = false ) {
		WeaponInfo* wep_info;
		int        item_def_index;
		float      recoil_index, r1, r2, r3, r4, s1, c1, s2, c2;

		// if we have no bullets, we have no spread.
		wep_info = GetWpnData( );
		if ( !wep_info || !wep_info->iBullets )
			return {};

		// get some data for later.
		item_def_index = m_iItemDefinitionIndex( );
		recoil_index = m_flRecoilIndex( );

		// seed randomseed.
		g_csgo.RandomSeed( ( seed & 0xff ) + 1 );

		// generate needed floats.
		r1 = g_csgo.RandomFloat( 0.f, 1.f );
		r2 = g_csgo.RandomFloat( 0.f, math::pi_2 );

		// todo - dex; need to make sure this is right for shotguns still.
		//             the 3rd arg to get_shotgun_spread is actually using the bullet iterator
		//             should also probably check for if the weapon is a shotgun, but it seems like GetShotgunSpread modifies some the r1 - r4 vars...
		// for( int i{}; i < wep_info->m_bullets; ++i )
		/*
			// shotgun shit, or first bullet
			if ( !bullet_i
			  || ((int (__thiscall *)(void ***))weapon_accuracy_shotgun_spread_patterns[13])(&weapon_accuracy_shotgun_spread_patterns) )
			{
			  r1 = RandomFloat(0, 0x3F800000);// rand 0.f, 1.f
			  r2 = RandomFloat(0, 0x40C90FDB);// rand 0.f, pi * 2.f
			  v47 = *(_DWORD *)v45;
			  r2_ = r2;
			  v48 = (*(int (__thiscall **)(int))(v47 + 48))(v45);

			  // not revolver?
			  if ( v48 != sub_101D9B10(&dword_14FA0DE0) || a7 != 1 )
			  {

				// not negev?
				v50 = (*(int (__thiscall **)(int))(*(_DWORD *)v45 + 48))(v45);
				if ( v50 != sub_101D9B10(&dword_14FA0DEC) || recoil_index >= 3.0 )
				{
				  r1_ = r1;
				}
				else
				{
				  r1__ = r1;
				  recoil_index_it1 = 3;
				  do
				  {
					--recoil_index_it1;
					r1__ = r1__ * r1__;
				  }
				  while ( (float)recoil_index_it1 > recoil_index );
				  r1_ = 1.0 - r1__;
				}
			  }
			  else
			  {
				r1_ = 1.0 - (float)(r1 * r1);
			  }

			  // some convar stuff / etc
			  if ( max_inaccuracy )
				r1_ = 1.0;

			  bullet_i = v87;
			  bullet_i1 = r1_ * v104;

			  if ( only_up_is_not_0 )
				v53 = 1.5707964;
			  else
				v53 = r2_;

			  r2_ = v53;
			}

			if ( ((int (__thiscall *)(void ***))weapon_accuracy_shotgun_spread_patterns[13])(&weapon_accuracy_shotgun_spread_patterns) )
			{
			  null0 = v54;
			  m_iItemDefinitionIndex = (**(int (***)(void))LODWORD(v88))();
			  get_shotgun_spread(
				m_iItemDefinitionIndex,
				null0,
				bullet_i + m_BulletsPerShot * (signed int)recoil_index,
				&r4,
				&r3);
			}
			else
			{
			  r3 = RandomFloat(0, 0x3F800000);// rand 0.f, 1.f
			  r4 = RandomFloat(0, 0x40C90FDB);// rand 0.f, pi * 2.f
			}
		*/

		if ( /*wep_info->m_weapon_type == WEAPONTYPE_SHOTGUN &&*/ g_csgo.weapon_accuracy_shotgun_spread_patterns->GetInt( ) > 0 )
			g_csgo.GetShotgunSpread( item_def_index, 0, 0 /*bullet_i*/ + wep_info->iBullets * recoil_index, &r4, &r3 );

		else {
			r3 = g_csgo.RandomFloat( 0.f, 1.f );
			r4 = g_csgo.RandomFloat( 0.f, math::pi_2 );
		}

		// revolver secondary spread.
		if ( item_def_index == WEAPON_REVOLVER && revolver2 ) {
			r1 = 1.f - ( r1 * r1 );
			r3 = 1.f - ( r3 * r3 );
		}

		// negev spread.
		else if ( item_def_index == WEAPON_NEGEV && recoil_index < 3.f ) {
			for ( int i{ 3 }; i > recoil_index; --i ) {
				r1 *= r1;
				r3 *= r3;
			}

			r1 = 1.f - r1;
			r3 = 1.f - r3;
		}

		// get needed sine / cosine values.
		c1 = std::cos( r2 );
		c2 = std::cos( r4 );
		s1 = std::sin( r2 );
		s2 = std::sin( r4 );

		// calculate spread vector.
		return {
			( c1 * ( r1 * inaccuracy ) ) + ( c2 * ( r3 * spread ) ),
			( s1 * ( r1 * inaccuracy ) ) + ( s2 * ( r3 * spread ) ),
			0.f
		};
	}

	__forceinline vec3_t CalculateSpread( int seed, bool revolver2 = false ) {
		return CalculateSpread( seed, GetInaccuracy( ), GetSpread( ), revolver2 );
	}

	__forceinline std::string GetLocalizedName( ) {
		WeaponInfo* wep_info = GetWpnData( );
		if ( !wep_info )
			return {};

		return util::WideToMultiByte( g_csgo.m_localize->Find( wep_info->szHudName ) );
	}
};

class CTraceFilterSimple_game {
public:
	void* m_vmt;
	const Entity* m_pass_ent1;
	int             m_collision_group;
	ShouldHitFunc_t m_shouldhit_check_fn;

public:
	__forceinline CTraceFilterSimple_game( ) :
		m_vmt{ g_csgo.CTraceFilterSimple_vmt.as< void* >( ) },
		m_pass_ent1{},
		m_collision_group{},
		m_shouldhit_check_fn{} {
	}

	__forceinline CTraceFilterSimple_game( const Entity* pass_ent1, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr ) :
		m_vmt{ g_csgo.CTraceFilterSimple_vmt.as< void* >( ) },
		m_pass_ent1{ pass_ent1 },
		m_collision_group{ collision_group },
		m_shouldhit_check_fn{ shouldhit_check_fn } {
	}

	__forceinline bool ShouldHitEntity( Entity* entity, int contents_mask ) {
		// note - dex; game is dumb, this gets the real vmt.
		void* real_vmt = *( void** )m_vmt;

		return util::get_method< bool( __thiscall* )( void*, Entity*, int ) >( real_vmt, 0 )( real_vmt, entity, contents_mask );
	}

	// note - dex; don't really care about calling the virtuals for these two functions, they only set members in the class for us.
	__forceinline void SetPassEntity( Entity* pass_ent1 ) {
		m_pass_ent1 = pass_ent1;

		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 2 )( m_vmt, pass_ent1 );
	}

	__forceinline void SetCollisionGroup( int collision_group ) {
		m_collision_group = collision_group;

		// util::get_method< void (__thiscall *)( void *, int ) >( m_vmt, 3 )( m_vmt, collision_group );
	}
};

class CTraceFilterSkipTwoEntities_game {
public:
	void* m_vmt;
	const Entity* m_pass_ent1;
	int             m_collision_group;
	ShouldHitFunc_t m_shouldhit_check_fn;
	const Entity* m_pass_ent2;

public:
	__forceinline CTraceFilterSkipTwoEntities_game( ) :
		m_vmt{ g_csgo.CTraceFilterSkipTwoEntities_vmt.as< void* >( ) },
		m_pass_ent1{},
		m_collision_group{},
		m_shouldhit_check_fn{},
		m_pass_ent2{} {
	}

	__forceinline CTraceFilterSkipTwoEntities_game( const Entity* pass_ent1, const Entity* pass_ent2, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr ) :
		m_vmt{ g_csgo.CTraceFilterSkipTwoEntities_vmt.as< void* >( ) },
		m_pass_ent1{ pass_ent1 },
		m_collision_group{ collision_group },
		m_shouldhit_check_fn{ shouldhit_check_fn },
		m_pass_ent2{ pass_ent2 } {
	}

	__forceinline bool ShouldHitEntity( Entity* entity, int contents_mask ) {
		// note - dex; game is dumb, this gets the real vmt.
		void* real_vmt = *( void** )m_vmt;

		return util::get_method< bool( __thiscall* )( void*, Entity*, int ) >( m_vmt, 0 )( m_vmt, entity, contents_mask );
	}

	// note - dex; don't really care about calling the virtuals for these two functions, they only set members in the class for us.
	__forceinline void SetPassEntity( Entity* pass_ent1 ) {
		m_pass_ent1 = pass_ent1;

		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 2 )( m_vmt, pass_ent1 );
	}

	__forceinline void SetCollisionGroup( int collision_group ) {
		m_collision_group = collision_group;

		// util::get_method< void (__thiscall *)( void *, int ) >( m_vmt, 3 )( m_vmt, collision_group );
	}

	__forceinline void SetPassEntity2( Entity* pass_ent2 ) {
		m_pass_ent2 = pass_ent2;
		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 4 )( m_vmt, pass_ent2 );
	}
};