SET(pwd ${CMAKE_CURRENT_LIST_DIR})

AUX_SOURCE_DIRECTORY(${pwd} src)
add_executable(wave ${src})

target_link_libraries(wave ${stdgl_libraries})
