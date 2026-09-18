#include "includes.h"

CSGO	   g_csgo{};;
WinAPI	   g_winapi{};;
Netvars	   g_netvars{};;
EntOffsets g_entoffsets{};;
Menu       g_menu{};;
Notify     g_notify{};;

bool CSGO::init( ) {
	m_done = false;

	if ( m_done )
		return false;

	// wait for the game to init.
	// "serverbrowser.dll" is the last module to be loaded.
	// if it gets loaded we can be ensured that the entire game done loading.

#ifndef KOLO
	while ( !m_serverbrowser_dll ) {
		m_serverbrowser_dll = PE::GetModule( HASH( "serverbrowser.dll" ) );
		if ( !m_serverbrowser_dll )
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
#endif

	// get the csgo window handle.
	g_csgo.m_window = FindWindowA( XOR( "Valve001" ), NULL );

	// grab some modules.
	m_kernel32_dll = PE::GetModule( HASH( "kernel32.dll" ) );
	m_user32_dll = PE::GetModule( HASH( "user32.dll" ) );
	m_shell32_dll = PE::GetModule( HASH( "shell32.dll" ) );
	m_shlwapi_dll = PE::GetModule( HASH( "shlwapi.dll" ) );
	m_client_dll = PE::GetModule( HASH( "client.dll" ) );
	m_engine_dll = PE::GetModule( HASH( "engine.dll" ) );
	m_vstdlib_dll = PE::GetModule( HASH( "vstdlib.dll" ) );
	m_tier0_dll = PE::GetModule( HASH( "tier0.dll" ) );

	// import winapi functions.
	g_winapi.WideCharToMultiByte = PE::GetExport( m_kernel32_dll, HASH( "WideCharToMultiByte" ) ).as< WinAPI::WideCharToMultiByte_t >( );
	g_winapi.MultiByteToWideChar = PE::GetExport( m_kernel32_dll, HASH( "MultiByteToWideChar" ) ).as< WinAPI::MultiByteToWideChar_t >( );
	g_winapi.GetTickCount = PE::GetExport( m_kernel32_dll, HASH( "GetTickCount" ) ).as< WinAPI::GetTickCount_t >( );
	g_winapi.VirtualProtect = PE::GetExport( m_kernel32_dll, HASH( "VirtualProtect" ) ).as< WinAPI::VirtualProtect_t >( );
	g_winapi.VirtualQuery = PE::GetExport( m_kernel32_dll, HASH( "VirtualQuery" ) ).as< WinAPI::VirtualQuery_t >( );
	g_winapi.CreateDirectoryA = PE::GetExport( m_kernel32_dll, HASH( "CreateDirectoryA" ) ).as< WinAPI::CreateDirectoryA_t >( );
	g_winapi.SetWindowLongA = PE::GetExport( m_user32_dll, HASH( "SetWindowLongA" ) ).as< WinAPI::SetWindowLongA_t >( );
	g_winapi.CallWindowProcA = PE::GetExport( m_user32_dll, HASH( "CallWindowProcA" ) ).as< WinAPI::CallWindowProcA_t >( );
	g_winapi.SHGetFolderPathA = PE::GetExport( m_shell32_dll, HASH( "SHGetFolderPathA" ) ).as< WinAPI::SHGetFolderPathA_t >( );
	g_winapi.PathAppendA = PE::GetExport( m_shlwapi_dll, HASH( "PathAppendA" ) ).as< WinAPI::PathAppendA_t >( );

	// run interface collection code.
	Interfaces interfaces{};

	// get interface pointers.
	m_client = interfaces.get< CHLClient* >( HASH( "VClient" ) );
	m_cvar = interfaces.get< ICvar* >( HASH( "VEngineCvar" ) );
	m_engine = interfaces.get< IVEngineClient* >( HASH( "VEngineClient" ) );
	m_entlist = interfaces.get< IClientEntityList* >( HASH( "VClientEntityList" ) );
	m_input_system = interfaces.get< IInputSystem* >( HASH( "InputSystemVersion" ) );
	m_surface = interfaces.get< ISurface* >( HASH( "VGUI_Surface" ) );
	m_panel = interfaces.get< IPanel* >( HASH( "VGUI_Panel" ) );
	m_engine_vgui = interfaces.get< IEngineVGui* >( HASH( "VEngineVGui" ) );
	m_prediction = interfaces.get< CPrediction* >( HASH( "VClientPrediction" ) );
	m_engine_trace = interfaces.get< IEngineTrace* >( HASH( "EngineTraceClient" ) );
	m_game_movement = interfaces.get< CGameMovement* >( HASH( "GameMovement" ) );
	m_render_view = interfaces.get< IVRenderView* >( HASH( "VEngineRenderView" ) );
	m_model_render = interfaces.get< IVModelRender* >( HASH( "VEngineModel" ) );
	m_material_system = interfaces.get< IMaterialSystem* >( HASH( "VMaterialSystem" ) );
	m_studio_render = interfaces.get< CStudioRenderContext* >( HASH( "VStudioRender" ) );
	m_model_info = interfaces.get< IVModelInfo* >( HASH( "VModelInfoClient" ) );
	m_debug_overlay = interfaces.get< IVDebugOverlay* >( HASH( "VDebugOverlay" ) );
	m_phys_props = interfaces.get< IPhysicsSurfaceProps* >( HASH( "VPhysicsSurfaceProps" ) );
	m_game_events = interfaces.get< IGameEventManager2* >( HASH( "GAMEEVENTSMANAGER" ), 1 );
	m_match_framework = interfaces.get< CMatchFramework* >( HASH( "MATCHFRAMEWORK_" ) );
	m_localize = interfaces.get< ILocalize* >( HASH( "Localize_" ) );
	m_networkstringtable = interfaces.get< INetworkStringTableContainer* >( HASH( "VEngineClientStringTable" ) );
	m_sound = interfaces.get< IEngineSound* >( HASH( "IEngineSoundClient" ) );
	m_effects = interfaces.get< IVEffects* >( HASH( "VEngineEffects" ) );

	// convars.
	clear = m_cvar->FindVar( HASH( "clear" ) );
	toggleconsole = m_cvar->FindVar( HASH( "toggleconsole" ) );
	name = m_cvar->FindVar( HASH( "name" ) );
	sv_maxunlag = m_cvar->FindVar( HASH( "sv_maxunlag" ) );
	sv_gravity = m_cvar->FindVar( HASH( "sv_gravity" ) );
	sv_jump_impulse = m_cvar->FindVar( HASH( "sv_jump_impulse" ) );
	sv_enablebunnyhopping = m_cvar->FindVar( HASH( "sv_enablebunnyhopping" ) );
	sv_airaccelerate = m_cvar->FindVar( HASH( "sv_airaccelerate" ) );
	sv_friction = m_cvar->FindVar( HASH( "sv_friction" ) );
	sv_stopspeed = m_cvar->FindVar( HASH( "sv_stopspeed" ) );
	cl_interp = m_cvar->FindVar( HASH( "cl_interp" ) );
	cl_interp_ratio = m_cvar->FindVar( HASH( "cl_interp_ratio" ) );
	cl_updaterate = m_cvar->FindVar( HASH( "cl_updaterate" ) );
	cl_cmdrate = m_cvar->FindVar( HASH( "cl_cmdrate" ) );
	cl_lagcompensation = m_cvar->FindVar( HASH( "cl_lagcompensation" ) );
	mp_teammates_are_enemies = m_cvar->FindVar( HASH( "mp_teammates_are_enemies" ) );
	weapon_debug_spread_show = m_cvar->FindVar( HASH( "weapon_debug_spread_show" ) );
	molotov_throw_detonate_time = m_cvar->FindVar( HASH( "molotov_throw_detonate_time" ) );
	weapon_molotov_maxdetonateslope = m_cvar->FindVar( HASH( "weapon_molotov_maxdetonateslope" ) );
	weapon_recoil_scale = m_cvar->FindVar( HASH( "weapon_recoil_scale" ) );
	view_recoil_tracking = m_cvar->FindVar( HASH( "view_recoil_tracking" ) );
	cl_fullupdate = m_cvar->FindVar( HASH( "cl_fullupdate" ) );
	r_DrawSpecificStaticProp = m_cvar->FindVar( HASH( "r_DrawSpecificStaticProp" ) );
	cl_crosshair_sniper_width = m_cvar->FindVar( HASH( "cl_crosshair_sniper_width" ) );
	hud_scaling = m_cvar->FindVar( HASH( "hud_scaling" ) );
	sv_clip_penetration_traces_to_players = m_cvar->FindVar( HASH( "sv_clip_penetration_traces_to_players" ) );
	weapon_accuracy_shotgun_spread_patterns = m_cvar->FindVar( HASH( "weapon_accuracy_shotgun_spread_patterns" ) );
	net_showfragments = m_cvar->FindVar( HASH( "net_showfragments" ) );

	// hehe xd.
	name->m_callbacks.RemoveAll( );
	//cl_lagcompensation->m_callbacks.RemoveAll( );
	//cl_lagcompensation->m_flags &= ~FCVAR_NOT_CONNECTED;

	// classes by sig.
	m_move_helper = pattern::find( m_client_dll, XOR( "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) ).add( 2 ).get< IMoveHelper* >( 2 );
	//m_game = pattern::find( m_engine_dll, XOR( "A1 ? ? ? ? B9 ? ? ? ? FF 75 08 FF 50 34" ) ).add( 1 ).get< CGame* >( );
	//m_render = pattern::find( m_engine_dll, XOR( "A1 ? ? ? ? B9 ? ? ? ? FF 75 0C FF 75 08 FF 50 0C" ) ).add( 1 ).get< CRender* >( );
	m_shadow_mgr = pattern::find( m_client_dll, XOR( "A1 ? ? ? ? FF 90 ? ? ? ? 6A 00" ) ).add( 1 ).get( ).as< IClientShadowMgr* >( );
	m_view_render = pattern::find( m_client_dll, XOR( "8B 0D ? ? ? ? 8B 01 FF 50 4C 8B 06" ) ).add( 2 ).get< CViewRender* >( 2 );
	// m_entity_listeners = pattern::find( m_client_dll, XOR( "B9 ? ? ? ? E8 ? ? ? ? 5E 5D C2 04" ) ).add( 1 ).get< IClientEntityListener** >( 2 );
	m_hud = pattern::find( m_client_dll, XOR( "B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89" ) ).add( 1 ).get( ).as< CHud* >( );
	m_gamerules = pattern::find( m_client_dll, XOR( "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A" ) ).add( 0x1 ).get< C_CSGameRules* >( );
	m_beams = pattern::find( m_client_dll, XOR( "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9" ) ).add( 1 ).get< IViewRenderBeams* >( );
	m_mem_alloc = PE::GetExport( m_tier0_dll, HASH( "g_pMemAlloc" ) ).get< IMemAlloc* >( );
	m_glow = pattern::find( m_client_dll, XOR( "0F 11 05 ? ? ? ? 83 C8 01" ) ).add( 3 ).get< CGlowObjectManager* >( );

	// classes by offset from virtual.
	m_globals = util::get_method( m_client, 11 ).add( 0xa ).get< CGlobalVarsBase* >( 2 );
	m_client_mode = util::get_method( m_client, CHLClient::HUDPROCESSINPUT ).add( 0x5 ).get< IClientMode* >( 2 );
	m_input = util::get_method( m_client, CHLClient::INACTIVATEMOUSE ).at< CInput* >( 0x1 );
	m_cl = util::get_method( m_engine, 12 ).add( 0x10 ).get< CClientState* >( 2 );

	m_hookable_cl = reinterpret_cast< void* >( *reinterpret_cast< uintptr_t** >( reinterpret_cast< uintptr_t >( m_cl ) + 0x8 ) );

	// functions.
	MD5_PseudoRandom = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 83 EC 70 6A 58" ) ).as< MD5_PseudoRandom_t >( );
	SetAbsAngles = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1" ) );
	InvalidateBoneCache = pattern::find( m_client_dll, XOR( "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) );
	LockStudioHdr = pattern::find( m_client_dll, XOR( "55 8B EC 51 53 8B D9 56 57 8D B3 ? ? ? ? FF" ) );
	SetAbsOrigin = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 51 53 56 57 8B F1" ) );
	IsBreakableEntity = pattern::find( m_client_dll, XOR( "55 8B EC 51 56 8B F1 85 F6 74 68 83 BE" ) ).as< IsBreakableEntity_t >( );
	SetAbsVelocity = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1" ) );
	AngleMatrix = pattern::find( m_client_dll, XOR( "E8 ? ? ? ? 8B 07 89 46 0C" ) ).rel32( 0x1 ).as< AngleMatrix_t >( );
	ComputeHitboxSurroundingBox = pattern::find( m_client_dll, XOR( "E9 ? ? ? ? 32 C0 5D" ) ).rel32( 0x1 );
	GetSequenceActivity = pattern::find( m_client_dll, XOR( "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );
	LoadFromBuffer = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89" ) ).rel32( 0x1 ).as< LoadFromBuffer_t >( );
	ServerRankRevealAll = pattern::find( m_client_dll, XOR( "55 8B EC 8B 0D ? ? ? ? 68" ) ).as< ServerRankRevealAll_t >( );
	HasC4 = pattern::find( m_client_dll, XOR( "56 8B F1 85 F6 74 31" ) );
	InvalidatePhysicsRecursive = pattern::find( m_client_dll, XOR( "E8 ? ? ? ? 89 5E 18" ) ).rel32( 0x1 );
	IsReady = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" ) ).as< IsReady_t >( );
	GetEconItemView = pattern::find( m_client_dll, XOR( "8B 81 ? ? ? ? 81 C1 ? ? ? ? FF 50 04 83 C0 40 C3" ) ).as< GetEconItemView_t >( );
	GetStaticData = pattern::find( m_client_dll, XOR( "55 8B EC 51 53 8B D9 8B 0D ? ? ? ? 56 57 8B B9" ) ).as< GetStaticData_t >( );
	TEFireBullets = pattern::find( m_client_dll, XOR( "C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? 66 A3" ) ).add( 0x2 ).to( );
	AddListenerEntity = pattern::find( m_client_dll, XOR( "55 8B EC 8B 0D ? ? ? ? 33 C0 56 85 C9 7E 32" ) ).as< AddListenerEntity_t >( );
	GetShotgunSpread = pattern::find( m_client_dll, XOR( "E8 ? ? ? ? EB 38 83 EC 08" ) ).rel32< GetShotgunSpread_t >( 1 );
	BoneAccessor = pattern::find( m_client_dll, XOR( "8D 81 ? ? ? ? 50 8D 84 24" ) ).add( 2 ).to< size_t >( );
	AnimOverlay = pattern::find( m_client_dll, XOR( "8B 89 ? ? ? ? 8D 0C D1" ) ).add( 2 ).to< size_t >( );
	SpawnTime = pattern::find( m_client_dll, XOR( "F3 0F 5C 88 ? ? ? ? 0F" ) ).add( 4 ).to< size_t >( );
	IsLocalPlayer = pattern::find( m_client_dll, XOR( "74 ? 8A 83 ? ? ? ? 88" ) ).add( 4 ).to< size_t >( );
	PlayerAnimState = pattern::find( m_client_dll, XOR( "8B 8E ? ? ? ? 85 C9 74 3E" ) ).add( 2 ).to< size_t >( );
	studioHdr = pattern::find( m_client_dll, XOR( "8B 86 ? ? ? ? 89 44 24 10 85 C0" ) ).add( 2 ).to< size_t >( );
	UTIL_TraceLine = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F0 83 EC 7C 56 52" ) );
	CTraceFilterSimple_vmt = UTIL_TraceLine.add( 0x3D ).to( );
	CTraceFilterSkipTwoEntities_vmt = pattern::find( m_client_dll, XOR( "C7 45 ? ? ? ? ? 89 45 E4 8B 01" ) ).add( 3 ).to( );
	LastBoneSetupTime = InvalidateBoneCache.add( 0x11 ).to< size_t >( );
	MostRecentModelBoneCounter = InvalidateBoneCache.add( 0x1B ).to< size_t >( );
	PlayerVTable = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C" ) ).at( 0x47 );
	SetCollisionBounds = pattern::find( m_client_dll, XOR( "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 18 56 57 8B 7B 08 8B D1 8B 4B 0C" ) ).as< SetCollisionBounds_t >( );
	ClearKillfeed = pattern::find( m_client_dll, XOR( "55 8B EC 83 EC 0C 53 56 8B 71 58" ) ).as< ClearNotices_t >( );
	ClearHudWeapons = pattern::find( m_client_dll, XOR( "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 34" ) );
	CL_Move = pattern::find( m_engine_dll, XOR( "55 8B EC 81 EC ? ? ? ? 53 56 8A F9 F3 0F 11 45 ? 8B 4D 04" ) ).as< CL_Move_t >( );
	WriteUsercmdDeltaToBuffer = pattern::find( m_client_dll, XOR( "55 8B EC B9 ? ? ? ? A1 ? ? ? ? 8B 40 14" ) );
	WriteUserCmd = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 51 53 56 8B D9" ) );
	ClampBonesInBBox = pattern::find( m_client_dll, XOR( "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38 83 BF" ) );
	ShouldSkipAnimationFrame = pattern::find( m_client_dll, XOR( "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02" ) );

	// exported functions.
	RandomSeed = PE::GetExport( m_vstdlib_dll, HASH( "RandomSeed" ) ).as< RandomSeed_t >( );
	RandomInt = PE::GetExport( m_vstdlib_dll, HASH( "RandomInt" ) ).as< RandomInt_t >( );
	RandomFloat = PE::GetExport( m_vstdlib_dll, HASH( "RandomFloat" ) ).as< RandomFloat_t >( );

	// exported interfaces.
	m_kv_system = PE::GetExport( m_vstdlib_dll, HASH( "KeyValuesSystem" ) ).as< void* ( __cdecl* )( ) >( )( );

	// prediction pointers.
	m_nPredictionRandomSeed = util::get_method( m_prediction, CPrediction::RUNCOMMAND ).add( 0x30 ).get< int* >( );
	m_pPredictionPlayer = util::get_method( m_prediction, CPrediction::RUNCOMMAND ).add( 0x54 ).get< Player* >( );

	// some weird tier0 stuff that prevents me from debugging properly...
#ifdef _DEBUG
	Address debugbreak = pattern::find( g_csgo.m_client_dll, XOR( "CC F3 0F 10 4D ? 0F 57 C0" ) );

	DWORD old;
	g_winapi.VirtualProtect( debugbreak, 1, PAGE_EXECUTE_READWRITE, &old );

	debugbreak.set< uint8_t >( 0x90 );

	g_winapi.VirtualProtect( debugbreak, 1, old, &old );
#endif

	// init everything else.
	g_config.init( );

	// g_netvars stores all netvar offsets into an unordered_map, EntOffsets is for the raw offset values so we don't have to access the unordered_map a bunch.
	g_netvars.init( );
	g_entoffsets.init( );

	g_listener.init( );
	render::init( );
	g_menu.init( );
	g_config.LoadHotkeys( );
	g_chams.init( );
	g_hooks.init( );

	// if we injected and we're ingame, run map load func.
	if ( m_engine->IsInGame( ) ) {
		g_cl.OnMapload( );
		//g_csgo.cl_fullupdate->m_callback( );
	}

	m_done = true;
	return true;
}

bool game::IsBreakable( Entity* ent ) {
	bool        ret;
	ClientClass* cc;
	const char* name;
	char* takedmg, old_takedmg;

	static size_t m_takedamage_offset{ *( size_t* )( ( uintptr_t )g_csgo.IsBreakableEntity + 38 ) };

	// skip null ents and the world ent.
	if ( !ent || ent->index( ) == 0 )
		return false;

	// get m_takedamage and save old m_takedamage.
	takedmg = ( char* )( ( uintptr_t )ent + m_takedamage_offset );
	old_takedmg = *takedmg;

	// get clientclass.
	cc = ent->GetClientClass( );

	if ( cc ) {
		// get clientclass network name.
		name = cc->m_pNetworkName;

		// CBreakableSurface, CBaseDoor, ...
		if ( name[ 1 ] != 'F' || name[ 4 ] != 'c' || name[ 5 ] != 'B' || name[ 9 ] != 'h' ) {
			*takedmg = DAMAGE_YES;
		}
	}

	ret = g_csgo.IsBreakableEntity( ent );
	*takedmg = old_takedmg;

	return ret;
}
