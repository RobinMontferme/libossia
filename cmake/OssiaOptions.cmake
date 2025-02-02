# Build settings :
option(OSSIA_STATIC "Make a static build" OFF)
option(OSSIA_COVERAGE "Run code coverage" OFF)
option(OSSIA_EXAMPLES "Build examples" OFF)
option(OSSIA_TESTING "Build tests" OFF)
option(OSSIA_CI "Continuous integration run" OFF)
option(OSSIA_FRAMEWORK "Build an OS X framework" OFF)
option(OSSIA_PCH "Enable PCH" ON)
option(OSSIA_NO_SONAME "Set NO_SONAME property" ON)
option(OSSIA_OSX_FAT_LIBRARIES "Build 32 and 64 bit fat libraries on OS X" OFF)
option(OSSIA_OSX_RETROCOMPATIBILITY "Build for older OS X versions" OFF)
option(OSSIA_DATAFLOW "Dataflow features" ON)
option(OSSIA_EDITOR "Editor features" ON)
option(OSSIA_GFX "Graphics features" ON)
option(OSSIA_HIDE_ALL_SYMBOLS "Hide all symbols from the ossia lib" OFF)

# Bindings :
option(OSSIA_JAVA "Build JNI bindings" OFF)
option(OSSIA_JAVA_ONLY "Build only JNI bindings" OFF)
option(OSSIA_PD "Build PureData externals" OFF)
option(OSSIA_PD_ONLY "Build and install only PureData externals (disable all other non mandatory options)" OFF)
option(OSSIA_MAX "Build Max/MSP externals" OFF)
option(OSSIA_MAX_ONLY "Build and install only Max/MSP externals (disable all other non mandatory options)" OFF)
option(OSSIA_PYTHON "Build Python bindings" OFF)
option(OSSIA_PYTHON_ONLY "Build and install only Python bindings (disable all other non mandatory options)" OFF)
option(OSSIA_C "Build C bindings" OFF)
option(OSSIA_CPP "Build CPP bindings" OFF)
option(OSSIA_CPP_ONLY "Build only ossia-cpp and install only needed header" OFF)
option(OSSIA_UNITY3D "Build Unity3D bindings" OFF)
option(OSSIA_UNITY3D_ONLY "Build only Unity3D bindings (disable all other non mandatory options)" OFF)
option(OSSIA_QT "Build Qt bindings. Necessary for websocket, serial, http." OFF)
option(OSSIA_QML "Build QML bindings" OFF)
option(OSSIA_QML_ONLY "Build and install only QML bindings" OFF)
option(OSSIA_QML_SCORE "Build QML bindings to scoring parts" OFF)
option(OSSIA_QML_DEVICE "Build QML device bindings" OFF)
option(OSSIA_NODEJS "Build Node.js bindings" OFF)

# Protocols
option(OSSIA_PROTOCOL_AUDIO "Enable Audio protocol" ON)
option(OSSIA_PROTOCOL_MIDI "Enable MIDI protocol" ON)
option(OSSIA_PROTOCOL_OSC "Enable OSC protocol" ON)
option(OSSIA_PROTOCOL_MINUIT "Enable Minuit protocol" ON)
option(OSSIA_PROTOCOL_OSCQUERY "Enable OSCQuery protocol" ON)
option(OSSIA_PROTOCOL_HTTP "Enable HTTP protocol" ON) # Requires Qt
option(OSSIA_PROTOCOL_WEBSOCKETS "Enable WebSockets protocol" OFF) # Requires Qt
option(OSSIA_PROTOCOL_SERIAL "Enable Serial port protocol" OFF) # Requires Qt
option(OSSIA_PROTOCOL_PHIDGETS "Enable Phidgets protocol" OFF) # Requires Phidgets library
option(OSSIA_PROTOCOL_LEAPMOTION "Enable Leapmotion protocol" OFF) # Requires LeapMotion Orion library
option(OSSIA_PROTOCOL_JOYSTICK "Enable Joystick protocol" ON)  # Requires SDL2 library
option(OSSIA_PROTOCOL_WIIMOTE "Enable Wiimote Protocol" ON) #use wiiuse
option(OSSIA_PROTOCOL_ARTNET "Enable artnet protocol" ON) #use libartnet
option(OSSIA_PROTOCOL_LIBMAPPER "Enable libmapper protocol" OFF) #use external libmapper

