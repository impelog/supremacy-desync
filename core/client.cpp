#include "includes.h"

Client g_cl{ };

// loader will set this fucker.
char username[ 33 ] = "\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90";

// init routine.
ulong_t __stdcall Client::init( void* arg ) {
	// add custom exception handler.
	SetUnhandledExceptionFilter( Exceptions::OnException );

	// if not in interwebz mode, the driver will not set the username.
	g_cl.m_user = XOR( "user" );

	// stop here if we failed to acquire all the data needed from csgo.
	if ( !g_csgo.init( ) )
		return 0;

	// welcome the user.
	g_notify.add( tfm::format( XOR( "welcome %s\n" ), g_cl.m_user ) );

	return 1;
}

void Client::KillFeed( ) {
	if ( !g_menu.main.misc.killfeed.get( ) )
		return;

	if ( !g_csgo.m_engine->IsInGame( ) || !g_cl.m_local )
		return;

	static bool should_clear = false;

	auto element = g_csgo.m_hud->FindElement( FNV1a::get( "CCSGO_HudDeathNotice" ) );
	if ( !element )
		return;

	auto death_notice = reinterpret_cast< uintptr_t >( element );
	if ( !death_notice )
		return;

	auto notice_ptr = reinterpret_cast< uintptr_t* >( death_notice - 0x14 );
	if ( !notice_ptr )
		return;

	auto lifetime = reinterpret_cast< float* >( death_notice + 0x50 );
	if ( !lifetime )
		return;

	static float last_spawn = 0.f;
	static bool last_state = false;

	bool cfg = g_menu.main.misc.killfeed.get( );
	float spawn = g_cl.m_local->m_flSpawnTime( );

	bool should_update = ( spawn != last_spawn ) || ( cfg != last_state );

	if ( should_update ) {
		last_spawn = spawn;
		last_state = cfg;

		*lifetime = cfg ? FLT_MAX : 1.5f;
	}

	static auto clear_fn = g_csgo.ClearKillfeed.as<void( __thiscall* )( uintptr_t* ) >( );

	if ( should_clear ) {
		should_clear = false;
		clear_fn( notice_ptr );
	}
}

void Client::OnPaint( ) {
	// update screen size.
	g_csgo.m_engine->GetScreenSize( m_width, m_height );

	// render stuff.
	g_visuals.think( );
	g_grenades.paint( );
	g_notify.think( );

	KillFeed( );

	// menu goes last.
	g_gui.think( );
}

void Client::OnMapload( ) {
	// store class ids.
	g_netvars.SetupClassData( );

	// createmove will not have been invoked yet.
	// but at this stage entites have been created.
	// so now we can retrive the pointer to the local player.
	m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );

	// world materials.
	Visuals::ModulateWorld( );

	// init knife shit.
	g_skins.load( );

	m_sequences.clear( );

	// if the INetChannelInfo pointer has changed, store it for later.
	g_csgo.m_net = g_csgo.m_engine->GetNetChannelInfo( );

	if ( g_csgo.m_net ) {
		g_hooks.m_net_channel.reset( );
		g_hooks.m_net_channel.init( g_csgo.m_net );
		g_hooks.m_net_channel.add( INetChannel::PROCESSPACKET, util::force_cast( &Hooks::ProcessPacket ) );
		g_hooks.m_net_channel.add( INetChannel::SENDDATAGRAM, util::force_cast( &Hooks::SendDatagram ) );
		g_hooks.m_net_channel.add( INetChannel::SENDNETMSG, util::force_cast( &Hooks::SendNetMsg ) );
	}
}

