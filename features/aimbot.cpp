#include "includes.h"

Aimbot g_aimbot{ };

void AimPlayer::OnNetUpdate( Player *player ) {
	bool reset = ( !g_menu.main.aimbot.enable.get( ) || player->m_lifeState( ) == LIFE_DEAD || !player->enemy( g_cl.m_local ) );
	bool disable = ( !reset && !g_cl.m_processing );

	if ( reset ) {
		player->m_bClientSideAnimation( ) = true;
		m_records.clear( );
		m_highest_simtime = 0.f;
		return;
	}

	if ( disable ) {
		player->m_bClientSideAnimation( ) = true;
		return;
	}

	if ( m_player != player ) {
		m_records.clear( );
		m_highest_simtime = 0.f;
	}

	m_player = player;

	if ( player->dormant( ) ) {
		bool insert = true;

		if ( !m_records.empty( ) ) {
			LagRecord *front = m_records.front( ).get( );
			if ( front->dormant( ) )
				insert = false;
		}

		if ( insert ) {
			m_records.emplace_front( std::make_shared< LagRecord >( player ) );
			LagRecord *current = m_records.front( ).get( );
			current->m_dormant = true;
		}
	}

	bool update = ( m_records.empty( ) || player->m_flSimulationTime( ) > m_records.front( ).get( )->m_sim_time );

	if ( update ) {
		m_records.emplace_front( std::make_shared< LagRecord >( player ) );
		LagRecord *record = m_records.front( ).get( );
		record->m_dormant = false;

		bool out_of_dormancy = false;
		if ( m_records.size( ) >= 2 && m_records[ 1 ]->dormant( ) ) {
			out_of_dormancy = true;
			auto front = m_records.front( );
			m_records.clear( );
			m_records.push_front( front );
		}

		if ( m_records.size( ) >= 2 && m_records[ 1 ] && !m_records[ 1 ]->dormant( ) )
			record->m_previous = m_records[ 1 ].get( );
		else
			record->m_previous = nullptr;

		if ( out_of_dormancy ) {
			record->m_velocity = player->m_vecVelocity( );
			record->m_lag = 1;
			record->m_broke_lc = false;
			record->m_shifting_tickbase = false;
			m_highest_simtime = record->m_sim_time;
		}
		else {
			record->m_shifting_tickbase = m_highest_simtime >= record->m_sim_time;
			if ( record->m_shifting_tickbase )
				record->m_broke_lc = true;
			if ( record->m_sim_time > m_highest_simtime || std::fabs( m_highest_simtime - record->m_sim_time ) > 3.f )
				m_highest_simtime = record->m_sim_time;
			if ( record->m_previous && ( record->m_origin - record->m_previous->m_origin ).length_2d_sqr( ) > 4096.f )
				record->m_broke_lc = true;
		}

		g_animations.UpdatePlayer( this, record );
	}

	while ( m_records.size( ) > 64 )
		m_records.pop_back( );
}

void AimPlayer::OnRoundStart( Player *player ) {
	m_player = player;
	m_walk_record = LagRecord{ };
	m_shots = 0;
	m_missed_shots = 0;

	m_anim_side = 0;
	m_prev_anim_side = 0;
	m_misses = 0;

	m_records.clear( );
	m_hitboxes.clear( );

	// IMPORTANT: DO NOT CLEAR LAST HIT SHIT.
}