# FFT stuff
if("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
  set(OSSIA_COMPILER_IS_CLANG 1)
  set(OSSIA_COMPILER_IS_NOT_CLANG 0)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Emscripten")
  set(OSSIA_COMPILER_IS_CLANG 1)
  set(OSSIA_COMPILER_IS_NOT_CLANG 0)
else()
  set(OSSIA_COMPILER_IS_CLANG 0)
  set(OSSIA_COMPILER_IS_NOT_CLANG 1)
endif()

option(OSSIA_ENABLE_FFT "Enable FFT support" OFF)
option(OSSIA_ENABLE_FFTW "Enable FFT through FFTW" OFF)
option(OSSIA_ENABLE_KFR "Enable KFR library" OFF)

# List of all the available protocols
set(OSSIA_AVAILABLE_PROTOCOLS
  AUDIO MIDI
  OSC MINUIT OSCQUERY
  HTTP WEBSOCKETS SERIAL
  PHIDGETS
  LEAPMOTION
  JOYSTICK
  WIIMOTE
  ARTNET
  LIBMAPPER
)

option(OSSIA_DISABLE_QT_PLUGIN "Disable building of a Qt plugin" OFF)
option(OSSIA_DNSSD "Enable DNSSD support" ON)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/CMake;${PROJECT_SOURCE_DIR}/cmake/cmake-modules;")
set(OSSIA_SUBMODULE_AUTOUPDATE ON CACHE BOOL "Auto update submodule")

set(CMAKE_PREFIX_PATH
  "${OSSIA_SDK}/SDL2"
  "${OSSIA_SDK}/SDL2/cmake"
  "${OSSIA_SDK}/portaudio/lib/cmake"
  "${CMAKE_PREFIX_PATH}"
)

if(OSSIA_CPP_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSCQuery)

  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 0)
  set(OSSIA_PD 0)
  set(OSSIA_MAX 0)
  set(OSSIA_PYTHON 0)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 1)
  set(OSSIA_UNITY3D 0)
  set(OSSIA_QML 0)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 0)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_PD_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSC MINUIT OSCQUERY)

  set(OSSIA_NO_INSTALL 1)
  set(OSSIA_STATIC 1)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 0)
  set(OSSIA_PD 1)
  set(OSSIA_PYTHON 0)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 0)
  set(OSSIA_UNITY3D 0)
  set(OSSIA_QML 0)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 0)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_MAX_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSC MINUIT OSCQUERY)

  set(OSSIA_NO_INSTALL 1)
  set(OSSIA_STATIC 1)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 0)
  set(OSSIA_MAX 1)
  set(OSSIA_PYTHON 0)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 0)
  set(OSSIA_UNITY3D 0)
  set(OSSIA_QML 0)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 0)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_PYTHON_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSC MINUIT OSCQUERY MIDI)

  set(OSSIA_NO_INSTALL 1)
  set(OSSIA_STATIC 1)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 0)
  set(OSSIA_PD 0)
  set(OSSIA_MAX 0)
  set(OSSIA_PYTHON 1)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 0)
  set(OSSIA_UNITY3D 0)
  set(OSSIA_QML 0)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 0)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_QML_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSC MINUIT OSCQUERY)

  set(OSSIA_NO_INSTALL 0)
  set(OSSIA_STATIC 0)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 0)
  set(OSSIA_PD 0)
  set(OSSIA_MAX 0)
  set(OSSIA_PYTHON 0)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 0)
  set(OSSIA_UNITY3D 0)
  set(OSSIA_QML 1)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 1)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_UNITY3D_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSC MINUIT OSCQUERY)

  set(OSSIA_NO_INSTALL 1)
  set(OSSIA_STATIC 0)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 0)
  set(OSSIA_PD 0)
  set(OSSIA_MAX 0)
  set(OSSIA_PYTHON 0)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 0)
  set(OSSIA_UNITY3D 1)
  set(OSSIA_QML 0)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 0)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_JAVA_ONLY)
  set(OSSIA_OVERRIDE_PROTOCOLS OSCQUERY)

  set(OSSIA_NO_INSTALL 1)
  set(OSSIA_STATIC 0)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_EDITOR 0)
  set(OSSIA_GFX 0)
  set(OSSIA_JAVA 1)
  set(OSSIA_PD 0)
  set(OSSIA_MAX 0)
  set(OSSIA_PYTHON 0)
  set(OSSIA_QT 0)
  set(OSSIA_C 0)
  set(OSSIA_CPP 0)
  set(OSSIA_UNITY3D 0)
  set(OSSIA_QML 0)
  set(OSSIA_QML_SCORE 0)
  set(OSSIA_QML_DEVICE 0)
  set(OSSIA_DISABLE_QT_PLUGIN 0)
  set(OSSIA_DNSSD 1)
