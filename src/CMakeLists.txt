cmake_minimum_required(VERSION 3.20)

set (GKIT_OBJS)

add_subdirectory(core)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_core>)

add_subdirectory(math)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_math>)

add_subdirectory(resource)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_resource>)

add_subdirectory(scene)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_scene>)

# add_subdirectory(utils)
# list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_utils>)

add_subdirectory(graphic)
list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_graphic>)

set (GKIT_LIBS)
if(GKIT_SHARED_BUILD)
    add_library(${PROJECT_NAME} SHARED ${GKIT_OBJS})
    list (APPEND GKIT_LIBS ${PROJECT_NAME})
endif()

if(GKIT_STATIC_BUILD)
    add_library(${PROJECT_NAME}_static STATIC ${GKIT_OBJS})
    list (APPEND GKIT_LIBS ${PROJECT_NAME}_static)
endif()

# link third party static libraries
foreach(GKIT_LIB ${GKIT_LIBS})
    target_compile_features(${GKIT_LIB} PUBLIC cxx_std_20)
    target_include_directories(${GKIT_LIB} PUBLIC ${GKIT_SOURCE_DIR}/include)
    set_target_properties(${GKIT_LIB} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_link_libraries(${GKIT_LIB} PRIVATE
        SDL3::SDL3-static
        SDL3_image::SDL3_image-static
        glad_gl
    )
endforeach()

if(GKIT_SANITIZER)
    set(GKIT_SANITIZER_FLAGS "-fsanitize=${GKIT_SANITIZER}" -fno-omit-frame-pointer)
    if(GKIT_SANITIZER STREQUAL "address")
        list(APPEND GKIT_SANITIZER_FLAGS -fsanitize=undefined)
    endif()
    foreach(module gkit_core gkit_math gkit_resource gkit_scene)
        target_compile_options(${module} PRIVATE ${GKIT_SANITIZER_FLAGS})
    endforeach()
    foreach(library ${GKIT_LIBS})
        target_link_options(${library} PUBLIC ${GKIT_SANITIZER_FLAGS})
        target_compile_options(${library} INTERFACE ${GKIT_SANITIZER_FLAGS})
    endforeach()
endif()
