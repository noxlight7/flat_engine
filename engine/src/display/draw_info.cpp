#include "draw_info.h"
#include "physics/physics.hpp"

SpaceObjectDrawInfo::SpaceObjectDrawInfo() : m_obj(), m_origin(), m_texture(nullptr) {
}

void SpaceObjectDrawInfo::init(LocatableObject* obj, Texture* texture, vec3 origin) {
	m_obj = obj;
	m_texture = texture;
	m_origin = origin;
}

void SpaceObjectDrawInfo::draw(){
	ObjectForm form = m_obj->getForm();
	switch (form.m_type)
	{
	case ObjectFormType::FORM_RECTANGLE:
		g_renderer->drawRectangleTextured(*form.m_data.m_rectangle, m_texture, m_origin);
		break;
	case ObjectFormType::FORM_CIRCLE:
		g_renderer->drawRectangleTextured(
			form.m_data.m_circle->m_radius * 2.f, 
			form.m_data.m_circle->m_radius * 2.f, 
			m_texture, m_origin);
		break;
	default:
		break;
	}
}

SpaceObjectDrawInfo::~SpaceObjectDrawInfo() {
	m_texture = nullptr;
}
