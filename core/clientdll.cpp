#include "includes.h"

void* Hooks::AllocKeyValues( size_t size ) {
	static Address AllocKeyValuesEngine{ pattern::find( g_csgo.m_engine_dll, XOR( "55 8B EC 56 57 8B F9 8B F2 83 FF 11 0F 87 ? ? ? ? 85 F6 0F 84 ? ? ? ?" ) ).add( 0x4A ) };
	static Address AllocKeyValuesClient{ pattern::find( g_csgo.m_client_dll, XOR( "55 8B EC 56 57 8B F9 8B F2 83 FF 11 0F 87 ? ? ? ? 85 F6 0F 84 ? ? ? ?" ) ).add( 0x3E ) };

	const uintptr_t ret = ( uintptr_t )_ReturnAddress( );

	if ( ret == AllocKeyValuesEngine.as< uintptr_t >( ) || ret == AllocKeyValuesClient.as< uintptr_t >( ) )
		return nullptr;

	return g_hooks.m_kv_system.GetOldMethod< AllocKeyValues_t >( 2 )( this, size );
}

void WINAPI Hooks::CHLCreateMoveProxy( int sequence_number, float input_sample_time, bool active, bool& send_packet ) {
	g_hooks.m_client.GetOldMethod< CHLCreateMove_t >( CHLClient::CREATEMOVE )( g_csgo.m_client, sequence_number, input_sample_time, active );

	g_cl.m_packet = send_packet = true;

	CUserCmd* cmd = &g_csgo.m_input->pCommands[ sequence_number % MULTIPLAYER_BACKUP ];
	if ( !cmd || !cmd->m_command_number )
		return;

	g_cl.OnTick( cmd );

	send_packet = g_cl.m_packet;

	CVerifiedUserCmd* verified_cmd = &g_csgo.m_input->pVerifiedCommands[ sequence_number % MULTIPLAYER_BACKUP ];
	if ( verified_cmd ) {
		verified_cmd->m_cmd = *cmd;
		verified_cmd->m_crc = game::CalcUserCmdChecksum( cmd );
	}
}

__declspec( naked ) void Hooks::CHLCreateMove( int sequence_number, float input_sample_time, bool active ) {
	__asm
	{
		push ebp // save register
		mov ebp, esp; // store stack to register
		push ebx; // save register
		push esp; // bSendPacket from caller stack
		push[ ebp + 10h ]; // bIsActive
		push[ ebp + 0Ch ]; // flInputSampleFrametime
		push[ ebp + 8 ]; // nSequenceNumber
		call CHLCreateMoveProxy
			pop ebx // restore register
			pop ebp // restore register
			retn 0Ch
	}
}

void Hooks::LevelInitPreEntity( const char* map ) {
	float rate{ 1.f / g_csgo.m_globals->m_interval };

	// set rates when joining a server.
	g_csgo.cl_updaterate->SetValue( rate );
	g_csgo.cl_cmdrate->SetValue( rate );

	g_cl.m_packet = true;

	g_aimbot.reset( );
	g_visuals.m_hit_start = g_visuals.m_hit_end = g_visuals.m_hit_duration = 0.f;

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPreEntity_t >( CHLClient::LEVELINITPREENTITY )( this, map );
}

void Hooks::LevelInitPostEntity( ) {
	g_cl.OnMapload( );

	g_cl.m_packet = true;

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPostEntity_t >( CHLClient::LEVELINITPOSTENTITY )( this );
}

void Hooks::LevelShutdown( ) {
	g_aimbot.reset( );

	g_cl.m_local = nullptr;
	g_cl.m_weapon = nullptr;
	g_cl.m_processing = false;
	g_cl.m_weapon_info = nullptr;
	g_cl.m_round_end = false;
	g_cl.m_packet = true;

	g_cl.m_sequences.clear( );

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelShutdown_t >( CHLClient::LEVELSHUTDOWN )( this );
}

/*int Hooks::IN_KeyEvent( int evt, int key, const char* bind ) {
	// see if this key event was fired for the drop bind.
	/*if( bind && FNV1a::get( bind ) == HASH( "drop" ) ) {
		// down.
		if( evt ) {
			g_cl.m_drop = true;
			g_cl.m_drop_query = 2;
			g_cl.print( "drop\n" );
		}

		// up.
		else
			g_cl.m_drop = false;

		// ignore the event.
		return 0;
	}

	return g_hooks.m_client.GetOldMethod< IN_KeyEvent_t >( CHLClient::INKEYEVENT )( this, evt, key, bind );
}*/

void Hooks::FrameStageNotify( Stage_t stage ) {
	// save stage.
	if ( stage != FRAME_START )
		g_cl.m_stage = stage;

	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );
	g_cl.m_processing = g_cl.m_local && g_cl.m_local->alive( );

	if ( stage == FRAME_RENDER_START ) {
		if ( g_exploits.ShouldCharge( ) )
			g_csgo.m_globals->m_interp_amt = 0.f;

		if ( g_cl.m_local && g_cl.m_local->alive( ) && g_menu.main.visuals.noflash.get( ) )
			g_cl.m_local->m_flFlashBangTime( ) = 0.f;

		g_cl.SetAngles( );
		g_cl.UpdateAnimations( );
		g_visuals.DrawBeams( );
	}

	// call og.
	g_hooks.m_client.GetOldMethod< FrameStageNotify_t >( CHLClient::FRAMESTAGENOTIFY )( this, stage );

	if ( stage == FRAME_RENDER_START ) {
		// ...
	}

	else if ( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
		g_inputpred.DetectPredictionErrors( stage );
		g_skins.think( );
	}

	else if ( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {
		g_visuals.NoSmoke( );
	}

	else if ( stage == FRAME_NET_UPDATE_END ) {
		const auto playing_demo = g_csgo.m_engine->IsPlayingDemo( );
		if ( !playing_demo && !g_csgo.m_engine->IsHLTV( ) ) {
			for ( auto ev = g_csgo.m_cl->pEvents; ev; ev = ev->pNext ) {
				auto client_class = ev->pClientClass;
				if ( client_class ) {
					if ( FNV1a::get( client_class->m_pNetworkName ) == HASH( "CTEFireBullets" ) )
						ev->flFireDelay = 0.f;
				}
			}

			g_csgo.m_engine->FireEvents( );
		}

		g_netdata.apply( );
		g_shots.OnNetUpdate( );

		for ( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
			if ( !player || player->m_bIsLocalPlayer( ) )
				continue;

			AimPlayer* data = &g_aimbot.m_players[ i - 1 ];
			data->OnNetUpdate( player );
		}
	}
}