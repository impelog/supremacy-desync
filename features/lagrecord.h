#pragma once

// pre-declare.
class LagRecord;

class BackupRecord {
public:
	BoneArray* m_bones;
	int        m_bone_count;
	vec3_t     m_origin, m_abs_origin;
	vec3_t     m_mins;
	vec3_t     m_maxs;
	ang_t      m_abs_ang;

public:
	__forceinline void store( Player* player ) {
		// get bone cache ptr.
		CBoneCache* cache = &player->m_BoneCache( );

		// store bone data.
		m_bones = cache->m_pCachedBones;
		m_bone_count = cache->m_CachedBoneCount;
		m_origin = player->m_vecOrigin( );
		m_mins = player->m_vecMins( );
		m_maxs = player->m_vecMaxs( );
		m_abs_origin = player->GetAbsOrigin( );
		m_abs_ang = player->GetAbsAngles( );
	}

	__forceinline void restore( Player* player ) {
		// get bone cache ptr.
		CBoneCache* cache = &player->m_BoneCache( );

		cache->m_pCachedBones = m_bones;
		cache->m_CachedBoneCount = m_bone_count;

		player->m_vecOrigin( ) = m_origin;
		player->SetCollisionBounds( m_mins, m_maxs );
		player->SetAbsAngles( m_abs_ang );
		player->SetAbsOrigin( m_origin );
	}
};

struct AnimSide_t {
	ang_t            m_angles;
	float            m_foot_yaw;
	float            m_cur_feet_yaw;
	float            m_poses[ 24 ];
	C_AnimationLayer m_layers[ 13 ];
};

class LagRecord {
public:
	// data.
	Player* m_player;
	float   m_immune;
	int     m_tick;
	int     m_lag;
	bool    m_dormant;

	// netvars.
	float  m_sim_time;
	float  m_old_sim_time;
	int    m_flags;
	vec3_t m_origin;
	vec3_t m_old_origin;
	vec3_t m_velocity;
	vec3_t m_mins;
	vec3_t m_maxs;
	ang_t  m_eye_angles;
	ang_t  m_abs_ang;
	float  m_body;
	float  m_duck;

	// anim stuff.
	C_AnimationLayer m_layers[ 13 ];
	float            m_poses[ 24 ];

	// bone stuff.
	bool      m_setup;
	BoneArray m_bones[ 128 ];

	// lagfix stuff.
	bool   m_broke_lc;
	bool   m_shifting_tickbase;
	vec3_t m_pred_origin;
	vec3_t m_pred_velocity;
	float  m_pred_time;
	int    m_pred_flags;

	// resolver stuff.
	bool       m_shot;
	bool       m_accurate_velocity;
	float      m_away;
	float      m_anim_time;
	int        m_anim_side;
	AnimSide_t m_anim_sides[ 3 ];

	LagRecord* m_previous;
	float  m_interp_time;
	int m_weapon_id;
public:

	__forceinline LagRecord( ) :
		m_setup{ false },
		m_broke_lc{ false },
		m_shifting_tickbase{ false },
		m_shot{ false },
		m_accurate_velocity{ false },
		m_anim_side{ 0 },
		m_lag{},
		m_previous{ nullptr } {
	}

	__forceinline LagRecord( Player* player ) :
		m_setup{ false },
		m_broke_lc{ false },
		m_shifting_tickbase{ false },
		m_shot{ false },
		m_accurate_velocity{ false },
		m_anim_side{ 0 },
		m_lag{},
		m_previous{ nullptr } {

		store( player );
	}

	__forceinline ~LagRecord( ) = default;

	void invalidate( ) {
		m_setup = false;
	}

