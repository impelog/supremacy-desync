#include "includes.h"

bool Hooks::ShouldDrawParticles( ) {
	return g_hooks.m_client_mode.GetOldMethod< ShouldDrawParticles_t >( IClientMode::SHOULDDRAWPARTICLES )( this );
}

bool Hooks::ShouldDrawFog( ) {
	// remove fog.
	if ( g_menu.main.visuals.nofog.get( ) )
		return false;

	return g_hooks.m_client_mode.GetOldMethod< ShouldDrawFog_t >( IClientMode::SHOULDDRAWFOG )( this );
}

void Hooks::OverrideView( CViewSetup* view ) {
	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );

	g_visuals.ThirdpersonThink( );

	if ( g_hvh.m_fake_duck && g_cl.m_local && g_cl.m_local->alive( ) )
		view->m_origin.z = g_cl.m_local->GetAbsOrigin( ).z + 64.f;

	g_hooks.m_client_mode.GetOldMethod< OverrideView_t >( IClientMode::OVERRIDEVIEW )( this, view );

	if ( g_menu.main.visuals.novisrecoil.get( ) && g_cl.m_local && g_cl.m_local->alive( ) ) {
		float scale = g_csgo.weapon_recoil_scale ? g_csgo.weapon_recoil_scale->GetFloat( ) : 2.f;
		float track = g_csgo.view_recoil_tracking ? g_csgo.view_recoil_tracking->GetFloat( ) : 0.45f;
		view->m_angles -= g_cl.m_local->m_viewPunchAngle( ) + ( g_cl.m_local->m_aimPunchAngle( ) * scale ) * track;
	}

	if ( g_menu.main.visuals.fov.get( ) ) {
		if ( g_cl.m_local && g_cl.m_local->m_bIsScoped( ) ) {
			if ( g_menu.main.visuals.fov_scoped.get( ) ) {
				if ( g_cl.m_local->GetActiveWeapon( ) && g_cl.m_local->GetActiveWeapon( )->m_zoomLevel( ) != 2 ) {
					view->m_fov = g_menu.main.visuals.fov_amt.get( );
				}
				else {
					view->m_fov += 45.f;
				}
			}
		}
		else {
			view->m_fov = g_menu.main.visuals.fov_amt.get( );
		}
	}

	if ( g_menu.main.visuals.viewmodel_fov.get( ) )
		view->m_viewmodel_fov = g_menu.main.visuals.viewmodel_fov_amt.get( );

	if ( g_menu.main.visuals.noscope.get( ) ) {
		if ( g_cl.m_local && g_cl.m_local->m_bIsScoped( ) )
			view->m_edge_blur = 0;
	}
}

bool Hooks::CreateMove( float time, CUserCmd* cmd ) {
	Stack   stack;
	bool    ret;

	// let original run first.
	ret = g_hooks.m_client_mode.GetOldMethod< CreateMove_t >( IClientMode::CREATEMOVE )( this, time, cmd );

	// called from CInput::ExtraMouseSample -> return original.
	if ( !cmd->m_command_number )
		return ret;

	// if we arrived here, called from -> CInput::CreateMove
	// call EngineClient::SetViewAngles according to what the original returns.
	if ( ret )
		g_csgo.m_engine->SetViewAngles( cmd->m_view_angles );

	// random_seed isn't generated in ClientMode::CreateMove yet, we must set generate it ourselves.
	cmd->m_random_seed = g_csgo.MD5_PseudoRandom( cmd->m_command_number ) & 0x7fffffff;

	return false;
}

bool Hooks::DoPostScreenSpaceEffects( CViewSetup* setup ) {
	g_visuals.RenderGlow( );

	return g_hooks.m_client_mode.GetOldMethod< DoPostScreenSpaceEffects_t >( IClientMode::DOPOSTSPACESCREENEFFECTS )( this, setup );
}