set (GKIT_RESOURCE "gkit_resource")

set (RESOURCE_SRC
    "./resource.cpp"
    "./resource_loader.cpp"
    "./texture.cpp"
    "./metadata.cpp"
)

add_library(${GKIT_RESOURCE} OBJECT ${RESOURCE_SRC})

target_include_directories(${GKIT_RESOURCE} PRIVATE ${CMAKE_SOURCE_DIR}/include)  # include for header file
target_include_directories(${GKIT_RESOURCE} PRIVATE ${CMAKE_SOURCE_DIR}/src)      # top include for source file