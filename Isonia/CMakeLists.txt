# CMakeList.txt : CMake project for Isonia, include source and define project specific logic here.
# Add glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(
    External/glfw
    External/tinyobjloader
)

# Add Vulkan (not submodule must be externally downloaded see https://vulkan.lunarg.com/)
find_package(Vulkan REQUIRED)

# Find all cpp files recursively
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# Excluding external
list(FILTER MY_SOURCES EXCLUDE REGEX "${PROJECT_NAME}/External/")

# Add project as ex
add_executable(${PROJECT_NAME})

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

target_sources(${PROJECT_NAME} PRIVATE ${MY_SOURCES})

# If using the VS compiler...
if(MSVC)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
	set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

target_include_directories(
    ${PROJECT_NAME} PUBLIC "External/glfw/include"
)
target_link_directories(
    ${PROJECT_NAME} PUBLIC "External/glfw/src"
)

target_link_libraries(${PROJECT_NAME}
    Vulkan::Vulkan
    glfw
)

# Compile shader into SPIR-V
include(CMakeParseArguments)

# Set the glslangValidator executable
set(glslCompiler "$ENV{VK_SDK_PATH}/Bin/glslc.exe")

# Get all shaders recursively
file(GLOB_RECURSE SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/*.frag")

#set(SHADERS "")

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
        COMMAND ${glslCompiler} ${SHADER} -o ${SHADER}.spv
        # Make a C header file with the SPIR-V shader
        COMMAND ${CMAKE_COMMAND} -DPATH="${SHADER}.spv" -DHEADER="${SHADER_HEADER}" -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/EmbedData.cmake"
        # Rebuild the header file if the shader is updated
        DEPENDS ${SHADER}
        COMMENT "Building ${SHADER}.spv and embedding it into ${SHADER_HEADER}"
    )

    # Add the custom target like a dependencies of the project
    add_dependencies(${PROJECT_NAME} ${HEADER_NAME})

    message(STATUS "Generating build commands for ${SHADER}")
    message(STATUS ${CMAKE_COMMAND} -DPATH="${SHADER}.spv" -DHEADER="${SHADER_HEADER}" -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/EmbedData.cmake")
endforeach()
