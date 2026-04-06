set (GKIT_UTILS "gkit_utils")

set (UTILS_SRC
    log.cpp
)

add_library(${GKIT_UTILS} OBJECT ${UTILS_SRC})

target_include_directories(${GKIT_UTILS} PRIVATE ${CMAKE_SOURCE_DIR}/include)  # include for header file
target_include_directories(${GKIT_UTILS} PRIVATE ${CMAKE_SOURCE_DIR}/src)      # top include for source file