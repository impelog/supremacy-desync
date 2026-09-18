#include "includes.h"

Bones g_bones{};

bool Bones::setup( Player* player, BoneArray* out, LagRecord* record ) {
	if ( !record->m_setup ) {
		if ( !BuildBones( player, 0x7FF00, record->m_bones, record ) )
			return false;

		record->m_setup = true;
	}

	if ( out && record->m_setup )
		std::memcpy( out, record->m_bones, sizeof( BoneArray ) * 128 );

	return true;
}

bool Bones::BuildBones( Player* target, int mask, BoneArray* out, LagRecord* record ) {
	float o_curtime = g_csgo.m_globals->m_curtime;
	float o_realtime = g_csgo.m_globals->m_realtime;
	float o_frametime = g_csgo.m_globals->m_frametime;
	float o_abs_frametime = g_csgo.m_globals->m_abs_frametime;
	int   o_framecount = g_csgo.m_globals->m_frame;
	int   o_tickcount = g_csgo.m_globals->m_tick_count;
	float o_interp_amount = g_csgo.m_globals->m_interp_amt;

	float anim_time = record->m_sim_time;
	int   anim_ticks = game::TIME_TO_TICKS( anim_time );

	g_csgo.m_globals->m_curtime = anim_time;
	g_csgo.m_globals->m_realtime = anim_time;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_abs_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frame = anim_ticks;
	g_csgo.m_globals->m_tick_count = anim_ticks;
	g_csgo.m_globals->m_interp_amt = 0.0f;

	vec3_t backup_origin = target->GetAbsOrigin( );
	ang_t backup_angles = target->GetAbsAngles( );
	float backup_poses[ 24 ];
	C_AnimationLayer backup_layers[ 13 ];
	target->GetPoseParameters( backup_poses );
	target->GetAnimLayers( backup_layers );
	int backup_eflags = target->m_iEFlags( );
	int backup_effects = target->m_fEffects( );
	bool backup_jiggle = target->m_bEnableJiggleBones( );

	target->m_iEFlags( ) |= 8;
	target->m_fEffects( ) |= 8;
	target->m_bEnableJiggleBones( ) = false;

	target->SetAbsOrigin( record->m_pred_origin );
	target->SetAbsAngles( record->m_abs_ang );
	target->SetPoseParameters( record->m_poses );
	target->SetAnimLayers( record->m_layers );

	target->InvalidateBoneCache( );

	bool success = target->SetupBones( out, 128, mask, record->m_sim_time );

	target->m_iEFlags( ) = backup_eflags;
	target->m_fEffects( ) = backup_effects;
	target->m_bEnableJiggleBones( ) = backup_jiggle;
	target->SetAbsOrigin( backup_origin );
	target->SetAbsAngles( backup_angles );
	target->SetPoseParameters( backup_poses );
	target->SetAnimLayers( backup_layers );

	g_csgo.m_globals->m_curtime = o_curtime;
	g_csgo.m_globals->m_realtime = o_realtime;
	g_csgo.m_globals->m_frametime = o_frametime;
	g_csgo.m_globals->m_abs_frametime = o_abs_frametime;
	g_csgo.m_globals->m_frame = o_framecount;
	g_csgo.m_globals->m_tick_count = o_tickcount;
	g_csgo.m_globals->m_interp_amt = o_interp_amount;

	return success;
}