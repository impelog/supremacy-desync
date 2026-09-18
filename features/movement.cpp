#include "includes.h"

Movement g_movement{ };

void Movement::JumpRelated( ) {
	if ( !g_cl.m_local || !g_cl.m_processing || g_cl.m_local->m_MoveType( ) == MOVETYPE_NOCLIP || g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER )
		return;

	if ( g_menu.main.movement.bhop.get( ) && ( g_cl.m_cmd->m_buttons & IN_JUMP ) ) {
		static ConVar* sv_auto_bhop = g_csgo.m_cvar ? g_csgo.m_cvar->FindVar( HASH( "sv_autobunnyhopping" ) ) : nullptr;
		if ( sv_auto_bhop && sv_auto_bhop->GetInt( ) )
			return;

		if ( g_cl.m_flags & FL_ONGROUND )
			g_cl.m_cmd->m_buttons |= IN_JUMP;
		else
			g_cl.m_cmd->m_buttons &= ~IN_JUMP;
	}
}

void Movement::Strafe( ) 
{
	if ( !g_cl.m_local || !g_cl.m_processing || !g_menu.main.movement.autostrafe.get( ) )
		return;

	if ( g_cl.m_flags & FL_ONGROUND )
		return;

	if ( g_menu.main.movement.slowwalk.get_status( ) )
		return;

	int move_type = g_cl.m_local->m_MoveType( );
	if ( move_type == MOVETYPE_NOCLIP || move_type == MOVETYPE_LADDER )
		return;

	const auto& velocity = g_cl.m_local->m_vecVelocity( );
	auto wish_angles = g_cl.m_view_angles;

	if ( g_cl.m_cmd->m_forward_move != 0.f || g_cl.m_cmd->m_side_move != 0.f ) {
		wish_angles.y = std::remainder(
			wish_angles.y
			+ std::remainder(
				math::rad_to_deg(
					std::atan2( g_cl.m_cmd->m_forward_move, g_cl.m_cmd->m_side_move )
				) - 90.f, 360.f
			), 360.f
		);
	}

	g_cl.m_cmd->m_forward_move = g_cl.m_cmd->m_side_move = 0.f;

	const auto speed_2d = velocity.length_2d( );
	const auto ideal_strafe = std::min( 90.f, math::rad_to_deg( std::asin( 15.f / std::max( speed_2d, 15.f ) ) ) );

	const auto mult = m_switch_value > 0.f ? 1.f : -1.f;
	m_switch_value = -m_switch_value;

	auto delta = std::remainder( wish_angles.y - m_old_yaw, 360.f );
	if ( delta )
		g_cl.m_cmd->m_side_move = delta < 0.f ? 450.f : -450.f;

	delta = std::abs( delta );

	if ( delta >= 30.f || ideal_strafe >= delta ) {
		const auto vel_angle = math::rad_to_deg( std::atan2( velocity.y, velocity.x ) );
		const auto vel_delta = std::remainder( wish_angles.y - vel_angle, 360.f );

		if ( speed_2d <= 15.f || ideal_strafe >= vel_delta ) {
			if ( speed_2d <= 15.f || vel_delta >= -ideal_strafe ) {
				g_cl.m_cmd->m_side_move = 450.f * mult;
				wish_angles.y += ideal_strafe * mult;
			}
			else {
				g_cl.m_cmd->m_side_move = 450.f;
				wish_angles.y = vel_angle - ideal_strafe;
			}
		}
		else {
			g_cl.m_cmd->m_side_move = -450.f;
			wish_angles.y = vel_angle + ideal_strafe;
		}
	}

	m_old_yaw = wish_angles.y;
	g_cl.m_strafe_angles = wish_angles;
}

