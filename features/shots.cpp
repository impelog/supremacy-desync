#include "includes.h"

Shots g_shots{ };

void Shots::OnShotFire( Player* target, float damage, int bullets, LagRecord* record, const vec3_t& target_pos, float hitchance, bool resolved ) {
	if( !target || !record )
		return;

	ShotRecord shot;
	shot.m_target = target;
	shot.m_target_index = target->index( );

	player_info_t info;
	if( g_csgo.m_engine->GetPlayerInfo( target->index( ), &info ) )
		shot.m_target_name = std::string( info.m_name ).substr( 0, 24 );
	else
		shot.m_target_name = XOR( "player" );

	shot.m_record = record;
	shot.m_anim_side = 0;
	if( record ) {
		shot.m_anim_side = record->m_anim_side;
		if( record->m_setup ) {
			std::memcpy( shot.m_matrix, record->m_bones, sizeof( BoneArray ) * 128 );
			shot.m_record_origin = record->m_pred_origin;
			shot.m_record_mins = record->m_mins;
			shot.m_record_maxs = record->m_maxs;
			shot.m_record_abs_ang = record->m_abs_ang;
		}
	}
	else if( target ) {
		AimPlayer* data = &g_aimbot.m_players[ target->index( ) - 1 ];
		if( data )
			shot.m_anim_side = data->m_anim_side;
	}

	shot.m_resolved = resolved;
	shot.m_hitchance = hitchance;
	shot.m_time = game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) );
	shot.m_lat = g_cl.m_latency;
	shot.m_damage = damage;
	shot.m_pos = g_cl.m_shoot_pos;
	shot.m_target_pos = target_pos;
	shot.m_shot_tick = g_csgo.m_globals->m_tick_count;

	if( target ) {
		AimPlayer* data = &g_aimbot.m_players[ target->index( ) - 1 ];
		if( data )
			++data->m_shots;
	}

	m_shots.push_front( shot );

	while( m_shots.size( ) > 64 )
		m_shots.pop_back( );
}

