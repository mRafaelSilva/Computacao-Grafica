cmake_minimum_required(VERSION 3.10)


project(CGProject)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(Generator generator.cpp)


add_executable(Engine engine.cpp)

find_package(OpenGL REQUIRED)

set(GLUT_INCLUDE_DIR "C:/vcpkg/installed/x86-windows/include")
set(GLUT_LIB_DIR     "C:/vcpkg/installed/x86-windows/lib")
set(GLUT_LIBRARIES   "C:/vcpkg/installed/x86-windows/lib/freeglut.lib")


if(NOT EXISTS "${GLUT_LIBRARIES}" OR NOT EXISTS "${GLUT_INCLUDE_DIR}")
    message(FATAL_ERROR "FreeGLUT não encontrado! Certifique-se de que está instalado via vcpkg (freeglut:x86-windows).")
endif()


include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIB_DIR})

set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x86-windows")


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

include_directories("C:/vcpkg/installed/x86-windows/include")


target_link_libraries(Generator 
    PRIVATE 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARIES} 
        ${TINYXML2_LIB}
)


target_link_libraries(Engine 
    PRIVATE 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARIES} 
        ${TINYXML2_LIB}
)


add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E echo "A apagar ficheiros .3d antigos da pasta generator/..."
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_SOURCE_DIR}/build/*.3d
    COMMAND ${CMAKE_COMMAND} -E echo "A executar Generator com comandos.txt"
    COMMAND Generator ${CMAKE_CURRENT_SOURCE_DIR}/comandos.txt
    DEPENDS Generator
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
    message(STATUS "Using FreeGLUT and tinyxml2 from vcpkg for x86 Windows")


    if (EXISTS "C:/vcpkg/installed/x86-windows/bin/freeglut.dll")
        file(COPY "C:/vcpkg/installed/x86-windows/bin/freeglut.dll" DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Generator)

else() 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    target_link_libraries(Generator PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_link_libraries(Engine    PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    
    if(NOT GLUT_FOUND)
        message(FATAL_ERROR "GLUT not found on Linux/macOS! Certifique-se de que foi instalado corretamente.")
    endif()
endif()
