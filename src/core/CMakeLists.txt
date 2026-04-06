set (GKIT_CORE "gkit_core")

set (CORE_SRC
    # Runner
    "./processer.cpp"

    # Input module
    "./input/action.cpp"
    "./input/cache.cpp"
    "./input/input.cpp"

    # Scene module
    "./scene/object.cpp"
    "./scene/unit.cpp"
)

add_library(${GKIT_CORE} OBJECT ${CORE_SRC})

target_include_directories(${GKIT_CORE} PRIVATE ${CMAKE_SOURCE_DIR}/include)  # include for header file
target_include_directories(${GKIT_CORE} PRIVATE ${CMAKE_SOURCE_DIR}/src)      # top include for source file