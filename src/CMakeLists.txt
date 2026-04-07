set (GKIT_OBJS)

add_subdirectory(core)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_core>)

add_subdirectory(math)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_math>)

add_subdirectory(resource)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_resource>)

add_subdirectory(scene)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_scene>)

add_subdirectory(utils)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_utils>)

add_library(${PROJECT_NAME} SHARED ${GKIT_OBJS})
add_library(${PROJECT_NAME}_static STATIC ${GKIT_OBJS})

set (GKIT_LIBS)
list (APPEND GKIT_LIBS ${PROJECT_NAME})
list (APPEND GKIT_LIBS ${PROJECT_NAME}_static)

# link third party static libraries
foreach(GKIT_LIB ${GKIT_LIBS})
    target_link_libraries(${GKIT_LIB} PRIVATE 
        SDL3::SDL3-static 
        SDL3_image::SDL3_image
    )
endforeach()