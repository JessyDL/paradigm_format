project(parser_tests LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
include(CTest)

# Fetch Catch2 v3
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.3.2  # Use latest v3.x version
)
FetchContent_MakeAvailable(catch2)

add_executable(parser_tests
  "src/types.cpp"
  "src/lexer.cpp"
  "src/common.cpp"
)

if (MSVC)
    target_compile_options(parser_tests PRIVATE /W4 /WX)
    target_compile_options(parser_tests PRIVATE /permissive- /Zc:preprocessor /Zc:__cplusplus)
    target_compile_options(parser_tests PRIVATE /MP)
else()
    target_compile_options(parser_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(parser_tests PRIVATE -pedantic-errors)
endif()

target_link_libraries(parser_tests PRIVATE Catch2WithMain parser)
include(Catch)
catch_discover_tests(parser_tests)

# deploy the data directory right next to the binary
add_custom_command(
  TARGET parser_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/data
  $<TARGET_FILE_DIR:parser_tests>/data
)
