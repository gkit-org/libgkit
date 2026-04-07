set (GKIT_SCENE "gkit_scene")

set (SCENE_SRC
    # 2D scene
    "2d/object2d.cpp"
    "2d/transform2d.cpp"

    # 3D scene
)

add_library(${GKIT_SCENE} OBJECT ${SCENE_SRC})

target_include_directories(${GKIT_SCENE} PRIVATE ${CMAKE_SOURCE_DIR}/include)  # include for header file
target_include_directories(${GKIT_SCENE} PRIVATE ${CMAKE_SOURCE_DIR}/src)      # top include for source file