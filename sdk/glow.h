#pragma once

#define END_OF_FREE_LIST	-1
#define ENTRY_IN_USE		-2

enum EGlowRenderStyle : int {
	GLOWRENDERSTYLE_DEFAULT = 0,
	GLOWRENDERSTYLE_RIMGLOW3D,
	GLOWRENDERSTYLE_EDGE_HIGHLIGHT,
	GLOWRENDERSTYLE_EDGE_HIGHLIGHT_PULSE,
	GLOWRENDERSTYLE_COUNT
};

struct GlowObjectDefinition_t {
	[[nodiscard]] inline bool IsEmpty( ) const {
		return nNextFreeSlot != ENTRY_IN_USE;
	}

	int nNextFreeSlot; // 0x00
	Entity *pEntity; // 0x04
	float arrColor[ 4 ]; // 0x08
	bool bAlphaCappedByRenderAlpha; // 0x18
	float flAlphaFunctionOfMaxVelocity; // 0x1C
	float flBloomAmount; // 0x20
	float flPulseOverdrive; // 0x24
	bool bRenderWhenOccluded; // 0x28
	bool bRenderWhenUnoccluded; // 0x29
	bool bFullBloomRender; // 0x2A
	int iFullBloomStencilTestValue; // 0x2C
	int nRenderStyle; // 0x30
	int nSplitScreenSlot; // 0x34
};
static_assert( sizeof( GlowObjectDefinition_t ) == 0x38 );

struct GlowBoxObject_t {
	vec3_t vecPosition; // 0x00
	ang_t angOrientation; // 0x0C
	vec3_t vecMins; // 0x18
	vec3_t vecMaxs; // 0x24
	float flBirthTimeIndex; // 0x30
	float flTerminationTimeIndex; // 0x34
	Color colBox; // 0x38
};
static_assert( sizeof( GlowBoxObject_t ) == 0x3C );

class CGlowObjectManager {
public:
	CUtlVector< GlowObjectDefinition_t >	m_object_definitions;
	int										m_first_free_slot;
	CUtlVector< GlowBoxObject_t >			m_glow_definitions;
};