void Client::StartMove( CUserCmd* cmd ) {
	// save some usercmd stuff.
	m_cmd = cmd;
	m_tick = cmd->m_tick;
	m_view_angles = cmd->m_view_angles;
	m_buttons = cmd->m_buttons;

	g_exploits.DefenseiveThisTick( ) = false;
	g_exploits.force_charge = false;

	// get local ptr.
	m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );
	if ( !m_local )
		return;

	m_corrected_tickbase = m_local->m_nTickBase( ) - m_tickbase_shift + m_lc_exploit;

	// store max choke
	// TODO; 11 -> m_bIsValveDS
	m_max_lag = ( m_local->m_fFlags( ) & FL_ONGROUND ) ? 16 : 15;
	m_lag = g_csgo.m_cl ? g_csgo.m_cl->nChokedCommands : 0;
	m_lerp = game::GetClientInterpAmount( );
	if ( g_csgo.m_net ) {
		float total_lat = g_csgo.m_net->GetLatency( INetChannel::FLOW_INCOMING ) + g_csgo.m_net->GetLatency( INetChannel::FLOW_OUTGOING );
		m_latency = total_lat;
		math::clamp( m_latency, 0.f, 1.f );
		m_latency_ticks = game::TIME_TO_TICKS( total_lat );
	}
	else {
		m_latency = 0.f;
		m_latency_ticks = 0;
	}
	m_server_tick = g_csgo.m_cl ? g_csgo.m_cl->clockDriftMgr.m_nServerTick : 0;
	m_arrival_tick = m_server_tick + 1 + m_latency_ticks + game::TIME_TO_TICKS( m_lerp );

	// processing indicates that the localplayer is valid and alive.
	m_processing = m_local && m_local->alive( );
	if ( !m_processing )
		return;

	// make sure prediction has ran on all usercommands.
	// because prediction runs on frames, when we have low fps it might not predict all usercommands.
	// also fix the tick being inaccurate.
	g_inputpred.update( );

	// store some stuff about the local player.
	m_flags = m_local->m_fFlags( );

	// ...
	m_shot = false;
}

void Client::BackupPlayers( bool restore ) {
	if ( restore ) {
		// restore stuff.
		for ( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );

			if ( !g_aimbot.IsValidTarget( player ) )
				continue;

			g_aimbot.m_backup[ i - 1 ].restore( player );
		}
	}

	else {
		// backup stuff.
		for ( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );

			if ( !g_aimbot.IsValidTarget( player ) )
				continue;

			g_aimbot.m_backup[ i - 1 ].store( player );
		}
	}
}

void Client::DoMove( ) {
	penetration::PenetrationOutput_t tmp_pen_data{ };

	m_weapon = nullptr;
	m_weapon_info = nullptr;
	m_weapon_id = -1;
	m_weapon_type = WEAPONTYPE_UNKNOWN;
	m_player_fire = m_weapon_fire = false;

	if ( m_local ) {
		m_weapon = m_local->GetActiveWeapon( );
		if ( m_weapon ) {
			m_weapon_info = m_weapon->GetWpnData( );
			if ( m_weapon_info ) {
				m_weapon_id = m_weapon->m_iItemDefinitionIndex( );
				m_weapon_type = m_weapon_info->nWeaponType;
			}
		}
	}

	m_strafe_angles = m_cmd->m_view_angles;

	g_movement.JumpRelated( );
	g_movement.Strafe( );
	g_movement.SlowWalk( );
	g_movement.AutoPeek( );
	g_hvh.FakeDuck( );

	g_exploits.DefensiveDoubletap( );

	g_inputpred.run( );

	m_cmd->m_view_angles = m_view_angles;

	math::AngleVectors( m_view_angles, &m_forward_dir );

	m_shoot_pos = g_hvh.m_fake_duck ? ( m_local->m_vecOrigin( ) + vec3_t( 0.f, 0.f, 64.f ) ) : m_local->Weapon_ShootPosition( );

	if ( m_weapon && m_weapon_info ) {
		if ( m_weapon_type != WEAPONTYPE_KNIFE && m_weapon_type != WEAPONTYPE_C4 && m_weapon_type != WEAPONTYPE_GRENADE )
			m_weapon->UpdateAccuracyPenalty( );

		if ( g_menu.main.visuals.pen_crosshair.get( ) && m_weapon_type != WEAPONTYPE_KNIFE && m_weapon_type != WEAPONTYPE_C4 && m_weapon_type != WEAPONTYPE_GRENADE ) {
			penetration::PenetrationInput_t in;
			in.m_from = m_local;
			in.m_target = nullptr;
			in.m_pos = m_shoot_pos + ( m_forward_dir * m_weapon_info->flRange );
			in.m_damage = 1.f;
			in.m_damage_pen = 1.f;
			in.m_can_pen = true;

			penetration::run( &in, &tmp_pen_data );
		}

		m_pen_data = tmp_pen_data;

		bool freeze = g_csgo.m_gamerules ? g_csgo.m_gamerules->m_bFreezePeriod( ) : false;
		m_player_fire = !freeze && !( g_cl.m_flags & FL_FROZEN );

		UpdateRevolverCock( );
		m_weapon_fire = CanFireWeapon( );
	}

	if ( g_menu.main.misc.last_tick_defuse.get_status( ) && g_visuals.m_c4_planted ) {
		float defuse = ( m_local->m_bHasDefuser( ) ) ? 5.f : 10.f;
		float remaining = g_visuals.m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;
		float dt = remaining - defuse - ( g_cl.m_latency / 2.f );

		m_cmd->m_buttons &= ~IN_USE;
		if ( dt <= game::TICKS_TO_TIME( 2 ) )
			m_cmd->m_buttons |= IN_USE;
	}

	if ( g_exploits.IsShifting( ) ) {
		g_hvh.AntiAim( );

		m_packet = ( m_tickbase_shift <= 1 );

		if ( m_weapon && m_weapon_type != WEAPONTYPE_GRENADE && m_weapon_type != WEAPONTYPE_KNIFE && m_weapon_type != WEAPONTYPE_C4 )
			m_cmd->m_buttons &= ~( IN_ATTACK | IN_ATTACK2 );

		return;
	}

	g_grenades.think( );

	if ( m_weapon && m_weapon_fire && ( ( m_cmd->m_buttons & IN_ATTACK ) || ( m_weapon_id == WEAPON_REVOLVER && ( m_cmd->m_buttons & IN_ATTACK2 ) ) ) ) {
		m_last_shot_time = g_csgo.m_globals->m_realtime;
		if ( g_exploits.GetExploitType( ) == Exploits::E_DoubleTap )
			g_exploits.ForceTeleport( );
		else if ( g_exploits.GetExploitType( ) == Exploits::E_HideShots )
			g_exploits.HideShot( );
	}

	g_hvh.SendPacket( );

	g_aimbot.think( );

	if ( ( m_shot || ( m_cmd->m_buttons & IN_ATTACK ) ) && g_movement.m_peeking ) {
		g_movement.m_retracting = true;
	}

	if ( g_exploits.TeleportThisTick( ) )
		m_packet = false;

	if ( m_packet ) {
		m_breaking_lag_compensation = ( m_local_sent_origin - m_local->m_vecOrigin( ) ).length_2d_sqr( ) > 4096.f;
		m_local_sent_origin = m_local->m_vecOrigin( );
	}

	g_hvh.AntiAim( );
}

