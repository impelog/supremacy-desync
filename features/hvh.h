#pragma once

class HVH {
public:
	int    m_pitch;
	int    m_yaw;
	float  m_jitter_range;
	float  m_rot_range;
	float  m_rot_speed;
	float  m_rand_update;
	float  m_yaw_offset;
	int    m_base_angle;

	bool   m_step_switch;
	int    m_random_lag;
	float  m_next_random_update;
	float  m_random_angle;
	float  m_direction;
	float  m_view;

	bool   m_choke_cycle_switch;
	int    m_prev_tick_count;
	float  m_delta;
	bool   m_fake_duck;

public:
	void AtTarget( float& yaw );
	void SelectYaw( float& yaw );
	void SetPitch( );
	void Process( float yaw );
	void IdealPitch( );
	bool DoEdgeAntiAim( Player* player, ang_t& out );
	bool DoFreestand( float& yaw );
	void AntiAim( );
	void FakeDuck( );
	void SendPacket( );
};

extern HVH g_hvh;