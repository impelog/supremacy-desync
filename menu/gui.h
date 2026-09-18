#pragma once

class GUI {
private:
	std::vector< Form* > m_forms;

public:
	bool  m_open;
	Form* m_drag_form;
	Point m_drag_offset;
	Color m_color{ colors::white };

public:
	void think( );
	void draw( );

	// registers a new form.
	__forceinline void RegisterForm( Form* form, int key = -1 ) {
		// set key to toggle form.
		form->SetToggle( key );

		// add form to our container.
		m_forms.push_back( form );
	}
};

class Input {
public:
	struct key_t {
		bool down;
		bool pressed;
		int  tick;
		int  oldtick;
	};

	std::array< key_t, 256 > m_keys;
	Point					 m_mouse;
	std::string				 m_buffer;
	bool					 m_first_update{ true };

public:
	__forceinline void update( ) {
		if( m_first_update ) {
			for( int i{}; i <= 254; ++i ) {
				key_t* key = &m_keys[ i ];
				key->down = ( ( GetAsyncKeyState( i ) & 0x8000 ) != 0 );
				key->pressed = false;
				key->oldtick = key->tick = g_winapi.GetTickCount( );
			}
			m_first_update = false;
			return;
		}

		for( int i{}; i <= 254; ++i ) {
			key_t* key = &m_keys[ i ];

			key->pressed = false;

			bool is_down = ( ( GetAsyncKeyState( i ) & 0x8000 ) != 0 ) || key->down;

			if( is_down && key->tick > key->oldtick ) {
				key->oldtick = key->tick;
				key->pressed = true;
			}
			else if( is_down && !key->down ) {
				key->oldtick = key->tick = g_winapi.GetTickCount( );
				key->pressed = true;
			}

			key->down = is_down;
		}
	}

	__forceinline bool IsCursorInBounds( int x, int y, int x2, int y2 ) const {
		return m_mouse.x > x && m_mouse.y > y && m_mouse.x < x2 && m_mouse.y < y2;
	}

	__forceinline bool IsCursorInRect( Rect area ) const {
		return IsCursorInBounds( area.x, area.y, area.x + area.w, area.y + area.h );
	}

	__forceinline void SetDown( int vk ) {
		key_t* key = &m_keys[ vk ];

		key->down = true;
		key->tick = g_winapi.GetTickCount( );
	}

	__forceinline void SetUp( int vk ) {
		key_t* key = &m_keys[ vk ];
		key->down    = false;
	}

	__forceinline bool GetKeyState( int vk ) {
		if( vk == -1 )
			return false;

		return ( ( GetAsyncKeyState( vk ) & 0x8000 ) != 0 ) || m_keys[ vk ].down;
	}

	__forceinline bool GetKeyState( Keybind& k ) {
		return k.get_status( );
	}

	__forceinline bool GetKeyState( Keybind* k ) {
		return k ? k->get_status( ) : false;
	}

	// key was pressed.
	__forceinline bool GetKeyPress( int vk ) {
		if( vk == -1 )
			return false;

		key_t* key = &m_keys[ vk ];
		return key->pressed;
	}
};

extern GUI	 g_gui;
extern Input g_input;