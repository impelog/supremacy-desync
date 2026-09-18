#pragma once

struct OffScreenDamageData_t {
    float m_time, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t( ) : m_time{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t( float time, float m_color_step, Color color ) : m_time{ time }, m_color{ color } {}
};

class Visuals {
public:
	std::array< bool, 64 >                  m_draw;
	std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
	vec2_t                                  m_crosshair;
	bool                                    m_thirdperson;
	float					                m_hit_start, m_hit_end, m_hit_duration;

    // info about planted c4.
    bool        m_c4_planted;
    Entity      *m_planted_c4;
    float       m_planted_c4_explode_time;
    vec3_t      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    std::string m_last_bombsite;

	IMaterial* smoke1;
	IMaterial* smoke2;
	IMaterial* smoke3;
	IMaterial* smoke4;

    std::unordered_map< int, char > m_weapon_icons = {
    	{ WEAPON_DEAGLE, 'F' },
    	{ WEAPON_ELITE, 'S' },
    	{ WEAPON_FIVESEVEN, 'U' },
    	{ WEAPON_GLOCK, 'C' },
    	{ WEAPON_AK47, 'B' },
    	{ WEAPON_AUG, 'E' },
    	{ WEAPON_AWP, 'R' },
    	{ WEAPON_FAMAS, 'T' },
    	{ WEAPON_G3SG1, 'I' },
    	{ WEAPON_GALILAR, 'V' },
    	{ WEAPON_M249, 'Z' },
    	{ WEAPON_M4A1, 'W' },
    	{ WEAPON_MAC10, 'L' },
    	{ WEAPON_P90, 'M' },
    	{ WEAPON_UMP45, 'Q' },
    	{ WEAPON_XM1014, ']' },
    	{ WEAPON_BIZON, 'D' },
    	{ WEAPON_MAG7, 'K' },
    	{ WEAPON_NEGEV, 'Z' },
    	{ WEAPON_SAWEDOFF, 'K' },
    	{ WEAPON_TEC9, 'C' },
    	{ WEAPON_TASER, 'Y' },
    	{ WEAPON_HKP2000, 'Y' },
    	{ WEAPON_MP7, 'X' },
    	{ WEAPON_MP9, 'D' },
    	{ WEAPON_NOVA, 'K' },
    	{ WEAPON_P250, 'Y' },
    	{ WEAPON_SCAR20, 'I' },
    	{ WEAPON_SG556, '[' },
    	{ WEAPON_SSG08, 'N' },
    	{ WEAPON_KNIFE, 'J' },
    	{ WEAPON_FLASHBANG, 'G' },
    	{ WEAPON_HEGRENADE, 'H' },
    	{ WEAPON_SMOKEGRENADE, 'P' },
    	{ WEAPON_MOLOTOV, 'H' },
    	{ WEAPON_DECOY, 'G' },
    	{ WEAPON_FIREBOMB, 'H' },
    	{ WEAPON_C4, '\\' },
    	{ WEAPON_KNIFE_T, 'J' },
    	{ WEAPON_M4A1_SILENCER, 'W' },
    	{ WEAPON_USP_SILENCER, 'Y' },
    	{ WEAPON_CZ75A, 'Y' },
    	{ WEAPON_REVOLVER, 'F' },
    	{ WEAPON_KNIFE_BAYONET, 'J' },
    	{ WEAPON_KNIFE_FLIP, 'J' },
    	{ WEAPON_KNIFE_GUT, 'J' },
    	{ WEAPON_KNIFE_KARAMBIT, 'J' },
    	{ WEAPON_KNIFE_M9_BAYONET, 'J' },
    	{ WEAPON_KNIFE_TACTICAL, 'J' },
    	{ WEAPON_KNIFE_FALCHION, 'J' },
    	{ WEAPON_KNIFE_SURVIVAL_BOWIE, 'J' },
    	{ WEAPON_KNIFE_BUTTERFLY, 'J' },
    	{ WEAPON_KNIFE_PUSH, 'J' },
    };

public:
	static void ModulateWorld( );
	void ThirdpersonThink( );
	void Hitmarker( );
	void NoSmoke( );
	void think( );
	void Spectators( );
	void StatusIndicators( );
	void SpreadCrosshair( );
    void PenetrationCrosshair( );
    void DrawPlantedC4();
	void draw( Entity* ent );
	void DrawProjectile( Weapon* ent );
	void DrawItem( Weapon* item );
	void OffScreen( Player* player, int alpha );
	void DrawPlayer( Player* player );
	bool GetPlayerBoxRect( Player* player, Rect& box );
	void DrawHistorySkeleton( Player* player, int opacity );
	void DrawSkeleton( Player* player, int opacity );
	void RenderGlow( );
	void DrawHitboxMatrix( LagRecord* record, Color col, float time );
    void DrawBeams( );
	void DebugAimbotPoints( Player* player );
	void DrawAutoPeek( );
};

extern Visuals g_visuals;