void Shots::OnImpact( IGameEvent* evt ) {
	if( !evt || !g_cl.m_local )
		return;

	int attacker = g_csgo.m_engine->GetPlayerForUserID( evt->GetInt( XOR( "userid" ) ) );
	if( attacker != g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	vec3_t pos = {
		evt->GetFloat( XOR( "x" ) ),
		evt->GetFloat( XOR( "y" ) ),
		evt->GetFloat( XOR( "z" ) )
	};

	if( g_menu.main.visuals.impact_beams.get( ) )
		m_vis_impacts.push_back( { pos, g_cl.m_local->GetShootPosition( ), g_cl.m_local->m_nTickBase( ) } );

	if( m_shots.empty( ) )
		return;

	ShotRecord* match = nullptr;
	for( auto it = m_shots.rbegin( ); it != m_shots.rend( ); ++it ) {
		if( it->m_processed )
			continue;

		if( it->m_impacted ) {
			if( it->m_impact_tick == g_csgo.m_globals->m_tick_count && it->m_impacts.size( ) < 4 ) {
				match = &( *it );
				break;
			}
			continue;
		}

		match = &( *it );
		break;
	}

	if( !match || match->m_impacts.size( ) >= 4 )
		return;

	match->m_impacts.push_back( pos );
	match->m_impacted = true;
	match->m_impact_pos = pos;
	match->m_impact_tick = g_csgo.m_globals->m_tick_count;
}

void Shots::OnHurt( IGameEvent* evt ) {
	if( !evt || !g_cl.m_local )
		return;

	int attacker = g_csgo.m_engine->GetPlayerForUserID( evt->GetInt( XOR( "attacker" ) ) );
	int victim = g_csgo.m_engine->GetPlayerForUserID( evt->GetInt( XOR( "userid" ) ) );

	if( attacker < 1 || attacker > 64 || victim < 1 || victim > 64 )
		return;

	if( attacker != g_csgo.m_engine->GetLocalPlayer( ) || victim == g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	int group = evt->GetInt( XOR( "hitgroup" ) );
	if( group == HITGROUP_GEAR )
		return;

	Player* target = g_csgo.m_entlist->GetClientEntity< Player* >( victim );
	if( !target )
		return;

	int damage = evt->GetInt( XOR( "dmg_health" ) );
	int hp = evt->GetInt( XOR( "health" ) );

	ShotRecord* match = nullptr;
	for( auto it = m_shots.rbegin( ); it != m_shots.rend( ); ++it ) {
		if( it->m_processed || it->m_hurt )
			continue;

		if( it->m_target_index == victim ) {
			match = &( *it );
			break;
		}

		if( !match )
			match = &( *it );
	}

	if( match ) {
		match->m_hurt = true;
		match->m_hurt_damage = damage;
		match->m_hurt_group = group;
		match->m_hurt_hp = hp;
	}

	if( g_menu.main.misc.hitmarker.get( ) ) {
		g_visuals.m_hit_duration = 1.f;
		g_visuals.m_hit_start = g_csgo.m_globals->m_curtime;
		g_visuals.m_hit_end = g_visuals.m_hit_start + g_visuals.m_hit_duration;
		g_csgo.m_sound->EmitAmbientSound( XOR( "buttons/arena_switch_press_02.wav" ), 1.f );
	}

	if( group == HITGROUP_GENERIC )
		return;

	if( !m_vis_impacts.empty( ) ) {
		for( auto& i : m_vis_impacts ) {
			if( i.m_tickbase == g_cl.m_local->m_nTickBase( ) )
				i.m_hit_player = true;
		}
	}
}

void Shots::OnPlayerDeath( IGameEvent* evt ) {
	if( !evt || !g_cl.m_local )
		return;

	int victim = g_csgo.m_engine->GetPlayerForUserID( evt->GetInt( XOR( "userid" ) ) );
	int attacker = g_csgo.m_engine->GetPlayerForUserID( evt->GetInt( XOR( "attacker" ) ) );

	if( victim == g_csgo.m_engine->GetLocalPlayer( ) ) {
		m_shots.clear( );
	}
	else {
		if( victim > 0 && victim <= 64 ) {
			AimPlayer* data = &g_aimbot.m_players[ victim - 1 ];
			if( data ) {
				data->m_missed_shots = 0;
				data->m_misses = 0;
			}
		}

		for( auto it = m_shots.rbegin( ); it != m_shots.rend( ); ++it ) {
			if( !it->m_processed && it->m_target_index == victim && attacker != g_csgo.m_engine->GetLocalPlayer( ) ) {
				it->m_target_death = true;
			}
		}
	}
}

void Shots::OnNetUpdate( ) {
	if( !g_cl.m_local || !g_cl.m_local->alive( ) || !g_csgo.m_engine->IsInGame( ) ) {
		m_shots.clear( );
		return;
	}

	for( auto it = m_shots.rbegin( ); it != m_shots.rend( ); ++it ) {
		auto& shot = *it;
		if( shot.m_processed )
			continue;

		if( !shot.m_target || shot.m_target_index <= 0 || !shot.m_record ) {
			shot.m_processed = true;
			continue;
		}

		if( shot.m_local_death ) {
			shot.m_processed = true;
			continue;
		}

		int cur_tick = g_csgo.m_globals->m_tick_count;
		if( !shot.m_impacted && !shot.m_target_death && !shot.m_hurt ) {
			if( std::abs( cur_tick - shot.m_shot_tick ) > 256 ) {
				shot.m_processed = true;
				if( shot.m_target )
					g_notify.add( tfm::format( XOR( "missed shot at %s due to unregistered shot\n" ), shot.m_target_name ) );
			}
			continue;
		}

		if( !shot.m_hurt && !shot.m_target_death && shot.m_impacted ) {
			int wait_ticks = std::clamp( g_cl.m_latency_ticks * 2 + 8, 8, 32 );
			if( cur_tick - shot.m_impact_tick < wait_ticks )
				continue;
		}

		shot.m_processed = true;

		if( shot.m_hurt ) {
			std::string group_str = ( shot.m_hurt_group >= 0 && shot.m_hurt_group < ( int )m_groups.size( ) ) ? m_groups[ shot.m_hurt_group ] : XOR( "generic" );
			g_notify.add( tfm::format( XOR( "hit %s in the %s for %i (%i remaining) [side: %i]\n" ), shot.m_target_name, group_str, shot.m_hurt_damage, shot.m_hurt_hp, shot.m_anim_side ) );

			if( g_menu.main.misc.hitmarker.get( ) ) {
				g_visuals.m_hit_duration = 1.f;
				g_visuals.m_hit_start = g_csgo.m_globals->m_curtime;
				g_visuals.m_hit_end = g_visuals.m_hit_start + g_visuals.m_hit_duration;
				g_csgo.m_sound->EmitAmbientSound( XOR( "buttons/arena_switch_press_02.wav" ), 1.f );
			}

			if( shot.m_target_index > 0 && shot.m_target_index <= 64 ) {
				AimPlayer* data = &g_aimbot.m_players[ shot.m_target_index - 1 ];
				if( data ) {
					data->m_missed_shots = 0;
					data->m_misses = 0;
					data->m_prev_anim_side = data->m_anim_side;
				}
			}
			continue;
		}

		if( shot.m_target_death ) {
			g_notify.add( tfm::format( XOR( "missed shot at %s due to target death\n" ), shot.m_target_name ) );
			continue;
		}

		if( !shot.m_impacted || shot.m_impacts.empty( ) ) {
			if( shot.m_target )
				g_notify.add( tfm::format( XOR( "missed shot at %s due to unregistered shot\n" ), shot.m_target_name ) );
			continue;
		}

		Player* target = shot.m_target;
		if( !target ) {
			g_notify.add( tfm::format( XOR( "missed shot at %s due to unknown\n" ), shot.m_target_name ) );
			continue;
		}

		vec3_t impact_pos = shot.m_impacts.back( );
		vec3_t dir = ( impact_pos - shot.m_pos ).normalized( );
		vec3_t end = shot.m_pos + ( dir * 8192.f );

		BackupRecord backup;
		backup.store( target );

		CBoneCache* cache = &target->m_BoneCache( );
		cache->m_pCachedBones = shot.m_matrix;
		cache->m_CachedBoneCount = 128;
		target->m_vecOrigin( ) = shot.m_record_origin;
		target->SetCollisionBounds( shot.m_record_mins, shot.m_record_maxs );
		target->SetAbsAngles( shot.m_record_abs_ang );
		target->SetAbsOrigin( shot.m_record_origin );

		CGameTrace trace;
		g_csgo.m_engine_trace->ClipRayToEntity( Ray( shot.m_pos, end ), MASK_SHOT, target, &trace );

		backup.restore( target );

		if( !trace.m_entity || trace.m_entity != target ) {
			g_notify.add( tfm::format( XOR( "missed shot at %s due to spread\n" ), shot.m_target_name ) );
			continue;
		}

		if( shot.m_record && shot.m_record->m_broke_lc ) {
			g_notify.add( tfm::format( XOR( "missed shot at %s due to lagcomp\n" ), shot.m_target_name ) );
			continue;
		}

		if( shot.m_resolved ) {
			g_notify.add( tfm::format( XOR( "missed shot at %s due to resolver [side: %i]\n" ), shot.m_target_name, shot.m_anim_side ) );

			if( shot.m_target_index > 0 && shot.m_target_index <= 64 ) {
				AimPlayer* data = &g_aimbot.m_players[ shot.m_target_index - 1 ];
				if( data )
					++data->m_missed_shots;
			}
			continue;
		}

		g_notify.add( tfm::format( XOR( "missed shot at %s due to unknown\n" ), shot.m_target_name ) );
	}
}

void Shots::Reset( ) {
	m_shots.clear( );
	m_vis_impacts.clear( );
	m_impacts.clear( );
	m_hits.clear( );
}