#include "includes.h"

bool Hooks::IsConnected( ) {
	static Address IsLoadoutAllowed{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 05 B0 01 5F" ) ) };

	if( g_menu.main.misc.unlock.get( ) && _ReturnAddress( ) == IsLoadoutAllowed )
		return false;

	return g_hooks.m_engine.GetOldMethod< IsConnected_t >( IVEngineClient::ISCONNECTED )( this );
}

bool Hooks::IsHLTV( ) {
	static Address SetupVelocity{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0" ) ) };
	static Address AccumulateLayers{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 0D F6 87" ) ) };
	static Address ReevaluateAnimLOD{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7" ) ) };

	// AccumulateLayers
	if( g_bones.m_running || _ReturnAddress( ) == AccumulateLayers )
		return true;

	// fix for animstate velocity and LOD.
	if( _ReturnAddress( ) == SetupVelocity || _ReturnAddress( ) == ReevaluateAnimLOD )
		return true;

	return g_hooks.m_engine.GetOldMethod< IsHLTV_t >( IVEngineClient::ISHLTV )( this );
}

bool Hooks::IsPaused( ) {
	static Address ReturnToExtrapolation{ pattern::find( g_csgo.m_client_dll, XOR( "0F B6 0D ? ? ? ? 84 C0 0F 44 CF 88 0D ? ? ? ?" ) ) };

	if ( ( uintptr_t )_ReturnAddress( ) == ReturnToExtrapolation.as< uintptr_t >( ) )
		return true;

	return g_hooks.m_engine.GetOldMethod< IsPaused_t >( IVEngineClient::ISPAUSED )( this );
}

void Hooks::EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const vec3_t* pOrigin, const vec3_t* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity ) {
	if( strstr( pSample, "null" ) ) {
		iFlags = ( 1 << 2 ) | ( 1 << 5 );
	}

	g_hooks.m_engine_sound.GetOldMethod<EmitSound_t>( IEngineSound::EMITSOUND )( this, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity );
}