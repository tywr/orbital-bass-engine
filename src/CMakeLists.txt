# set(CMAKE_BUILD_TYPE Debug)

juce_add_plugin(${PROJECT_NAME}
    PLIST_TO_MERGE "<key>NSMicrophoneUsageDescription</key><string>This app needs to use your microphone to process the audio signal.</string>"
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # COMPANY_NAME ...                          # Specify the name of the plugin's author
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Juce               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dem0                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    # FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    FORMATS Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME ${PROJECT_NAME}        # The name of the final executable, which can differ from the target name
    MICROPHONE_PERMISSION_ENABLED TRUE
    
)

target_sources(${PROJECT_NAME}
    PRIVATE
        assets/ImpulseResponseBinary.cpp
        plugin_editor.cpp
        plugin_audio_processor.cpp
        plugin_audio_process_parameters.cpp
        preset_manager.cpp
        session_manager.cpp
        gui/looks/amp_small_look_and_feel.cpp
        gui/looks/base_look_and_feel.cpp
        gui/looks/compressor_look_and_feel.cpp
        gui/looks/header_look_and_feel.cpp
        gui/looks/eq_look_and_feel.cpp
        gui/looks/compressor_selector_look_and_feel.cpp
        gui/looks/tuner_look_and_feel.cpp
        gui/preset_bar.cpp
        gui/preset_icon_buttons.cpp
        gui/session_name_display.cpp
        gui/compressor/compressor_knobs_component.cpp
        gui/compressor/compressor_meter_component.cpp
        gui/compressor/compressor_component.cpp
        gui/synth/synth_component.cpp
        gui/synth/synth_voice_knobs_component.cpp
        gui/amp/amp_component.cpp
        gui/amp/amp_knobs_component.cpp
        gui/chorus/chorus_component.cpp
        gui/chorus/chorus_knobs_component.cpp
        gui/eq/eq_component.cpp
        gui/eq/eq_sliders_component.cpp
        gui/ir/ir_component.cpp
        gui/meter.cpp
        gui/header.cpp
        gui/tabs.cpp
        gui/post_rack.cpp
        gui/tuner.cpp
        dsp/compressor.cpp
        dsp/pitch_detector.cpp
        dsp/ir.cpp
        dsp/overdrives/helios.cpp
        dsp/overdrives/borealis.cpp
        dsp/eq.cpp
        dsp/chorus.cpp
        dsp/synth_voices/square_voice.cpp
        dsp/synth_voices/octave_voice.cpp
        dsp/synth_voices/triangle_voice.cpp
        dsp/synth_voices.cpp
        )

if(JUCE_BUILD_STANDALONE)
    target_sources(${PROJECT_NAME} PRIVATE src/standalone.cpp)
endif()


target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_DISABLE_ASSERTIONS=1
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_plugin_client
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
