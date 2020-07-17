add_library ( imgui_base STATIC
              imgui/imgui.h
              imgui/imgui.cpp
              imgui/imgui_draw.cpp
              imgui/imgui_widgets.cpp
              imgui/examples/imgui_impl_glfw.cpp
              imgui/examples/imgui_impl_glfw.h
              imgui/examples/imgui_impl_opengl3.cpp
              imgui/examples/imgui_impl_opengl3.h
              imgui/examples/libs/gl3w/GL/gl3w.c
              imgui/examples/libs/gl3w/GL/gl3w.h
              imgui/examples/libs/gl3w/GL/glcorearb.h
              )
target_include_directories ( imgui_base PUBLIC
                             ${CMAKE_CURRENT_LIST_DIR}/imgui
                             ${CMAKE_CURRENT_LIST_DIR}/imgui/examples/libs/gl3w
                             )
target_link_libraries ( imgui_base INTERFACE glfw )
if ( UNIX AND NOT APPLE )
    target_link_libraries ( imgui_base PRIVATE dl )
else ()
    message ( FATAL_ERROR "Imgui dependencies were not configured for this platform" )
endif ()
if ( BUILD_SHARED_LIBS )
    set_property ( TARGET imgui_base PROPERTY POSITION_INDEPENDENT_CODE ON )
endif ()
target_compile_definitions ( imgui_base PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W )

add_executable ( imgui_demo
                 imgui/imgui_demo.cpp
                 imgui/examples/example_glfw_opengl3/main.cpp
                 )
target_link_libraries ( imgui_demo PRIVATE imgui_base )
target_include_directories ( imgui_demo PRIVATE imgui/examples )
