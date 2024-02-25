include(tools)

include(FindGit)
find_package(Git)

assertVariableSet(projectName projectTestName Git_FOUND espMockName)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.12.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable_With_Check(googletest)

add_executable(${projectTestName} "")

target_sources (${projectTestName} 
    PRIVATE MagnetoSensorMock.h 
    PRIVATE MagnetoSensorTest.cpp MagnetoSensorHmcTest.cpp MagnetoSensorNullTest.cpp MagnetoSensorQmcTest.cpp MagnetoSensorMock.cpp
)

FetchContent_MakeAvailable_With_Check(${espMockName})

target_link_libraries(${projectTestName} ${projectName} gtest_main ${espMockName})

add_test(NAME ${projectTestName} COMMAND ${projectTestName})