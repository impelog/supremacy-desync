#pragma once

class InputPrediction {
public:
	float m_curtime;
	float m_frametime;

	int    m_predicted_cmd_number;
	int    m_predicted_tick_base;
	ang_t  m_predicted_view_punch;
	ang_t  m_predicted_aim_punch;
	ang_t  m_predicted_aim_punch_vel;
	vec3_t m_predicted_view_offset;
	vec3_t m_predicted_origin;

public:
	void update( );
	void run( );
	void restore( );
	void DetectPredictionErrors( int stage );
};

extern InputPrediction g_inputpred;