endif()

if(OSSIA_OVERRIDE_PROTOCOLS)
  foreach(protocol ${OSSIA_AVAILABLE_PROTOCOLS})
    set(OSSIA_PROTOCOL_${protocol} OFF)
  endforeach()

  foreach(protocol ${OSSIA_OVERRIDE_PROTOCOLS})
    string(TOUPPER ${protocol} protocol)
    set(OSSIA_PROTOCOL_${protocol} ON)
  endforeach()
endif()

if(OSSIA_QML)
    set(OSSIA_OSX_FAT_LIBRARIES 0)
    set(OSSIA_QT 1)
    set(OSSIA_NO_SONAME 1)
endif()
if(OSSIA_PD)
  set(OSSIA_HIDE_ALL_SYMBOLS 1)
endif()
if(OSSIA_MAX)
    set(OSSIA_OSX_FAT_LIBRARIES 1)
    set(OSSIA_HIDE_ALL_SYMBOLS 1)
endif()
if(OSSIA_UNITY3D)
    set(OSSIA_OSX_FAT_LIBRARIES 1)
    set(OSSIA_NO_SONAME 1)
    set(OSSIA_C 1)
endif()
if(OSSIA_JAVA)
    set(OSSIA_OSX_FAT_LIBRARIES 1)
    set(OSSIA_NO_SONAME 1)
    set(OSSIA_C 1)
endif()

if(OSSIA_OSX_FAT_LIBRARIES)
    string(REGEX MATCH "[0-9]+.[0-9]+" MACOS_SDK_VERSION "${CMAKE_OSX_SYSROOT}")
    if(MACOS_SDK_VERSION VERSION_LESS 10.14)
      set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
      set(OSSIA_PCH 0)
    else()
      message(WARNING "macOS ${MACOS_SDK_VERSION} do not support building 32-bit anymore.")
    endif()
endif()

if(OSSIA_NO_QT)
  set(OSSIA_QT 0)
  set(OSSIA_QML 0)
endif()

if(OSSIA_OSX_RETROCOMPATIBILITY)
  set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)

  if(OSSIA_MAX OR OSSIA_PD OR OSSIA_JAVA OR OSSIA_UNITY3D OR OSSIA_PYTHON)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif()

if(NOT OSSIA_DATAFLOW)
  set(OSSIA_EDITOR 0)
endif()


function(ossia_set_visibility TheTarget)
  if(OSSIA_STATIC_EXPORT)
    set_target_properties(${TheTarget} PROPERTIES
      C_VISIBILITY_PRESET default
      CXX_VISIBILITY_PRESET default
      VISIBILITY_INLINES_HIDDEN 0
    )
  else()
    set_target_properties(${TheTarget} PROPERTIES
      C_VISIBILITY_PRESET hidden
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
    )
  endif()
endfunction()


function(ossia_add_test TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME} ${TESTSRCS})
    target_compile_options(ossia_${TESTNAME} PUBLIC ${OSSIA_COMPILE_OPTIONS})
    if(APPLE)
      target_compile_definitions(ossia_${TESTNAME} PUBLIC  CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS)
    endif()
    target_include_directories(ossia_${TESTNAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/catch")
    target_link_libraries(ossia_${TESTNAME} PUBLIC ${OSSIA_LINK_OPTIONS} ossia PRIVATE Catch2::Catch2WithMain)

    add_test(NAME ossia_target_${TESTNAME} COMMAND ossia_${TESTNAME})
endFunction()