	// function: allocates memory and stores all data needed.
	void store( Player* player ) {
		m_setup = false;

		// player data.
		m_player = player;
		m_immune = player->m_fImmuneToGunGameDamageTime( );
		m_tick = g_csgo.m_cl->clockDriftMgr.m_nServerTick;
		m_anim_side = 0;
		m_accurate_velocity = false;

		// netvars.
		m_pred_time = m_sim_time = player->m_flSimulationTime( );
		m_old_sim_time = player->m_flOldSimulationTime( );
		m_pred_flags = m_flags = player->m_fFlags( );
		m_pred_origin = m_origin = player->m_vecOrigin( );
		m_old_origin = player->m_vecOldOrigin( );
		m_eye_angles = player->m_angEyeAngles( );
		m_abs_ang = player->GetAbsAngles( );
		m_body = player->m_flLowerBodyYawTarget( );
		m_mins = player->m_vecMins( );
		m_duck = player->m_flDuckAmount( );
		m_maxs = player->m_vecMaxs( );
		m_pred_velocity = m_velocity = player->m_vecVelocity( );
		{
			Weapon* w = player->GetActiveWeapon( );
			m_weapon_id = w ? w->m_iItemDefinitionIndex( ) : 0;
		}

		// save networked animlayers.
		player->GetAnimLayers( m_layers );

		// normalize eye angles.
		m_eye_angles.normalize( );
		math::clamp( m_eye_angles.x, -90.f, 90.f );

		m_lag = game::TIME_TO_TICKS( m_sim_time - m_old_sim_time );
		m_anim_time = m_old_sim_time + g_csgo.m_globals->m_interval;
	}

	// function: restores 'predicted' variables to their original.
	__forceinline void predict( ) {
		m_broke_lc = false;
		m_shifting_tickbase = false;
		m_pred_origin = m_origin;
		m_pred_velocity = m_velocity;
		m_pred_time = m_sim_time;
		m_pred_flags = m_flags;
	}

	// function: writes current record to bone cache.
	__forceinline void cache( ) {
		// get bone cache ptr.
		CBoneCache* cache = &m_player->m_BoneCache( );

		cache->m_pCachedBones = m_bones;
		cache->m_CachedBoneCount = 128;

		m_player->m_vecOrigin( ) = m_pred_origin;
		//m_player->m_vecMins( )   = m_mins;
		//m_player->m_vecMaxs( )   = m_maxs;
		m_player->SetCollisionBounds( m_mins, m_maxs );
		m_player->SetAbsAngles( m_abs_ang );
		m_player->SetAbsOrigin( m_pred_origin );
	}

	__forceinline bool dormant( ) const {
		return m_dormant;
	}

	__forceinline bool immune( ) const {
		return m_immune > 0.f;
	}

	inline bool valid( ) const {
		if ( m_broke_lc || m_shifting_tickbase )
			return false;
		auto nci = g_csgo.m_engine->GetNetChannelInfo( );
		if ( !nci )
			return false;
		float latency = nci->GetLatency( INetChannel::FLOW_INCOMING ) + nci->GetLatency( INetChannel::FLOW_OUTGOING );
		float lerp = g_cl.m_lerp;
		if ( lerp == 0.f )
			lerp = game::GetClientInterpAmount( );
		float max_unlag = g_csgo.sv_maxunlag ? g_csgo.sv_maxunlag->GetFloat( ) : 0.2f;
		int last_server = g_csgo.m_cl ? g_csgo.m_cl->clockDriftMgr.m_nServerTick : game::TIME_TO_TICKS( g_csgo.m_globals->m_curtime );
		int possible_future = last_server + game::TIME_TO_TICKS( latency ) + 8;
		int dead = ( int )( game::TICKS_TO_TIME( last_server ) + latency - max_unlag );
		if ( m_sim_time <= ( float )dead || game::TIME_TO_TICKS( m_sim_time + lerp ) > possible_future )
			return false;
		float correct = std::clamp( latency + lerp, 0.f, max_unlag );
		int corrected = g_cl.m_corrected_tickbase ? g_cl.m_corrected_tickbase : ( g_cl.m_local ? g_cl.m_local->m_nTickBase( ) : game::TIME_TO_TICKS( g_csgo.m_globals->m_curtime ) );
		float time = game::TICKS_TO_TIME( corrected - 1 );
		float delta = correct - ( time - m_sim_time );
		return std::fabsf( delta ) < 0.2f;
	}
};