#include "includes.h"

InputPrediction g_inputpred{};;

void InputPrediction::update( ) {
	bool        valid{ g_csgo.m_cl->iDeltaTick > 0 };
	//int         outgoing_command, current_command;
	//CUserCmd    *cmd;

	// render start was not called.
	if ( g_cl.m_stage == FRAME_NET_UPDATE_END ) {
		/*outgoing_command = g_csgo.m_cl->m_last_outgoing_command + g_csgo.m_cl->m_choked_commands;

		// this must be done before update ( update will mark the unpredicted commands as predicted ).
		for( int i{}; ; ++i ) {
			current_command = g_csgo.m_cl->m_last_command_ack + i;

			// caught up / invalid.
			if( current_command > outgoing_command || i >= MULTIPLAYER_BACKUP )
				break;

			// get command.
			cmd = g_csgo.m_input->GetUserCmd( current_command );
			if( !cmd )
				break;

			// cmd hasn't been predicted.
			// m_nTickBase is incremented inside RunCommand ( which is called frame by frame, we are running tick by tick here ) and prediction hasn't run yet,
			// so we must fix tickbase by incrementing it ourselves on non-predicted commands.
			if( !cmd->m_predicted )
				++g_cl.m_local->m_nTickBase( );
		}*/

		// EDIT; from what ive seen RunCommand is called when u call Prediction::Update
		// so the above code is not fucking needed.

		int start = g_csgo.m_cl->iLastCommandAck;
		int stop = g_csgo.m_cl->iLastOutgoingCommand + g_csgo.m_cl->nChokedCommands;

		// call CPrediction::Update.
		g_csgo.m_prediction->Update( g_csgo.m_cl->iDeltaTick, valid, start, stop );
	}

	static bool unlocked_fakelag = false;
	if ( !unlocked_fakelag ) {
		auto cl_move_clamp_addr = pattern::find( g_csgo.m_engine_dll, XOR( "B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC" ) );
		if ( cl_move_clamp_addr ) {
			auto cl_move_clamp = cl_move_clamp_addr + 1;
			unsigned long protect = 0;

			VirtualProtect( ( void* )cl_move_clamp, 4, PAGE_EXECUTE_READWRITE, &protect );
			*( std::uint32_t* )cl_move_clamp = 62;
			VirtualProtect( ( void* )cl_move_clamp, 4, protect, &protect );
		}
		unlocked_fakelag = true;
	}
}

void InputPrediction::run( ) {
	static CMoveData data{};

	g_csgo.m_prediction->m_in_prediction = true;

	// CPrediction::StartCommand
	g_cl.m_local->m_pCurrentCommand( ) = g_cl.m_cmd;
	g_cl.m_local->m_PlayerCommand( ) = *g_cl.m_cmd;

	*g_csgo.m_nPredictionRandomSeed = g_cl.m_cmd->m_random_seed;
	g_csgo.m_pPredictionPlayer = g_cl.m_local;

	// backup globals.
	m_curtime = g_csgo.m_globals->m_curtime;
	m_frametime = g_csgo.m_globals->m_frametime;

	// CPrediction::RunCommand

	// set globals appropriately.
	g_csgo.m_globals->m_curtime = game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) );
	g_csgo.m_globals->m_frametime = g_csgo.m_prediction->m_engine_paused ? 0.f : g_csgo.m_globals->m_interval;

	// set target player ( host ).
	g_csgo.m_move_helper->SetHost( g_cl.m_local );
	g_csgo.m_game_movement->StartTrackPredictionErrors( g_cl.m_local );

	// setup input.
	g_csgo.m_prediction->SetupMove( g_cl.m_local, g_cl.m_cmd, g_csgo.m_move_helper, &data );

	// run movement.
	g_csgo.m_game_movement->ProcessMovement( g_cl.m_local, &data );
	g_csgo.m_prediction->FinishMove( g_cl.m_local, g_cl.m_cmd, &data );
	g_csgo.m_game_movement->FinishTrackPredictionErrors( g_cl.m_local );

	g_csgo.m_move_helper->SetHost( nullptr );

	m_predicted_cmd_number = g_cl.m_cmd->m_command_number;
	m_predicted_tick_base = g_cl.m_local->m_nTickBase( );
	m_predicted_view_punch = g_cl.m_local->m_viewPunchAngle( );
	m_predicted_aim_punch = g_cl.m_local->m_aimPunchAngle( );
	m_predicted_aim_punch_vel = g_cl.m_local->m_aimPunchAngleVel( );
	m_predicted_view_offset = g_cl.m_local->m_vecViewOffset( );
	m_predicted_origin = g_cl.m_local->m_vecOrigin( );
}

