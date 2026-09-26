# CMakeList.txt : CMake project for Xantos, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Xantos")

# Add source to this project's executable.
add_executable (Xantos 
"src/Xantos.cpp" 
"src/Xantos.h"
"Libs/GLAD/src/glad.c"
"Libs/STB/stb_image.cpp" 
"src/Render/VBO.cpp" 
"src/Render/VAO.cpp" 
"src/Render/EBO.cpp"
"src/Render/Shader.cpp"
"src/Render/Camera.cpp"
"src/Render/Texture.cpp"
"src/Util/File.cpp"
"src/Util/Time.cpp"
"src/Util/File.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Xantos PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

target_include_directories(Xantos PRIVATE
"Libs/GLAD"
"Libs/GLFW"
"Libs/GLM"
"Libs/STB"
)

target_link_libraries(Xantos PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/Libs/GLFW/glfw3.lib"
)

add_custom_command(TARGET Xantos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Xantos>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders"
        "$<TARGET_FILE_DIR:Xantos>/Shaders"
    COMMENT "Copying Shaders to output directory..."
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Xantos>/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets"
        "$<TARGET_FILE_DIR:Xantos>/Assets"
)
