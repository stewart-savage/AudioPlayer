cmake_minimum_required(VERSION 3.30)
project(AudioPlayer_project VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded") #Compile with -Z7 to produce object files with full symbolic debugging information. (instead of generating .pdb files)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
if(MSVC)
    add_compile_options(/W4)
else()
    target_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(modules/JUCE)

juce_add_gui_app(AudioPlayer
        PLUGIN_MANUFACTURER_CODE Stew
        PLUGIN_CODE Dem0
        FORMATS VST3
        PRODUCT_NAME "AudioPlayer"
)
target_include_directories(AudioPlayer PRIVATE
        include/
)
target_sources(AudioPlayer PRIVATE
                                       src/Main.cpp
        include/Application.h          src/Application.cpp
        include/MainWindow.h           src/MainWindow.cpp
        include/AudioPlayerComponent.h src/AudioPlayerComponent.cpp
)
target_compile_definitions(AudioPlayer PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(AudioPlayer PRIVATE
        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
juce_generate_juce_header(AudioPlayer)