void InputPrediction::restore( ) {
	g_csgo.m_prediction->m_in_prediction = false;

	*g_csgo.m_nPredictionRandomSeed = -1;
	g_csgo.m_pPredictionPlayer = nullptr;

	g_csgo.m_globals->m_curtime = m_curtime;
	g_csgo.m_globals->m_frametime = m_frametime;
}

void InputPrediction::DetectPredictionErrors( int stage ) {
	if ( stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		return;

	if ( !g_cl.m_local || !g_cl.m_processing )
		return;

	int last_ack = g_csgo.m_cl ? g_csgo.m_cl->iLastCommandAck : 0;
	if ( m_predicted_cmd_number != last_ack )
		return;

	bool has_error = false;

	if ( m_predicted_tick_base != g_cl.m_local->m_nTickBase( ) ) {
		if ( std::abs( g_csgo.m_globals->m_curtime - g_exploits.LastTeleportTime( ) ) > 0.25f )
			has_error = true;
	}

	const auto view_punch_angle_delta = ( g_cl.m_local->m_viewPunchAngle( ) - m_predicted_view_punch );
	if ( std::abs( view_punch_angle_delta.x ) <= 0.03125f && std::abs( view_punch_angle_delta.y ) <= 0.03125f && std::abs( view_punch_angle_delta.z ) <= 0.03125f )
		g_cl.m_local->m_viewPunchAngle( ) = m_predicted_view_punch;

	const auto aim_punch_angle_delta = ( g_cl.m_local->m_aimPunchAngle( ) - m_predicted_aim_punch );
	if ( std::abs( aim_punch_angle_delta.x ) > 0.03125f
		|| std::abs( aim_punch_angle_delta.y ) > 0.03125f
		|| std::abs( aim_punch_angle_delta.z ) > 0.03125f ) {
		m_predicted_aim_punch = g_cl.m_local->m_aimPunchAngle( );
		has_error = true;
	}

	const auto aim_punch_angle_vel_delta = ( g_cl.m_local->m_aimPunchAngleVel( ) - m_predicted_aim_punch_vel );
	if ( std::abs( aim_punch_angle_vel_delta.x ) > 0.03125f
		|| std::abs( aim_punch_angle_vel_delta.y ) > 0.03125f
		|| std::abs( aim_punch_angle_vel_delta.z ) > 0.03125f ) {
		m_predicted_aim_punch_vel = g_cl.m_local->m_aimPunchAngleVel( );
		has_error = true;
	}

	const auto view_offset_delta = std::abs( g_cl.m_local->m_vecViewOffset( ).z - m_predicted_view_offset.z );
	if ( view_offset_delta <= 0.03125f )
		g_cl.m_local->m_vecViewOffset( ).z = m_predicted_view_offset.z;

	const auto origin_delta = g_cl.m_local->m_vecOrigin( ) - m_predicted_origin;
	if ( origin_delta.length_sqr( ) >= 0.0625f ) {
		m_predicted_origin = g_cl.m_local->m_vecOrigin( );
		has_error = true;
	}

	if ( !has_error )
		return;

	if ( g_csgo.m_prediction ) {
		g_csgo.m_prediction->m_previous_startframe = -1;
		g_csgo.m_prediction->m_commands_predicted = 0;
		*( bool* )( ( uintptr_t )g_csgo.m_prediction + 0x24 ) = true;
	}
}