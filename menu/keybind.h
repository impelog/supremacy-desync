#pragma once

#define KEYBIND_X_OFFSET      20
#define KEYBIND_BOX_HEIGHT    20
#define KEYBIND_ITEM_X_OFFSET 10
#define KEYBIND_POPUP_HEIGHT  18

class Keybind : public Element {
	friend class GUI;
	friend class Config;

	using ToggleCallback_t = void( *)( );

public:
	enum Mode : size_t {
		ALWAYS = 0,
		HOLD,
		TOGGLE
	};

public:
	__forceinline Keybind( ) : m_key{ -1 }, m_mode{ HOLD }, m_active{ false }, m_open{ false }, m_anim_height{ 0.f }, m_label{}, m_set{}, m_old_set{}, m_toggle{} {
		m_flags  = ElementFlags::DRAW | ElementFlags::CLICK | ElementFlags::ACTIVE | ElementFlags::SAVE;
		m_type   = ElementTypes::KEYBIND;
		m_base_h = m_h = 15 + KEYBIND_BOX_HEIGHT;
	}

	__forceinline void setup( const std::string& label, const std::string& file_id, int key = -1, size_t mode = HOLD ) {
		m_label   = label;
		m_file_id = file_id;
		m_key     = key;
		m_mode    = mode;
	}

	__forceinline void SetToggleCallback( ToggleCallback_t t ) {
		m_toggle = t;
	}

	__forceinline void set( int key ) {
		bool changed = m_key != key;

		m_key = key;

		if( changed && m_callback )
			m_callback( );
	}

	__forceinline void set_mode( size_t mode ) {
		m_mode = mode;
	}

	__forceinline int get( ) const {
		return m_key;
	}

	__forceinline size_t get_mode( ) const {
		return m_mode;
	}

	bool get_status( );
	operator bool( );

protected:
	int			     m_key;
	size_t           m_mode;
	bool             m_active;
	bool             m_open;
	float            m_anim_height;
	std::string      m_label;
	bool		     m_set;
	bool		     m_old_set;
	ToggleCallback_t m_toggle;

protected:
	void draw( ) override;
	void think( ) override;
	void click( ) override;
};