#pragma once

struct color_with_ex_t {
	unsigned char r, g, b;
	signed char exponent;
};

struct dlight_t {
	int flags;
	vec3_t origin;
	float radius;
	color_with_ex_t color;
	float die;
	float decay;
	float minlight;
	int key;
	int style;
	vec3_t m_Direction;
	float m_InnerAngle;
	float m_OuterAngle;
};

class IVEffects {
public:
	virtual int Draw_DecalIndexFromName( char* name ) = 0;
	virtual void DecalShoot( int textureIndex, int entity, const model_t* model, const vec3_t& model_origin, const ang_t& model_angles, const vec3_t& position, const vec3_t* saxis, int flags ) = 0;
	virtual void DecalColorShoot( int textureIndex, int entity, const model_t* model, const vec3_t& model_origin, const ang_t& model_angles, const vec3_t& position, const vec3_t* saxis, int flags, int rgbaColor ) = 0;
	virtual void PlayerDecalShoot( void* material, void* userdata, int entity, const model_t* model, const vec3_t& model_origin, const ang_t& model_angles, const vec3_t& position, const vec3_t* saxis, int flags, int rgbaColor ) = 0;
	virtual dlight_t* CL_AllocDlight( int key ) = 0;
	virtual dlight_t* CL_AllocElight( int key ) = 0;
	virtual int CL_GetActiveDLights( dlight_t* pList[ 32 ] ) = 0;
	virtual const char* Draw_DecalNameFromIndex( int nIndex ) = 0;
	virtual dlight_t* GetElightByKey( int key ) = 0;
};