void AimPlayer::SetupHitboxes( LagRecord *record, bool history ) {
	// reset hitboxes.
	m_hitboxes.clear( );

	if ( g_cl.m_weapon_id == WEAPON_TASER ) {
		// hitboxes for the zeus.
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );
		return;
	}

	// prefer, always.
	if ( g_menu.main.aimbot.baim1.get( 0 ) )
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );

	// prefer, lethal.
	if ( g_menu.main.aimbot.baim1.get( 1 ) )
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::LETHAL } );

	// prefer, lethal x2.
	if ( g_menu.main.aimbot.baim1.get( 2 ) )
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::LETHAL2 } );

	// prefer, fake.
	if ( g_menu.main.aimbot.baim1.get( 3 ) && !record->m_shot )
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );

	// prefer, in air.
	if ( g_menu.main.aimbot.baim1.get( 4 ) && !( record->m_pred_flags & FL_ONGROUND ) )
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );

	bool only{ false };

	// only, always.
	if ( g_menu.main.aimbot.baim2.get( 0 ) ) {
		only = true;
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );
	}

	// only, health.
	if ( g_menu.main.aimbot.baim2.get( 1 ) && m_player->m_iHealth( ) <= ( int )g_menu.main.aimbot.baim_hp.get( ) ) {
		only = true;
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );
	}

	// only, fake.
	if ( g_menu.main.aimbot.baim2.get( 2 ) && !record->m_shot ) {
		only = true;
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );
	}

	// only, in air.
	if ( g_menu.main.aimbot.baim2.get( 3 ) && !( record->m_pred_flags & FL_ONGROUND ) ) {
		only = true;
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );
	}

	if ( g_menu.main.aimbot.baim_key.get_status( ) ) {
		only = true;
		m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::PREFER } );
	}

	// only baim conditions have been met.
	// do not insert more hitboxes.
	if ( only )
		return;

	std::vector< size_t > hitbox{ history ? g_menu.main.aimbot.hitbox_history.GetActiveIndices( ) : g_menu.main.aimbot.hitbox.GetActiveIndices( ) };
	if ( hitbox.empty( ) )
		return;

	for ( const auto &h : hitbox ) {
		// head.
		if ( h == 0 )
			m_hitboxes.push_back( { HITBOX_HEAD, HitscanMode::NORMAL } );

		// chest.
		if ( h == 1 ) {
			m_hitboxes.push_back( { HITBOX_THORAX, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_CHEST, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_UPPER_CHEST, HitscanMode::NORMAL } );
		}

		// stomach.
		if ( h == 2 ) {
			m_hitboxes.push_back( { HITBOX_PELVIS, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_BODY, HitscanMode::NORMAL } );
		}

		// arms.
		if ( h == 3 ) {
			m_hitboxes.push_back( { HITBOX_L_UPPER_ARM, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_R_UPPER_ARM, HitscanMode::NORMAL } );
		}

		// legs.
		if ( h == 4 ) {
			m_hitboxes.push_back( { HITBOX_L_THIGH, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_R_THIGH, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_L_CALF, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_R_CALF, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_L_FOOT, HitscanMode::NORMAL } );
			m_hitboxes.push_back( { HITBOX_R_FOOT, HitscanMode::NORMAL } );
		}
	}
}

void Aimbot::init( ) {
	// clear old targets.
	m_targets.clear( );

	m_target = nullptr;
	m_aim = vec3_t{ };
	m_angle = ang_t{ };
	m_damage = 0.f;
	m_record = nullptr;
	m_stop = false;

	m_best_dist = std::numeric_limits< float >::max( );
	m_best_fov = 180.f + 1.f;
	m_best_damage = 0.f;
	m_best_hp = 100 + 1;
	m_best_lag = std::numeric_limits< float >::max( );
	m_best_height = std::numeric_limits< float >::max( );
}

void Aimbot::StripAttack( ) {
	if ( g_cl.m_weapon_id == WEAPON_REVOLVER )
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK2;

	else
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK;
}

void Aimbot::think( ) {
	// do all startup routines.
	init( );

	// sanity.
	if ( !g_cl.m_weapon )
		return;

	// no grenades or bomb.
	if ( g_cl.m_weapon_type == WEAPONTYPE_GRENADE || g_cl.m_weapon_type == WEAPONTYPE_C4 )
		return;

	if ( !g_cl.m_weapon_fire )
		StripAttack( );

	// we have no aimbot enabled.
	if ( !g_menu.main.aimbot.enable.get( ) )
		return;

	// animation silent aim, prevent the ticks with the shot in it to become the tick that gets processed.
	// we can do this by always choking the tick before we are able to shoot.
	bool revolver = g_cl.m_weapon_id == WEAPON_REVOLVER && g_cl.m_revolver_cock != 0;

	// one tick before being able to shoot.
	if ( revolver && g_cl.m_revolver_cock > 0 && g_cl.m_revolver_cock == g_cl.m_revolver_query ) {
		g_cl.m_packet = false;
		return;
	}

	if ( !g_cl.m_weapon_fire )
		return;

	// setup bones for all valid targets.
	for ( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
		Player *player = g_csgo.m_entlist->GetClientEntity< Player * >( i );

		if ( !IsValidTarget( player ) )
			continue;

		AimPlayer *data = &m_players[ i - 1 ];
		if ( !data )
			continue;

		// store player as potential target this tick.
		m_targets.emplace_back( data );
	}

	// run knifebot.
	if ( g_cl.m_weapon_type == WEAPONTYPE_KNIFE && g_cl.m_weapon_id != WEAPON_TASER ) {

		if ( g_menu.main.aimbot.knifebot.get( ) )
			knife( );

		return;
	}

	// scan available targets... if we even have any.
	find( );

	// finally set data when shooting.
	apply( );
}

