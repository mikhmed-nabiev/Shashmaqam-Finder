#ifndef SHASHMAQAMFINDER_DATABASE_HPP_
#define SHASHMAQAMFINDER_DATABASE_HPP_

#include <sqlite3.h>

#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


const size_t BATCH_SIZE = 1000;
class Song {
 public:
  Song(const std::string& artist, const std::string& title,
       const std::string& album, int64_t song_id_);
  Song(const std::string& file_path, int64_t song_id_);

  // Getters
  std::string getArtist() const;
  std::string getAlbum() const;
  std::string getTitle() const;
  int64_t getID() const;

  // bool operator =(const Song& other);

 private:
  std::string artist_;
  std::string title_;
  std::string album_;
  int64_t song_id_;
};

class Hash {
 public:
  Hash(const std::string& hashValue, const int64_t offset, const int64_t ID);

  // Getters
  std::string getHashValue() const;
  int64_t getID() const;
  int64_t getOffset() const;

 private:
  std::string hash_value_;
  int64_t offset_;
  int64_t id_;
};

class DataBase {
 public:
  DataBase(const std::string& name);
  ~DataBase();

  sqlite3* GetDb() const;
  std::optional<Song> getBestMatch(
      const std::vector<std::pair<std::string, int>>& HashValues) const;

  std::optional<Song> GetSongById(const int64_t id) const;

  void AddSong(const Song& song);
  void AddHash(const Hash& hash);

  void BeginTransaction();
  void CommitTransaction();
  void RollbackTransaction();

  int64_t getSongRowCount() const;

 private:
  sqlite3* db_;
  std::string db_name_;

  void CreateTables();
  void ExecuteQuery(const std::string& query);

  Song ExtractSongFromStatement(sqlite3_stmt* statement) const;

  std::unordered_map<int, std::vector<int>> GetMatches(
      const std::vector<std::pair<std::string, int>>& HashValues,
      int64_t threshold = 5) const;

  void ProcessHashValuesBatch(
      const std::vector<std::pair<std::string, int>>& hashValues,
      std::unordered_map<int, std::vector<int>>& songMatches) const;

  void FilterMatches(
      std::unordered_map<int, std::unordered_map<int, int>>& songMatches,
      const std::unordered_map<int, int>& songMatchCounts,
      int64_t threshold) const;

  void BatchHashValues(
      const std::vector<std::pair<std::string, int>>& hashValues,
      std::function<void(const std::vector<std::pair<std::string, int>>&)>&&
          processBatch) const;
};

#endif  // SHASHMAQAMFINDER_DATABASE_HPP_