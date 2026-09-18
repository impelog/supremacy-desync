#pragma once

enum CSRoundEndReason {
	UNKNOWN1 = 0,
	BOMB_DETONATED,			// terrorists planted bomb and it detonated.
	UNKNOWN2,
	UNKNOWN3,
	T_ESCAPED,				// dunno if used.
	CT_STOPPED_ESCAPE,		// dunno if used.
	T_STOPPED,				// dunno if used
	BOMB_DEFUSED,			// counter-terrorists defused the bomb.
	CT_WIN,					// counter-terrorists killed all terrorists.
	T_WIN,					// terrorists killed all counter-terrorists.
	ROUND_DRAW,				// draw ( likely due to time ).
	HOSTAGE_RESCUED,		// counter-terrorists rescued a hostage.
	CT_WIN_TIME,
	T_WIN_TIME,
	T_NOT_ESACPED,
	UNKNOWN4,
	GAME_START,
	T_SURRENDER,
	CT_SURRENDER,
};


class CKeyValuesSystem {
public:
	template< typename T = CKeyValuesSystem* >
	static __forceinline T KeyValuesSystem( ) {
		static auto KeyValuesFactory = PE::GetExport( PE::GetModule( HASH( "vstdlib.dll" ) ), HASH( "KeyValuesSystem" ) );
		return KeyValuesFactory.as< T( *)( ) >( )( );
	}
};

class KeyValues;

class CGameEventCallback {
public:
	void *m_callback;
	int   m_listener_type;
};

class CGameEventDescriptor {
public:
	char							  m_name[ 32 ];
	int								  m_id;
	KeyValues						 *m_keys;
	bool							  m_is_local;
	bool							  m_is_reliable;
	CUtlVector< CGameEventCallback* > m_listeners;
};

class IGameEvent {
public:
	CGameEventDescriptor *m_descriptor;
	KeyValues			 *m_keys;

	virtual ~IGameEvent( ) {};
	virtual const char *GetName( ) const = 0;
	virtual bool IsReliable( ) const = 0;
	virtual bool IsLocal( ) const = 0;
	virtual bool IsEmpty( const char *keyName = nullptr ) = 0;
	virtual bool GetBool( const char *keyName = nullptr, bool defaultValue = false ) = 0;
	virtual int GetInt( const char *keyName = nullptr, int defaultValue = 0 ) = 0;
	virtual unsigned long long GetUint64( char const *keyName = nullptr, unsigned long long defaultValue = 0 ) = 0;
	virtual float GetFloat( const char *keyName = nullptr, float defaultValue = 0.0f ) = 0;
	virtual const char *GetString( const char *keyName = nullptr, const char *defaultValue = "" ) = 0;
	virtual const wchar_t *GetWString( char const *keyName = nullptr, const wchar_t *defaultValue = L"" ) = 0;
	virtual void SetBool( const char *keyName, bool value ) = 0;
	virtual void SetInt( const char *keyName, int value ) = 0;
	virtual void SetUInt64( const char *keyName, unsigned long long value ) = 0;
	virtual void SetFloat( const char *keyName, float value ) = 0;
	virtual void SetString( const char *keyName, const char *value ) = 0;
	virtual void SetWString( const char *keyName, const wchar_t *value ) = 0;
};

class IGameEventListener2 {
public:
	virtual	~IGameEventListener2( void ) {};
	virtual void FireGameEvent( IGameEvent *event ) = 0;
	virtual int GetEventDebugID( void ) {
		return m_debug_id;
	}
public:
	int	m_debug_id;
};

class IGameEventManager2 {
public:
	CUtlVector< CGameEventDescriptor > m_events;
	CUtlVector< CGameEventCallback* >  m_listeners;

	virtual	~IGameEventManager2( void ) {};
	virtual int LoadEventsFromFile( const char *filename ) = 0;
	virtual void Reset( ) = 0;
	virtual bool AddListener( IGameEventListener2 *listener, const char *name, bool bServerSide ) = 0;
	virtual bool FindListener( IGameEventListener2 *listener, const char *name ) = 0;
	virtual void RemoveListener( IGameEventListener2 *listener ) = 0;

	/*bool __forceinline add_listener_internal( IGameEventListener2 *listener, CGameEventDescriptor *descriptor, bool serverside ) {
		// char __thiscall CGameEventManager::AddListener_Internal( void *this, int listener, int a3, bool serverside )
		// 55 8B EC 83 EC 08 8B C1 56 57

		// return GameEventManager::AddListener_Internal( this, listener, descriptor, serverside == false );
	}

	bool __forceinline add_listener_by_hash( IGameEventListener2 *listener, hash_t hash, bool serverside ) {
		CGameEventDescriptor *descriptor;

		for( int i{}; i < m_events.count( ); i++ ) {
			descriptor = &m_events.element( i );

			if( !descriptor )
				continue;

			if( hash::fnv1a( descriptor->m_name ) == hash )
				break;

			else
				descriptor = nullptr;
		}

		if( !descriptor )
			return false;

		return add_listener_internal( listener, descriptor, serverside );
	}*/
};