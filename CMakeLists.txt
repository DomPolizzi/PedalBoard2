cmake_minimum_required(VERSION 3.15)

project(PedalBoard2 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Check if JUCE_DIR is defined or if JUCE exists in the expected locations
# First check in the source directory
set(JUCE_LOCAL_PATH "${CMAKE_SOURCE_DIR}/JUCE" CACHE PATH "Path to local JUCE installation")
# Also check one level up from source directory (for Docker container structure)
set(JUCE_PARENT_PATH "${CMAKE_SOURCE_DIR}/../JUCE")

if(DEFINED JUCE_DIR)
    message(STATUS "Using JUCE_DIR: ${JUCE_DIR}")
    add_subdirectory(${JUCE_DIR} JUCE)
elseif(EXISTS "${JUCE_LOCAL_PATH}/CMakeLists.txt")
    message(STATUS "Using local JUCE installation at: ${JUCE_LOCAL_PATH}")
    add_subdirectory(${JUCE_LOCAL_PATH} JUCE)
elseif(EXISTS "${JUCE_PARENT_PATH}/CMakeLists.txt")
    message(STATUS "Using JUCE installation from parent directory: ${JUCE_PARENT_PATH}")
    add_subdirectory(${JUCE_PARENT_PATH} JUCE)
else()
    # Fall back to FetchContent if no local JUCE is found
    message(STATUS "No local JUCE found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        7.0.12
    )
    FetchContent_MakeAvailable(juce)
endif()

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

# Exclude LADSPA plugin format files from the build
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*LADSPAPluginFormat\.(cpp|h)$")

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

# === JUCE Configuration ===
target_compile_definitions(PedalBoard2 PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:PedalBoard2,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:PedalBoard2,JUCE_VERSION>"
)

# Linux-specific configuration
if(UNIX AND NOT APPLE)
    # Find required packages
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)
    pkg_check_modules(JACK REQUIRED jack)
    
    # Add GTK include directories and libraries
    target_include_directories(PedalBoard2 PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(PedalBoard2 PRIVATE ${GTK3_LIBRARIES})
    target_include_directories(PedalBoard2 PRIVATE ${WEBKIT_INCLUDE_DIRS})
    target_link_libraries(PedalBoard2 PRIVATE ${WEBKIT_LIBRARIES})
    
    # Add JACK library
    target_include_directories(PedalBoard2 PRIVATE ${JACK_INCLUDE_DIRS})
    target_link_libraries(PedalBoard2 PRIVATE ${JACK_LIBRARIES})
    
    # Add compiler flags
    target_compile_options(PedalBoard2 PRIVATE ${GTK3_CFLAGS_OTHER})
    target_compile_options(PedalBoard2 PRIVATE ${JACK_CFLAGS_OTHER})
endif()

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