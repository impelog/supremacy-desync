#pragma once

class ShotRecord;

class Resolver {
public:
	LagRecord* FindIdealRecord( AimPlayer* data );
	LagRecord* FindLastRecord( AimPlayer* data );

	void MatchShot( AimPlayer* data, LagRecord* record );
	void SolveStand( AimPlayer* data, Player* player, LagRecord* record );
	void SolveMove( AimPlayer* data, Player* player, LagRecord* record );
	void ResolveAngles( Player* player, LagRecord* record );

public:
	std::array< vec3_t, 64 > m_impacts;
};

extern Resolver g_resolver;