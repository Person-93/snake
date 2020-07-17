#pragma once

#include <boost/throw_exception.hpp>

#define IM_ASSERT( x ) \
do {                   \
    if ( !(x) ) {      \
        BOOST_THROW_EXCEPTION(std::runtime_error{std::string{"ImGui assertion failed: "} + #x }); \
    }                  \
} while ( false )
