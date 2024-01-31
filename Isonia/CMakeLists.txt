# CMakeList.txt : CMake project for Isonia, include source and define project specific logic here.
# Add project as ex or dll depending on choise
if(DLL_BUILD)
    add_library(${PROJECT_NAME} SHARED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DLL_BUILD=1)
else()
    add_executable(${PROJECT_NAME})
endif()

# Define conditional debug
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC DEBUG=1)
endif()

# Check cmake version
if (CMAKE_VERSION VERSION_GREATER 3.12)
    # Set cxx standard
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
endif()

if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./Resources/") 
	# remove the option to debug asserts.
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
	# This is useful to get an ASSETS_PATH in your IDE during development
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

# Compile shader into SPIR-V
include(CMakeParseArguments)

# Set the glslangValidator executable
set(glslCompiler "$ENV{VK_SDK_PATH}/Bin/glslc.exe")

# Get all shaders recursively
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/*.frag")

# For each shader, we create a header file
foreach(SHADER ${SHADERS})
    # Prepare a header name and a global variable for this shader 
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    get_filename_component(SHADER_DIRECTORY ${SHADER} DIRECTORY)
    string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" "" SHADER_SUB_DIRECTORY "${SHADER_DIRECTORY}")
    string(REPLACE "/" "_" SHADER_SUB_NAMESPACE "${SHADER_SUB_DIRECTORY}")
    string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)
    set(TARGET_NAME "Isonia_Shader${SHADER_SUB_NAMESPACE}_${HEADER_NAME}")

    set(SHADER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Include${SHADER_SUB_DIRECTORY}/")
    set(SHADER_HEADER "${SHADER_DIRECTORY}/${HEADER_NAME}.h")
    set(SHADER_SPV "${SHADER_DIRECTORY}/${SHADER_NAME}.spv")

    file(MAKE_DIRECTORY ${SHADER_DIRECTORY})

    add_custom_target(
        ${TARGET_NAME}
        # Compile any GLSL shader into SPIR-V shader
        COMMAND ${glslCompiler} ${SHADER} -o ${SHADER_SPV}
        # Make a C header file with the SPIR-V shader
        COMMAND ${CMAKE_COMMAND} -DPATH="${SHADER_SPV}" -DHEADER="${SHADER_HEADER}" -DGLOBAL="${GLOBAL_SHADER_VAR}" -DSUB_DIRECTORY="${SHADER_SUB_DIRECTORY}" -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/EmbedData.cmake"
        # Rebuild the header file if the shader is updated
        DEPENDS ${SHADER}
        COMMENT "Building ${SHADER_SPV} and embedding it into ${SHADER_HEADER}"
    )

    # Add the custom target like a dependencies of the project
    add_dependencies(${PROJECT_NAME} ${TARGET_NAME})

    message(STATUS "Generating build commands for ${SHADER}")
endforeach()

# Find all cpp files recursively
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
# Excluding external
list(FILTER MY_SOURCES EXCLUDE REGEX "${PROJECT_NAME}/External/")
# Include the specific header file from External/tinyobjloader/tiny_obj_loader
list(APPEND MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/External/tinyobjloader/tiny_obj_loader.h")

# Add sources
target_sources(${PROJECT_NAME} PUBLIC ${MY_SOURCES})

# If using the VS compiler...
if(MSVC)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
    if(NOT DLL_BUILD)
        set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
endif()

# Add Vulkan (not submodule must be externally downloaded see https://vulkan.lunarg.com/)
find_package(Vulkan REQUIRED)

# Add glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(External/glfw)
target_include_directories(${PROJECT_NAME} PUBLIC "External/glfw/include")
target_link_directories(${PROJECT_NAME} PUBLIC "External/glfw/src")

# Add tinyobjloader
add_subdirectory(External/tinyobjloader)

# Add glm
set(GLM_PERF_TEST_ENABLE OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(External/glm)

# Target link
target_link_libraries(${PROJECT_NAME}
    PUBLIC Vulkan::Vulkan
    PUBLIC glfw
    PUBLIC glm::glm
)

# Copy resources if they are newer
file(GLOB_RECURSE RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*")
foreach(RESOURCE_FILE ${RESOURCE_FILES})
    get_filename_component(RESOURCE_FILENAME ${RESOURCE_FILE} NAME) 
    get_filename_component(RESOURCE_DIRECTORY ${RESOURCE_FILE} DIRECTORY)  # Added this line
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" RESOURCE_SUBDIRECTORY ${RESOURCE_DIRECTORY})

    set(RESOURCE_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_SUBDIRECTORY}/${RESOURCE_FILENAME}")  # Modified this line

    add_custom_command(
        OUTPUT ${RESOURCE_DESTINATION}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_FILE} ${RESOURCE_DESTINATION}
        DEPENDS ${RESOURCE_FILE}
        COMMENT "Copying ${RESOURCE_FILENAME} to ${CMAKE_CURRENT_BINARY_DIR}/Resources/"
    )

    list(APPEND COPIED_RESOURCES ${RESOURCE_DESTINATION})
endforeach()

# Add a custom target to group the copy commands
add_custom_target(CopyResources DEPENDS ${COPIED_RESOURCES})

# Add the CopyResources target as a dependency for your main target
add_dependencies(${PROJECT_NAME} CopyResources)
