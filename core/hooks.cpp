#include "includes.h"

Hooks                g_hooks{ };;
CustomEntityListener g_custom_entity_listener{ };;

void Pitch_proxy( CRecvProxyData* data, Address ptr, Address out ) {
	// normalize this fucker.
	math::NormalizeAngle( data->m_Value.m_Float );

	// clamp to remove retardedness.
	math::clamp( data->m_Value.m_Float, -90.f, 90.f );

	// call original netvar proxy.
	if ( g_hooks.m_Pitch_original )
		g_hooks.m_Pitch_original( data, ptr, out );
}

void Body_proxy( CRecvProxyData* data, Address ptr, Address out ) {
	/*Stack stack;

	static Address RecvTable_Decode{ pattern::find( g_csgo.m_engine_dll, XOR( "EB 0D FF 77 10" ) ) };

	// call from entity going into pvs.
	if ( stack.next( ).next( ).ReturnAddress( ) != RecvTable_Decode ) {
		// convert to player.
		Player *player = ptr.as< Player * >( );

		// store data about the update.
		g_resolver.OnBodyUpdate( player, data->m_Value.m_Float );
	}*/

	// call original proxy.
	if ( g_hooks.m_Body_original )
		g_hooks.m_Body_original( data, ptr, out );
}

void AbsYaw_proxy( CRecvProxyData* data, Address ptr, Address out ) {
	// convert to ragdoll.
	//Ragdoll* ragdoll = ptr.as< Ragdoll* >( );

	// get ragdoll owner.
	//Player* player = ragdoll->GetPlayer( );

	// get data for this player.
	/*AimPlayer* aim = &g_aimbot.m_players[ player->index( ) - 1 ];

	if( player && aim ) {
	if( !aim->m_records.empty( ) ) {
	LagRecord* match{ nullptr };

	// iterate records.
	for( const auto &it : aim->m_records ) {
	// find record that matches with simulation time.
	if( it->m_sim_time == player->m_flSimulationTime( ) ) {
	match = it.get( );
	break;
	}
	}

	// we have a match.
	// and it is standing
	// TODO; add air?
	if( match /*&& match->m_mode == Resolver::Modes::RESOLVE_STAND*/// ) {
	/*	RagdollRecord record;
	record.m_record   = match;
	record.m_rotation = math::NormalizedAngle( data->m_Value.m_Float );
	record.m_delta    = math::NormalizedAngle( record.m_rotation - match->m_lbyt );

	float death = math::NormalizedAngle( ragdoll->m_flDeathYaw( ) );

	// store.
	//aim->m_ragdoll.push_front( record );

	//g_cl.print( tfm::format( XOR( "rot %f death %f delta %f\n" ), record.m_rotation, death, record.m_delta ).data( ) );
	}
	}*/
	//}

	// call original netvar proxy.
	if ( g_hooks.m_AbsYaw_original )
		g_hooks.m_AbsYaw_original( data, ptr, out );
}

void Force_proxy( CRecvProxyData* data, Address ptr, Address out ) {
	if ( g_menu.main.misc.ragdoll_force.get( ) && g_cl.m_local && ptr ) {
		Ragdoll* ragdoll = ptr.as< Ragdoll* >( );
		if ( ragdoll ) {
			EHANDLE handle = ragdoll->m_hPlayer( );
			if ( handle.IsValid( ) ) {
				Player* player = g_csgo.m_entlist->GetClientEntityFromHandle< Player* >( handle );
				if ( player && player->enemy( g_cl.m_local ) ) {
					vec3_t vel = { data->m_Value.m_Vector[ 0 ], data->m_Value.m_Vector[ 1 ], data->m_Value.m_Vector[ 2 ] };

					vel *= 1000.f;

					if ( vel.z <= 1.f )
						vel.z = 2.f;

					vel.z *= 2.f;

					math::clamp( vel.x, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );
					math::clamp( vel.y, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );
					math::clamp( vel.z, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );

					data->m_Value.m_Vector[ 0 ] = vel.x;
					data->m_Value.m_Vector[ 1 ] = vel.y;
					data->m_Value.m_Vector[ 2 ] = vel.z;
				}
			}
		}
	}

	if ( g_hooks.m_Force_original )
		g_hooks.m_Force_original( data, ptr, out );
}

