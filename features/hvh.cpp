#include "includes.h"

HVH g_hvh{ };

void HVH::IdealPitch( ) {
	g_cl.m_cmd->m_view_angles.x = 89.f;
}

void HVH::SetPitch( ) {
	bool safe = g_menu.main.config.mode.get( ) == 0;

	switch ( m_pitch ) {
		case 1:
			g_cl.m_cmd->m_view_angles.x = safe ? 89.f : 720.f;
			break;

		case 2:
			g_cl.m_cmd->m_view_angles.x = safe ? -89.f : -720.f;
			break;

		default:
			break;
	}
}

void HVH::AtTarget( float& yaw ) {
	Player* best_player = nullptr;
	float best_val = std::numeric_limits< float >::max( );

	for ( int i = 1; i <= g_csgo.m_globals->m_max_clients; ++i ) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
		if ( !player || !g_aimbot.IsValidTarget( player ) || player->dormant( ) )
			continue;

		if ( m_base_angle == 0 ) {
			float fov = math::GetFOV( g_cl.m_view_angles, g_cl.m_shoot_pos, player->WorldSpaceCenter( ) );
			if ( fov < best_val ) {
				best_val = fov;
				best_player = player;
			}
		}
		else if ( m_base_angle == 1 ) {
			float dist = ( player->m_vecOrigin( ) - g_cl.m_local->m_vecOrigin( ) ).length( );
			if ( dist < best_val ) {
				best_val = dist;
				best_player = player;
			}
		}
	}

	if ( !best_player )
		return;

	float dx = best_player->m_vecOrigin( ).x - g_cl.m_local->m_vecOrigin( ).x;
	float dy = best_player->m_vecOrigin( ).y - g_cl.m_local->m_vecOrigin( ).y;
	yaw = ( dx == 0.f && dy == 0.f ) ? 0.f : math::rad_to_deg( std::atan2( dy, dx ) );
}

void HVH::SelectYaw( float& yaw ) {
	if ( g_cl.m_packet )
		m_choke_cycle_switch = !m_choke_cycle_switch;

	if ( m_yaw == 0 )
		return;

	yaw = g_cl.m_view_angles.y;

	AtTarget( yaw );

	switch ( m_yaw ) {
		case 1:
			yaw += 180.f;
			break;

		case 2:
			yaw += 180.f + ( m_jitter_range * ( m_choke_cycle_switch ? 0.5f : -0.5f ) );
			break;

		default:
			break;
	}

	yaw += m_yaw_offset;
}

void HVH::Process( float yaw ) {
	if ( !g_menu.main.antiaim.desync.get( ) ) {
		m_delta = 0.f;
		g_cl.m_cmd->m_view_angles.y = math::NormalizedAngle( yaw );
		return;
	}

	float delta = std::clamp( g_menu.main.antiaim.desync_delta.get( ), -60.f, 60.f );
	m_delta = std::abs( delta );

	if ( g_cl.m_local && g_cl.m_local->m_vecVelocity( ).length_sqr( ) < 5.f ) {
		g_cl.m_cmd->m_side_move = ( g_cl.m_cmd->m_command_number % 2 == 0 ? -1.1f : 1.1f );
		if ( g_cl.m_cmd->m_buttons & IN_DUCK )
			g_cl.m_cmd->m_side_move = ( g_cl.m_cmd->m_command_number % 2 == 0 ? -3.3f : 3.3f );
	}

	if ( g_cl.m_packet ) {
		g_cl.m_cmd->m_view_angles.y = math::NormalizedAngle( yaw );
		return;
	}

	g_cl.m_cmd->m_view_angles.y = math::NormalizedAngle( yaw - delta );
}

