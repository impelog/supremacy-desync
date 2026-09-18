#include "includes.h"

// execution callbacks..
void callbacks::SkinUpdate( ) {
	g_skins.m_update = true;
}

void callbacks::ForceFullUpdate( ) {
	//static DWORD tick{};
	//
	//if( tick != g_winapi.GetTickCount( ) ) {
	//	g_csgo.cl_fullupdate->m_callback( );
	//	tick = g_winapi.GetTickCount( );
	//

	g_csgo.m_cl->iDeltaTick = -1;
}

void callbacks::ToggleThirdPerson( ) {
	g_visuals.m_thirdperson = !g_visuals.m_thirdperson;
}

void callbacks::ToggleFakeLatency( ) {
	g_aimbot.m_fake_latency = !g_aimbot.m_fake_latency;
}

void callbacks::ToggleDmgOverride( ) {
	g_aimbot.m_dmg_override = !g_aimbot.m_dmg_override;
}

void callbacks::ToggleKillfeed( )
{
	if ( !g_csgo.m_engine->IsInGame( ) )
		return;

	auto element = g_csgo.m_hud->FindElement( FNV1a::get( "CCSGO_HudDeathNotice" ) );
	if ( !element )
		return;

	auto death_notice = reinterpret_cast< uintptr_t >( element );
	if ( !death_notice )
		return;

	auto notice_ptr = death_notice - 0x14;
	if ( !notice_ptr )
		return;

	g_csgo.ClearKillfeed.as<CSGO::ClearNotices_t>( )( notice_ptr );
}

void callbacks::SaveHotkeys( ) {
	g_config.SaveHotkeys( );
}

void callbacks::ConfigLoad1( ) {
	g_config.load( &g_menu.main, XOR( "1.sup" ) );
	g_menu.main.config.config.select( 1 - 1 );

	g_notify.add( tfm::format( XOR( "loaded config 1\n" ) ) );
}

void callbacks::ConfigLoad2( ) {
	g_config.load( &g_menu.main, XOR( "2.sup" ) );
	g_menu.main.config.config.select( 2 - 1 );
	g_notify.add( tfm::format( XOR( "loaded config 2\n" ) ) );
}

void callbacks::ConfigLoad3( ) {
	g_config.load( &g_menu.main, XOR( "3.sup" ) );
	g_menu.main.config.config.select( 3 - 1 );
	g_notify.add( tfm::format( XOR( "loaded config 3\n" ) ) );
}

void callbacks::ConfigLoad4( ) {
	g_config.load( &g_menu.main, XOR( "4.sup" ) );
	g_menu.main.config.config.select( 4 - 1 );
	g_notify.add( tfm::format( XOR( "loaded config 4\n" ) ) );
}

void callbacks::ConfigLoad5( ) {
	g_config.load( &g_menu.main, XOR( "5.sup" ) );
	g_menu.main.config.config.select( 5 - 1 );
	g_notify.add( tfm::format( XOR( "loaded config 5\n" ) ) );
}

void callbacks::ConfigLoad6( ) {
	g_config.load( &g_menu.main, XOR( "6.sup" ) );
	g_menu.main.config.config.select( 6 - 1 );
	g_notify.add( tfm::format( XOR( "loaded config 6\n" ) ) );
}

void callbacks::ConfigLoad( ) {
	std::string config = g_menu.main.config.config.GetActiveItem( );
	std::string file   = tfm::format( XOR( "%s.sup" ), config.data( ) );

	g_config.load( &g_menu.main, file );
	g_notify.add( tfm::format( XOR( "loaded config %s\n" ), config.data( ) ) );
}

void callbacks::ConfigSave( ) {
	std::string config = g_menu.main.config.config.GetActiveItem( );
	std::string file   = tfm::format( XOR( "%s.sup" ), config.data( ) );

	g_config.save( &g_menu.main, file );
	g_notify.add( tfm::format( XOR( "saved config %s\n" ), config.data( ) ) );
}

bool callbacks::IsBaimHealth( ) {
	return g_menu.main.aimbot.baim2.get( 1 );
}

bool callbacks::IsFovOn( ) {
	return g_menu.main.aimbot.fov.get( );
}

bool callbacks::IsHitchanceOn( ) {
	return g_menu.main.aimbot.hitchance.get( );
}

