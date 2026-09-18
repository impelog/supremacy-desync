#pragma once

struct AnimData_t {
	int                  m_flags;
	float                m_duck_amount;
	float                m_lby;
	ang_t                m_eye_angles;
	float                m_feet_weight;
	float                m_feet_cycle;
	CCSGOPlayerAnimState m_state;

	void backup( Player* player );
	void restore( Player* player );
};

class Animations {
public:
	AnimData_t m_data[ 65 ];

	void SimulateAnimation( Player* player, float simtime );
	void SimulateSide( Player* player, LagRecord* record, int side );
	void RestoreAnimation( Player* player, LagRecord* record );

	void UpdatePlayer( AimPlayer* data, LagRecord* record );
};

extern Animations g_animations;