LRESULT __stdcall WndProcess( HWND wnd, UINT msg, WPARAM wp, LPARAM lp ) {
	switch ( msg ) {
		case WM_LBUTTONDOWN:
			g_input.SetDown( VK_LBUTTON );
			break;

		case WM_LBUTTONUP:
			g_input.SetUp( VK_LBUTTON );
			break;

		case WM_RBUTTONDOWN:
			g_input.SetDown( VK_RBUTTON );
			break;

		case WM_RBUTTONUP:
			g_input.SetUp( VK_RBUTTON );
			break;

		case WM_MBUTTONDOWN:
			g_input.SetDown( VK_MBUTTON );
			break;

		case WM_MBUTTONUP:
			g_input.SetUp( VK_MBUTTON );
			break;

		case WM_XBUTTONDOWN:
			if ( GET_XBUTTON_WPARAM( wp ) == XBUTTON1 )
				g_input.SetDown( VK_XBUTTON1 );

			else if ( GET_XBUTTON_WPARAM( wp ) == XBUTTON2 )
				g_input.SetDown( VK_XBUTTON2 );

			break;

		case WM_XBUTTONUP:
			if ( GET_XBUTTON_WPARAM( wp ) == XBUTTON1 )
				g_input.SetUp( VK_XBUTTON1 );

			else if ( GET_XBUTTON_WPARAM( wp ) == XBUTTON2 )
				g_input.SetUp( VK_XBUTTON2 );

			break;

		case WM_KEYDOWN:
			if ( ( size_t )wp < g_input.m_keys.size( ) )
				g_input.SetDown( wp );

			break;

		case WM_KEYUP:
			if ( ( size_t )wp < g_input.m_keys.size( ) )
				g_input.SetUp( wp );

			break;

		case WM_SYSKEYDOWN:
			if ( wp == VK_MENU )
				g_input.SetDown( VK_MENU );

			break;

		case WM_SYSKEYUP:
			if ( wp == VK_MENU )
				g_input.SetUp( VK_MENU );

			break;

		case WM_CHAR:
			switch ( wp ) {
				case VK_BACK:
					if ( !g_input.m_buffer.empty( ) )
						g_input.m_buffer.pop_back( );
					break;

				case VK_ESCAPE:
				case VK_TAB:
				case VK_RETURN:
					break;

				default:
					if ( std::isdigit( static_cast< unsigned char >( wp ) ) )
						g_input.m_buffer += static_cast< char >( wp );

					break;
			}

			break;

		default:
			break;
	}

	return CallWindowProc( g_hooks.m_old_wndproc, wnd, msg, wp, lp );
}