bool callbacks::IsPenetrationOn( ) {
	return g_menu.main.aimbot.penetrate.get( );
}

bool callbacks::IsMultipointOn( ) {
	return !g_menu.main.aimbot.multipoint.GetActiveIndices( ).empty( );
}

bool callbacks::IsMultipointBodyOn( ) {
	return g_menu.main.aimbot.multipoint.get( 2 );
}

bool callbacks::IsDesyncEnabled( ) {
	return g_menu.main.antiaim.desync.get( );
}

bool callbacks::IsConfigMM( ) {
	return g_menu.main.config.mode.get( ) == 0;
}

bool callbacks::IsConfigNS( ) {
	return g_menu.main.config.mode.get( ) == 1;
}

bool callbacks::IsConfig1( ) {
	return g_menu.main.config.config.get( ) == 0;
}

bool callbacks::IsConfig2( ) {
	return g_menu.main.config.config.get( ) == 1;
}

bool callbacks::IsConfig3( ) {
	return g_menu.main.config.config.get( ) == 2;
}

bool callbacks::IsConfig4( ) {
	return g_menu.main.config.config.get( ) == 3;
}

bool callbacks::IsConfig5( ) {
	return g_menu.main.config.config.get( ) == 4;
}

bool callbacks::IsConfig6( ) {
	return g_menu.main.config.config.get( ) == 5;
}

// weaponcfgs callbacks.
bool callbacks::DEAGLE( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_DEAGLE;
}

bool callbacks::ELITE( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_ELITE;
}

bool callbacks::FIVESEVEN( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_FIVESEVEN;
}

bool callbacks::GLOCK( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_GLOCK;
}

bool callbacks::AK47( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_AK47;
}

bool callbacks::AUG( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_AUG;
}

bool callbacks::AWP( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_AWP;
}

bool callbacks::FAMAS( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_FAMAS;
}

bool callbacks::G3SG1( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_G3SG1;
}

bool callbacks::GALIL( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_GALILAR;
}

bool callbacks::M249( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_M249;
}

bool callbacks::M4A4( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_M4A1;
}

bool callbacks::MAC10( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_MAC10;
}

bool callbacks::P90( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_P90;
}

bool callbacks::UMP45( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_UMP45;
}

bool callbacks::XM1014( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_XM1014;
}

bool callbacks::BIZON( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_BIZON;
}

bool callbacks::MAG7( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_MAG7;
}

bool callbacks::NEGEV( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_NEGEV;
}

bool callbacks::SAWEDOFF( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_SAWEDOFF;
}

bool callbacks::TEC9( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_TEC9;
}

bool callbacks::P2000( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_HKP2000;
}

bool callbacks::MP7( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_MP7;
}

bool callbacks::MP9( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_MP9;
}

bool callbacks::NOVA( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_NOVA;
}

bool callbacks::P250( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_P250;
}

bool callbacks::SCAR20( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_SCAR20;
}

bool callbacks::SG553( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_SG556;
}

bool callbacks::SSG08( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_SSG08;
}

bool callbacks::M4A1S( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_M4A1_SILENCER;
}

bool callbacks::USPS( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_USP_SILENCER;
}

bool callbacks::CZ75A( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_CZ75A;
}

bool callbacks::REVOLVER( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_REVOLVER;
}

bool callbacks::KNIFE_BAYONET( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_BAYONET;
}

bool callbacks::KNIFE_FLIP( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_FLIP;
}

bool callbacks::KNIFE_GUT( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_GUT;
}

bool callbacks::KNIFE_KARAMBIT( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_KARAMBIT;
}

bool callbacks::KNIFE_M9_BAYONET( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_M9_BAYONET;
}

bool callbacks::KNIFE_HUNTSMAN( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_TACTICAL;
}

bool callbacks::KNIFE_FALCHION( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_FALCHION;
}

bool callbacks::KNIFE_BOWIE( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_SURVIVAL_BOWIE;
}

bool callbacks::KNIFE_BUTTERFLY( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_BUTTERFLY;
}

bool callbacks::KNIFE_SHADOW_DAGGERS( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_processing )
		return false;

	return g_cl.m_weapon_id == WEAPON_KNIFE_PUSH;
}

bool callbacks::AUTO_STOP( ) {
	return !g_menu.main.movement.autostop_always_on.get();
}