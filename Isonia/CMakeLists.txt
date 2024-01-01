# CMakeList.txt : CMake project for Isonia, include source and define project specific logic here.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Add an executable for the file Main.cpp
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(FILTER MY_SOURCES EXCLUDE REGEX "/External/")
add_executable(Isonia ${MY_SOURCES})

# Add Vulkan (not submodule must be externally downloaded see https://vulkan.lunarg.com/)
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# Add glfw, glm
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(
    External/glfw
    External/glm
)
target_include_directories(
    ${PROJECT_NAME} PUBLIC "External/glfw/include"
    ${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR}
)
target_link_directories(
    ${PROJECT_NAME} PUBLIC "External/glfw/src"
    ${PROJECT_NAME} PUBLIC "External/glm/glm"
)
target_link_libraries(${PROJECT_NAME}
    glfw
    glm
)

# Compile shader into SPIR-V
include(CMakeParseArguments)

# Set the glslangValidator executable
set(glslCompiler "C:/VulkanSDK/1.3.268.0/Bin/glslc.exe")

# Get all shaders recursively
file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/Shaders/**/*.vert" "${CMAKE_SOURCE_DIR}/Shaders/**/*.frag")

# For each shader, we create a header file
foreach(SHADER ${SHADERS})
    # Prepare a header name and a global variable for this shader
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
    string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

    set(SHADER_HEADER "${PROJECT_SOURCE_DIR}/include/${HEADER_NAME}.h")

    add_custom_target(
        ${HEADER_NAME}
        # Compile any GLSL shader into SPIR-V shader
        COMMAND ${glslCompiler} -V ${SHADER} -o ${SHADER}.spv
        # Make a C header file with the SPIR-V shader
        COMMAND ${CMAKE_COMMAND} -DPATH="${SHADER}.spv" -DHEADER="${SHADER_HEADER}" -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${CMAKE_SOURCE_DIR}/cmake/scripts/embed-data.cmake"
        # Rebuild the header file if the shader is updated
        DEPENDS ${SHADER}
        COMMENT "Building ${SHADER}.spv and embedding it into ${SHADER_HEADER}"
    )

    # Add the custom target like a dependencies of the project
    add_dependencies(${PROJECT_NAME} ${HEADER_NAME})

    message(STATUS "Generating build commands for ${SHADER}")
endforeach()

# Check cmake version
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()