void Movement::DoPrespeed( ) {
	float   mod, min, max, step, strafe, time, angle;
	vec3_t  plane;

	// min and max values are based on 128 ticks.
	mod = g_csgo.m_globals->m_interval * 128.f;

	// scale min and max based on tickrate.
	min = 2.25f * mod;
	max = 5.f * mod;

	// compute ideal strafe angle for moving in a circle.
	strafe = m_ideal * 2.f;

	// clamp ideal strafe circle value to min and max step.
	math::clamp( strafe, min, max );

	// calculate time.
	time = 320.f / m_speed;

	// clamp time.
	math::clamp( time, 0.35f, 1.f );

	// init step.
	step = strafe;

	while ( true ) {
		// if we will not collide with an object or we wont accelerate from such a big step anymore then stop.
		if ( !WillCollide( time, step ) || max <= step )
			break;

		// if we will collide with an object with the current strafe step then increment step to prevent a collision.
		step += 0.2f;
	}

	if ( step > max ) {
		// reset step.
		step = strafe;

		while ( true ) {
			// if we will not collide with an object or we wont accelerate from such a big step anymore then stop.
			if ( !WillCollide( time, step ) || step <= -min )
				break;

			// if we will collide with an object with the current strafe step decrement step to prevent a collision.
			step -= 0.2f;
		}

		if ( step < -min ) {
			if ( GetClosestPlane( plane ) ) {
				// grab the closest object normal
				// compute the angle of the normal
				// and push us away from the object.
				angle = math::rad_to_deg( std::atan2( plane.y, plane.x ) );
				step = -math::NormalizedAngle( m_circle_yaw - angle ) * 0.1f;
			}
		}

		else
			step -= 0.2f;
	}

	else
		step += 0.2f;

	// add the computed step to the steps of the previous circle iterations.
	m_circle_yaw = math::NormalizedAngle( m_circle_yaw + step );

	// apply data to usercmd.
	g_cl.m_cmd->m_view_angles.y = m_circle_yaw;
	g_cl.m_cmd->m_side_move = ( step >= 0.f ) ? -450.f : 450.f;
}

bool Movement::GetClosestPlane( vec3_t& plane ) {
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;
	vec3_t                start{ m_origin };
	float                 smallest{ 1.f };
	const float		      dist{ 75.f };

	// trace around us in a circle
	for ( float step{ }; step <= math::pi_2; step += ( math::pi / 10.f ) ) {
		// extend endpoint x units.
		vec3_t end = start;
		end.x += std::cos( step ) * dist;
		end.y += std::sin( step ) * dist;

		g_csgo.m_engine_trace->TraceRay( Ray( start, end, m_mins, m_maxs ), CONTENTS_SOLID, &filter, &trace );

		// we found an object closer, then the previouly found object.
		if ( trace.m_fraction < smallest ) {
			// save the normal of the object.
			plane = trace.m_plane.m_normal;
			smallest = trace.m_fraction;
		}
	}

	// did we find any valid object?
	return smallest != 1.f && plane.z < 0.1f;
}

bool Movement::WillCollide( float time, float change ) {
	struct PredictionData_t {
		vec3_t start;
		vec3_t end;
		vec3_t velocity;
		float  direction;
		bool   ground;
		float  predicted;
	};

	PredictionData_t      data;
	CGameTrace            trace;
	CTraceFilterWorldOnly filter;

	// set base data.
	data.ground = g_cl.m_flags & FL_ONGROUND;
	data.start = m_origin;
	data.end = m_origin;
	data.velocity = g_cl.m_local->m_vecVelocity( );
	data.direction = math::rad_to_deg( std::atan2( data.velocity.y, data.velocity.x ) );

	for ( data.predicted = 0.f; data.predicted < time; data.predicted += g_csgo.m_globals->m_interval ) {
		// predict movement direction by adding the direction change.
		// make sure to normalize it, in case we go over the -180/180 turning point.
		data.direction = math::NormalizedAngle( data.direction + change );

		// pythagoras.
		float hyp = data.velocity.length_2d( );

		// adjust velocity for new direction.
		data.velocity.x = std::cos( math::deg_to_rad( data.direction ) ) * hyp;
		data.velocity.y = std::sin( math::deg_to_rad( data.direction ) ) * hyp;

		// assume we bhop, set upwards impulse.
		if ( data.ground )
			data.velocity.z = g_csgo.sv_jump_impulse->GetFloat( );

		else
			data.velocity.z -= g_csgo.sv_gravity->GetFloat( ) * g_csgo.m_globals->m_interval;

		// we adjusted the velocity for our new direction.
		// see if we can move in this direction, predict our new origin if we were to travel at this velocity.
		data.end += ( data.velocity * g_csgo.m_globals->m_interval );

		// trace
		g_csgo.m_engine_trace->TraceRay( Ray( data.start, data.end, m_mins, m_maxs ), MASK_PLAYERSOLID, &filter, &trace );

		// check if we hit any objects.
		if ( trace.m_fraction != 1.f && trace.m_plane.m_normal.z <= 0.9f )
			return true;
		if ( trace.m_startsolid || trace.m_allsolid )
			return true;

		// adjust start and end point.
		data.start = data.end = trace.m_endpos;

		// move endpoint 2 units down, and re-trace.
		// do this to check if we are on th floor.
		g_csgo.m_engine_trace->TraceRay( Ray( data.start, data.end - vec3_t{ 0.f, 0.f, 2.f }, m_mins, m_maxs ), MASK_PLAYERSOLID, &filter, &trace );

		// see if we moved the player into the ground for the next iteration.
		data.ground = trace.hit( ) && trace.m_plane.m_normal.z > 0.7f;
	}

	// the entire loop has ran
	// we did not hit shit.
	return false;
}

