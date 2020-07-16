include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library ( version version.hpp ${version_file} )

add_library ( logging logging.cpp logging.hpp )
target_link_libraries ( logging PUBLIC Boost::log Threads::Threads )

add_executable ( snake main.cpp )
target_link_libraries ( snake PUBLIC Boost::boost logging version )
