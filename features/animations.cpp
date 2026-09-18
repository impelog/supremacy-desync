#include "includes.h"

Animations g_animations{};

void AnimData_t::backup( Player* player ) {
	m_flags = player->m_fFlags( );
	m_duck_amount = player->m_flDuckAmount( );
	m_lby = player->m_flLowerBodyYawTarget( );
	m_eye_angles = player->m_angEyeAngles( );
	if ( player->m_PlayerAnimState( ) ) {
		m_feet_weight = player->m_PlayerAnimState( )->m_feet_weight;
		m_feet_cycle = player->m_PlayerAnimState( )->m_feet_cycle;
		std::memcpy( &m_state, player->m_PlayerAnimState( ), sizeof( CCSGOPlayerAnimState ) );
	}
}

void AnimData_t::restore( Player* player ) {
	player->m_fFlags( ) = m_flags;
	player->m_flDuckAmount( ) = m_duck_amount;
	player->m_flLowerBodyYawTarget( ) = m_lby;
	player->m_angEyeAngles( ) = m_eye_angles;
	if ( player->m_PlayerAnimState( ) ) {
		player->m_PlayerAnimState( )->m_feet_weight = m_feet_weight;
		player->m_PlayerAnimState( )->m_feet_cycle = m_feet_cycle;
		std::memcpy( player->m_PlayerAnimState( ), &m_state, sizeof( CCSGOPlayerAnimState ) );
	}
}

void Animations::SimulateAnimation( Player* player, float simtime ) {
	float o_curtime = g_csgo.m_globals->m_curtime;
	float o_realtime = g_csgo.m_globals->m_realtime;
	float o_frametime = g_csgo.m_globals->m_frametime;
	float o_abs_frametime = g_csgo.m_globals->m_abs_frametime;
	int   o_framecount = g_csgo.m_globals->m_frame;
	int   o_tickcount = g_csgo.m_globals->m_tick_count;
	float o_interp_amount = g_csgo.m_globals->m_interp_amt;

	float anim_time = simtime;
	int   anim_ticks = game::TIME_TO_TICKS( anim_time );

	g_csgo.m_globals->m_curtime = anim_time;
	g_csgo.m_globals->m_realtime = anim_time;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_abs_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frame = anim_ticks;
	g_csgo.m_globals->m_tick_count = anim_ticks;
	g_csgo.m_globals->m_interp_amt = 0.0f;

	player->m_bClientSideAnimation( ) = true;
	player->UpdateClientSideAnimation( );
	player->m_bClientSideAnimation( ) = false;

	g_csgo.m_globals->m_curtime = o_curtime;
	g_csgo.m_globals->m_realtime = o_realtime;
	g_csgo.m_globals->m_frametime = o_frametime;
	g_csgo.m_globals->m_abs_frametime = o_abs_frametime;
	g_csgo.m_globals->m_frame = o_framecount;
	g_csgo.m_globals->m_tick_count = o_tickcount;
	g_csgo.m_globals->m_interp_amt = o_interp_amount;
}

void Animations::RestoreAnimation( Player* player, LagRecord* record ) {
	CCSGOPlayerAnimState* state = player->m_PlayerAnimState( );
	if ( state )
		std::memcpy( state, &m_data[ player->index( ) ].m_state, sizeof( CCSGOPlayerAnimState ) );
	player->SetAnimLayers( record->m_layers );
	player->SetPoseParameters( record->m_poses );
}