void Aimbot::find( ) {
	struct BestTarget_t { Player *player; vec3_t pos; float damage; LagRecord *record; };

	vec3_t       tmp_pos;
	float        tmp_damage;
	BestTarget_t best;
	best.player = nullptr;
	best.damage = -1.f;
	best.pos = vec3_t{ };
	best.record = nullptr;

	if ( m_targets.empty( ) )
		return;

	if ( g_cl.m_weapon_id == WEAPON_TASER && !g_menu.main.aimbot.zeusbot.get( ) )
		return;

	// iterate all targets.
	for ( const auto &t : m_targets ) {
		if ( t->m_records.empty( ) )
			continue;

		// this player broke lagcomp.
		// his bones have been resetup by our lagcomp.
		// therfore now only the front record is valid.
		if ( g_lagcomp.StartPrediction( t ) ) {
			LagRecord *front = t->m_records.front( ).get( );
			if ( front->m_shifting_tickbase )
				continue;

			t->SetupHitboxes( front, false );
			if ( t->m_hitboxes.empty( ) )
				continue;

			if ( t->GetBestAimPosition( tmp_pos, tmp_damage, front ) && SelectTarget( front, tmp_pos, tmp_damage ) ) {
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = front;
			}
		}
		else {
			LagRecord *ideal = g_resolver.FindIdealRecord( t );
			if ( ideal && !ideal->m_shifting_tickbase && !ideal->m_broke_lc ) {
				t->SetupHitboxes( ideal, false );
				if ( !t->m_hitboxes.empty( ) ) {
					if ( t->GetBestAimPosition( tmp_pos, tmp_damage, ideal ) && SelectTarget( ideal, tmp_pos, tmp_damage ) ) {
						best.player = t->m_player;
						best.pos = tmp_pos;
						best.damage = tmp_damage;
						best.record = ideal;
					}
				}
			}

			LagRecord *last = g_resolver.FindLastRecord( t );
			if ( last && last != ideal && !last->m_shifting_tickbase && !last->m_broke_lc ) {
				if ( !best.record || last->m_shot || best.damage < ( float )t->m_player->m_iHealth( ) ) {
					t->SetupHitboxes( last, true );
					if ( !t->m_hitboxes.empty( ) ) {
						if ( t->GetBestAimPosition( tmp_pos, tmp_damage, last ) && ( !best.record || ( last->m_shot && tmp_damage > best.damage ) ) ) {
							best.player = t->m_player;
							best.pos = tmp_pos;
							best.damage = tmp_damage;
							best.record = last;
						}
					}
				}
			}
		}
	}

	if ( !best.player && ( g_cl.m_flags & FL_ONGROUND ) && !( g_cl.m_buttons & IN_JUMP ) ) {
		bool can_stop = g_menu.main.movement.autostop_always_on.get( ) || ( !g_menu.main.movement.autostop_always_on.get( ) && g_menu.main.movement.autostop.get_status( ) );
		if ( can_stop || g_menu.main.movement.autopeek.get_status( ) ) {
			vec3_t vel = g_cl.m_local->m_vecVelocity( );
			if ( vel.length_2d( ) > 20.f ) {
				vec3_t pred_shoot_pos = g_cl.m_shoot_pos + vel * ( g_csgo.m_globals->m_interval * 3.f );
				for ( const auto &t : m_targets ) {
					if ( t->m_records.empty( ) )
						continue;

					LagRecord* front = t->m_records.front( ).get( );
					if ( !front || front->dormant( ) || front->immune( ) || !front->valid( ) )
						continue;

					penetration::PenetrationInput_t in;
					in.m_damage = 1.f;
					in.m_damage_pen = 1.f;
					in.m_can_pen = true;
					in.m_target = t->m_player;
					in.m_from = g_cl.m_local;
					in.m_pos = front->m_pred_origin + vec3_t( 0.f, 0.f, 48.f );

					penetration::PenetrationOutput_t out;
					vec3_t backup_shoot_pos = g_cl.m_shoot_pos;
					g_cl.m_shoot_pos = pred_shoot_pos;
					bool can_hit = penetration::run( &in, &out );
					g_cl.m_shoot_pos = backup_shoot_pos;

					float min_dmg = ( float )g_menu.main.aimbot.minimal_damage.get( );
					if ( g_menu.main.aimbot.minimal_damage_hp.get( ) )
						min_dmg = std::ceil( ( min_dmg * t->m_player->m_iHealth( ) ) / 100.f );
					if ( can_hit && out.m_damage >= min_dmg ) {
						m_stop = true;
						g_movement.QuickStop( );
						break;
					}
				}
			}
		}
	}

	if ( best.player && best.record ) {
		math::VectorAngles( best.pos - g_cl.m_shoot_pos, m_angle );

		m_target = best.player;
		m_aim = best.pos;
		m_damage = best.damage;
		m_record = best.record;

		m_record->cache( );

		m_stop = !( g_cl.m_buttons & IN_JUMP );

		bool can_stop = g_menu.main.movement.autostop_always_on.get( ) || ( !g_menu.main.movement.autostop_always_on.get( ) && g_menu.main.movement.autostop.get_status( ) );
		if ( ( g_menu.main.movement.autopeek.get_status( ) || can_stop ) && !( g_cl.m_cmd->m_buttons & IN_JUMP ) && !( g_cl.m_buttons & IN_JUMP ) ) {
			g_movement.QuickStop( );
		}

		if ( can_stop && ( g_cl.m_flags & FL_ONGROUND ) && !( g_cl.m_cmd->m_buttons & IN_JUMP ) && !( g_cl.m_buttons & IN_JUMP ) && g_cl.m_local->m_vecVelocity( ).length_2d( ) > 20.f && ( g_cl.m_weapon_type == WEAPONTYPE_SNIPER_RIFLE || g_cl.m_weapon_id == WEAPON_DEAGLE || g_cl.m_weapon_id == WEAPON_REVOLVER ) )
			return;

		bool on = g_menu.main.aimbot.hitchance.get( ) && g_menu.main.config.mode.get( ) == 0;
		bool hit = on && CheckHitchance( m_target, m_angle );

		// if we can scope.
		bool can_scope = !g_cl.m_local->m_bIsScoped( ) && ( g_cl.m_weapon_id == WEAPON_AUG || g_cl.m_weapon_id == WEAPON_SG556 || g_cl.m_weapon_type == WEAPONTYPE_SNIPER_RIFLE );

		if ( can_scope ) {
			// always.
			if ( g_menu.main.aimbot.zoom.get( ) == 1 ) {
				g_cl.m_cmd->m_buttons |= IN_ATTACK2;
				return;
			}

			// hitchance fail.
			else if ( g_menu.main.aimbot.zoom.get( ) == 2 && on && !hit ) {
				g_cl.m_cmd->m_buttons |= IN_ATTACK2;
				return;
			}
		}

		if ( hit || !on ) {
			// right click attack.
			if ( g_menu.main.config.mode.get( ) == 1 && g_cl.m_weapon_id == WEAPON_REVOLVER )
				g_cl.m_cmd->m_buttons |= IN_ATTACK2;

			// left click attack.
			else
				g_cl.m_cmd->m_buttons |= IN_ATTACK;
		}
	}
}

