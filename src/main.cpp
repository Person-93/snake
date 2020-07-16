#include "logging.hpp"
#include "version.hpp"
#include <boost/exception/diagnostic_information.hpp>

int main( int argc, char* argv[] ) {
    logging::Logger logger = logging::makeLogger( "Main" );

    try {
        LOG( info ) << "Running version " << version::longVersion();
    }
    catch ( ... ) {
        LOG( fatal ) << boost::current_exception_diagnostic_information( true );
        return -1;
    }

    return 0;
}
