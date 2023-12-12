#ifndef SHASHMAQAMFINDER_DATABASE_HPP_
#define SHASHMAQAMFINDER_DATABASE_HPP_

#include <sqlite3.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Song {
 public:
  Song(const std::string& artist, const std::string& title,
       const std::string& album, int song_id_);
  Song(const std::string& file_path, int song_id_);

  // Getters
  std::string getArtist() const;
  std::string getAlbum() const;
  std::string getName() const;
  int getID() const;

  // bool operator =(const Song& other);

 private:
  std::string artist_;
  std::string title_;
  std::string album_;
  int song_id_;
};

class Hash {
 public:
  Hash(const std::string& hashValue, const int offset, const int ID);

  // Getters
  std::string getHashValue() const;
  int getID() const;
  int getOffset() const;

 private:
  std::string hash_value_;
  int offset_;
  int id_;
};

class DataBase {
 public:
  DataBase(const std::string& name);
  ~DataBase();

  Song getBestMatch(
      const std::vector<std::pair<std::string, int>>& HashValues) const;

  Song GetSongById(const int id) const;

  void AddSong(const Song& song);
  void AddHash(const Hash& hash);

  int getSongRowCount() const;

 private:
  sqlite3* db_;
  std::string db_name_;

  void CreateTables();
  void ExecuteQuery(const std::string& query);

  std::unordered_map<int, std::unordered_map<int, int>> GetMatches(
      const std::vector<std::pair<std::string, int>>& HashValues,
      int threshold = 5) const;
};

#endif  // SHASHMAQAMFINDER_DATABASE_HPP_