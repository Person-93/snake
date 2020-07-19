#include "logging.hpp"
#include "version.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <atomic>
#include <csignal>
#include "ImGuiWrapper.hpp"
#include "Snake.hpp"
#include <random>
#include <chrono>
#include <GLFW/glfw3.h>

std::atomic_bool shouldRun = true;

extern "C" void signalHandler( int ) { shouldRun = false; }

std::pair<int, int> randomSpot() {
    std::mt19937                    generator{ std::random_device{}() };
    std::uniform_int_distribution<> distribution{ 0, 99 };
    return { distribution( generator ), distribution( generator ) };
}

void fillLocation( int x, int y, ImColor color, ImDrawList* drawList, float boxSize );

int main( int argc, char* argv[] ) {
    logging::Logger logger = logging::makeLogger( "Main" );

    try {
        std::signal( SIGTERM, signalHandler );
        LOG( info ) << "Running version " << version::longVersion();
        ImGuiWrapper imGuiWrapper{ "Snake" };
        WindowConfig config{ "game", nullptr, ImGuiWindowFlags_NoDecoration };

        ImColor snakeColor{ 143, 255, 102 };
        ImColor foodColor{ 101, 176, 169 };
        Snake   snake{ Snake::Direction::up, 50, 95, 3 };
        auto    foodLocation = randomSpot();

        auto lastAdvance = std::chrono::steady_clock::now();

        while ( !imGuiWrapper.shouldClose() && shouldRun ) {
            auto  f              = imGuiWrapper.frame();
            auto  size           = ImGui::GetIO().DisplaySize;
            float gameWindowSize = size.y * .8f;
            ImGui::SetNextWindowPos( { size.x / 2, size.y / 2 }, 0, { 0.5f, 0.5f } );
            ImGui::SetNextWindowSize( { gameWindowSize, gameWindowSize } );
            imGuiWrapper.window( config, [ & ] {
                float boxSize = ImGui::GetWindowHeight() / 100;
                auto  fill    = snake.GetFilledIn();
                if ( fill.at( 0 ) == foodLocation ) {
                    snake.Grow( 1 );
                    foodLocation = randomSpot();
                }
                auto drawList = ImGui::GetWindowDrawList();
                fillLocation( foodLocation.first, foodLocation.second, foodColor, drawList, boxSize );
                for ( const auto& space: fill ) {
                    fillLocation( space.first, space.second, snakeColor, drawList, boxSize );
                }
            } );

            if ( std::chrono::steady_clock::now() - lastAdvance >= std::chrono::milliseconds{ 200 } ) {
                lastAdvance = std::chrono::steady_clock::now();
                snake.Advance( 1 );
            }

            if ( imGuiWrapper.GetKey( GLFW_KEY_UP ) == GLFW_PRESS ) snake.Up();
            else if ( imGuiWrapper.GetKey( GLFW_KEY_DOWN ) == GLFW_PRESS ) snake.Down();
            else if ( imGuiWrapper.GetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS ) snake.Right();
            else if ( imGuiWrapper.GetKey( GLFW_KEY_LEFT ) == GLFW_PRESS ) snake.Left();
        }
    }
    catch ( ... ) {
        LOG( fatal ) << boost::current_exception_diagnostic_information( true );
        return -1;
    }

    return 0;
}

void fillLocation( int x, int y, ImColor color, ImDrawList* drawList, float boxSize ) {
    static auto p = ImGui::GetCursorScreenPos();
    drawList->AddRectFilled( { p.x + x * boxSize, p.y + y * boxSize },
                             { p.x + ( x + 1.f ) * boxSize, p.y + ( y + 1.f ) * boxSize },
                             color );
}