void Movement::FixMove( CUserCmd* cmd, const ang_t& wish_angles ) {
	if ( cmd->m_view_angles.z != 0.f && !( g_cl.m_flags & FL_ONGROUND ) )
		cmd->m_side_move = 0.f;

	vec2_t move_2d = { cmd->m_forward_move, cmd->m_side_move };

	if ( const auto speed_2d = move_2d.length( ) ) {
		const auto delta = cmd->m_view_angles.y - wish_angles.y;

		float rad = math::deg_to_rad(
			std::remainder(
				math::rad_to_deg(
					std::atan2( move_2d.y / speed_2d, move_2d.x / speed_2d )
				) + delta, 360.f
			)
		);

		vec2_t v1 = { std::sin( rad ), std::cos( rad ) };

		const auto cos_x = std::cos(
			math::deg_to_rad(
				std::remainder(
					math::rad_to_deg( std::atan2( 0.f, speed_2d ) ), 360.f
				)
			)
		);

		move_2d.x = cos_x * v1.y * speed_2d;
		move_2d.y = cos_x * v1.x * speed_2d;

		if ( g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER ) {
			if ( wish_angles.x < 45.f
				&& std::abs( delta ) <= 65.f
				&& cmd->m_view_angles.x >= 45.f ) {
				move_2d.x *= -1.f;
			}
		}
		else if ( std::abs( cmd->m_view_angles.x ) > 90.f )
			move_2d.x *= -1.f;
	}

	cmd->m_forward_move = std::clamp( move_2d.x, -450.f, 450.f );
	cmd->m_side_move = std::clamp( move_2d.y, -450.f, 450.f );
	cmd->m_up_move = std::clamp( cmd->m_up_move, -320.f, 320.f );

	if ( g_menu.main.config.mode.get( ) == 0 ) {
		cmd->m_view_angles.x = std::clamp( cmd->m_view_angles.x, -89.f, 89.f );
		cmd->m_view_angles.y = math::NormalizedAngle( cmd->m_view_angles.y );
		cmd->m_view_angles.z = 0.f;
	}

	cmd->m_buttons &= ~( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT );

	if ( g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER ) {
		if ( std::abs( cmd->m_forward_move ) > 200.f )
			cmd->m_buttons |= cmd->m_forward_move > 0.f ? IN_FORWARD : IN_BACK;

		if ( std::abs( cmd->m_side_move ) > 200.f )
			cmd->m_buttons |= cmd->m_side_move > 0.f ? IN_MOVERIGHT : IN_MOVELEFT;

		return;
	}

	if ( cmd->m_forward_move > 0.f )
		cmd->m_buttons |= IN_FORWARD;
	else if ( cmd->m_forward_move < 0.f )
		cmd->m_buttons |= IN_BACK;

	if ( cmd->m_side_move > 0.f )
		cmd->m_buttons |= IN_MOVERIGHT;
	else if ( cmd->m_side_move < 0.f )
		cmd->m_buttons |= IN_MOVELEFT;
}

