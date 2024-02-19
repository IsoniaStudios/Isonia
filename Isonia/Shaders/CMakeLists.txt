# Compile shader into SPIR-V
include(CMakeParseArguments)

# Set the glslangValidator executable
set(glslCompiler "$ENV{VK_SDK_PATH}/Bin/glslc.exe")

# Get all shaders recursively
file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.geom"
)

# For each shader, we create a header file
foreach(SHADER ${SHADERS})
    # Prepare a header name and a global variable for this shader 
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    get_filename_component(SHADER_DIRECTORY ${SHADER} DIRECTORY)
    string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" SHADER_SUB_DIRECTORY "${SHADER_DIRECTORY}")
    string(REPLACE "/" "_" SHADER_SUB_NAMESPACE "${SHADER_SUB_DIRECTORY}")
    string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)
    set(TARGET_NAME "Isonia_Shader${SHADER_SUB_NAMESPACE}_${HEADER_NAME}")

    set(SHADER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Include${SHADER_SUB_DIRECTORY}")
    set(SHADER_HEADER "${SHADER_DIRECTORY}/${HEADER_NAME}.h")
    set(SHADER_SPV "${SHADER_DIRECTORY}/${SHADER_NAME}.spv")

    file(MAKE_DIRECTORY ${SHADER_DIRECTORY})
 
    # Compile any GLSL shader into SPIR-V shader
    # Make a C header file with the SPIR-V shader
    # Rebuild the header file if the shader is updated
    execute_process(
        COMMAND ${glslCompiler} ${SHADER} -o ${SHADER_SPV}
    )
    include(../CMake/EmbedData.cmake)

    add_custom_command(
        OUTPUT ${SHADER_SPV} ${SHADER_HEADER}
        COMMAND ${glslCompiler} ${SHADER} -o ${SHADER_SPV}
        COMMAND ${CMAKE_COMMAND} -DSHADER_SPV="${SHADER_SPV}" -DSHADER_HEADER="${SHADER_HEADER}" -DGLOBAL_SHADER_VAR="${GLOBAL_SHADER_VAR}" -DSHADER_SUB_DIRECTORY="${SHADER_SUB_DIRECTORY}" -P "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/EmbedData.cmake"
        DEPENDS ${SHADER}
        COMMENT "Building ${SHADER_SPV} and embedding it into ${SHADER_HEADER}"
    )

    # Add the custom command as a target
    add_custom_target(${TARGET_NAME} ALL DEPENDS ${SHADER})

    message(STATUS "Generating build commands for ${SHADER}")
endforeach()