void Animations::SimulateSide( Player* player, LagRecord* record, int side ) {
	CCSGOPlayerAnimState* state = player->m_PlayerAnimState( );
	if ( !state )
		return;

	AnimData_t* anim_data = &m_data[ player->index( ) ];
	anim_data->backup( player );

	player->SetAbsOrigin( player->m_vecOrigin( ) );

	if ( !record->m_previous || record->m_previous->dormant( ) )
		player->SetAnimLayers( record->m_layers );
	else if ( !record->m_previous->dormant( ) ) {
		player->SetAnimLayers( record->m_previous->m_layers );
		state->m_feet_cycle = record->m_previous->m_layers[ 6 ].m_cycle;
		state->m_feet_weight = record->m_previous->m_layers[ 6 ].m_weight;
		state->m_acceleration_weight = record->m_previous->m_layers[ 12 ].m_weight;
		state->m_strafe_cycle = record->m_previous->m_layers[ 7 ].m_cycle;
		state->m_strafe_weight = record->m_previous->m_layers[ 7 ].m_weight;
	}

	float max_desync = state->GetMaxDesyncDelta( );
	float rotation = 0.0f;
	if ( side != 0 ) {
		if ( side <= 0 )
			rotation = record->m_eye_angles.y - max_desync;
		else
			rotation = record->m_eye_angles.y + max_desync;
		state->m_goal_feet_yaw = math::NormalizedAngle( rotation );
	}
	player->m_flDuckAmount( ) = record->m_duck;
	player->m_vecVelocity( ) = record->m_velocity;
	player->m_fFlags( ) = record->m_flags;
	player->m_angEyeAngles( ) = record->m_eye_angles;
	player->m_angEyeAngles( ).z = 0.0f;
	SimulateAnimation( player, record->m_sim_time );

	anim_data->restore( player );
	player->InvalidatePhysicsRecursive( ( InvalidatePhysicsBits_t )8 );
}

