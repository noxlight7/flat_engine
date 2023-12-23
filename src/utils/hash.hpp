#pragma once

#include "../entry.hpp"

template<typename _ty, _ty _value>
struct CompileTimeHolder {
	constexpr static _ty m_value = _value;
};

namespace fnv_edited {
	using value_type = unsigned long long;

	constexpr auto k_seed = 0x45c3ull;
	constexpr auto k_prime = 0x1193ull;

	// run time
	ALWAYS_INLINE value_type rt( const char *txt ) {
		auto entry = k_seed;

		for ( size_t i = 0; i < strlen( txt ); ++i ) {
			entry ^= txt[ i ];
			entry *= k_prime;
		}

		return entry;
	}

	// compiled time
	ALWAYS_INLINE constexpr value_type ct( const char *txt, value_type entry = k_seed ) {
		return !*txt ? entry 
			: ct( txt + 1, 
				static_cast< value_type >( 1ull * ( entry 
					^ static_cast< uint8_t >( *txt ) ) 
					* k_prime ) );
	}
}

#define HASH( txt ) static_cast< decltype( fnv_edited::ct( txt ) ) >( CompileTimeHolder<decltype( fnv_edited::ct( txt ) ), fnv_edited::ct( txt )>::m_value )