void Client::EndMove( CUserCmd* cmd ) {
	// update client-side animations.
	UpdateInformation( );

	m_cmd->m_view_angles.SanitizeAngle( );

	// fix our movement.
	g_movement.FixMove( cmd, m_strafe_angles );

	if ( g_exploits.IsShifting( ) ) {
		m_lc_exploit_prev = m_lc_exploit;
		if ( m_packet )
			m_lc_exploit = 0;

		if ( m_lc_exploit != m_lc_exploit_prev ) {
			m_lc_exploit_change = cmd->m_command_number;
			m_lc_exploit_diff = m_lc_exploit - m_lc_exploit_prev;
		}

		m_shifted_commands.push_back( cmd->m_command_number );
		if ( m_packet )
			m_sented_commands.push_back( cmd->m_command_number );

		m_teleported_last_tick = true;
		m_old_packet = m_packet;
		m_old_shot = m_shot;
		return;
	}

	// this packet will be sent.
	if ( m_packet ) {
		g_hvh.m_step_switch = ( bool )g_csgo.RandomInt( 0, 1 );

		// we are sending a packet, so this will be reset soon.
		// store the old value.
		m_old_lag = m_lag;

		// get radar angles.
		m_radar = cmd->m_view_angles;
		m_radar.normalize( );

		// get current origin.
		vec3_t cur = m_local->m_vecOrigin( );

		// get prevoius origin.
		vec3_t prev = m_net_pos.empty( ) ? cur : m_net_pos.front( ).m_pos;

		// check if we broke lagcomp.
		m_lagcomp = ( cur - prev ).length_sqr( ) > 4096.f;

		// save sent origin and time.
		m_net_pos.emplace_front( g_csgo.m_globals->m_curtime, cur );
	}

	m_lc_exploit_prev = m_lc_exploit;
	if ( m_packet )
		m_lc_exploit = g_exploits.LC_TickbaseShift( );

	if ( m_lc_exploit != m_lc_exploit_prev ) {
		m_lc_exploit_change = cmd->m_command_number;
		m_lc_exploit_diff = m_lc_exploit - m_lc_exploit_prev;
	}

	if ( m_packet ) {
		m_sented_commands.push_back( cmd->m_command_number );
	}

	m_teleported_last_tick = false;

	// store some values for next tick.
	m_old_packet = m_packet;
	m_old_shot = m_shot;
}

