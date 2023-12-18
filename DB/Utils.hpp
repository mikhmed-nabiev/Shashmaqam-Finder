#include <filesystem>

#include "Database.hpp"
#include "../Math/Spectrogram.hpp"
#include "../FingerPrint/FingerPrint.hpp"
#include "../AudioModule/AudioConverter.hpp"

namespace Utils {
    // Load all .wav file into db_
    void LoadFolder(DataBase& db_, const std::filesystem::path& folder_path);

    // Recognize all .wav files in folder
    void TestFolder(DataBase& db_, const std::filesystem::path& folder_path);
}