bool Aimbot::CheckHitchance( Player *player, const ang_t &angle ) {
	if ( !g_cl.m_weapon || !g_cl.m_weapon_info )
		return false;

	float hc_amount = g_menu.main.aimbot.hitchance_amount.get( );
	if ( hc_amount <= 0.f )
		return true;

	if ( !( g_cl.m_flags & FL_ONGROUND ) && g_cl.m_weapon_id == WEAPON_SSG08 ) {
		if ( g_cl.m_weapon->GetInaccuracy( ) < 0.009f )
			return true;
	}

	g_cl.m_weapon->UpdateAccuracyPenalty( );
	float inaccuracy = g_cl.m_weapon->GetInaccuracy( );
	float spread = g_cl.m_weapon->GetSpread( );

	if ( inaccuracy <= 0.0001f )
		return true;

	if ( ( g_cl.m_weapon_id == WEAPON_AWP || g_cl.m_weapon_id == WEAPON_SSG08 || g_cl.m_weapon_id == WEAPON_SCAR20 || g_cl.m_weapon_id == WEAPON_G3SG1 ) && g_cl.m_local && g_cl.m_local->m_bIsScoped( ) ) {
		if ( ( g_cl.m_flags & FL_ONGROUND ) && g_cl.m_local->m_vecVelocity( ).length_2d( ) < 5.f ) {
			float stand_acc = ( g_cl.m_flags & FL_DUCKING ) ? g_cl.m_weapon_info->flInaccuracyCrouch[ 1 ] : g_cl.m_weapon_info->flInaccuracyStand[ 1 ];
			if ( inaccuracy <= stand_acc + 0.0001f )
				return true;
		}
	}

	constexpr float HITCHANCE_MAX = 100.f;
	constexpr int   TOTAL_SEEDS = 128;

	vec3_t     start{ g_cl.m_shoot_pos }, end, fwd, right, up, dir, wep_spread;
	CGameTrace tr;
	size_t     total_hits{ }, needed_hits{ ( size_t )std::ceil( ( hc_amount * ( float )TOTAL_SEEDS ) / HITCHANCE_MAX ) };

	math::AngleVectors( angle, &fwd, &right, &up );

	bool revolver2 = g_cl.m_weapon_id == WEAPON_REVOLVER && ( g_cl.m_cmd->m_buttons & IN_ATTACK2 );

	for ( int i{ }; i < TOTAL_SEEDS; ++i ) {
		wep_spread = g_cl.m_weapon->CalculateSpread( i, inaccuracy, spread, revolver2 );
		dir = ( fwd + ( right * wep_spread.x ) + ( up * wep_spread.y ) ).normalized( );
		end = start + ( dir * g_cl.m_weapon_info->flRange );

		g_csgo.m_engine_trace->ClipRayToEntity( Ray( start, end ), MASK_SHOT, player, &tr );

		if ( tr.m_entity == player && game::IsValidHitgroup( tr.m_hitgroup ) )
			++total_hits;

		if ( total_hits >= needed_hits )
			return true;

		if ( ( TOTAL_SEEDS - ( i + 1 ) + total_hits ) < needed_hits )
			return false;
	}

	return false;
}

