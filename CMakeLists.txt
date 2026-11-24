cmake_minimum_required(VERSION 3.22)

project(ChordCraft VERSION 1.0.0 LANGUAGES C CXX)

set(JUCE_PATH "" CACHE PATH "Ruta a la carpeta de JUCE que contiene CMakeLists.txt y modules")

if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt"
   OR NOT EXISTS "${JUCE_PATH}/modules/juce_core/juce_core.h")
    message(FATAL_ERROR
        "Indica una copia local de JUCE con -DJUCE_PATH=\"/ruta/a/JUCE\". "
        "La carpeta debe contener CMakeLists.txt y modules.")
endif()

add_subdirectory("${JUCE_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/JUCE" EXCLUDE_FROM_ALL)

# Conserva los identificadores generados por NewProject.jucer para los DAW.
juce_add_plugin(ChordCraft
    COMPANY_NAME "yourcompany"
    COMPANY_WEBSITE "www.yourcompany.com"
    BUNDLE_ID "com.yourcompany.ChordCraft"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Jjm2
    PRODUCT_NAME "ChordCraft"
    FORMATS VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(ChordCraft)

target_sources(ChordCraft PRIVATE
    NewProject/Source/ChordEngine.cpp
    NewProject/Source/ChordEngine.h
    NewProject/Source/PluginEditor.cpp
    NewProject/Source/PluginEditor.h
    NewProject/Source/PluginProcessor.cpp
    NewProject/Source/PluginProcessor.h)

target_compile_features(ChordCraft PUBLIC cxx_std_17)
set_target_properties(ChordCraft PROPERTIES CXX_EXTENSIONS OFF)

target_compile_definitions(ChordCraft PUBLIC
    DONT_SET_USING_JUCE_NAMESPACE=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0)

target_link_libraries(ChordCraft
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

if(APPLE)
    find_program(CODESIGN_EXECUTABLE codesign REQUIRED)
    # El manifiesto VST3 y los recursos deben estar presentes antes de firmar.
    foreach(format IN ITEMS VST3 Standalone)
        add_custom_command(TARGET ChordCraft_${format} POST_BUILD
            COMMAND "${CODESIGN_EXECUTABLE}" --force --sign - --timestamp=none
                "$<TARGET_BUNDLE_DIR:ChordCraft_${format}>"
            COMMENT "Firmando ChordCraft ${format} para pruebas locales"
            VERBATIM)
    endforeach()
endif()

include(CTest)

if(BUILD_TESTING)
    add_executable(ChordEngineTests
        NewProject/Source/ChordEngine.cpp
        NewProject/Tests/ChordEngineTests.cpp)
    target_compile_features(ChordEngineTests PRIVATE cxx_std_17)
    set_target_properties(ChordEngineTests PROPERTIES CXX_EXTENSIONS OFF)
    add_test(NAME ChordEngineTests COMMAND ChordEngineTests)
endif()
