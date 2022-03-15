cmake_minimum_required(VERSION 3.17)

project(test)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(OpenGL)
find_package(Qt6Core REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6OpenGL REQUIRED)
find_package(Qt6Xml REQUIRED)
find_package(Qt6Widgets REQUIRED)

#======================= INCLUSION OF VTK ======================#
set(VTK_DIR $ENV{VTK_DIR})
find_package(VTK REQUIRED)
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

#======================= INCLUSION OF Our Code ======================#
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/forms")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resource")

include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})
include_directories(${RESOURCE_DIR})

file(GLOB_RECURSE SOURCES
    "${FORMS_DIR}/*.ui"
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
    "${RESOURCE_DIR}/*"
)

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

# Add the executable
if (WIN32) 
    add_executable(test MACOSX_BUNDLE ${SOURCES})
elseif(UNIX)
    add_executable(test ${SOURCES})
endif()

# Add the target includes
target_include_directories(test PRIVATE ${FORMS_DIR})
target_include_directories(test PRIVATE ${INCLUDE_DIR})
target_include_directories(test PRIVATE ${SOURCE_DIR})
target_include_directories(test PRIVATE ${RESOURCE_DIR})

#===================== LINKING LIBRARIES =======================#
target_link_libraries( test ${VTK_LIBRARIES})


vtk_module_autoinit(
    TARGETS test
    MODULES ${VTK_LIBRARIES}
    )