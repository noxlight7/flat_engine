#pragma once

#include "../entry.hpp"

enum eEntityType {
	OBJECT = 0
};

class IHandleEntity {
public:
	typedef unsigned handle_t;
public:
	ALWAYS_INLINE IHandleEntity( ) = default;

	ALWAYS_INLINE IHandleEntity( const handle_t &rhs ) 
			: m_handle{ rhs } {};

	ALWAYS_INLINE IHandleEntity( const IHandleEntity &rhs )
			: m_handle{ rhs.m_handle } {}

	ALWAYS_INLINE handle_t getHandle( ) const { return m_handle; }
protected:
	handle_t m_handle{};
};

class CBaseEntity : public IHandleEntity {
protected:
	int		m_health{};
	int		m_type{};
	void	*m_texture{};
};