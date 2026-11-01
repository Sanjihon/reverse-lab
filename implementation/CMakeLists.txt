cmake_minimum_required(VERSION 3.20)
project(independent_fighter_core LANGUAGES CXX)

include(CTest)

add_library(gameplay_core
    src/character/character_definition.cpp
    src/core/game_loop.cpp
    src/core/game_session.cpp
    src/input/input_manager.cpp
    src/player/player_manager.cpp
    src/player/movement_system.cpp
)

target_include_directories(gameplay_core PUBLIC include)
target_compile_features(gameplay_core PUBLIC cxx_std_20)
set_target_properties(gameplay_core PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(gameplay_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(gameplay_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(graphics_core
    src/animation/animation_system.cpp
    src/graphics/frame_pacer.cpp
    src/graphics/resource_manager.cpp
    src/graphics/software_renderer.cpp
    src/graphics/window_manager.cpp
)

target_include_directories(graphics_core PUBLIC include)
target_compile_features(graphics_core PUBLIC cxx_std_20)
set_target_properties(graphics_core PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(graphics_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(graphics_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(audio_core
    src/audio/audio_engine.cpp
    src/audio/audio_output.cpp
    src/audio/audio_resource_manager.cpp
)

target_include_directories(audio_core PUBLIC include)
target_compile_features(audio_core PUBLIC cxx_std_20)
target_link_libraries(audio_core PUBLIC gameplay_core)
set_target_properties(audio_core PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(audio_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(audio_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(application_core
    src/combat/collision_system.cpp
    src/combat/combat_system.cpp
    src/core/game_application.cpp
)

target_include_directories(application_core PUBLIC include)
target_compile_features(application_core PUBLIC cxx_std_20)
target_link_libraries(application_core PUBLIC gameplay_core graphics_core audio_core)
set_target_properties(application_core PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(application_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(application_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    add_executable(gameplay_tests tests/gameplay_tests.cpp)
    target_link_libraries(gameplay_tests PRIVATE gameplay_core)
    if(MSVC)
        target_compile_options(gameplay_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(gameplay_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME gameplay_tests COMMAND gameplay_tests)

    add_executable(graphics_tests tests/graphics_tests.cpp)
    target_link_libraries(graphics_tests PRIVATE graphics_core)
    if(MSVC)
        target_compile_options(graphics_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(graphics_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME graphics_tests COMMAND graphics_tests)

    add_executable(audio_tests tests/audio_tests.cpp)
    target_link_libraries(audio_tests PRIVATE audio_core)
    if(MSVC)
        target_compile_options(audio_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(audio_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME audio_tests COMMAND audio_tests)

    add_executable(input_tests tests/input_tests.cpp)
    target_link_libraries(input_tests PRIVATE gameplay_core)
    if(MSVC)
        target_compile_options(input_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(input_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME input_tests COMMAND input_tests)

    add_executable(integration_tests tests/integration_tests.cpp)
    target_link_libraries(integration_tests PRIVATE application_core)
    if(MSVC)
        target_compile_options(integration_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(integration_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME integration_tests COMMAND integration_tests)
endif()
