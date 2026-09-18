#include "includes.h"

LagCompensation g_lagcomp{};

bool LagCompensation::StartPrediction( AimPlayer* data ) {
	if ( data->m_records.size( ) < 2 )
		return false;

	if ( data->m_player->dormant( ) )
		return false;

	// compute the true amount of updated records
	// since the last time the player entered pvs.
	size_t size{};

	// iterate records.
	for ( const auto& it : data->m_records ) {
		if ( it->dormant( ) )
			break;

		// increment total amount of data.
		++size;
	}

	// get first record.
	LagRecord* record = data->m_records[ 0 ].get( );

	// reset all prediction related variables.
	// this has been a recurring problem in all my hacks lmfao.
	// causes the prediction to stack on eachother.
	record->predict( );

	// check if lc broken.
	if ( size > 1 && ( ( record->m_origin - data->m_records[ 1 ]->m_origin ).length_2d_sqr( ) > 4096.f
		|| size > 2 && ( data->m_records[ 1 ]->m_origin - data->m_records[ 2 ]->m_origin ).length_2d_sqr( ) > 4096.f ) )
		record->m_broke_lc = true;

	// we are not breaking lagcomp at this point.
	// return false so it can aim at all the records it once
	// since server-sided lagcomp is still active and we can abuse that.
	if ( !record->m_broke_lc )
		return false;

	int simulation = game::TIME_TO_TICKS( record->m_sim_time );

	// this is too much lag to fix.
	if ( std::abs( g_cl.m_arrival_tick - simulation ) >= 128 )
		return true;

	// compute the amount of lag that we will predict for, if we have one set of data, use that.
	// if we have more data available, use the prevoius lag delta to counter weird fakelags that switch between 14 and 2.
	int lag = ( size <= 2 ) ? game::TIME_TO_TICKS( record->m_sim_time - data->m_records[ 1 ]->m_sim_time )
		: game::TIME_TO_TICKS( data->m_records[ 1 ]->m_sim_time - data->m_records[ 2 ]->m_sim_time );

	// clamp this just to be sure.
	math::clamp( lag, 1, 15 );

	// get the delta in ticks between the last server net update
	// and the net update on which we created this record.
	int updatedelta = g_cl.m_server_tick - record->m_tick;

	// if the lag delta that is remaining is less than the current netlag
	// that means that we can shoot now and when our shot will get processed
	// the origin will still be valid, therefore we do not have to predict.
	if ( g_cl.m_latency_ticks <= lag - updatedelta )
		return true;

	// the next update will come in, wait for it.
	int next = record->m_tick + 1;
	if ( next + lag >= g_cl.m_arrival_tick )
		return true;

	float change = 0.f, dir = 0.f;

	// get the direction of the current velocity.
	if ( record->m_velocity.y != 0.f || record->m_velocity.x != 0.f )
		dir = math::rad_to_deg( std::atan2( record->m_velocity.y, record->m_velocity.x ) );

	// we have more than one update
	// we can compute the direction.
	if ( size > 1 ) {
		// get the delta time between the 2 most recent records.
		float dt = record->m_sim_time - data->m_records[ 1 ]->m_sim_time;

		// init to 0.
		float prevdir = 0.f;

		// get the direction of the prevoius velocity.
		if ( data->m_records[ 1 ]->m_velocity.y != 0.f || data->m_records[ 1 ]->m_velocity.x != 0.f )
			prevdir = math::rad_to_deg( std::atan2( data->m_records[ 1 ]->m_velocity.y, data->m_records[ 1 ]->m_velocity.x ) );

		// compute the direction change per tick.
		change = ( math::NormalizedAngle( dir - prevdir ) / dt ) * g_csgo.m_globals->m_interval;
	}

	if ( std::abs( change ) > 6.f )
		change = 0.f;

	// get the pointer to the players animation state.
	CCSGOPlayerAnimState* state = data->m_player->m_PlayerAnimState( );

	// backup the animation state.
	CCSGOPlayerAnimState backup{};
	if ( state )
		std::memcpy( &backup, state, sizeof( CCSGOPlayerAnimState ) );

	// add in the shot prediction here.
	int shot = 0;

	/*Weapon* pWeapon = data->m_player->GetActiveWeapon( );
	if( pWeapon && !data->m_fire_bullet.empty( ) ) {

		static Address offset = g_netvars.get( HASH( "DT_BaseCombatWeapon" ), HASH( "m_fLastShotTime" ) );
		float last = pWeapon->get< float >( offset );

		if( game::TIME_TO_TICKS( data->m_fire_bullet.front( ).m_sim_time - last ) == 1 ) {
			WeaponInfo* wpndata = pWeapon->GetWpnData( );

			if( wpndata )
				shot = game::TIME_TO_TICKS( last + wpndata->m_cycletime ) + 1;
		}
	}*/

	int pred = 0;

	// start our predicton loop.
	while ( true ) {
		// can the player shoot within his lag delta.
		/*if( shot && shot >= simulation && shot < simulation + lag ) {

			// if so his new lag will be the time until he shot again.
			lag = shot - simulation;
			math::clamp( lag, 3, 15 );

			// only predict a shot once.
			shot = 0;
		}*/

		// see if by predicting this amount of lag
		// we do not break stuff.
		next += lag;
		if ( next >= g_cl.m_arrival_tick )
			break;

		for ( int sim{}; sim < lag; ++sim ) {
			if ( !( record->m_pred_flags & FL_ONGROUND ) )
				record->m_pred_velocity.z -= g_csgo.sv_gravity->GetFloat( ) * g_csgo.m_globals->m_interval;

			PlayerMove( record );

			record->m_pred_time += g_csgo.m_globals->m_interval;
			++pred;

			if ( sim == 0 && state )
				PredictAnimations( state, record );
		}
	}

	// restore state.
	if ( state )
		std::memcpy( state, &backup, sizeof( CCSGOPlayerAnimState ) );

	if ( pred <= 0 )
		return true;

	// lagcomp broken, invalidate bones.
	record->invalidate( );

	// re-setup bones for this record.
	g_bones.setup( data->m_player, nullptr, record );

	return true;
}