bool AimPlayer::SetupHitboxPoints( LagRecord *record, BoneArray *bones, int index, std::vector< vec3_t > &points ) {
	// reset points.
	points.clear( );

	const model_t *model = m_player->GetModel( );
	if ( !model )
		return false;

	studiohdr_t *hdr = g_csgo.m_model_info->GetStudioModel( model );
	if ( !hdr )
		return false;

	mstudiohitboxset_t *set = hdr->GetHitboxSet( m_player->m_nHitboxSet( ) );
	if ( !set )
		return false;

	mstudiobbox_t *bbox = set->GetHitbox( index );
	if ( !bbox )
		return false;

	float scale = g_menu.main.aimbot.scale.get( ) / 100.f;
	float bscale = g_menu.main.aimbot.body_scale.get( ) / 100.f;

	// these indexes represent boxes.
	if ( bbox->m_radius <= 0.f ) {
		// references: 
		//      https://developer.valvesoftware.com/wiki/Rotation_Tutorial
		//      CBaseAnimating::GetHitboxBonePosition
		//      CBaseAnimating::DrawServerHitboxes

		// convert rotation angle to a matrix.
		matrix3x4_t rot_matrix;
		g_csgo.AngleMatrix( bbox->m_angle, rot_matrix );

		// apply the rotation to the entity input space (local).
		matrix3x4_t matrix;
		math::ConcatTransforms( bones[ bbox->m_bone ], rot_matrix, matrix );

		// extract origin from matrix.
		vec3_t origin = matrix.GetOrigin( );

		// compute raw center point.
		vec3_t center = ( bbox->m_mins + bbox->m_maxs ) / 2.f;

		// the feet hiboxes have a side, heel and the toe.
		if ( index == HITBOX_R_FOOT || index == HITBOX_L_FOOT ) {
			float d1 = ( bbox->m_mins.z - center.z ) * 0.875f;

			// invert.
			if ( index == HITBOX_L_FOOT )
				d1 *= -1.f;

			// side is more optimal then center.
			points.push_back( { center.x, center.y, center.z + d1 } );

			if ( g_menu.main.aimbot.multipoint.get( 3 ) ) {
				// get point offset relative to center point
				// and factor in hitbox scale.
				float d2 = ( bbox->m_mins.x - center.x ) * scale;
				float d3 = ( bbox->m_maxs.x - center.x ) * scale;

				// heel.
				points.push_back( { center.x + d2, center.y, center.z } );

				// toe.
				points.push_back( { center.x + d3, center.y, center.z } );
			}
		}

		// nothing to do here we are done.
		if ( points.empty( ) )
			return false;

		// rotate our bbox points by their correct angle
		// and convert our points to world space.
		for ( auto &p : points ) {
			// VectorRotate.
			// rotate point by angle stored in matrix.
			p = { p.dot( matrix[ 0 ] ), p.dot( matrix[ 1 ] ), p.dot( matrix[ 2 ] ) };

			// transform point to world space.
			p += origin;
		}
	}

	// these hitboxes are capsules.
	else {
		// factor in the pointscale.
		float r = bbox->m_radius * scale;
		float br = bbox->m_radius * bscale;

		// compute raw center point.
		vec3_t center = ( bbox->m_mins + bbox->m_maxs ) / 2.f;

		// head has 5 points.
		if ( index == HITBOX_HEAD ) {
			points.push_back( center );
			if ( g_menu.main.aimbot.multipoint.get( 0 ) ) {
				vec3_t mins = bbox->m_mins;
				vec3_t maxs = bbox->m_maxs;
				vec3_t side = center - mins;
				float width = side.length( ) + bbox->m_radius;
				points.push_back( { center.x, center.y, center.z + width * scale * 0.89f } );
				points.push_back( { center.x + r * 0.70710678f, center.y - r * 0.70710678f, center.z } );
				points.push_back( { center.x, center.y + r, center.z } );
				points.push_back( { center.x, center.y - r, center.z } );
				points.push_back( { center.x, center.y, center.z - r } );
				CCSGOPlayerAnimState *state = record->m_player->m_PlayerAnimState( );
				if ( state && record->m_velocity.length( ) <= 0.1f && record->m_eye_angles.x <= state->m_min_pitch ) {
					points.push_back( { center.x - r, center.y, center.z } );
				}
			}
		}

		else if ( index == HITBOX_BODY ) {
			points.push_back( center );

			if ( g_menu.main.aimbot.multipoint.get( 2 ) ) {
				points.push_back( { center.x, bbox->m_maxs.y - br, center.z } );
				points.push_back( { center.x, bbox->m_mins.y + br, center.z } );
				points.push_back( { center.x, center.y, bbox->m_maxs.z - br } );
				points.push_back( { center.x, center.y, bbox->m_mins.z + br } );
			}
		}

		else if ( index == HITBOX_PELVIS || index == HITBOX_UPPER_CHEST ) {
			points.push_back( center );

			if ( g_menu.main.aimbot.multipoint.get( 1 ) ) {
				points.push_back( { center.x, bbox->m_maxs.y - br, center.z } );
				points.push_back( { center.x, bbox->m_mins.y + br, center.z } );
				points.push_back( { center.x, center.y, bbox->m_maxs.z - br } );
				points.push_back( { center.x, center.y, bbox->m_mins.z + br } );
			}
		}

		else if ( index == HITBOX_THORAX || index == HITBOX_CHEST ) {
			points.push_back( center );

			if ( g_menu.main.aimbot.multipoint.get( 1 ) ) {
				points.push_back( { center.x, bbox->m_maxs.y - br, center.z } );
				points.push_back( { center.x, bbox->m_mins.y + br, center.z } );
				points.push_back( { center.x, center.y, bbox->m_maxs.z - br } );
				points.push_back( { center.x, center.y, bbox->m_mins.z + br } );
			}
		}

		else if ( index == HITBOX_R_CALF || index == HITBOX_L_CALF ) {
			// add center.
			points.push_back( center );

			// half bottom.
			if ( g_menu.main.aimbot.multipoint.get( 3 ) )
				points.push_back( { bbox->m_maxs.x - ( bbox->m_radius / 2.f ), bbox->m_maxs.y, bbox->m_maxs.z } );
		}

		else if ( index == HITBOX_R_THIGH || index == HITBOX_L_THIGH ) {
			// add center.
			points.push_back( center );
		}

		// arms get only one point.
		else if ( index == HITBOX_R_UPPER_ARM || index == HITBOX_L_UPPER_ARM ) {
			// elbow.
			points.push_back( { bbox->m_maxs.x + bbox->m_radius, center.y, center.z } );
		}

		// nothing left to do here.
		if ( points.empty( ) )
			return false;

		// transform capsule points.
		for ( auto &p : points )
			math::VectorTransform( p, bones[ bbox->m_bone ], p );
	}

	return true;
}