void Client::OnTick( CUserCmd* cmd ) {
	// store some data and update prediction.
	StartMove( cmd );

	// not much more to do here.
	if ( !m_processing )
		return;

	// save the original state of players.
	BackupPlayers( false );

	// run all movement related code.
	DoMove( );

	// store stome additonal stuff for next tick
	// sanetize our usercommand if needed and fix our movement.
	EndMove( cmd );

	// restore the players.
	BackupPlayers( true );

	// restore curtime/frametime
	// and prediction seed/player.
	g_inputpred.restore( );
}

void Client::SetAngles( ) {
	if ( !g_cl.m_local || !g_cl.m_processing )
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState( );
	if ( !state )
		return;

	if ( g_csgo.m_input->CAM_IsThirdPerson( ) )
		g_csgo.m_prediction->SetLocalViewAngles( m_radar );
}

void Client::UpdateAnimations( ) {
	if ( !g_cl.m_local || !g_cl.m_processing )
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState( );
	if ( !state )
		return;

	g_cl.m_local->m_AnimOverlay( )[ 12 ].m_weight = 0.f;

	if ( g_hvh.m_fake_duck ) {
		g_cl.m_local->m_flDuckAmount( ) = 1.f;
		g_cl.m_local->SetPoseParameters( g_cl.m_poses );
		g_cl.m_local->SetAnimLayers( g_cl.m_layers );
	}
	else {
		g_cl.m_local->m_flPoseParameter( )[ 12 ] = ( m_radar.x + 90.f ) / 180.f;
		g_cl.m_local->m_flLowerBodyYawTarget( ) = g_cl.m_abs_yaw;
	}

	g_cl.m_local->SetAbsAngles( ang_t( 0.f, g_cl.m_abs_yaw, 0.f ) );
}

void Client::UpdateInformation( ) {
	if ( !g_cl.m_local || !g_cl.m_processing )
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState( );
	if ( !state )
		return;

	float backup_curtime = g_csgo.m_globals->m_curtime;
	float backup_frametime = g_csgo.m_globals->m_frametime;

	g_csgo.m_globals->m_curtime = game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) );
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;

	m_anim_frame = g_csgo.m_globals->m_curtime - m_anim_time;
	m_anim_time = g_csgo.m_globals->m_curtime;

	m_angle = ( g_menu.main.antiaim.desync.get( ) && !g_cl.m_packet ) ? g_cl.m_radar : g_cl.m_cmd->m_view_angles;

	math::clamp( m_angle.x, -90.f, 90.f );
	m_angle.normalize( );

	vec3_t backup_velocity = g_cl.m_local->m_vecAbsVelocity( );
	g_cl.m_local->m_vecAbsVelocity( ) = g_cl.m_local->m_vecVelocity( );

	int backup_eflags = g_cl.m_local->m_iEFlags( );
	g_cl.m_local->m_iEFlags( ) &= ~0x1000;

	if ( state->m_frame >= g_csgo.m_globals->m_frame )
		state->m_frame = g_csgo.m_globals->m_frame - 1;

	if ( state->m_prev_update_time >= g_csgo.m_globals->m_curtime )
		state->m_prev_update_time = g_csgo.m_globals->m_curtime - g_csgo.m_globals->m_interval;

	if ( !state->m_player )
		state->m_player = g_cl.m_local;

	game::UpdateAnimationState( state, m_angle );

	state->m_land = g_cl.m_local->m_AnimOverlay( )[ 5 ].m_weight > 0.f && state->m_ground;

	if ( g_cl.m_packet ) {
		g_cl.m_local->GetPoseParameters( g_cl.m_poses );
		g_cl.m_local->GetAnimLayers( g_cl.m_layers );
		g_cl.m_abs_yaw = state->m_goal_feet_yaw;
		g_csgo.m_prediction->SetLocalViewAngles( m_radar );
	}

	m_rotation = g_cl.m_local->m_angAbsRotation( );
	m_speed = state->m_speed;
	m_ground = state->m_ground;

	g_cl.m_local->m_iEFlags( ) = backup_eflags;
	g_cl.m_local->m_vecAbsVelocity( ) = backup_velocity;

	g_csgo.m_globals->m_curtime = backup_curtime;
	g_csgo.m_globals->m_frametime = backup_frametime;
}

