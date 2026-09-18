#pragma once

class AimPlayer;

class LagCompensation {
public:
	enum LagType : size_t {
		INVALID = 0,
		CONSTANT,
		ADAPTIVE,
		RANDOM,
	};

public:
	bool StartPrediction( AimPlayer* player );
	void PlayerMove( LagRecord* record );
	void PredictAnimations( CCSGOPlayerAnimState* state, LagRecord* record );
};

extern LagCompensation g_lagcomp;