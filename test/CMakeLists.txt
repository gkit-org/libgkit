set (EXECUTABLE_OUTPUT_PATH ${BIN_FOLDER}/test)

include_directories(${GKIT_SOURCE_DIR}/src)
include_directories(${GKIT_SOURCE_DIR}/include)
include_directories(${GKIT_SOURCE_DIR}/test)
include_directories(${GKIT_SOURCE_DIR}/src/c_abi/include)
include_directories(${GKIT_SOURCE_DIR}/third_party/glad_generated/include)

# build core tests
file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS
    "core/*.cpp"
    "math/*.cpp"
    "utils/*.cpp"
    "resource/*.cpp"
    "graphic/*.cpp"
)

foreach(test_file ${TEST_SRCS})
    get_filename_component(FILE_BASE_NAME ${test_file} NAME_WE)
    add_executable(${FILE_BASE_NAME} ${test_file})
    target_link_libraries(${FILE_BASE_NAME} PRIVATE gkit_static SDL3::SDL3-static)
    if(NOT test_file MATCHES "/graphic/" AND
       NOT FILE_BASE_NAME MATCHES "^test_(key_input|mouse_input)$")
        add_test(NAME ${FILE_BASE_NAME} COMMAND ${FILE_BASE_NAME})
        set_tests_properties(${FILE_BASE_NAME} PROPERTIES TIMEOUT 30 LABELS unit)
    endif()
endforeach()
# Compile each public header in isolation; no umbrella include can hide missing dependencies.
file(GLOB_RECURSE PUBLIC_HEADERS CONFIGURE_DEPENDS "${GKIT_SOURCE_DIR}/include/gkit/*.hpp")
set(HEADER_CHECK_SOURCES)
foreach(header ${PUBLIC_HEADERS})
    file(RELATIVE_PATH relative "${GKIT_SOURCE_DIR}/include" "${header}")
    string(MAKE_C_IDENTIFIER "${relative}" identifier)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/headers/${identifier}.cpp")
    file(GENERATE OUTPUT "${source}" CONTENT "#include <${relative}>\n")
    list(APPEND HEADER_CHECK_SOURCES "${source}")
endforeach()
add_library(gkit_header_check OBJECT ${HEADER_CHECK_SOURCES})
target_link_libraries(gkit_header_check PRIVATE gkit_static)

# Independent CMake consumers receive only the public include path and library file.
foreach(library gkit_static gkit)
    if(TARGET ${library} AND NOT GKIT_SANITIZER)
        add_test(NAME consumer_${library} COMMAND ${CMAKE_COMMAND}
            "-DLIBRARY=$<TARGET_LINKER_FILE:${library}>"
            "-DRUNTIME_DIR=$<TARGET_FILE_DIR:${library}>"
            "-DINCLUDE_DIR=${GKIT_SOURCE_DIR}/include"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/consumer_${library}"
            "-DCXX=${CMAKE_CXX_COMPILER}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/consumer.cmake")
        set_tests_properties(consumer_${library} PROPERTIES TIMEOUT 60 LABELS integration)
    endif()
endforeach()

add_executable(test_framework_failure framework_failure.cpp)
target_link_libraries(test_framework_failure PRIVATE gkit_static)
add_test(NAME test_framework_failure COMMAND test_framework_failure)
set_tests_properties(test_framework_failure PROPERTIES WILL_FAIL TRUE TIMEOUT 10 LABELS unit)