bool HVH::DoEdgeAntiAim( Player* player, ang_t& out ) {
	CGameTrace trace;
	static CTraceFilterSimple_game filter{ };

	if ( !player || player->m_MoveType( ) == MOVETYPE_LADDER )
		return false;

	filter.SetPassEntity( player );

	vec3_t mins = player->m_vecMins( );
	vec3_t maxs = player->m_vecMaxs( );

	mins.x -= 20.f;
	mins.y -= 20.f;
	maxs.x += 20.f;
	maxs.y += 20.f;

	vec3_t start = player->GetAbsOrigin( );
	start.z += 56.f;

	g_csgo.m_engine_trace->TraceRay( Ray( start, start, mins, maxs ), CONTENTS_SOLID, ( ITraceFilter* )&filter, &trace );
	if ( !trace.m_startsolid )
		return false;

	float  smallest = 1.f;
	vec3_t plane;

	for ( float step{ }; step <= math::pi_2; step += ( math::pi / 10.f ) ) {
		vec3_t end = start;
		end.x += std::cos( step ) * 32.f;
		end.y += std::sin( step ) * 32.f;

		g_csgo.m_engine_trace->TraceRay( Ray( start, end, { -1.f, -1.f, -8.f }, { 1.f, 1.f, 8.f } ), CONTENTS_SOLID, ( ITraceFilter* )&filter, &trace );

		if ( trace.m_fraction < smallest ) {
			plane = trace.m_plane.m_normal;
			smallest = trace.m_fraction;
		}
	}

	if ( smallest == 1.f || plane.z >= 0.1f )
		return false;

	vec3_t inv = -plane;
	vec3_t dir = inv;
	dir.normalize( );

	vec3_t point = start;
	point.x += ( dir.x * 24.f );
	point.y += ( dir.y * 24.f );

	if ( g_csgo.m_engine_trace->GetPointContents( point, CONTENTS_SOLID ) & CONTENTS_SOLID ) {
		g_csgo.m_engine_trace->TraceRay( Ray( point + vec3_t{ 0.f, 0.f, 16.f }, point ), CONTENTS_SOLID, ( ITraceFilter* )&filter, &trace );

		if ( trace.m_fraction < 1.f && !trace.m_startsolid && trace.m_plane.m_normal.z > 0.7f ) {
			out.y = math::rad_to_deg( std::atan2( inv.y, inv.x ) );
			return true;
		}
	}

	mins = { ( dir.x * -3.f ) - 1.f, ( dir.y * -3.f ) - 1.f, -1.f };
	maxs = { ( dir.x * 3.f ) + 1.f, ( dir.y * 3.f ) + 1.f, 1.f };

	vec3_t left = start;
	left.x = point.x - ( inv.y * 48.f );
	left.y = point.y - ( inv.x * -48.f );

	g_csgo.m_engine_trace->TraceRay( Ray( left, point, mins, maxs ), CONTENTS_SOLID, ( ITraceFilter* )&filter, &trace );
	float l = trace.m_startsolid ? 0.f : trace.m_fraction;

	vec3_t right = start;
	right.x = point.x + ( inv.y * 48.f );
	right.y = point.y + ( inv.x * -48.f );

	g_csgo.m_engine_trace->TraceRay( Ray( right, point, mins, maxs ), CONTENTS_SOLID, ( ITraceFilter* )&filter, &trace );
	float r = trace.m_startsolid ? 0.f : trace.m_fraction;

	if ( l == 0.f && r == 0.f )
		return false;

	out.y = math::rad_to_deg( std::atan2( inv.y, inv.x ) );

	if ( l == 0.f ) {
		out.y += 90.f;
		return true;
	}

	if ( r == 0.f ) {
		out.y -= 90.f;
		return true;
	}

	return false;
}

