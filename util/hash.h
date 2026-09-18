#pragma once

// todo - dex; check arg type for macros?

// hash type.
using hash32_t = uint32_t;

// compile-time hash.
#define HASH( str )                                     \
    []() {                                              \
        constexpr hash32_t out{ FNV1a::get( str ) };    \
                                                        \
        return out;                                     \
    }()

// compile-time const hash.
#define CONST_HASH( str ) FNV1a::get( str )

class FNV1a {
private:
    // fnv1a constants.
    enum : uint32_t {
        PRIME = 0x1000193u,
        BASIS = 0x811C9DC5u
    };

    static __forceinline constexpr size_t ct_strlen( const char *str, bool include_nullchar = false ) {
        if( !str )
            return 0;

        size_t out{};

        while( str[ out ] != '\0' )
            ++out;

        if( include_nullchar )
            ++out;

        return out;
    }

public:
    static __forceinline constexpr hash32_t get( const uint8_t *data, const size_t len ) {
        hash32_t out{ BASIS };

        for( size_t i{}; i < len; ++i )
            out = ( out ^ data[ i ] ) * PRIME;

        return out;
    }

    static __forceinline constexpr hash32_t get( const char *str ) {
        if( !str )
            return 0;

        hash32_t out{ BASIS };
        size_t   len{ ct_strlen( str ) };

        for( size_t i{}; i < len; ++i )
            out = ( out ^ str[ i ] ) * PRIME;

        return out;
    }

    // hash c++-style string ( run-time only ).
    static __forceinline hash32_t get( const std::string &str ) {
        return get( (uint8_t *)str.c_str( ), str.size( ) );
    }
};