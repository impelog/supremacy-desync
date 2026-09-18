#include "includes.h"

#define NET_FRAMES_BACKUP 64 // must be power of 2. 
#define NET_FRAMES_MASK ( NET_FRAMES_BACKUP - 1 )

int Hooks::SendDatagram( void* data ) {
	int backup2 = g_csgo.m_net->m_in_seq;

	if ( g_aimbot.m_fake_latency ) {
		int ping = g_menu.main.misc.fake_latency_amt.get( );

		// the target latency.
		float correct = std::max( 0.f, ( ping / 1000.f ) - g_cl.m_latency - g_cl.m_lerp );

		g_csgo.m_net->m_in_seq += 2 * NET_FRAMES_MASK - static_cast< uint32_t >( NET_FRAMES_MASK * correct );
	}

	int ret = g_hooks.m_net_channel.GetOldMethod< SendDatagram_t >( INetChannel::SENDDATAGRAM )( this, data );

	g_csgo.m_net->m_in_seq = backup2;

	return ret;
}

void Hooks::ProcessPacket( void* packet, bool header ) {
	g_hooks.m_net_channel.GetOldMethod< ProcessPacket_t >( INetChannel::PROCESSPACKET )( this, packet, header );

	g_cl.UpdateIncomingSequences( );

	// get this from CL_FireEvents string "Failed to execute event for classId" in engine.dll
	for ( CEventInfo* it{ g_csgo.m_cl->pEvents }; it != nullptr; it = it->pNext ) {
		if ( !it->iClassID )
			continue;

		// set all delays to instant.
		it->flFireDelay = 0.f;
	}

	// game events are actually fired in OnRenderStart which is WAY later after they are received
	// effective delay by lerp time, now we call them right after theyre received (all receive proxies are invoked without delay).
	g_csgo.m_engine->FireEvents( );
}

bool Hooks::SendNetMsg( INetMessage* msg, bool force_reliable, bool voice ) {
	// disable files crc check.
	if ( msg->GetType( ) == 14 )
		return false;

	// fix lag with chocking packets when voice chat is active.
	if ( msg->GetGroup( ) == INetChannel::VOICE )
		voice = true;

	return g_hooks.m_net_channel.GetOldMethod< SendNetMsg_t >( INetChannel::SENDNETMSG )( this, msg, force_reliable, voice );
}
