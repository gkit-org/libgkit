set (CORE "gkit_core")

## Core Sources file ##
file(GLOB_RECURSE CORE_SRC
  "./application/*.cpp"
  "./scene/*.cpp"
  "./resource/*cpp"
  "./utils/*.cpp"
  "./input/*.cpp"
  "./*.cpp"
)

add_library(${CORE} OBJECT ${CORE_SRC})

target_include_directories(${CORE} PRIVATE ${CMAKE_SOURCE_DIR}/include)  # include for header file
target_include_directories(${CORE} PRIVATE ${CMAKE_SOURCE_DIR}/src)      # top include for source file