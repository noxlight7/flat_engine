#include "../entry.hpp"

class IEntityList {
	using entries_t = std::unordered_map< IHandleEntity::handle_t, IHandleEntity >;
public:
	ALWAYS_INLINE IEntityList( ) = default;

	void CreateEntity( int serial, int entry );
protected:
	entries_t	m_entries{ };
	size_t		m_high_serial{ };
};

inline const auto g_pEntityList = std::make_shared< IEntityList >( );