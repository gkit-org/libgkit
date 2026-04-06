set (GKIT_MATH "gkit_math")

set (MATH_SRC
    "./vector2.cpp"
    "./vector3.cpp"
)

add_library(${GKIT_MATH} OBJECT ${MATH_SRC})

target_include_directories(${GKIT_MATH} PRIVATE ${CMAKE_SOURCE_DIR}/include)  # include for header file
target_include_directories(${GKIT_MATH} PRIVATE ${CMAKE_SOURCE_DIR}/src)      # top include for source file