void Hooks::init( ) {
	// hook wndproc.
	m_old_wndproc = ( WNDPROC )SetWindowLongPtr( g_csgo.m_window, GWL_WNDPROC, reinterpret_cast< LONG_PTR >( WndProcess ) );

	// setup normal VMT hooks.
	m_kv_system.init( g_csgo.m_kv_system );
	m_kv_system.add( 2, util::force_cast( &Hooks::AllocKeyValues ) );

	m_panel.init( g_csgo.m_panel );
	m_panel.add( IPanel::PAINTTRAVERSE, util::force_cast( &Hooks::PaintTraverse ) );

	m_client.init( g_csgo.m_client );
	m_client.add( CHLClient::LEVELINITPREENTITY, util::force_cast( &Hooks::LevelInitPreEntity ) );
	m_client.add( CHLClient::LEVELINITPOSTENTITY, util::force_cast( &Hooks::LevelInitPostEntity ) );
	m_client.add( CHLClient::LEVELSHUTDOWN, util::force_cast( &Hooks::LevelShutdown ) );
	//m_client.add( CHLClient::INKEYEVENT, util::force_cast( &Hooks::IN_KeyEvent ) );
	m_client.add( CHLClient::FRAMESTAGENOTIFY, util::force_cast( &Hooks::FrameStageNotify ) );
	m_client.add( CHLClient::CREATEMOVE, util::force_cast( &Hooks::CHLCreateMove ) );

	m_engine.init( g_csgo.m_engine );
	m_engine.add( IVEngineClient::ISCONNECTED, util::force_cast( &Hooks::IsConnected ) );
	m_engine.add( IVEngineClient::ISHLTV, util::force_cast( &Hooks::IsHLTV ) );
	m_engine.add( IVEngineClient::ISPAUSED, util::force_cast( &Hooks::IsPaused ) );

	//m_engine_sound.init( g_csgo.m_sound );
	//m_engine_sound.add( IEngineSound::EMITSOUND, util::force_cast( &Hooks::EmitSound ) );

	m_prediction.init( g_csgo.m_prediction );
	m_prediction.add( CPrediction::INPREDICTION, util::force_cast( &Hooks::InPrediction ) );
	m_prediction.add( CPrediction::RUNCOMMAND, util::force_cast( &Hooks::RunCommand ) );

	m_client_mode.init( g_csgo.m_client_mode );
	m_client_mode.add( IClientMode::SHOULDDRAWPARTICLES, util::force_cast( &Hooks::ShouldDrawParticles ) );
	m_client_mode.add( IClientMode::SHOULDDRAWFOG, util::force_cast( &Hooks::ShouldDrawFog ) );
	m_client_mode.add( IClientMode::OVERRIDEVIEW, util::force_cast( &Hooks::OverrideView ) );
	m_client_mode.add( IClientMode::CREATEMOVE, util::force_cast( &Hooks::CreateMove ) );
	m_client_mode.add( IClientMode::DOPOSTSPACESCREENEFFECTS, util::force_cast( &Hooks::DoPostScreenSpaceEffects ) );

	m_surface.init( g_csgo.m_surface );
	//m_surface.add( ISurface::GETSCREENSIZE, util::force_cast( &Hooks::GetScreenSize ) );
	m_surface.add( ISurface::LOCKCURSOR, util::force_cast( &Hooks::LockCursor ) );
	m_surface.add( ISurface::PLAYSOUND, util::force_cast( &Hooks::PlaySound ) );
	m_surface.add( ISurface::ONSCREENSIZECHANGED, util::force_cast( &Hooks::OnScreenSizeChanged ) );

	m_model_render.init( g_csgo.m_model_render );
	m_model_render.add( IVModelRender::DRAWMODELEXECUTE, util::force_cast( &Hooks::DrawModelExecute ) );

	m_render_view.init( g_csgo.m_render_view );
	m_render_view.add( IVRenderView::SCENEEND, util::force_cast( &Hooks::SceneEnd ) );

	m_shadow_mgr.init( g_csgo.m_shadow_mgr );
	m_shadow_mgr.add( IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES, util::force_cast( &Hooks::ComputeShadowDepthTextures ) );

	m_view_render.init( g_csgo.m_view_render );
	m_view_render.add( CViewRender::ONRENDERSTART, util::force_cast( &Hooks::OnRenderStart ) );
	m_view_render.add( CViewRender::RENDERVIEW, util::force_cast( &Hooks::RenderView ) );
	m_view_render.add( CViewRender::RENDER2DEFFECTSPOSTHUD, util::force_cast( &Hooks::Render2DEffectsPostHUD ) );
	m_view_render.add( CViewRender::RENDERSMOKEOVERLAY, util::force_cast( &Hooks::RenderSmokeOverlay ) );

	m_match_framework.init( g_csgo.m_match_framework );
	m_match_framework.add( CMatchFramework::GETMATCHSESSION, util::force_cast( &Hooks::GetMatchSession ) );

	m_material_system.init( g_csgo.m_material_system );
	m_material_system.add( IMaterialSystem::OVERRIDECONFIG, util::force_cast( &Hooks::OverrideConfig ) );

	//m_player_vtable.init( g_csgo.PlayerVTable );
	//m_player_vtable.add( Player::BUILDTRANSFORMATIONS, util::force_cast( &Hooks::BuildTransformations ) );
	//m_player_vtable.add( Player::UPDATECLIENTSIDEANIMATION, util::force_cast( &Hooks::UpdateClientSideAnimation ) );
	//m_player_vtable.add( Player::DOEXTRABONEPROCESSING, util::force_cast( &Hooks::DoExtraBoneProcessing ) );
	//m_player_vtable.add( Player::GETACTIVEWEAPON, util::force_cast( &Hooks::GetActiveWeapon ) );

	//m_fire_bullets.init( g_csgo.TEFireBullets );
	//m_fire_bullets.add( 7, util::force_cast( &Hooks::PostDataUpdate ) );

	//m_client_state.init( g_csgo.m_hookable_cl );
	//m_client_state.add( CClientState::TEMPENTITIES, util::force_cast( &Hooks::TempEntities ) );

	// register our custom entity listener.
	// todo - dex; should we push our listeners first? should be fine like this.
	g_custom_entity_listener.init( );

	// cvar hooks.
	//m_debug_spread.init( g_csgo.net_showfragments );
	//m_debug_spread.add( ConVar::GETINT, util::force_cast( &Hooks::DebugSpreadGetInt ) );

	//m_net_show_fragments.init( g_csgo.net_showfragments );
	//m_net_show_fragments.add( ConVar::GETBOOL, util::force_cast( &Hooks::NetShowFragmentsGetBool ) );

	if ( g_csgo.CL_Move ) {
		m_CL_Move_original = ( CSGO::CL_Move_t )DetourFunction( ( PBYTE )g_csgo.CL_Move, ( PBYTE )Hooks::CL_Move );
	}

	if ( g_csgo.WriteUsercmdDeltaToBuffer ) {
		m_WriteUsercmdDeltaToBuffer_original = ( WriteUsercmdDeltaToBuffer_t )DetourFunction( ( PBYTE )g_csgo.WriteUsercmdDeltaToBuffer.as< void* >( ), ( PBYTE )Hooks::WriteUsercmdDeltaToBuffer );
	}

	if ( g_csgo.ClampBonesInBBox ) {
		m_ClampBonesInBBox_original = ( ClampBonesInBBox_t )DetourFunction( ( PBYTE )g_csgo.ClampBonesInBBox.as< void* >( ), ( PBYTE )Hooks::ClampBonesInBBox );
	}

	if ( g_csgo.ShouldSkipAnimationFrame ) {
		m_ShouldSkipAnimationFrame_original = ( ShouldSkipAnimationFrame_t )DetourFunction( ( PBYTE )g_csgo.ShouldSkipAnimationFrame.as< void* >( ), ( PBYTE )Hooks::ShouldSkipAnimationFrame );
	}

	// set netvar proxies.
	g_netvars.SetProxy( HASH( "DT_CSPlayer" ), HASH( "m_angEyeAngles[0]" ), Pitch_proxy, m_Pitch_original );
	g_netvars.SetProxy( HASH( "DT_CSPlayer" ), HASH( "m_flLowerBodyYawTarget" ), Body_proxy, m_Body_original );
	g_netvars.SetProxy( HASH( "DT_CSRagdoll" ), HASH( "m_vecForce" ), Force_proxy, m_Force_original );
	g_netvars.SetProxy( HASH( "DT_CSRagdoll" ), HASH( "m_flAbsYaw" ), AbsYaw_proxy, m_AbsYaw_original );
}

