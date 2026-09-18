#include "includes.h"

Resolver g_resolver{};

LagRecord* Resolver::FindIdealRecord( AimPlayer* data ) {
	if ( data->m_records.empty( ) )
		return nullptr;

	for ( const auto& it : data->m_records ) {
		if ( it->dormant( ) || it->immune( ) || !it->valid( ) )
			continue;

		return it.get( );
	}

	return nullptr;
}

LagRecord* Resolver::FindLastRecord( AimPlayer* data ) {
	if ( data->m_records.empty( ) )
		return nullptr;

	for ( auto it = data->m_records.crbegin( ); it != data->m_records.crend( ); ++it ) {
		auto current = it->get( );
		if ( current->valid( ) && !current->immune( ) && !current->dormant( ) )
			return current;
	}

	return nullptr;
}

void Resolver::MatchShot( AimPlayer* data, LagRecord* record ) {
	if ( g_menu.main.config.mode.get( ) == 1 )
		return;

	float shoot_time = -1.f;
	Weapon* weapon = data->m_player->GetActiveWeapon( );
	if ( weapon )
		shoot_time = weapon->m_fLastShotTime( ) + g_csgo.m_globals->m_interval;

	if ( game::TIME_TO_TICKS( shoot_time ) == game::TIME_TO_TICKS( record->m_sim_time ) ) {
		if ( record->m_lag <= 2 )
			record->m_shot = true;
		else if ( record->m_previous ) {
			record->m_eye_angles.x = record->m_previous->m_eye_angles.x;
		}
	}
}

void Resolver::SolveStand( AimPlayer* data, Player* player, LagRecord* record ) {
	if ( data->m_move_side != 0 && data->m_missed_shots == 0 ) {
		record->m_anim_side = data->m_move_side;
		return;
	}

	if ( record->m_layers[ 3 ].m_weight == 0.0f && record->m_layers[ 3 ].m_cycle == 0.0f )
		record->m_anim_side = 2 * ( math::NormalizedAngle( record->m_eye_angles.y - record->m_anim_sides[ 0 ].m_foot_yaw ) <= 0.0f ) - 1;
}

void Resolver::SolveMove( AimPlayer* data, Player* player, LagRecord* record ) {
	if ( !record->m_previous || !( record->m_flags & FL_ONGROUND ) || !( record->m_previous->m_flags & FL_ONGROUND ) )
		return;

	const bool accelerating = static_cast< int >( record->m_layers[ 6 ].m_weight * 1000.f ) == static_cast< int >( record->m_previous->m_layers[ 6 ].m_weight * 1000.f );

	if ( ( !accelerating || static_cast< int >( record->m_layers[ 12 ].m_weight * 1000.0f ) != 0 ) && !record->m_accurate_velocity )
		return;

	const float first_delta = std::abs( record->m_anim_sides[ 0 ].m_layers[ 6 ].m_playback_rate - record->m_layers[ 6 ].m_playback_rate );
	const float second_delta = std::abs( record->m_anim_sides[ 2 ].m_layers[ 6 ].m_playback_rate - record->m_layers[ 6 ].m_playback_rate );
	const float third_delta = std::abs( record->m_anim_sides[ 1 ].m_layers[ 6 ].m_playback_rate - record->m_layers[ 6 ].m_playback_rate );

	if ( first_delta >= third_delta && second_delta >= third_delta )
		record->m_anim_side = 1;
	else
		record->m_anim_side = -1;

	data->m_move_side = record->m_anim_side;
}

void Resolver::ResolveAngles( Player* player, LagRecord* record ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	record->m_anim_side = 0;

	MatchShot( data, record );

	if ( g_menu.main.config.mode.get( ) == 1 )
		record->m_eye_angles.x = 90.f;

	if ( record->m_shot )
		return;

	if ( record->m_flags & FL_ONGROUND ) {
		if ( record->m_velocity.length_2d( ) > 0.1f )
			SolveMove( data, player, record );
		else
			SolveStand( data, player, record );
	}

	data->m_anim_side = record->m_anim_side;
	data->m_prev_anim_side = record->m_anim_side;
}