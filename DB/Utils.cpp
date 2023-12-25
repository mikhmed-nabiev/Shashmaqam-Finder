#include "Utils.hpp"
#include "Logger.hpp"

namespace fs = std::filesystem;

#include <thread>

int negs = 0, pos = 0;

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
  // LOG_DEBUG("Processing ", file_path.filename().string(), "...");
  AudioConverter convertor(file_path.string());
  auto spectogram = GenerateSpectrogram(convertor.GetData());
  auto finger_prints = GenerateFingerPrints(spectogram);
  // std::cout << "\t Adding to DB..." << std::endl;
  LOG_DEBUG("Size = ", finger_prints.size());
  LOG_DEBUG("Adding to DB...");
  int song_id = db_.getSongRowCount();
  Song song(file_path.filename().string(), song_id);
  db_.AddSong(song);
  AddFingerPrintsToDB(db_, finger_prints, song_id);
  LOG_DEBUG("Processed with ", file_path.filename().string(), "\n\n");
  // std::cout << "\n\n" << std::endl;
}

void Utils::LoadFolder(DataBase& db_, const fs::path& folder_path) {
  int i = 0;
  LOG_INFO("Started Loading Folder");
  for (const auto& entry : fs::directory_iterator(folder_path)) {
    if (fs::is_directory(entry.path())) {
      LoadFolder(db_, entry.path());
    } else if (entry.is_regular_file() && entry.path().extension() == ".wav") {
      LOG_INFO( i++, "Loading ", entry.path().string());
      AddDataToDB(db_, entry.path());
      // std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
  LOG_INFO("Loaded Folder");
}

void Recognize(DataBase& db_, const fs::path& file_path) {
  // std::cout << "Recognizing " << file_path.filename() << "..." << std::endl;
  LOG_DEBUG("Recognizing ", file_path.filename(), "...");
  AudioConverter convertor(file_path.string());
  auto spectogram = GenerateSpectrogram(convertor.GetData());
  auto finger_prints = GenerateFingerPrints(spectogram);
  // std::cout << << std::endl;
  LOG_TRACE("\t\t\tStatistic\n\t\t\t\t\tArtist\tTitle\tScore");
  std::optional<Song> song = db_.getBestMatch(finger_prints);
  std::optional<Song> rSong = Song(file_path.filename().string(), -1);
  if (song != std::nullopt) {
    LOG_DEBUG("Best match :",song->getArtist(), song->getTitle(),song->getID());
    if (rSong->getTitle() == song->getTitle() && rSong->getArtist() == song->getArtist()) {
      LOG_INFO("MAATTTCCCHHH!");
      pos++;
    } else {
      LOG_ERROR(rSong->getTitle() , "==" , song->getTitle() , "&&", rSong->getArtist(), "==" , song->getArtist());
      LOG_ERROR("AHHH, NOOOO!");
      negs++;
    }
  } else {
    // std::cout << "Can't  recognize song" << std::endl;
    negs++;
    LOG_ERROR("Can't  recognize song");
  }
  LOG_DEBUG("=================================================");
  // std::cout << "=================================================" << std::endl;
}

void Utils::TestFolder(DataBase& db_, const fs::path& folder_path) {
  LOG_INFO("Started Testing Folder");
  std::vector<fs::path> wav_files;
  int idx = 0;

  for (const auto& entry : fs::directory_iterator(folder_path)) {
    if (fs::is_directory(entry.path())) {
      LoadFolder(db_, entry.path());
    } else if (entry.is_regular_file() && entry.path().extension() == ".wav") {
      wav_files.push_back(entry.path());
    }
  }

  std::sort(wav_files.begin(),wav_files.end());

  for (const auto& entry : wav_files) {

      LOG_INFO("TEST #", ++idx);
      // LOG_DEBUG("Recognizing ", entry.filename(), "...");

      Recognize(db_, entry);
  }

  std::cout << wav_files.size() << std::endl;

  LOG_INFO("Done Testing Folder");
  LOG_TRACE("Summary:\n\t MATCHES = ", pos, "\n\t NEGATVS", negs);
}
/*
NEGS = 19
POS = 41
*/