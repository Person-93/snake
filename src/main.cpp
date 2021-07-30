#include "ImGuiWrapper.hpp"
#include "Snake.hpp"
#include "logging.hpp"
#include "version.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <atomic>
#include <boost/exception/diagnostic_information.hpp>
#include <chrono>
#include <csignal>
#include <optional>
#include <random>


std::atomic_bool shouldRun = true;

extern "C" void signalHandler( int ) { shouldRun = false; }

std::pair<int, int> randomSpot() {
    std::mt19937                    generator{ std::random_device{}() };
    std::uniform_int_distribution<> distribution{ 0, 99 };
    return { distribution( generator ), distribution( generator ) };
}

void fillLocation( int x, int y, ImColor color, ImDrawList* drawList, float boxSize, const ImVec2& screenCursorPos );

void HandleInput( ImGuiWrapper& imGuiWrapper, Snake& snake );

logging::Logger logger = logging::makeLogger( "Main" );

int main( int argc, char* argv[] ) {
    try {
        std::signal( SIGTERM, signalHandler );
        LOG( info ) << "Running version " << version::longVersion();
        ImGuiWrapper imGuiWrapper{ "Snake" };
        WindowConfig config{ "game", nullptr, ImGuiWindowFlags_NoDecoration };
        WindowConfig statusWindowConfig{ "status", nullptr, ImGuiWindowFlags_NoDecoration };

        ImColor snakeColor{ 143, 255, 102 };
        ImColor foodColor{ 101, 176, 169 };
        Snake   snake{ Snake::Direction::up, 50, 95, 3 };
        auto    foodLocation = randomSpot();

        auto           lastAdvance       = std::chrono::steady_clock::now();
        bool           gameRunning       = false;
        unsigned short userFriendlySpeed = 1;
        unsigned short actualSpeed;

        const auto CalculateSpeed = [ & ] {
            const std::array<unsigned short, 10> speeds{ 0, 200, 150, 100, 80, 50, 50, 30, 20, 10 };
            actualSpeed = speeds.at( userFriendlySpeed );
        };
        CalculateSpeed();

        while ( !imGuiWrapper.shouldClose() && shouldRun ) {
            auto  f              = imGuiWrapper.frame();
            auto  size           = ImGui::GetIO().DisplaySize;
            float gameWindowSize = size.y * .8f;

            float statusWindowSize = size.y * .05f;

            ImGui::SetNextWindowPos( { size.x / 2, size.y / 20 }, 0, { 0.5f, 0.5f } );
            ImGui::SetNextWindowSize( { gameWindowSize, statusWindowSize } );
            imGuiWrapper.window( statusWindowConfig, [ & ] {
                {
                    auto d     = imGuiWrapper.disableControls( gameRunning );
                    auto style = ImGui::GetStyle();
                    if ( ImGui::Button( "Start" )) {
                        gameRunning = true;
                        snake       = Snake{ Snake::Direction::up, 50, 95, 3 };
                    }
                    ImGui::SameLine();
                    ImGui::Text( "Speed: %d", userFriendlySpeed );
                    ImGui::SameLine( 0, style.ItemInnerSpacing.x );
                    {
                        auto e = imGuiWrapper.disableControls( userFriendlySpeed >= 9 );
                        if ( ImGui::Button( "+" )) {
                            ++userFriendlySpeed;
                            CalculateSpeed();
                        }
                    }
                    ImGui::SameLine( 0, style.ItemInnerSpacing.x );
                    auto e = imGuiWrapper.disableControls( userFriendlySpeed <= 1 );
                    if ( ImGui::Button( "-" )) {
                        --userFriendlySpeed;
                        CalculateSpeed();
                    }
                }
            } );


            ImGui::SetNextWindowPos( { size.x / 2, size.y / 2 }, 0, { 0.5f, 0.5f } );
            ImGui::SetNextWindowSize( { gameWindowSize, gameWindowSize } );
            imGuiWrapper.window( config, [ & ] {
                float boxSize = ImGui::GetWindowHeight() / 100;
                auto  fill    = snake.GetFilledIn();

                if ( gameRunning ) {
                    auto      head = fill.at( 0 );
                    if ( head == foodLocation ) {
                        snake.Grow( 1 );
                        foodLocation = randomSpot();
                        LOG( debug ) << "Food spawning at " << foodLocation.first << ", " << foodLocation.second;
                    }
                    // check if head is colliding with wall
                    if ( head.first >= 100 || head.first < 0 || head.second >= 100 || head.second < 0 ) {
                        gameRunning = false;
                        LOG( debug ) << "Head collided with wall at " << head.first << ", " << head.second;
                    }
                    // check if head is colliding with tail
                    for ( int i    = 1; i < fill.size(); ++i ) {
                        if ( fill[ i ] == head ) {
                            gameRunning = false;
                            LOG( debug ) << "Head collided with tail at " << head.first << ", " << head.second;
                            break;
                        }
                    }
                }

                auto cursorPos = ImGui::GetCursorScreenPos();
                auto drawList  = ImGui::GetWindowDrawList();
                fillLocation( foodLocation.first, foodLocation.second, foodColor, drawList, boxSize, cursorPos );
                for ( const auto& space: fill ) {
                    fillLocation( space.first, space.second, snakeColor, drawList, boxSize, cursorPos );
                }
            } );

            if ( gameRunning ) {
                if ( std::chrono::steady_clock::now() - lastAdvance >= std::chrono::milliseconds{ actualSpeed } ) {
                    lastAdvance = std::chrono::steady_clock::now();
                    snake.Advance( 1 );
                }
                HandleInput( imGuiWrapper, snake );
            }
        }
    }
    catch ( ... ) {
        LOG( fatal ) << boost::current_exception_diagnostic_information( true );
        return -1;
    }

    return 0;
}