bool HVH::DoFreestand( float& yaw ) {
	if ( !g_cl.m_local || !g_cl.m_processing )
		return false;

	Player* best_player = nullptr;
	float best_val = std::numeric_limits< float >::max( );

	for ( int i = 1; i <= g_csgo.m_globals->m_max_clients; ++i ) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
		if ( !player || !g_aimbot.IsValidTarget( player ) || player->dormant( ) )
			continue;

		float fov = math::GetFOV( g_cl.m_view_angles, g_cl.m_shoot_pos, player->WorldSpaceCenter( ) );
		if ( fov < best_val ) {
			best_val = fov;
			best_player = player;
		}
	}

	vec3_t local_eye = g_cl.m_local->GetShootPosition( );

	if ( best_player ) {
		vec3_t enemy_eye = best_player->GetShootPosition( );
		ang_t angle_to_enemy;
		math::VectorAngles( enemy_eye - local_eye, angle_to_enemy );

		vec3_t right;
		math::AngleVectors( ang_t{ 0.f, angle_to_enemy.y + 90.f, 0.f }, nullptr, &right, nullptr );

		vec3_t left_pos = local_eye - ( right * 24.f );
		vec3_t right_pos = local_eye + ( right * 24.f );

		CTraceFilterWorldOnly filter;
		CGameTrace left_trace, right_trace;

		g_csgo.m_engine_trace->TraceRay( Ray( left_pos, enemy_eye ), MASK_SHOT_HULL | CONTENTS_GRATE, &filter, &left_trace );
		g_csgo.m_engine_trace->TraceRay( Ray( right_pos, enemy_eye ), MASK_SHOT_HULL | CONTENTS_GRATE, &filter, &right_trace );

		if ( left_trace.m_fraction < right_trace.m_fraction ) {
			yaw = angle_to_enemy.y + 90.f;
			return true;
		}
		else if ( right_trace.m_fraction < left_trace.m_fraction ) {
			yaw = angle_to_enemy.y - 90.f;
			return true;
		}
		else {
			yaw = angle_to_enemy.y + 180.f;
			return true;
		}
	}

	ang_t edge_ang;
	if ( DoEdgeAntiAim( g_cl.m_local, edge_ang ) ) {
		yaw = edge_ang.y;
		return true;
	}

	return false;
}

void HVH::AntiAim( ) {
	if ( !g_menu.main.antiaim.enable.get( ) ) {
		m_delta = 0.f;
		return;
	}

	if ( !g_cl.m_local || !g_cl.m_processing ) {
		m_delta = 0.f;
		return;
	}

	bool attack = g_cl.m_cmd->m_buttons & IN_ATTACK;
	bool attack2 = g_cl.m_cmd->m_buttons & IN_ATTACK2;

	if ( g_cl.m_weapon ) {
		if ( g_cl.m_weapon_fire ) {
			bool knife = g_cl.m_weapon_type == WEAPONTYPE_KNIFE && g_cl.m_weapon_id != WEAPON_TASER;
			bool revolver = g_cl.m_weapon_id == WEAPON_REVOLVER;

			if ( attack || ( attack2 && ( knife || revolver ) ) ) {
				m_delta = 0.f;
				return;
			}
		}

		if ( g_cl.m_weapon_type == WEAPONTYPE_GRENADE ) {
			if ( !g_cl.m_weapon->m_bPinPulled( ) || attack || attack2 ) {
				if ( g_cl.m_weapon->m_fThrowTime( ) > 0.f && g_cl.m_weapon->m_fThrowTime( ) < g_csgo.m_globals->m_curtime ) {
					m_delta = 0.f;
					return;
				}
			}
		}
	}

	bool freeze = g_csgo.m_gamerules ? g_csgo.m_gamerules->m_bFreezePeriod( ) : false;
	if ( freeze || ( g_cl.m_flags & FL_FROZEN ) || g_cl.m_round_end || ( g_cl.m_cmd->m_buttons & IN_USE ) || g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER || g_cl.m_local->m_MoveType( ) == MOVETYPE_NOCLIP ) {
		m_delta = 0.f;
		return;
	}

	m_pitch = g_menu.main.antiaim.pitch.get( );
	m_yaw = g_menu.main.antiaim.yaw.get( );
	m_yaw_offset = g_menu.main.antiaim.yaw_offset.get( );
	m_jitter_range = g_menu.main.antiaim.jitter_range.get( );
	m_base_angle = g_menu.main.antiaim.base_angle.get( );
	m_rot_range = 360.f;
	m_rot_speed = 10.f;
	m_rand_update = 0.1f;

	SetPitch( );

	if ( g_menu.main.antiaim.freestand.get_status( ) && g_cl.m_local->m_vecVelocity( ).length( ) < 320.f ) {
		float freestand_yaw = 0.f;
		if ( DoFreestand( freestand_yaw ) ) {
			Process( freestand_yaw );
			math::NormalizeAngle( g_cl.m_cmd->m_view_angles.y );
			return;
		}
	}

	if ( g_menu.main.antiaim.edge.get( ) && g_cl.m_local->m_vecVelocity( ).length( ) < 320.f ) {
		ang_t edge_ang;
		if ( DoEdgeAntiAim( g_cl.m_local, edge_ang ) ) {
			g_cl.m_cmd->m_view_angles.y = edge_ang.y;
			math::NormalizeAngle( g_cl.m_cmd->m_view_angles.y );
			return;
		}
	}

	float yaw = g_cl.m_view_angles.y;

	SelectYaw( yaw );
	Process( yaw );

	math::NormalizeAngle( g_cl.m_cmd->m_view_angles.y );
}

