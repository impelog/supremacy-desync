#include "includes.h"

bool Hooks::TempEntities( void* msg ) {
	if ( !g_cl.m_processing ) {
		return g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );
	}

	const bool ret = g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );

	CEventInfo* ei = g_csgo.m_cl->pEvents;
	CEventInfo* next = nullptr;

	if ( !ei ) {
		return ret;
	}

	do {
		next = *reinterpret_cast< CEventInfo** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		uint16_t classID = ei->iClassID - 1;

		auto m_pCreateEventFn = ei->pClientClass->m_pCreateEvent;
		if ( !m_pCreateEventFn ) {
			continue;
		}

		void* pCE = m_pCreateEventFn( );
		if ( !pCE ) {
			continue;
		}

		if ( classID == 170 ) {
			ei->flFireDelay = 0.0f;
		}
		ei = next;
	} while ( next != nullptr );

	return ret;
}