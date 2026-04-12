set (GKIT_MISC "gkit_misc")

set (MISC_SRC
    "./sdl_event_dispatcher.cpp"
)

add_library(${GKIT_MISC} OBJECT ${MISC_SRC})

target_include_directories(${GKIT_MISC} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${GKIT_MISC} PRIVATE ${CMAKE_SOURCE_DIR}/src)