bool AimPlayer::GetBestAimPosition( vec3_t &aim, float &damage, LagRecord *record ) {
	bool                  done, pen;
	float                 dmg, pendmg;
	HitscanData_t         scan;
	std::vector< vec3_t > points;

	// get player hp.
	int hp = std::min( 100, m_player->m_iHealth( ) );

	if ( g_cl.m_weapon_id == WEAPON_TASER ) {
		dmg = pendmg = hp;
		pen = false;
	}

	else {
		dmg = g_menu.main.aimbot.minimal_damage.get( );
		if ( g_menu.main.aimbot.minimal_damage_hp.get( ) )
			dmg = std::ceil( ( dmg / 100.f ) * hp );

		if ( g_aimbot.m_dmg_override ) {
			dmg = g_menu.main.aimbot.minimal_damage_override_amt.get( );
		}

		pendmg = dmg;
		pen = g_menu.main.aimbot.penetrate.get( );
	}

	if ( !g_bones.setup( m_player, nullptr, record ) )
		return false;

	record->cache( );

	// iterate hitboxes.
	for ( const auto &it : m_hitboxes ) {
		done = false;

		// setup points on hitbox.
		if ( !SetupHitboxPoints( record, record->m_bones, it.m_index, points ) )
			continue;

		// iterate points on hitbox.
		for ( const auto &point : points ) {
			penetration::PenetrationInput_t in;

			in.m_damage = dmg;
			in.m_damage_pen = pendmg;
			in.m_can_pen = pen;
			in.m_target = m_player;
			in.m_from = g_cl.m_local;
			in.m_pos = point;

			// ignore mindmg.
			if ( it.m_mode == HitscanMode::LETHAL || it.m_mode == HitscanMode::LETHAL2 )
				in.m_damage = in.m_damage_pen = 1.f;

			penetration::PenetrationOutput_t out;

			// we can hit p!
			if ( penetration::run( &in, &out ) ) {

				// prefered hitbox, just stop now.
				if ( it.m_mode == HitscanMode::PREFER )
					done = true;

				// this hitbox requires lethality to get selected, if that is the case.
				// we are done, stop now.
				else if ( it.m_mode == HitscanMode::LETHAL && out.m_damage >= m_player->m_iHealth( ) )
					done = true;

				// 2 shots will be sufficient to kill.
				else if ( it.m_mode == HitscanMode::LETHAL2 && ( out.m_damage * 2.f ) >= m_player->m_iHealth( ) )
					done = true;

				// this hitbox has normal selection, it needs to have more damage.
				else if ( it.m_mode == HitscanMode::NORMAL ) {
					if ( out.m_damage > scan.m_damage ) {
						scan.m_damage = out.m_damage;
						scan.m_pos = point;

						if ( out.m_damage >= m_player->m_iHealth( ) ) {
							done = true;
							break;
						}
					}
				}

				// we found a preferred / lethal hitbox.
				if ( done ) {
					// save new best data.
					scan.m_damage = out.m_damage;
					scan.m_pos = point;
					break;
				}
			}
		}

		// ghetto break out of outer loop.
		if ( done )
			break;
	}

	// we found something that we can damage.
	// set out vars.
	if ( scan.m_damage > 0.f ) {
		aim = scan.m_pos;
		damage = scan.m_damage;
		return true;
	}

	return false;
}

