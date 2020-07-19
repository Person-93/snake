include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library ( imgui
              ImGuiWrapper.hpp
              ImGuiWrapper.cpp
              )
target_link_libraries ( imgui PUBLIC imgui_base )
target_compile_definitions ( imgui PUBLIC IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/src/imgui_config.hpp" )

add_library ( version version.hpp ${version_file} )

add_library ( logging logging.cpp logging.hpp )
target_link_libraries ( logging PUBLIC Boost::log Threads::Threads )

add_executable ( snake main.cpp Snake.cpp Snake.hpp )
target_link_libraries ( snake PUBLIC Boost::boost logging version imgui )
