cmake_minimum_required(VERSION 3.16)
project(Shashmaqam-Finder)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(AudioModule Math DB FingerPrint UI)

# Add your source files
add_executable(Shashmaqam-Finder
    AudioModule/AudioConverter.cpp
    AudioModule/AudioExceptions.cpp
    AudioModule/AudioRecorder.cpp
    Math/Field.cpp
    Math/FastFourierTransform.cpp
    DB/Database.cpp
    FingerPrint/FingerPrint.cpp
    UI/Button.cpp
    UI/PopUp.cpp
    UI/ShazamUI.cpp
    main.cpp
)

# Link libraries
target_link_libraries(Shashmaqam-Finder portaudio ssl crypto tag sqlite3 sfml-graphics sfml-window sfml-system)