bool Aimbot::SelectTarget( LagRecord *record, const vec3_t &aim, float damage ) {
	float dist, fov, height;
	int   hp;

	// fov check.
	if ( g_menu.main.aimbot.fov.get( ) ) {
		// if out of fov, retn false.
		if ( math::GetFOV( g_cl.m_view_angles, g_cl.m_shoot_pos, aim ) > g_menu.main.aimbot.fov_amount.get( ) )
			return false;
	}

	switch ( g_menu.main.aimbot.selection.get( ) ) {

		// distance.
	case 0:
		dist = ( record->m_pred_origin - g_cl.m_shoot_pos ).length( );

		if ( dist < m_best_dist ) {
			m_best_dist = dist;
			return true;
		}

		break;

		// crosshair.
	case 1:
		fov = math::GetFOV( g_cl.m_view_angles, g_cl.m_shoot_pos, aim );

		if ( fov < m_best_fov ) {
			m_best_fov = fov;
			return true;
		}

		break;

		// damage.
	case 2:
		if ( damage > m_best_damage ) {
			m_best_damage = damage;
			return true;
		}

		break;

		// lowest hp.
	case 3:
		// fix for retarded servers?
		hp = std::min( 100, record->m_player->m_iHealth( ) );

		if ( hp < m_best_hp ) {
			m_best_hp = hp;
			return true;
		}

		break;

		// least lag.
	case 4:
		if ( record->m_lag < m_best_lag ) {
			m_best_lag = record->m_lag;
			return true;
		}

		break;

		// height.
	case 5:
		height = record->m_pred_origin.z - g_cl.m_local->m_vecOrigin( ).z;

		if ( height < m_best_height ) {
			m_best_height = height;
			return true;
		}

		break;

	default:
		return false;
	}

	return false;
}

