#pragma once

class ShotRecord {
public:
	__forceinline ShotRecord( ) : 
		m_target{}, m_target_index{}, m_target_name{}, m_record{}, m_record_origin{}, m_record_mins{}, m_record_maxs{}, m_record_abs_ang{},
		m_resolved{}, m_anim_side{}, m_hitchance{}, m_time{}, m_lat{}, m_damage{}, m_pos{}, m_target_pos{}, m_impact_pos{}, m_impacts{}, m_shot_tick{}, m_impact_tick{},
		m_matched{}, m_impacted{}, m_hurt{}, m_hurt_damage{}, m_hurt_group{}, m_hurt_hp{},
		m_local_death{}, m_target_death{}, m_processed{} {}

public:
	Player*              m_target;
	int                  m_target_index;
	std::string          m_target_name;
	LagRecord*           m_record;
	BoneArray            m_matrix[ 128 ];
	vec3_t               m_record_origin;
	vec3_t               m_record_mins;
	vec3_t               m_record_maxs;
	ang_t                m_record_abs_ang;
	bool                 m_resolved;
	int                  m_anim_side;
	float                m_hitchance;
	float                m_time, m_lat, m_damage;
	vec3_t               m_pos;
	vec3_t               m_target_pos;
	vec3_t               m_impact_pos;
	std::vector< vec3_t > m_impacts;
	int                  m_shot_tick;
	int                  m_impact_tick;
	bool                 m_matched;
	bool                 m_impacted;
	bool                 m_hurt;
	int                  m_hurt_damage;
	int                  m_hurt_group;
	int                  m_hurt_hp;
	bool                 m_local_death;
	bool                 m_target_death;
	bool                 m_processed;
};

class VisualImpactData_t {
public:
    vec3_t m_impact_pos, m_shoot_pos;
    int    m_tickbase;
    bool   m_ignore, m_hit_player;

public:
    __forceinline VisualImpactData_t( const vec3_t &impact_pos, const vec3_t &shoot_pos, int tickbase ) : 
        m_impact_pos{ impact_pos }, m_shoot_pos{ shoot_pos }, m_tickbase{ tickbase }, m_ignore{ false }, m_hit_player{ false } {}
};

class ImpactRecord {
public:
	__forceinline ImpactRecord( ) : m_shot{}, m_pos{}, m_tick{} {}

public:
	ShotRecord* m_shot;
	int         m_tick;
	vec3_t      m_pos;
};

class HitRecord {
public:
	__forceinline HitRecord( ) : m_impact{}, m_group{ -1 }, m_damage{} {}

public:
	ImpactRecord* m_impact;
	int           m_group;
	float         m_damage;
};

class Shots {
private:
    std::array< std::string, 8 > m_groups = {
        XOR( "body" ),
		XOR( "head" ),
		XOR( "chest" ),
		XOR( "stomach" ),
		XOR( "left arm" ),
		XOR( "right arm" ),
		XOR( "left leg" ),
		XOR( "right leg" )
    };

public:
	void OnShotFire( Player* target, float damage, int bullets, LagRecord* record, const vec3_t& target_pos, float hitchance = 0.f, bool resolved = false );
	void OnImpact( IGameEvent* evt );
	void OnHurt( IGameEvent* evt );
	void OnPlayerDeath( IGameEvent* evt );
	void OnNetUpdate( );
	void Reset( );

public:
	std::deque< ShotRecord >          m_shots;
    std::vector< VisualImpactData_t > m_vis_impacts;
	std::deque< ImpactRecord >        m_impacts;
	std::deque< HitRecord >           m_hits;
};

extern Shots g_shots;