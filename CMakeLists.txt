cmake_minimum_required(VERSION 3.14.0)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME macs_resource_generator)
set(TARGET_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} C CXX)

add_subdirectory(anyoption)
add_subdirectory(zlib)
set(PNG_SHARED OFF CACHE BOOL "Build shared lib" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Build libpng tests" FORCE)
set(PNG_BUILD_ZLIB ON CACHE BOOL "Custom zlib Location, else find_package is used" FORCE)
set(SKIP_INSTALL_ALL 1)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARY zlibstatic)
add_subdirectory(libpng)

IF(APPLE)
    SET (PROJECT_OS_MACOS TRUE BOOL INTERNAL)
ENDIF (APPLE)
IF(WIN32)
    SET (PROJECT_OS_WIN32 TRUE BOOL INTERNAL)
ENDIF (WIN32)
if(UNIX AND NOT APPLE) 
    SET (PROJECT_OS_LINUX TRUE BOOL INTERNAL)
ENDIF (UNIX AND NOT APPLE)



include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")



include_directories (${LIBPNG_INCLUDE_DIRS})
link_directories (${LIBPNG_LIBRARY_DIRS})
link_libraries (${LIBPNG_LIBRARIES})

if (DEFINED ZLIB_LIBRARY)
link_directories (${ZLIB_LIBRARY_DIRS})
link_libraries (${ZLIB_LIBRARY})
endif()



message(${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE PROJECT_SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.*
)



if(CMAKE_GENERATOR STREQUAL Xcode)

    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
        ${PROJECT_SOURCES})

elseif(PROJECT_OS_MACOS)

    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
        ${PROJECT_SOURCES})

    add_custom_command(TARGET ${PROJECT_NAME} 
        POST_BUILD COMMAND 
        ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../../Frameworks/"
        $<TARGET_FILE:${PROJECT_NAME}>)

elseif(WIN32)

    add_executable(${PROJECT_NAME} 
        ${PROJECT_SOURCES})
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE MODEL_LIBRARY_STATIC)
    
else ()

    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    
endif()



if (PROJECT_OS_MACOS)

    list(APPEND SRE_INCLUDE_DIRS /System/Library/Frameworks)
    
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework ImageIO")
    target_link_libraries(${PROJECT_NAME} "-lc")
    
endif()

target_link_libraries(${PROJECT_NAME} anyoption)
target_link_libraries(${PROJECT_NAME} zlibstatic)
target_link_libraries(${PROJECT_NAME} png_static)
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR}
        ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR}
)
