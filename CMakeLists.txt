cmake_minimum_required(VERSION 3.15)

project(PedalBoard2 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use FetchContent to fetch JUCE automatically
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.8 # or your preferred version
)
FetchContent_MakeAvailable(juce)

# === Add app target ===
juce_add_gui_app(PedalBoard2
    PRODUCT_NAME "PedalBoard2"
    VERSION "1.0.1"
    COMPANY_NAME "Void Realms Solutions"
    BUNDLE_ID "com.voidrealmsolutions.pedalboard2"
)
juce_generate_juce_header(PedalBoard2)

# === Source files ===
file(GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/app/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/app/src/*.h"
)

target_sources(PedalBoard2 PRIVATE ${SOURCE_FILES})

# === JUCE Modules ===
target_link_libraries(PedalBoard2 PRIVATE
    juce::juce_core
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_opengl
)

# Optional: add icon, resources etc.

# === (Recommended) Fetch JUCE automatically if not using JUCE_DIR ===
# Uncomment to use FetchContent instead of a local JUCE checkout
# include(FetchContent)
# FetchContent_Declare(
#     juce
#     GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
#     GIT_TAG        7.0.8 # or your preferred version
# )
# FetchContent_MakeAvailable(juce)

# === Add Windows icon resource ===
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_SOURCE_DIR}/app/icon/icon.ico")
if(WIN32)
    target_sources(PedalBoard2 PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

# === Add other resource handling as needed ===
# e.g., images, vectors, etc.