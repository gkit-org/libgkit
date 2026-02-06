set(C_ABI "gkit_c_abi")

## CABI sources ##
file(GLOB_RECURSE CABI_SRC
    "application/*.c"
    "application/*.cpp"
    "graphic/*.c"
    "graphic/*.cpp"
)

add_library(${C_ABI} OBJECT ${CABI_SRC})
add_dependencies(${C_ABI} ${CORE})

target_include_directories(${C_ABI} PRIVATE "include/") # include for header file
target_include_directories(${C_ABI} PRIVATE "../")      # top include for source file

# target_link_libraries(${C_ABI} PRIVATE ${CORE})