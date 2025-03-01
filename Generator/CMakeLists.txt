﻿cmake_minimum_required(VERSION 3.10)

# Definir o nome do projeto
project(CGProject)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Definir o Toolchain do vcpkg para encontrar as bibliotecas automaticamente
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Definir padrão C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Criar executável do Generator
add_executable(Generator generator.cpp)

# Criar executável do Engine
add_executable(Engine engine.cpp)

# Encontrar e incluir OpenGL
find_package(OpenGL REQUIRED)

# Definir manualmente o caminho do FreeGLUT (instalado via vcpkg)
set(GLUT_INCLUDE_DIR "C:/vcpkg/installed/x86-windows/include")
set(GLUT_LIB_DIR     "C:/vcpkg/installed/x86-windows/lib")
set(GLUT_LIBRARIES   "C:/vcpkg/installed/x86-windows/lib/freeglut.lib")

# Verificar se os diretórios existem
if(NOT EXISTS "${GLUT_LIBRARIES}" OR NOT EXISTS "${GLUT_INCLUDE_DIR}")
    message(FATAL_ERROR "FreeGLUT não encontrado! Certifique-se de que está instalado via vcpkg (freeglut:x86-windows).")
endif()

# Incluir diretórios de OpenGL e FreeGLUT
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIB_DIR})

# Adicionar caminho do vcpkg para encontrar bibliotecas
set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x86-windows")

# Encontrar a biblioteca tinyxml2 (NÃO tinyxml)
find_library(TINYXML2_LIB 
    NAMES tinyxml2
    PATHS "C:/vcpkg/installed/x86-windows/lib"
    NO_DEFAULT_PATH
)
if(NOT TINYXML2_LIB)
    message(FATAL_ERROR 
        "tinyxml2 library not found! Certifique-se de que foi instalado via vcpkg (tinyxml2:x86-windows)."
    )
endif()

# Incluir diretório de cabeçalhos do tinyxml2
include_directories("C:/vcpkg/installed/x86-windows/include")

# Adicionar bibliotecas ao executável do Generator
target_link_libraries(Generator 
    PRIVATE 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARIES} 
        ${TINYXML2_LIB}
)

# Adicionar bibliotecas ao executável do Engine
target_link_libraries(Engine 
    PRIVATE 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARIES} 
        ${TINYXML2_LIB}
)

# Configuração específica para Windows x86
if(WIN32)
    message(STATUS "Using FreeGLUT and tinyxml2 from vcpkg for x86 Windows")

    # Copiar a DLL do FreeGLUT para a pasta de build (necessário para execução)
    if (EXISTS "C:/vcpkg/installed/x86-windows/bin/freeglut.dll")
        file(COPY "C:/vcpkg/installed/x86-windows/bin/freeglut.dll" DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    # Define o projeto inicial no Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Generator)

else() # Linux e macOS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    target_link_libraries(Generator PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_link_libraries(Engine    PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    
    if(NOT GLUT_FOUND)
        message(FATAL_ERROR "GLUT not found on Linux/macOS! Certifique-se de que foi instalado corretamente.")
    endif()
endif()