void __cdecl Hooks::CL_Move( float accumulated_extra_samples, bool final_tick ) {
	if ( !g_cl.m_local || !g_cl.m_local->alive( ) ) {
		if ( g_hooks.m_CL_Move_original )
			g_hooks.m_CL_Move_original( accumulated_extra_samples, final_tick );
		return;
	}

	g_exploits.Run( );

	if ( g_exploits.ShouldCharge( ) ) {
		g_cl.m_tickbase_shift++;
		g_cl.m_shifted_last_tick++;
		return;
	}

	if ( g_hooks.m_CL_Move_original )
		g_hooks.m_CL_Move_original( accumulated_extra_samples, final_tick );

	if ( g_hooks.m_CL_Move_original )
		g_exploits.HandleTeleport( g_hooks.m_CL_Move_original );
}

static void WriteUserCmdHelper( void* buf, CUserCmd* cmd_in, CUserCmd* cmd_out ) {
	if ( !g_csgo.WriteUserCmd )
		return;

	__asm {
		mov ecx, buf
		mov edx, cmd_in
		push cmd_out
		call g_csgo.WriteUserCmd
		add esp, 4
	}
}

bool __fastcall Hooks::WriteUsercmdDeltaToBuffer( void* thisptr, void* edx, int slot, void* buf, int from, int to, bool isnewcommand ) {
	if ( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_local || !g_cl.m_local->alive( ) || !g_cl.m_lc_exploit || !g_cl.m_tickbase_shift ) {
		if ( g_hooks.m_WriteUsercmdDeltaToBuffer_original )
			return g_hooks.m_WriteUsercmdDeltaToBuffer_original( thisptr, slot, buf, from, to, isnewcommand );
		return false;
	}

	if ( from != -1 )
		return true;

	uintptr_t* stack_pointer;
	__asm mov stack_pointer, ebp;

	CCLCMsg_Move_t* moveMsg = reinterpret_cast< CCLCMsg_Move_t* >( *stack_pointer - 0x58 );

	auto new_commands = moveMsg->new_commands + moveMsg->backup_commands;
	auto next_cmd_nr = g_csgo.m_cl->iLastOutgoingCommand + g_csgo.m_cl->nChokedCommands + 1;

	moveMsg->new_commands = std::clamp( moveMsg->new_commands + g_cl.m_lc_exploit, 1, 15 );

	for ( to = next_cmd_nr - new_commands + 1; to <= next_cmd_nr; to++ ) {
		if ( !g_hooks.m_WriteUsercmdDeltaToBuffer_original( thisptr, slot, buf, from, to, true ) )
			return false;

		from = to;
	}

	CUserCmd* user_cmd = &g_csgo.m_input->pCommands[ from % MULTIPLAYER_BACKUP ];
	if ( !user_cmd )
		return true;

	CUserCmd from_cmd = *user_cmd;
	CUserCmd to_cmd = from_cmd;

	to_cmd.m_command_number++;
	to_cmd.m_tick = INT_MAX;

	for ( int i = 0; i < g_cl.m_lc_exploit; i++ ) {
		WriteUserCmdHelper( buf, &to_cmd, &from_cmd );

		from_cmd = to_cmd;
		to_cmd.m_command_number++;
	}

	return true;
}

void __fastcall Hooks::ClampBonesInBBox( void* ecx, void* edx, matrix3x4_t* bones, int boneMask ) {
	
}

bool __fastcall Hooks::ShouldSkipAnimationFrame( void* ecx, void* edx ) {
	return false;
}