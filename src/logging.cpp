#include "logging.hpp"
#include <boost/log/expressions.hpp>
#include <iostream>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

std::ostream& operator<<( std::ostream& stream, SeverityLevel level ) {
    switch ( level ) {
        case debug: stream << "DEBUG";
            break;
        case info: stream << "INFO";
            break;
        case warning: stream << "WARNING";
            break;
        case error: stream << "ERROR";
            break;
        case fatal: stream << "FATAL";
            break;
    }

    return stream;
}

namespace logging {
    namespace expressions = boost::log::expressions;

    auto standardSink = []() {
        auto sink = boost::log::add_console_log( std::cout );
        sink->set_formatter(
                expressions::stream
                        << expressions::format_date_time<boost::posix_time::ptime>( "TimeStamp",
                                                                                    "%Y-%m-%d %H:%M:%S.%f" )
                        << "  ["
                        << expressions::attr<boost::log::attributes::current_thread_id::value_type>( "ThreadID" )
                        << "]  " << expressions::attr<SeverityLevel>( "Severity" )
                        << "  " << expressions::attr<std::string_view>( "ClassName" )
                        << "  " << expressions::message
                           );
        sink->set_filter( expressions::attr<SeverityLevel>( "Severity" ) < SeverityLevel::error );
        return sink;
    }();

    auto errorSink = []() {
        auto sink = boost::log::add_console_log( std::cerr );
        sink->set_formatter(
                expressions::stream
                        << "\n================================================================================\n"
                        << expressions::format_date_time<boost::posix_time::ptime>( "TimeStamp",
                                                                                    "%Y-%m-%d %H:%M:%S.%f" )
                        << "  ["
                        << expressions::attr<boost::log::attributes::current_thread_id::value_type>( "ThreadID" )
                        << "]  " << expressions::attr<SeverityLevel>( "Severity" )
                        << "  " << expressions::attr<std::string_view>( "ClassName" )
                        << "\n" << expressions::message
                        << "\n================================================================================\n"
                           );
        sink->set_filter( expressions::attr<SeverityLevel>( "Severity" ) >= SeverityLevel::error );
        return sink;
    }();

    Logger makeLogger( std::string_view name ) {
        Logger logger;
        logger.add_attribute( "TimeStamp", boost::log::attributes::local_clock{} );
        logger.add_attribute( "ThreadID", boost::log::attributes::current_thread_id{} );
        logger.add_attribute( "ClassName", boost::log::attributes::constant<std::string_view>( name ));
        return logger;
    }
}
