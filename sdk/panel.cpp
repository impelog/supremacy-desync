#include "includes.h"

void Hooks::PaintTraverse( VPANEL panel, bool repaint, bool force ) {
	hash32_t panel_hash = FNV1a::get( g_csgo.m_panel->GetName( panel ) );

	// render hack stuff.
	if ( panel_hash == HASH( "MatSystemTopPanel" ) )
		g_cl.OnPaint( );

	// don't call the original function if we want to remove the scope.
	if ( panel_hash == HASH( "HudZoom" ) && g_menu.main.visuals.noscope.get( ) )
		return;
		
	g_hooks.m_panel.GetOldMethod< PaintTraverse_t >( IPanel::PAINTTRAVERSE )( this, panel, repaint, force );
}