void fillLocation( int x, int y, ImColor color, ImDrawList* drawList, float boxSize, const ImVec2& screenCursorPos ) {
    drawList->AddRectFilled( { screenCursorPos.x + x * boxSize, screenCursorPos.y + y * boxSize },
                             { screenCursorPos.x + ( x + 1.f ) * boxSize, screenCursorPos.y + ( y + 1.f ) * boxSize },
                             color );
}

void HandleInput( ImGuiWrapper& imGuiWrapper, Snake& snake ) {
    enum class Command {
        none,
        right,
        left,
        up,
        down,
    };
    static std::optional< Command > deferredCommand{};
    Snake::Response response;
    if ( deferredCommand.has_value() ) {
        switch ( *deferredCommand ) {
            case Command::up:
                response = snake.Up();
                break;
            case Command::down:
                response = snake.Down();
                break;
            case Command::right:
                response = snake.Right();
                break;
            case Command::left:
                response = snake.Left();
                break;
            case Command::none:// this should never happen
                throw std::runtime_error{ "Empty deferred command" };
        }
        if ( response == Snake::Response::process )
            deferredCommand.reset();
        return;
    }

    Command command =
            imGuiWrapper.GetKey( GLFW_KEY_UP ) == GLFW_PRESS      ? Command::up
            : imGuiWrapper.GetKey( GLFW_KEY_DOWN ) == GLFW_PRESS  ? Command::down
            : imGuiWrapper.GetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS ? Command::right
            : imGuiWrapper.GetKey( GLFW_KEY_LEFT ) == GLFW_PRESS  ? Command::left
                                                                  : Command::none;

    switch ( command ) {
        case Command::none:
            return;
        case Command::right:
            response = snake.Right();
            break;
        case Command::left:
            response = snake.Left();
            break;
        case Command::up:
            response = snake.Up();
            break;
        case Command::down:
            response = snake.Down();
            break;
    }

    switch ( response ) {
        case Snake::Response::ignore:// TODO add sound effect for ignored command
        case Snake::Response::process:
            break;
        case Snake::Response::defer:
            deferredCommand = command;
            LOG( debug ) << "Command was deferred";
            break;
    }
}
