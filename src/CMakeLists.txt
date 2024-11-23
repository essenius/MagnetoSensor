set(myHeaders MagnetoSensor.h MagnetoSensorHmc.h MagnetoSensorNull.h MagnetoSensorQmc.h SensorData.h)
set(mySources MagnetoSensor.cpp MagnetoSensorHmc.cpp MagnetoSensorQmc.cpp)

if (ESP_PLATFORM AND DEFINED ENV{IDF_PATH})
    idf_component_register(SRCS ${mySources}
                    INCLUDE_DIRS ".")
else()
    include(tools)
    assertVariableSet(projectName espMockName)
    add_library(${projectName} "")

    foreach (entry ${CMAKE_PREFIX_PATH})
        list(APPEND includeFolders ${entry}/include)
    endforeach() 

    target_sources (${projectName} PUBLIC ${myHeaders} PRIVATE ${mySources})
    target_link_libraries(${projectName} PUBLIC ${espMockName})
    target_include_directories(${projectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${includeFolders})

    install(TARGETS ${projectName} DESTINATION lib)
    install(FILES ${myHeaders} DESTINATION include) 
endif()