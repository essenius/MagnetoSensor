include(tools)

assertVariableSet(projectName)

file(APPEND ${PROJECT_BINARY_DIR}/output.txt "In Project ${projectName} src: CMAKE_PROJECT_NAME=${CMAKE_PROJECT_NAME}")

file(APPEND ${PROJECT_BINARY_DIR}/output.txt "CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
file(APPEND ${PROJECT_BINARY_DIR}/output.txt "PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")
file(APPEND ${PROJECT_BINARY_DIR}/output.txt "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    file(APPEND ${PROJECT_BINARY_DIR}/output.txt "${_variableName}=${${_variableName}}\n")
endforeach()

add_library(${matrixName} "")

set(myHeaders MagnetoSensor.h MagnetoSensorHmc.h MagnetoSensorNull.h MagnetoSensorQmc.h)
set(mySources MagnetoSensor.cpp MagnetoSensorHmc.cpp MagnetoSensorNull.cpp MagnetoSensorQmc.cpp)

target_sources (${matrixName} PUBLIC ${myHeaders} PRIVATE ${mySources})
target_include_directories(${matrixName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${matrixName} DESTINATION lib)
install(FILES ${myHeaders} DESTINATION include) 