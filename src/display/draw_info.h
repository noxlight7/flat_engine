#pragma once

#include "renderer.hpp"
#include "physics/base_physics.hpp"

class LocatableObject;

class SpaceObjectDrawInfo {
public:
	SpaceObjectDrawInfo();
	void init(LocatableObject* obj, Texture *texture, vec3 origin = vec3());
	void draw();
	inline void setOrigin(vec3 origin) { m_origin = origin; }
	inline vec3 getOrigin() { return m_origin; }

	inline Texture* getTexture() { return m_texture; }
	inline void setTexture(Texture* texture) { m_texture = texture; }

	~SpaceObjectDrawInfo();

protected:
	Texture* m_texture;
	LocatableObject* m_obj;
	vec3 m_origin;
};