void Client::print( const std::string text, ... ) {
	va_list     list;
	int         size;
	std::string buf;

	if ( text.empty( ) )
		return;

	va_start( list, text );

	// count needed size.
	size = std::vsnprintf( 0, 0, text.c_str( ), list );

	// allocate.
	buf.resize( size );

	// print to buffer.
	std::vsnprintf( buf.data( ), size + 1, text.c_str( ), list );

	va_end( list );

	// print to console.
	g_csgo.m_cvar->ConsoleColorPrintf( colors::burgundy, XOR( "[supremacy] " ) );
	g_csgo.m_cvar->ConsoleColorPrintf( colors::white, buf.c_str( ) );
}

bool Client::CanFireWeapon( ) {
	if ( !m_player_fire )
		return false;

	if ( m_weapon_type == WEAPONTYPE_GRENADE )
		return false;

	if ( m_weapon_type != WEAPONTYPE_KNIFE && m_weapon->m_iClip1( ) < 1 )
		return false;

	float curtime = game::TICKS_TO_TIME( ( m_corrected_tickbase && m_weapon_id != WEAPON_REVOLVER ) ? m_corrected_tickbase : m_local->m_nTickBase( ) );

	if ( m_local->m_flNextAttack( ) > curtime )
		return false;

	if ( ( m_weapon_id == WEAPON_GLOCK || m_weapon_id == WEAPON_FAMAS ) && m_weapon->m_iBurstShotsRemaining( ) > 0 ) {
		if ( curtime >= m_weapon->m_fNextBurstShot( ) )
			return true;
	}

	if ( m_weapon_id == WEAPON_REVOLVER ) {
		int act = m_weapon->m_Activity( );

		if ( !m_revolver_fire ) {
			if ( ( act == 185 || act == 193 ) && m_revolver_cock == 0 )
				return curtime >= m_weapon->m_flNextPrimaryAttack( );

			return false;
		}
	}

	if ( curtime >= m_weapon->m_flNextPrimaryAttack( ) )
		return true;

	return false;
}

void Client::UpdateRevolverCock( ) {
	// default to false.
	m_revolver_fire = false;

	// reset properly.
	if ( m_revolver_cock == -1 )
		m_revolver_cock = 0;

	// we dont have a revolver.
	// we have no ammo.
	// player cant fire
	// we are waiting for we can shoot again.
	if ( m_weapon_id != WEAPON_REVOLVER || m_weapon->m_iClip1( ) < 1 || !m_player_fire || g_csgo.m_globals->m_curtime < m_weapon->m_flNextPrimaryAttack( ) ) {
		// reset.
		m_revolver_cock = 0;
		m_revolver_query = 0;
		return;
	}

	// calculate max number of cocked ticks.
	// round to 6th decimal place for custom tickrates..
	int shoot = ( int )( 0.25f / ( std::round( g_csgo.m_globals->m_interval * 1000000.f ) / 1000000.f ) );

	// amount of ticks that we have to query.
	m_revolver_query = shoot - 1;

	// we held all the ticks we needed to hold.
	if ( m_revolver_query == m_revolver_cock ) {
		// reset cocked ticks.
		m_revolver_cock = -1;

		// we are allowed to fire, yay.
		m_revolver_fire = true;
	}

	else {
		// we still have ticks to query.
		// apply inattack.
		if ( g_menu.main.config.mode.get( ) == 0 && m_revolver_query > m_revolver_cock )
			m_cmd->m_buttons |= IN_ATTACK;

		// count cock ticks.
		// do this so we can also count 'legit' ticks
		// that didnt originate from the hack.
		if ( m_cmd->m_buttons & IN_ATTACK )
			m_revolver_cock++;

		// inattack was not held, reset.
		else m_revolver_cock = 0;
	}

	// remove inattack2 if cocking.
	if ( m_revolver_cock > 0 )
		m_cmd->m_buttons &= ~IN_ATTACK2;
}

void Client::UpdateIncomingSequences( ) {
	if ( !g_csgo.m_net )
		return;

	if ( m_sequences.empty( ) || g_csgo.m_net->m_in_seq > m_sequences.front( ).m_seq ) {
		// store new stuff.
		m_sequences.emplace_front( g_csgo.m_globals->m_realtime, g_csgo.m_net->m_in_rel_state, g_csgo.m_net->m_in_seq );
	}

	// do not save too many of these.
	while ( m_sequences.size( ) > 2048 )
		m_sequences.pop_back( );
}