void Aimbot::apply( ) {
	bool attack, attack2;

	// attack states.
	attack = ( g_cl.m_cmd->m_buttons & IN_ATTACK );
	attack2 = ( g_cl.m_weapon_id == WEAPON_REVOLVER && g_cl.m_cmd->m_buttons & IN_ATTACK2 );

	// ensure we're attacking.
	if ( attack || attack2 ) {
		// choke every shot.
		g_cl.m_packet = false;

		if ( m_target ) {
			if ( m_record && !m_record->m_broke_lc && !m_record->m_shifting_tickbase )
				g_cl.m_cmd->m_tick = game::TIME_TO_TICKS( m_record->m_sim_time + g_cl.m_lerp );

			g_cl.m_cmd->m_view_angles = m_angle;

			// if not silent aim, apply the viewangles.
			if ( !g_menu.main.aimbot.silent.get( ) )
				g_csgo.m_engine->SetViewAngles( m_angle );

			g_visuals.DrawHitboxMatrix( m_record, colors::white, 10.f );
		}

		// nospread.
		if ( g_menu.main.aimbot.nospread.get( ) && g_menu.main.config.mode.get( ) == 1 )
			NoSpread( );

		// norecoil.
		if ( g_menu.main.aimbot.norecoil.get( ) )
			g_cl.m_cmd->m_view_angles -= g_cl.m_local->m_aimPunchAngle( ) * g_csgo.weapon_recoil_scale->GetFloat( );

		float hc = g_menu.main.aimbot.hitchance.get( ) ? g_menu.main.aimbot.hitchance_amount.get( ) : 0.f;
		bool resolved = g_menu.main.aimbot.correct.get( ) && m_record && !m_record->m_shot;
		g_shots.OnShotFire( m_target ? m_target : nullptr, m_target ? m_damage : -1.f, g_cl.m_weapon_info->iBullets, m_target ? m_record : nullptr, m_aim, hc, resolved );

		// set that we fired.
		g_cl.m_shot = true;
		g_cl.m_last_shot_time = g_csgo.m_globals->m_realtime;

		if ( g_exploits.GetExploitType( ) == Exploits::E_DoubleTap )
			g_exploits.ForceTeleport( );
		else if ( g_exploits.GetExploitType( ) == Exploits::E_HideShots )
			g_exploits.HideShot( );

		if ( !g_hvh.m_fake_duck )
			g_cl.m_packet = true;
	}
}

void Aimbot::NoSpread( ) {
	bool    attack2;
	vec3_t  spread, forward, right, up, dir;

	// revolver state.
	attack2 = ( g_cl.m_weapon_id == WEAPON_REVOLVER && ( g_cl.m_cmd->m_buttons & IN_ATTACK2 ) );

	// get spread.
	spread = g_cl.m_weapon->CalculateSpread( g_cl.m_cmd->m_random_seed, attack2 );

	// compensate.
	g_cl.m_cmd->m_view_angles -= { -math::rad_to_deg( std::atan( spread.length_2d( ) ) ), 0.f, math::rad_to_deg( std::atan2( spread.x, spread.y ) ) };
}