void LagCompensation::PlayerMove( LagRecord* record ) {
    vec3_t                start, end, normal;
    CGameTrace            trace;
    CTraceFilterWorldOnly filter;

    // define trace start.
    start = record->m_pred_origin;

    // move trace end one tick into the future using predicted velocity.
    end = start + ( record->m_pred_velocity * g_csgo.m_globals->m_interval );

    // trace.
    g_csgo.m_engine_trace->TraceRay( Ray( start, end, record->m_mins, record->m_maxs ), CONTENTS_SOLID, &filter, &trace );

    // we hit shit
    // we need to fix hit.
    if ( trace.m_fraction != 1.f ) {
        // fix sliding on planes.
        for ( int i{}; i < 2; ++i ) {
            if ( record->m_pred_velocity.length( ) == 0.f )
                break;

            record->m_pred_velocity -= trace.m_plane.m_normal * record->m_pred_velocity.dot( trace.m_plane.m_normal );

            float adjust = record->m_pred_velocity.dot( trace.m_plane.m_normal );
            if ( adjust < 0.f )
                record->m_pred_velocity -= ( trace.m_plane.m_normal * adjust );

            start = trace.m_endpos;
            end = start + ( record->m_pred_velocity * ( g_csgo.m_globals->m_interval * ( 1.f - trace.m_fraction ) ) );

            g_csgo.m_engine_trace->TraceRay( Ray( start, end, record->m_mins, record->m_maxs ), CONTENTS_SOLID, &filter, &trace );
            if ( trace.m_fraction == 1.f )
                break;
        }
    }

    // set new final origin.
    start = end = record->m_pred_origin = trace.m_endpos;

    // move endpos 2 units down.
    // this way we can check if we are in/on the ground.
    end.z -= 2.f;

    // trace.
    g_csgo.m_engine_trace->TraceRay( Ray( start, end, record->m_mins, record->m_maxs ), CONTENTS_SOLID, &filter, &trace );

    // strip onground flag.
    if ( trace.m_fraction == 1.f && trace.m_plane.m_normal.z < 0.7f )
        record->m_pred_flags &= ~FL_ONGROUND;

    // add back onground flag if we are onground.
    else
        record->m_pred_flags |= FL_ONGROUND;

    if ( record->m_pred_flags & FL_ONGROUND )
        record->m_layers[ 4 ].m_cycle = record->m_layers[ 4 ].m_weight = 0.f;
}

void LagCompensation::PredictAnimations( CCSGOPlayerAnimState* state, LagRecord* record ) {
	struct AnimBackup_t {
		float  curtime;
		float  frametime;
		int    flags;
		int    eflags;
		vec3_t velocity;
	};

	// get player ptr.
	Player* player = record->m_player;

	// backup data.
	AnimBackup_t backup;
	backup.curtime = g_csgo.m_globals->m_curtime;
	backup.frametime = g_csgo.m_globals->m_frametime;
	backup.flags = player->m_fFlags( );
	backup.eflags = player->m_iEFlags( );
	backup.velocity = player->m_vecAbsVelocity( );

	// set globals appropriately for animation.
	g_csgo.m_globals->m_curtime = record->m_pred_time;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;

	// EFL_DIRTY_ABSVELOCITY
	// skip call to C_BaseEntity::CalcAbsoluteVelocity
	player->m_iEFlags( ) &= ~0x1000;

	// set predicted flags and velocity.
	player->m_fFlags( ) = record->m_pred_flags;
	player->m_vecAbsVelocity( ) = record->m_pred_velocity;

	// enable re-animation in the same frame if animated already.
	if ( state->m_frame >= g_csgo.m_globals->m_frame )
		state->m_frame = g_csgo.m_globals->m_frame - 1;

	if ( !state->m_player )
		state->m_player = player;

	game::UpdateAnimationState( state, record->m_eye_angles );

	// get new rotation poses and layers.
	player->GetPoseParameters( record->m_poses );
	player->GetAnimLayers( record->m_layers );
	record->m_abs_ang = player->GetAbsAngles( );

	// restore globals.
	g_csgo.m_globals->m_curtime = backup.curtime;
	g_csgo.m_globals->m_frametime = backup.frametime;

	// restore player data.
	player->m_fFlags( ) = backup.flags;
	player->m_iEFlags( ) = backup.eflags;
	player->m_vecAbsVelocity( ) = backup.velocity;
}