void HVH::FakeDuck( ) {
	m_fake_duck = false;

	if ( !g_cl.m_local || !g_cl.m_processing || !( g_cl.m_flags & FL_ONGROUND ) || !g_menu.main.antiaim.fake_duck.get_status( ) )
		return;

	g_cl.m_cmd->m_buttons |= IN_BULLRUSH;

	if ( g_csgo.m_cl->nChokedCommands < 7 )
		g_cl.m_cmd->m_buttons &= ~IN_DUCK;
	else
		g_cl.m_cmd->m_buttons |= IN_DUCK;

	m_fake_duck = true;
}

void HVH::SendPacket( ) {
	if ( !g_cl.m_local || !g_cl.m_processing )
		return;

	if ( m_fake_duck ) {
		g_cl.m_packet = ( g_cl.m_lag >= 14 );
		return;
	}

	bool freeze = g_csgo.m_gamerules ? g_csgo.m_gamerules->m_bFreezePeriod( ) : false;
	if ( g_menu.main.antiaim.lag_enable.get( ) && !freeze && !( g_cl.m_flags & FL_FROZEN ) ) {
		int limit = std::min( ( int )g_menu.main.antiaim.lag_limit.get( ), g_cl.m_max_lag );
		if ( g_cl.m_tickbase_shift > 0 )
			limit = std::max( ( 16 - 2 ) - g_cl.m_tickbase_shift, 1 );
		if ( g_menu.main.aimbot.doubletap_key.get_status( ) && ( g_cl.m_tickbase_shift < 13 || ( g_csgo.m_globals->m_realtime - g_cl.m_last_shot_time ) < 0.5f ) )
			limit = 2;
		bool active = false;

		vec3_t cur = g_cl.m_local->m_vecOrigin( );
		vec3_t prev = g_cl.m_net_pos.empty( ) ? g_cl.m_local->m_vecOrigin( ) : g_cl.m_net_pos.front( ).m_pos;
		float delta = ( cur - prev ).length_sqr( );

		auto activation = g_menu.main.antiaim.lag_active.GetActiveIndices( );
		for ( auto it = activation.begin( ); it != activation.end( ); it++ ) {
			if ( *it == 0 && delta > 0.1f && g_cl.m_speed > 0.1f ) {
				active = true;
				break;
			}
			else if ( *it == 1 && ( ( g_cl.m_buttons & IN_JUMP ) || !( g_cl.m_flags & FL_ONGROUND ) ) ) {
				active = true;
				break;
			}
			else if ( *it == 2 && ( g_cl.m_flags & FL_DUCKING ) ) {
				active = true;
				break;
			}
		}

		if ( active ) {
			int mode = g_menu.main.antiaim.lag_mode.get( );
			if ( mode == 0 )
				g_cl.m_packet = false;
			else if ( mode == 1 ) {
				if ( delta <= 4096.f )
					g_cl.m_packet = false;
			}
			else if ( mode == 2 ) {
				if ( ( bool )g_csgo.RandomInt( 0, 1 ) )
					g_cl.m_packet = false;
			}
			else if ( mode == 3 ) {
				if ( m_step_switch ) {
					if ( delta <= 4096.f )
						g_cl.m_packet = false;
				}
				else g_cl.m_packet = false;
			}

			if ( g_cl.m_lag >= limit )
				g_cl.m_packet = true;
		}
		else {
			if ( g_menu.main.antiaim.desync.get( ) && g_cl.m_lag < 1 )
				g_cl.m_packet = false;
		}
	}
	else {
		if ( g_menu.main.antiaim.desync.get( ) && g_cl.m_lag < 1 )
			g_cl.m_packet = false;
	}
}