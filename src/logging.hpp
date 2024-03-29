#pragma once

#include <boost/core/demangle.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <string_view>
#include <typeinfo>

enum SeverityLevel { debug,
                     info,
                     warning,
                     error,
                     fatal };

std::ostream& operator<<( std::ostream& stream, SeverityLevel level );

#define LOG( severity ) BOOST_LOG_SEV( logger, SeverityLevel::severity )

namespace logging {
    using Logger = boost::log::sources::severity_logger< SeverityLevel >;

    Logger makeLogger( std::string_view name );
}// namespace logging

#define DEFINE_LOGGER( class_name ) static logging::Logger logger = ::logging::makeLogger( ::boost::core::demangle( typeid( class_name ).name() ) )
