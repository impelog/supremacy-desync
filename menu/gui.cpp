#include "includes.h"

GUI	  g_gui{};;
Input g_input{};;

void GUI::think( ) {
	m_open = false;

	// update keyboard input.
	g_input.update( );

	// run bound key callbacks.
	if( !m_forms.empty( ) ) {
		bool done = false;

		for( const auto& f : m_forms ) {
			if( !f || f->m_tabs.empty( ) )
				continue;

			for( const auto& t : f->m_tabs ) {
				if( !t || t->m_elements.empty( ) )
					continue;

				for( const auto& e : t->m_elements ) {
					if( !e || e->m_type != ElementTypes::KEYBIND )
						continue;

					Keybind* k = ( Keybind* )e;

					if( k->m_set || k->m_old_set )
						continue;

					if( k->m_key != -1 && k->m_mode == Keybind::TOGGLE && g_input.GetKeyPress( k->get( ) ) ) {
						k->m_active = !k->m_active;
						if( k->m_toggle )
							k->m_toggle( );
					}	
				}
			}
		}
	}

	// sort forms based on last touched tick.
	std::sort( m_forms.begin( ), m_forms.end( ), []( Form* a, Form* b ) {
		return a->m_tick > b->m_tick;
	} );

	// update form open states
	// iterage forms.
	for( const auto& f : m_forms ) {
		if( !f )
			continue;

		if( f->m_key != -1 && g_input.GetKeyPress( f->m_key ) )
			f->toggle( );

		if( !m_open && f->m_open )
			m_open = true;

		// if closed, deactivate any active elements.
		if( !f->m_open && f->m_active_element )
			f->m_active_element = nullptr;
	}

	// update element visibility.
	for( const auto& f : m_forms ) {
		if( !f || !f->m_active_tab || f->m_active_tab->m_elements.empty( ) )
			continue;

		// reset.
		f->m_active_tab->m_element_offset.fill( ELEMENT_BORDER );

		for( auto& e : f->m_active_tab->m_elements ) {
			// default to true.
			e->m_show = true;

			// this element has show callbacks.
			if( e->m_show_callbacks.size( ) ) {

				// iterate all callbacks.
				for( auto& cb : e->m_show_callbacks ) {

					// if one callback evaluates to false.
					// set show to false.
					if( !cb( ) )
						e->m_show = false;
				}
			}

			// this element will be shown, fix its y coordinate.
			if( e->m_show ) {
				// slight correction.
				if( !e->m_use_label )
					f->m_active_tab->m_element_offset[ e->m_col ] -= ( ELEMENT_DISTANCE - ELEMENT_COMBINED_DISTANCE );

				e->m_pos.y = f->m_active_tab->m_element_offset[ e->m_col ];

				// set offset for next element.
				f->m_active_tab->m_element_offset[ e->m_col ] += ( ELEMENT_COMBINED_DISTANCE + e->m_base_h );
			}
		}
	}

	bool pressed  = g_input.GetKeyPress( VK_LBUTTON );
	bool rpressed = g_input.GetKeyPress( VK_RBUTTON );
	bool down     = g_input.GetKeyState( VK_LBUTTON );

	g_csgo.m_input_system->GetCursorPosition( &g_input.m_mouse.x, &g_input.m_mouse.y );

	math::clamp( g_input.m_mouse.x, 0, g_cl.m_width );
	math::clamp( g_input.m_mouse.y, 0, g_cl.m_height );

	if( m_drag_form && !down )
		m_drag_form = nullptr;

	if( m_drag_form && down && !pressed ) {
		m_drag_form->m_x = g_input.m_mouse.x - m_drag_offset.x;
		m_drag_form->m_y = g_input.m_mouse.y - m_drag_offset.y;
	}

	if( pressed || rpressed || down ) {
		for( const auto& f : m_forms ) {
			if( !f )
				continue;

			if( g_input.IsCursorInRect( f->GetFormRect( ) ) ) {
				f->m_tick = g_winapi.GetTickCount( );
				break;
			}
		}
	}

	std::sort( m_forms.begin( ), m_forms.end( ), []( Form* a, Form* b ) {
		return a->m_tick > b->m_tick;
	} );

	for( const auto& f : m_forms ) {
		if( !f  || !f->m_open )
			continue;

		bool element_input{};

		Rect cl = f->GetClientRect( );
		Rect el = f->GetElementsRect( );

		if( pressed || rpressed ) {
			if( g_input.IsCursorInRect( f->GetFormRect( ) ) ) {
				if( g_input.IsCursorInRect( cl ) ) {
					Rect tabs_area = f->GetTabsRect( );

					if( pressed && g_input.IsCursorInRect( tabs_area ) && !f->m_tabs.empty( ) ) {
						if( f->m_active_element )
							f->m_active_element = nullptr;

						for( size_t i{}; i < f->m_tabs.size( ); ++i ) {
							const auto& t = f->m_tabs[ i ];

							Rect tab{ tabs_area.x, tabs_area.y + ( i * 16 ), tabs_area.w, 16 };

							if( g_input.IsCursorInRect( tab ) )
								f->m_active_tab = t;
						}
					}
					else if( g_input.IsCursorInRect( el ) )
						element_input = true;
				}
				else if( pressed ) {
					m_drag_form = f;

					m_drag_offset.x = g_input.m_mouse.x - f->m_x;
					m_drag_offset.y = g_input.m_mouse.y - f->m_y;

					if( f->m_active_element )
						f->m_active_element = nullptr;
				}
			}
			else if( f->m_active_element )
				f->m_active_element = nullptr;
		}

		if( f->m_active_element && f->m_active_element->m_show ) {
			f->m_active_element->think( );

			Rect area{ el.x + f->m_active_element->m_pos.x, el.y + f->m_active_element->m_pos.y, f->m_active_element->m_w, f->m_active_element->m_h };

			if( element_input && ( f->m_active_element->m_flags & ElementFlags::CLICK ) ) {
				f->m_active_element->click( );

				if( f->m_active_element && ( f->m_active_element->m_flags & ElementFlags::DEACIVATE ) )
					f->m_active_element = nullptr;

				element_input = false;
			}
		}

		if( f->m_active_tab && !f->m_active_tab->m_elements.empty( ) ) {
			for( const auto& e : f->m_active_tab->m_elements ) {
				if( !e || ( f->m_active_element && e == f->m_active_element ) )
					continue;

				if( !e->m_show )
					continue;

				e->think( );

				Rect area{ el.x + e->m_pos.x, el.y + e->m_pos.y, e->m_w, e->m_h };

				if( element_input && ( e->m_flags & ElementFlags::CLICK ) && g_input.IsCursorInRect( area ) ) {
					e->click( );

					element_input = false;

					if( e->m_flags & ElementFlags::ACTIVE )
						f->m_active_element = e;
					else if( f->m_active_element )
						f->m_active_element = nullptr;
				}
			}
		}

		// we ended up clicking nothing
		// nothing matched with a known element.
		if( element_input && f->m_active_element )
			f->m_active_element = nullptr;
	}

	// do the rendering.
	draw( );
}

void GUI::draw( ) {
	// sort forms based on last touched tick ( in reverse ).
	std::sort( m_forms.begin( ), m_forms.end( ), []( Form* a, Form* b ) {
		return a->m_tick < b->m_tick;
	} );

	// iterate forms.
	for( const auto& f : m_forms ) {
		if( !f )
			continue;

		f->draw( );
	}
}