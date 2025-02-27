cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(Generator)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Definir o Toolchain do vcpkg para encontrar FreeGLUT automaticamente
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Adicionar o executável
add_executable(${PROJECT_NAME} generator.cpp)

# Encontrar e incluir OpenGL
find_package(OpenGL REQUIRED)

# Definir o caminho do FreeGLUT manualmente
set(GLUT_INCLUDE_DIR "C:/vcpkg/installed/x64-windows/include")
set(GLUT_LIB_DIR "C:/vcpkg/installed/x64-windows/lib")
set(GLUT_LIBRARIES "C:/vcpkg/installed/x64-windows/lib/freeglut.lib")

# Incluir diretórios de OpenGL e FreeGLUT
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
link_directories(${OPENGL_LIBRARY_DIRS} ${GLUT_LIB_DIR})

# Adicionar bibliotecas ao executável
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

# Configuração específica para Windows x64
if(WIN32)
    message(STATUS "Using FreeGLUT from vcpkg for x64 Windows")

    # Copiar a DLL do FreeGLUT para a pasta de build (necessário para execução)
    if (EXISTS "C:/vcpkg/installed/x64-windows/bin/freeglut.dll")
        file(COPY "C:/vcpkg/installed/x64-windows/bin/freeglut.dll" DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else() # Linux e macOS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    
    if(NOT GLUT_FOUND)
        message(FATAL_ERROR "GLUT not found on Linux/macOS! Certifique-se de que foi instalado corretamente.")
    endif()
endif()
