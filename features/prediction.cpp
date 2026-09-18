#include "includes.h"

bool Hooks::InPrediction( ) {
	Stack stack;
	ang_t* angles;

	if ( g_exploits.ShouldCharge( ) )
		g_csgo.m_globals->m_interp_amt = 0.f;

	static Address CalcPlayerView_ret1{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C" ) ) };
	static Address CalcPlayerView_ret2{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06" ) ) };

	if ( g_menu.main.visuals.novisrecoil.get( ) ) {
		if ( stack.ReturnAddress( ) == CalcPlayerView_ret1 )
			return true;

		if ( stack.ReturnAddress( ) == CalcPlayerView_ret2 ) {
			angles = stack.next( ).arg( 0xC ).to< ang_t* >( );

			if ( angles ) {
				Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );
				if ( player && player->alive( ) ) {
					float scale = g_csgo.weapon_recoil_scale ? g_csgo.weapon_recoil_scale->GetFloat( ) : 2.f;
					float track = g_csgo.view_recoil_tracking ? g_csgo.view_recoil_tracking->GetFloat( ) : 0.45f;
					*angles -= player->m_viewPunchAngle( ) + ( player->m_aimPunchAngle( ) * scale ) * track;
				}
			}

			return true;
		}
	}

	return g_hooks.m_prediction.GetOldMethod< InPrediction_t >( CPrediction::INPREDICTION )( this );
}

void Hooks::RunCommand( Entity* ent, CUserCmd* cmd, IMoveHelper* movehelper ) {
	Player* player = ent->as< Player* >( );
	if ( !player || !player->alive( ) || player != g_cl.m_local ) {
		g_hooks.m_prediction.GetOldMethod< RunCommand_t >( CPrediction::RUNCOMMAND )( this, ent, cmd, movehelper );
		return;
	}

	int& tickbase = player->m_nTickBase( );

	if ( cmd->m_tick >= std::numeric_limits< int >::max( ) || cmd->m_tick == INT_MAX ) {
		tickbase++;
		return;
	}

	if ( g_cl.m_lc_exploit_change == cmd->m_command_number )
		tickbase -= g_cl.m_lc_exploit_diff;

	if ( cmd->m_command_number == g_exploits.charged_command + 1 )
		tickbase += g_cl.m_shifted_last_tick;

	const int backup_tickbase = tickbase;
	const float backup_velocity_modifier = player->m_flVelocityModifier( );

	g_hooks.m_prediction.GetOldMethod< RunCommand_t >( CPrediction::RUNCOMMAND )( this, ent, cmd, movehelper );

	player->m_flVelocityModifier( ) = backup_velocity_modifier;

	for ( auto i = g_cl.m_shifted_commands.begin( ); i != g_cl.m_shifted_commands.end( ); ) {
		auto command = *i;

		if ( cmd->m_command_number - command > 32 ) {
			i = g_cl.m_shifted_commands.erase( i );
			continue;
		}

		if ( command == cmd->m_command_number )
			tickbase = backup_tickbase;

		++i;
	}

	g_netdata.store( );
}