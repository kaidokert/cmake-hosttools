cmake_minimum_required(VERSION 3.12)
project(HostToolsTest)

set(CMAKE_CXX_STANDARD 11)

# Dump env
message("CMAKE_SYSTEM:" ${CMAKE_SYSTEM})
message("CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})
message("env compiler CXX:" $ENV{CXX})
# execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

set(host_tools_list generate_foo blah)

if(CMAKE_CROSSCOMPILING)
    message("This is cross")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools)
    file(TO_NATIVE_PATH ${CMAKE_COMMAND} native_cmake_command)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} native_cmake_current_source_dir)
    message("Command: ${native_cmake_command} -G${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ${native_cmake_current_source_dir}")
    message("Workingdir ${CMAKE_CURRENT_BINARY_DIR}/host_tools")
    set(ENV{CXX} "")
    set(ENV{CC} "")
    execute_process(
        COMMAND "${native_cmake_command}" "-G${CMAKE_GENERATOR}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" "${native_cmake_current_source_dir}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools
    )
    add_custom_target(host_tools
        COMMAND ${CMAKE_COMMAND} --build . --target host_tools --config $<CONFIG>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools
    )
    include(${CMAKE_CURRENT_BINARY_DIR}/host_tools/host_tools.cmake)

    foreach(tgt IN ITEMS ${host_tools_list})
        add_dependencies(host${tgt} host_tools)
    endforeach()
else()
    # Add an empty target, host tools are built inplace
    add_custom_target(host_tools
        DEPENDS ${host_tools_list}
    )
endif()

if(NOT CMAKE_CROSSCOMPILING)
    add_executable(generate_foo hostmain.cpp)
    add_executable(blah hostmain.cpp)
endif()

if(NOT CMAKE_CROSSCOMPILING)
    foreach(tgt IN ITEMS ${host_tools_list})
        add_executable(host${tgt} ALIAS ${tgt})
    endforeach()

    export(TARGETS ${host_tools_list} NAMESPACE host FILE host_tools.cmake)
endif()

set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)
file(MAKE_DIRECTORY ${GEN_DIR})

add_custom_command(OUTPUT version.h
    COMMAND $<TARGET_FILE:hostgenerate_foo>
    WORKING_DIRECTORY ${GEN_DIR}
    DEPENDS host_tools)

add_executable(maintest main.cpp version.h)
target_include_directories(maintest PRIVATE ${GEN_DIR})
