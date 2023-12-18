#include "Utils.hpp"

namespace fs = std::filesystem;

void AddFingerPrintsToDB(
    DataBase& db_,
    const std::vector<std::pair<std::string, int>>& finger_prints,
    int song_id) {
  db_.BeginTransaction();

  std::string query =
      "INSERT INTO HASHES (SONGID, Offset, HashValue) VALUES (?, ?, ?)";

  sqlite3_stmt* statement;
  if (sqlite3_prepare_v2(db_.GetDb(), query.c_str(), -1, &statement, nullptr) !=
      SQLITE_OK) {
    db_.RollbackTransaction();
    return;
  }

  sqlite3_bind_int(statement, 1, song_id);

  for (const auto& fingerprint : finger_prints) {
    auto [hash_value, offset] = fingerprint;

    sqlite3_bind_int(statement, 2, offset);
    sqlite3_bind_text(statement, 3, hash_value.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(statement) != SQLITE_DONE) {
      db_.RollbackTransaction();
      sqlite3_finalize(statement);
      return;
    }

    sqlite3_reset(statement);
  }

  sqlite3_finalize(statement);
  db_.CommitTransaction();
}

void AddDataToDB(DataBase& db_, const fs::path& file_path) {
  std::cout << "Processing " << file_path.filename() << "..." << std::endl;
  AudioConverter convertor(file_path.string());
  auto spectogram = GenerateSpectrogram(convertor.GetData());
  auto finger_prints = GenerateFingerPrints(spectogram);
  std::cout << "Done fingerpinting" << std::endl;
  std::cout << "\t Adding to DB..." << std::endl;
  int song_id = db_.getSongRowCount();
  Song song(file_path.filename().string(), song_id);
  db_.AddSong(song);
  AddFingerPrintsToDB(db_, finger_prints, song_id);
  std::cout << "\n\n" << std::endl;
}

void Utils::LoadFolder(DataBase& db_, const fs::path& folder_path) {
  for (const auto& entry : fs::directory_iterator(folder_path)) {
    if (fs::is_directory(entry.path())) {
      LoadFolder(db_, entry.path());
    } else if (entry.is_regular_file() && entry.path().extension() == ".wav") {
      AddDataToDB(db_, entry.path());
    }
  }
}

void Recognize(DataBase& db_, const fs::path& file_path) {
  std::cout << "Recognizing " << file_path.filename() << "..." << std::endl;
  AudioConverter convertor(file_path.string());
  auto spectogram = GenerateSpectrogram(convertor.GetData());
  auto finger_prints = GenerateFingerPrints(spectogram);
  std::cout << "\tStatistic\n  Artist     Title     Score" << std::endl;
  std::optional<Song> song = db_.getBestMatch(finger_prints);
  if (song != std::nullopt) {
    std::cout << "Best match : "
              << " " << song->getArtist() << song->getTitle() << " "
              << song->getID() << std::endl;
  } else {
    std::cout << "Can't  recognize song" << std::endl;
  }
  std::cout << "=================================================" << std::endl;
}

void Utils::TestFolder(DataBase& db_, const fs::path& folder_path) {
  for (const auto& entry : fs::directory_iterator(folder_path)) {
    if (fs::is_directory(entry.path())) {
      LoadFolder(db_, entry.path());
    } else if (entry.is_regular_file() && entry.path().extension() == ".wav") {
      Recognize(db_, entry.path());
    }
  }
}
