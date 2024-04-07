
include(tools)
include(FindGit)
find_package(Git)

assertVariableSet(Git_FOUND espMockName TOP_LEVEL)

include (FetchContent)

FetchContent_Declare(
    ${espMockName}
    GIT_REPOSITORY https://github.com/essenius/esp32-mock
    GIT_TAG v0.1.7    
)

FetchContent_MakeAvailable_With_Check(${espMockName})

if (TOP_LEVEL)

    FetchContent_Declare(
        code-coverage
        URL https://raw.githubusercontent.com/StableCoder/cmake-scripts/23.04/code-coverage.cmake
        SOURCE_DIR cmake
        DOWNLOAD_NO_EXTRACT TRUE
    )

    FetchContent_MakeAvailable_With_Check(code-coverage)
endif()
