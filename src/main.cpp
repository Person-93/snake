#include "logging.hpp"
#include "version.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <atomic>
#include <csignal>
#include "ImGuiWrapper.hpp"

std::atomic_bool shouldRun = true;

extern "C" void signalHandler( int ) { shouldRun = false; }

int main( int argc, char* argv[] ) {
    logging::Logger logger = logging::makeLogger( "Main" );

    try {
        std::signal( SIGTERM, signalHandler );
        LOG( info ) << "Running version " << version::longVersion();
        ImGuiWrapper imGuiWrapper{ "Snake" };
        while ( !imGuiWrapper.shouldClose() && shouldRun ) {
            auto f = imGuiWrapper.frame();
        }
    }
    catch ( ... ) {
        LOG( fatal ) << boost::current_exception_diagnostic_information( true );
        return -1;
    }

    return 0;
}
