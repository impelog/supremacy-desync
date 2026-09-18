#include "includes.h"

namespace render {
	Font menu;;
	Font menu_shade;;
	Font esp;;
	Font esp_small;;
	Font hud;;
	Font cs;;
	Font indicator;;
	Font warning_icon;;
}

void render::init( ) {
	menu = Font( XOR( "Tahoma" ), 12, FW_NORMAL, FONTFLAG_NONE );
	menu_shade = Font( XOR( "Tahoma" ), 12, FW_NORMAL, FONTFLAG_DROPSHADOW );
	esp = Font( XOR( "Verdana" ), 12, FW_BOLD, FONTFLAG_DROPSHADOW );
	esp_small = Font( XOR( "Small Fonts" ), 8, FW_NORMAL, FONTFLAG_OUTLINE );
	hud = Font( XOR( "Tahoma" ), 16, FW_NORMAL, FONTFLAG_ANTIALIAS );
	cs = Font( XOR( "Counter-Strike" ), 28, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
	indicator = Font( XOR( "Verdana" ), 26, FW_BOLD, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
	warning_icon = Font( XOR( "Calibri" ), 25, FW_BOLD, FONTFLAG_ANTIALIAS );
}

bool render::WorldToScreen( const vec3_t& world, vec2_t& screen ) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix( );

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[ 3 ][ 0 ] * world.x + matrix[ 3 ][ 1 ] * world.y + matrix[ 3 ][ 2 ] * world.z + matrix[ 3 ][ 3 ];
	if ( w < 0.001f )
		return false;

	// calculate x and y.
	screen.x = matrix[ 0 ][ 0 ] * world.x + matrix[ 0 ][ 1 ] * world.y + matrix[ 0 ][ 2 ] * world.z + matrix[ 0 ][ 3 ];
	screen.y = matrix[ 1 ][ 0 ] * world.x + matrix[ 1 ][ 1 ] * world.y + matrix[ 1 ][ 2 ] * world.z + matrix[ 1 ][ 3 ];

	screen /= w;

	// calculate screen position.
	screen.x = ( g_cl.m_width / 2 ) + ( screen.x * g_cl.m_width ) / 2;
	screen.y = ( g_cl.m_height / 2 ) - ( screen.y * g_cl.m_height ) / 2;

	return true;
}

void render::line( vec2_t v0, vec2_t v1, Color color ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawLine( v0.x, v0.y, v1.x, v1.y );
}

void render::line( int x0, int y0, int x1, int y1, Color color ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawLine( x0, y0, x1, y1 );
}

void render::rect( int x, int y, int w, int h, Color color ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawOutlinedRect( x, y, x + w, y + h );
}

void render::rect_filled( int x, int y, int w, int h, Color color ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawFilledRect( x, y, x + w, y + h );
}

void render::rect_filled_fade( int x, int y, int w, int h, Color color, int a1, int a2 ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawFilledRectFade( x, y, x + w, y + h, a1, a2, false );
}

void render::rect_outlined( int x, int y, int w, int h, Color color, Color color2 ) {
	rect( x, y, w, h, color );
	rect( x - 1, y - 1, w + 2, h + 2, color2 );
	rect( x + 1, y + 1, w - 2, h - 2, color2 );
}

void render::circle( int x, int y, int radius, int segments, Color color ) {
	static int texture = g_csgo.m_surface->CreateNewTextureID( true );

	g_csgo.m_surface->DrawSetTextureRGBA( texture, &colors::white, 1, 1 );
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawSetTexture( texture );

	std::vector< Vertex > vertices{};

	float step = math::pi_2 / segments;
	for ( float i{ 0.f }; i < math::pi_2; i += step )
		vertices.emplace_back( vec2_t{ x + ( radius * std::cos( i ) ), y + ( radius * std::sin( i ) ) } );

	g_csgo.m_surface->DrawTexturedPolygon( vertices.size( ), vertices.data( ) );
}

void render::circle_outline( int x, int y, int radius, int segments, Color color ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawOutlinedCircle( x, y, radius, segments );
}

void render::circle_arc( int x, int y, int radius, float start_angle, float end_angle, int segments, Color color ) {
	float total = end_angle - start_angle;
	if ( total <= 0.f )
		return;

	float step = total / ( float )segments;
	vec2_t prev = { ( float )x + ( ( float )radius * std::cos( start_angle ) ), ( float )y + ( ( float )radius * std::sin( start_angle ) ) };

	for ( float i{ start_angle + step }; i <= end_angle + 0.001f; i += step ) {
		vec2_t next = { ( float )x + ( ( float )radius * std::cos( i ) ), ( float )y + ( ( float )radius * std::sin( i ) ) };
		render::line( prev.x, prev.y, next.x, next.y, color );
		prev = next;
	}
}
void render::gradient( int x, int y, int w, int h, Color color1, Color color2 ) {
	g_csgo.m_surface->DrawSetColor( color1 );
	g_csgo.m_surface->DrawFilledRectFade( x, y, x + w, y + h, color1.a( ), 0, false );

	g_csgo.m_surface->DrawSetColor( color2 );
	g_csgo.m_surface->DrawFilledRectFade( x, y, x + w, y + h, 0, color2.a( ), false );
}

void render::sphere( vec3_t origin, float radius, float angle, float scale, Color color ) {
	std::vector< Vertex > vertices{};

	// compute angle step for input radius and precision.
	float step = ( 1.f / radius ) + math::deg_to_rad( angle );

	for ( float lat{}; lat < ( math::pi * scale ); lat += step ) {
		// reset.
		vertices.clear( );

		for ( float lon{}; lon < math::pi_2; lon += step ) {
			vec3_t point{
				origin.x + ( radius * std::sin( lat ) * std::cos( lon ) ),
				origin.y + ( radius * std::sin( lat ) * std::sin( lon ) ),
				origin.z + ( radius * std::cos( lat ) )
			};

			vec2_t screen;
			if ( WorldToScreen( point, screen ) )
				vertices.emplace_back( screen );
		}

		if ( vertices.empty( ) )
			continue;

		g_csgo.m_surface->DrawSetColor( color );
		g_csgo.m_surface->DrawTexturedPolyLine( vertices.size( ), vertices.data( ) );
	}
}

void render::world_circle( vec3_t origin, float radius, Color color, Color color_fill ) {
	vec2_t center;
	if ( !WorldToScreen( origin + vec3_t( 0.f, 0.f, 1.5f ), center ) )
		return;

	static int texture = g_csgo.m_surface->CreateNewTextureID( true );
	unsigned char buffer[ 4 ] = { 255, 255, 255, 255 };
	g_csgo.m_surface->DrawSetTextureRGBA( texture, buffer, 1, 1 );

	vec2_t prev_screen;
	bool has_prev = false;
	float step = ( math::pi_2 ) / 64.f;

	for ( float a = 0.f; a <= math::pi_2 + step; a += step ) {
		vec3_t world_point = {
			origin.x + ( radius * std::cos( a ) ),
			origin.y + ( radius * std::sin( a ) ),
			origin.z + 1.5f
		};

		vec2_t screen_point;
		if ( WorldToScreen( world_point, screen_point ) ) {
			if ( has_prev ) {
				Vertex verts[ 3 ] = {
					Vertex{ center },
					Vertex{ screen_point },
					Vertex{ prev_screen }
				};

				g_csgo.m_surface->DrawSetColor( color_fill );
				g_csgo.m_surface->DrawSetTexture( texture );
				g_csgo.m_surface->DrawTexturedPolygon( 3, verts );

				render::line( prev_screen.x, prev_screen.y, screen_point.x, screen_point.y, color );
			}

			prev_screen = screen_point;
			has_prev = true;
		}
		else {
			has_prev = false;
		}
	}
}

Vertex render::RotateVertex( const vec2_t& p, const Vertex& v, float angle ) {
	// convert theta angle to sine and cosine representations.
	float c = std::cos( math::deg_to_rad( angle ) );
	float s = std::sin( math::deg_to_rad( angle ) );

	return {
		p.x + ( v.m_pos.x - p.x ) * c - ( v.m_pos.y - p.y ) * s,
		p.y + ( v.m_pos.x - p.x ) * s + ( v.m_pos.y - p.y ) * c
	};
}

void render::Font::string( int x, int y, Color color, const std::string& text, StringFlags_t flags /*= render::ALIGN_LEFT */ ) {
	wstring( x, y, color, util::MultiByteToWide( text ), flags );
}

void render::Font::string( int x, int y, Color color, const std::stringstream& text, StringFlags_t flags /*= render::ALIGN_LEFT */ ) {
	wstring( x, y, color, util::MultiByteToWide( text.str( ) ), flags );
}

void render::Font::wstring( int x, int y, Color color, const std::wstring& text, StringFlags_t flags /*= render::ALIGN_LEFT */ ) {
	int w, h;

	g_csgo.m_surface->GetTextSize( m_handle, text.c_str( ), w, h );
	g_csgo.m_surface->DrawSetTextFont( m_handle );
	g_csgo.m_surface->DrawSetTextColor( color );

	if ( flags & ALIGN_RIGHT )
		x -= w;
	if ( flags & render::ALIGN_CENTER )
		x -= w / 2;

	g_csgo.m_surface->DrawSetTextPos( x, y );
	g_csgo.m_surface->DrawPrintText( text.c_str( ), ( int )text.size( ) );
}

render::FontSize_t render::Font::size( const std::string& text ) {
	return wsize( util::MultiByteToWide( text ) );
}

render::FontSize_t render::Font::wsize( const std::wstring& text ) {
	FontSize_t res;
	g_csgo.m_surface->GetTextSize( m_handle, text.data( ), res.m_width, res.m_height );
	return res;
}