void Movement::AutoPeek( ) {
	if ( !g_cl.m_local || !g_cl.m_processing || !g_cl.m_local->alive( ) ) {
		m_peeking = false;
		m_retracting = false;
		return;
	}

	if ( g_menu.main.movement.autopeek.get_status( ) ) {
		if ( !m_peeking ) {
			if ( g_cl.m_flags & FL_ONGROUND ) {
				m_peeking = true;
				m_retracting = false;
				m_peek_pos = g_cl.m_local->m_vecOrigin( );
			}
		}
		else {
			vec3_t current_pos = g_cl.m_local->m_vecOrigin( );
			vec3_t delta = m_peek_pos - current_pos;
			delta.z = 0.f;

			float dist = delta.length_2d( );

			Weapon* weapon = g_cl.m_weapon;
			if ( weapon && g_cl.m_weapon_type != WEAPONTYPE_GRENADE ) {
				bool is_attack = ( g_cl.m_cmd->m_buttons & IN_ATTACK ) || ( weapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER && ( g_cl.m_cmd->m_buttons & IN_ATTACK2 ) );
				if ( is_attack || g_cl.m_shot || g_cl.m_old_shot )
					m_retracting = true;
			}

			if ( m_retracting ) {
				vec3_t velocity = g_cl.m_local->m_vecVelocity( );
				velocity.z = 0.f;
				float speed = velocity.length_2d( );

				if ( dist <= 2.5f ) {
					QuickStop( );
					if ( speed <= 15.f )
						m_retracting = false;
				}
				else {
					ang_t ang;
					math::VectorAngles( delta, ang );

					ang.y = math::NormalizedAngle( g_cl.m_cmd->m_view_angles.y - ang.y );

					float vel = 450.f;
					if ( dist < 30.f )
						vel = std::clamp( dist * 12.f, 20.f, 450.f );

					vec3_t dir;
					math::AngleVectors( ang, &dir );
					dir *= vel;

					g_cl.m_cmd->m_forward_move = dir.x;
					g_cl.m_cmd->m_side_move = dir.y;
				}
			}
		}
	}
	else {
		m_peeking = false;
		m_retracting = false;
	}

	bool can_stop = g_menu.main.movement.autostop_always_on.get( ) || ( !g_menu.main.movement.autostop_always_on.get( ) && g_menu.main.movement.autostop.get_status( ) );
	if ( can_stop && g_aimbot.m_stop && !m_retracting ) {
		Movement::QuickStop( );
	}
}

void Movement::QuickStop( ) {
	if ( !g_cl.m_local || !g_cl.m_processing || !( g_cl.m_flags & FL_ONGROUND ) )
		return;

	if ( ( g_cl.m_cmd->m_buttons & IN_JUMP ) || ( g_cl.m_buttons & IN_JUMP ) )
		return;

	vec3_t velocity = g_cl.m_local->m_vecVelocity( );
	velocity.z = 0.f;

	float speed_2d = velocity.length_2d( );
	if ( speed_2d <= 1.f ) {
		g_cl.m_cmd->m_forward_move = 0.f;
		g_cl.m_cmd->m_side_move = 0.f;
		return;
	}

	ang_t angle;
	math::VectorAngles( velocity * -1.f, angle );
	angle.y = math::NormalizedAngle( g_cl.m_cmd->m_view_angles.y - angle.y );

	vec3_t direction;
	math::AngleVectors( angle, &direction );

	float max_speed = 450.f;
	if ( g_cl.m_weapon_info )
		max_speed = g_cl.m_local->m_bIsScoped( ) ? g_cl.m_weapon_info->flMaxSpeed[ 1 ] : g_cl.m_weapon_info->flMaxSpeed[ 0 ];

	float target_move = ( speed_2d > 20.f ) ? 450.f : std::min( speed_2d * 22.5f, max_speed );

	g_cl.m_cmd->m_buttons &= ~IN_SPEED;

	g_cl.m_cmd->m_forward_move = direction.x * target_move;
	g_cl.m_cmd->m_side_move = direction.y * target_move;

	float duck_amount = g_cl.m_local->m_flDuckAmount( );
	if ( duck_amount > 0.f ) {
		float factor = duck_amount * 0.34f + 1.f - duck_amount;
		if ( factor > 0.f ) {
			g_cl.m_cmd->m_forward_move /= factor;
			g_cl.m_cmd->m_side_move /= factor;
		}
	}
}

void Movement::SlowWalk( ) {
	if ( !g_cl.m_local || !g_cl.m_processing || !( g_cl.m_flags & FL_ONGROUND ) )
		return;

	if ( !g_menu.main.movement.slowwalk.get_status( ) )
		return;

	Weapon* weapon = g_cl.m_weapon;
	if ( !weapon )
		return;

	WeaponInfo* data = g_cl.m_weapon_info;
	if ( !data )
		return;

	float max_speed = g_cl.m_local->m_bIsScoped( ) ? data->flMaxSpeed[ 1 ] : data->flMaxSpeed[ 0 ];
	float target_speed = max_speed * 0.33f;

	vec3_t move{ g_cl.m_cmd->m_forward_move, g_cl.m_cmd->m_side_move, 0.f };
	float speed = move.length( );

	if ( speed > 0.f ) {
		float scale = target_speed / speed;
		if ( scale < 1.f ) {
			g_cl.m_cmd->m_forward_move *= scale;
			g_cl.m_cmd->m_side_move *= scale;
		}
	}
}

