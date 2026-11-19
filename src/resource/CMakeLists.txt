set (GKIT_RESOURCE "gkit_resource")

set (RESOURCE_SRC
    "./register_types.cpp"
    "./resource.cpp"
    "./resource_loader.cpp"
    "./texture.cpp"
    "./shader_source.cpp"
)

add_library(${GKIT_RESOURCE} OBJECT ${RESOURCE_SRC})

target_include_directories(${GKIT_RESOURCE} PRIVATE ${GKIT_SOURCE_DIR}/include)  # include for header file
target_include_directories(${GKIT_RESOURCE} PRIVATE ${GKIT_SOURCE_DIR}/src)      # top include for source file
target_compile_features(gkit_resource PUBLIC cxx_std_20)
target_link_libraries(gkit_resource PRIVATE SDL3::SDL3-static SDL3_image::SDL3_image-static)