void Animations::UpdatePlayer( AimPlayer* data, LagRecord* record ) {
	Player* player = data->m_player;
	CCSGOPlayerAnimState* state = player->m_PlayerAnimState( );
	if ( !state )
		return;

	if ( player->m_flSpawnTime( ) != data->m_spawn ) {
		game::ResetAnimationState( state );
		data->m_spawn = player->m_flSpawnTime( );
	}

	bool bot = game::IsFakePlayer( player->index( ) );
	if ( g_menu.main.aimbot.enable.get( ) ) {
		if ( record->m_previous ) {
			if ( !( ( player->m_fFlags( ) & FL_ONGROUND ) && record->m_layers[ 5 ].m_weight > 0.0f && record->m_previous->m_layers[ 5 ].m_weight > 0.0f ) ) {
				int act = player->GetSequenceActivity( record->m_layers[ 5 ].m_sequence );
				if ( ( act == 988 || act == 989 ) && record->m_layers[ 5 ].m_playback_rate > 0.f ) {
					float land_time = record->m_sim_time - ( record->m_layers[ 5 ].m_cycle / record->m_layers[ 5 ].m_playback_rate );
					if ( land_time >= record->m_previous->m_sim_time )
						player->m_fFlags( ) |= FL_ONGROUND;
					else if ( !( record->m_previous->m_flags & FL_ONGROUND ) )
						player->m_fFlags( ) &= ~FL_ONGROUND;
				}
			}

			record->m_pred_flags = record->m_flags = player->m_fFlags( );

			if ( record->m_layers[ 6 ].m_playback_rate == 0.0f )
				record->m_velocity = vec3_t( 0.0f, 0.0f, 0.0f );

			if ( ( player->m_fFlags( ) & FL_ONGROUND ) && ( record->m_previous->m_flags & FL_ONGROUND ) ) {
				Weapon* weapon = player->GetActiveWeapon( );
				WeaponInfo* weapon_data = weapon ? weapon->GetWpnData( ) : nullptr;
				float max_speed = weapon_data ? std::max( 0.1f, weapon_data->flMaxSpeed[ player->m_bIsScoped( ) ? 1 : 0 ] ) : 260.0f;
				float speed = ( 0.55f - ( ( record->m_layers[ 11 ].m_weight - 1.0f ) * 0.35f ) ) * max_speed;
				float average_speed = record->m_velocity.length_2d( );

				if ( ( record->m_layers[ 11 ].m_weight >= 1.0f && average_speed > speed ) || ( record->m_layers[ 11 ].m_weight < 1.0f && ( speed >= average_speed || record->m_layers[ 11 ].m_weight > 0.0f ) ) ) {
					if ( average_speed > 0.f ) {
						record->m_velocity.x = ( record->m_velocity.x / average_speed ) * speed;
						record->m_velocity.y = ( record->m_velocity.y / average_speed ) * speed;
						record->m_accurate_velocity = true;
					}
				}
			}

			int ticks = std::max( 1, game::TIME_TO_TICKS( record->m_sim_time - record->m_previous->m_sim_time ) );
			if ( record->m_previous->m_layers[ 11 ].m_playback_rate == record->m_layers[ 11 ].m_playback_rate ) {
				if ( record->m_previous->m_weapon_id == record->m_weapon_id && record->m_layers[ 11 ].m_playback_rate > 0.f && record->m_layers[ 11 ].m_cycle > record->m_previous->m_layers[ 11 ].m_cycle ) {
					int v16 = ticks + 1;
					int v17 = game::TIME_TO_TICKS( ( record->m_layers[ 11 ].m_cycle - record->m_previous->m_layers[ 11 ].m_cycle ) / record->m_layers[ 11 ].m_playback_rate );
					if ( v17 > v16 && v17 <= 19 )
						ticks = v17;
				}
			}
			record->m_lag = ticks;
			record->m_duck = record->m_previous->m_duck + ( ( player->m_flDuckAmount( ) - record->m_previous->m_duck ) / ( float )ticks );
		}
	}

	player->GetAnimLayers( record->m_layers );
	player->GetPoseParameters( record->m_poses );

	SimulateSide( player, record, 0 );
	player->GetAnimLayers( record->m_anim_sides[ 0 ].m_layers );
	player->GetPoseParameters( record->m_anim_sides[ 0 ].m_poses );
	record->m_anim_sides[ 0 ].m_foot_yaw = state->m_goal_feet_yaw;
	record->m_anim_sides[ 0 ].m_cur_feet_yaw = state->m_cur_feet_yaw;
	record->m_anim_sides[ 0 ].m_angles = record->m_eye_angles;
	RestoreAnimation( player, record );

	if ( !bot && g_menu.main.aimbot.correct.get( ) ) {
		SimulateSide( player, record, -1 );
		player->GetAnimLayers( record->m_anim_sides[ 2 ].m_layers );
		player->GetPoseParameters( record->m_anim_sides[ 2 ].m_poses );
		record->m_anim_sides[ 2 ].m_foot_yaw = state->m_goal_feet_yaw;
		record->m_anim_sides[ 2 ].m_cur_feet_yaw = state->m_cur_feet_yaw;
		record->m_anim_sides[ 2 ].m_angles = record->m_eye_angles;
		RestoreAnimation( player, record );

		SimulateSide( player, record, 1 );
		player->GetAnimLayers( record->m_anim_sides[ 1 ].m_layers );
		player->GetPoseParameters( record->m_anim_sides[ 1 ].m_poses );
		record->m_anim_sides[ 1 ].m_foot_yaw = state->m_goal_feet_yaw;
		record->m_anim_sides[ 1 ].m_cur_feet_yaw = state->m_cur_feet_yaw;
		record->m_anim_sides[ 1 ].m_angles = record->m_eye_angles;
		RestoreAnimation( player, record );

		g_resolver.ResolveAngles( player, record );

		float max_desync = state->GetMaxDesyncDelta( );
		float desync_yaw = max_desync * ( float )record->m_anim_side;
		float rotation = record->m_eye_angles.y + desync_yaw;

		state->m_goal_feet_yaw = math::NormalizedAngle( rotation );
		SimulateAnimation( player, record->m_sim_time );
	}

	player->GetPoseParameters( record->m_poses );
	player->GetAnimLayers( record->m_layers );
	record->m_abs_ang = player->GetAbsAngles( );
}
