set (CORE "gkit_core")

## Core Sources file ##
file(GLOB_RECURSE CORE_SRC
  "./application/*.cpp"
  "./graphic/*.cpp"
  "./input/*.cpp"
)

add_library(${CORE} OBJECT ${CORE_SRC})

target_include_directories(${CORE} PRIVATE "./include/") # include for header file
target_include_directories(${CORE} PRIVATE "../")        # top include for source file