#include "Database.hpp"

Song::Song(const std::string& artist, const std::string& title,
           const std::string& album, const int song_id)
    : artist_(artist), title_(title), album_(album), song_id_(song_id) {}

// Костыль
Song::Song(const std::string& file_path, const int song_id)
    : song_id_(song_id) {
  std::vector<std::string> metadatas(3, "");
  std::string filltred_file_path(file_path.begin(), file_path.end() - 4);

  std::string curr_data = "";
  size_t idx = 0;

  for (auto it : filltred_file_path) {
    if (it == '_') {
      metadatas[idx] = curr_data;
      curr_data = "";
      idx++;
    } else {
      curr_data += it;
    }
  }

  if (!curr_data.empty()) {
    metadatas[idx] = curr_data;
  }

  artist_ = metadatas[0];
  title_ = metadatas[1];
  album_ = metadatas[2];
}

std::string Song::getArtist() const { return artist_; }

std::string Song::getAlbum() const { return album_; }

std::string Song::getName() const { return title_; }

int Song::getID() const { return song_id_; }

// bool Song::operator=(const Song& other) {
//     return (other.album_ == album_) && (other.title_ == title_) &&
//     (other.title_ == title_);
// }

Hash::Hash(const std::string& hash_value, const int offset, const int id)
    : hash_value_(hash_value), offset_(offset), id_(id) {}

std::string Hash::getHashValue() const { return hash_value_; }

int Hash::getID() const { return id_; }

int Hash::getOffset() const { return offset_; }

DataBase::DataBase(const std::string& db_name) : db_name_(db_name) {
  int rc = sqlite3_open(db_name.c_str(), &db_);

  if (rc != SQLITE_OK) {
    throw std::runtime_error(" Can't open database: " +
                             std::string(sqlite3_errmsg(db_)));
  }

  CreateTables();
}

DataBase::~DataBase() { sqlite3_close(db_); }

void DataBase::CreateTables() {
  // Create SONG table with unique ID
  ExecuteQuery(
      "CREATE TABLE IF NOT EXISTS SONG (ID INTEGER PRIMARY KEY UNIQUE, Artist "
      "TEXT, Album TEXT, Name TEXT)");

  // Create HASHES table with unique ID
  ExecuteQuery(
      "CREATE TABLE IF NOT EXISTS HASHES (ID INTEGER PRIMARY KEY, SONGID "
      "INTEGER, Offset INTEGER, HashValue TEXT)");
}

void DataBase::ExecuteQuery(const std::string& query) {
  int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, nullptr);

  if (rc != SQLITE_OK) {
    std::cout << query.c_str() << '\n';
    throw std::runtime_error("Query execution failed: " +
                             std::string(sqlite3_errmsg(db_)));
  }
}

void DataBase::AddSong(const Song& song) {
  std::string query = "INSERT INTO SONG (ID, Artist, Album, Name) VALUES (" +
                      std::to_string(song.getID()) + ", '" + song.getArtist() +
                      "', '" + song.getAlbum() + "', '" + song.getName() + "')";

  try {
    ExecuteQuery(query);
  } catch (std::runtime_error& e) {
    // std::cout << "Hi from AddSong\n";
    throw;
  }
}
int64_t x = 0;
void DataBase::AddHash(const Hash& hash) {
  std::string query =
      "INSERT INTO HASHES (SONGID, Offset, HashValue) VALUES (" +
      std::to_string(hash.getID()) + "," + std::to_string(hash.getOffset()) +
      ", \'" + hash.getHashValue() + "\')";

  ExecuteQuery(query);
}

/*
  BUG : There's bag after second query "Smth with API issueR"
*/
std::unordered_map<int, std::unordered_map<int, int>> DataBase::GetMatches(
    const std::vector<std::pair<std::string, int>>& hashValues,
    int threshold) const {
  std::unordered_map<int, std::unordered_map<int, int>> matches;
  std::unordered_map<int, int> mathes_size;

  for (const auto& hashValue : hashValues) {
    std::string query =
        "SELECT SONGID, Offset FROM HASHES WHERE HashValue = '" +
        hashValue.first + "'";
    sqlite3_stmt* statement;

    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, nullptr) ==
        SQLITE_OK) {
      // Bind the parameter
      sqlite3_bind_text(statement, 1, hashValue.first.c_str(), -1,
                        SQLITE_STATIC);

      while (sqlite3_step(statement) == SQLITE_ROW) {
        int songId = sqlite3_column_int(statement, 0);
        int offset = sqlite3_column_int(statement, 1);
        matches[songId][offset - hashValue.second]++;
        mathes_size[songId]++;
      }

      sqlite3_finalize(statement);
    } else {
      std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db_)
                << std::endl;
      int x;
      std::cin >> x;
    }
  }

  // Filter matches based on threshold
  for (auto it = matches.begin(); it != matches.end();) {
    std::cout << it->first << " " << it->second.size() << ' '
              << mathes_size[it->first] << std::endl;
    if (mathes_size[it->first] < threshold) {
      it = matches.erase(it);
    } else {
      ++it;
    }
  }

  return matches;
}

Song DataBase::GetSongById(const int id) const {
  std::string query = "SELECT * FROM SONG WHERE ID = " + std::to_string(id);
  sqlite3_stmt* statement;

  if (sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, nullptr) ==
      SQLITE_OK) {
    if (sqlite3_step(statement) == SQLITE_ROW) {
      std::string artist =
          reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
      std::string title =
          reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
      std::string album =
          reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
      int songId = sqlite3_column_int(statement, 0);

      // Assuming you have a constructor that takes artist, title, album, and id
      Song song(artist, title, album, songId);

      sqlite3_finalize(statement);
      return song;
    } else {
      std::cerr << "No song found with ID: " << id << std::endl;
    }
  } else {
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db_)
              << std::endl;
  }

  // Return an "empty" song if not found
  return Song("", "", "", -1);
}

Song DataBase::getBestMatch(
    const std::vector<std::pair<std::string, int>>& hashValues) const {
  std::unordered_map<int, std::unordered_map<int, int>> matches =
      GetMatches(hashValues);
  int id = 0;
  int cur_mx = 0;
  for (auto it : matches) {
    for (auto it2 : it.second) {
      if (it2.second > cur_mx) {
        id = it.first;
        cur_mx = it2.second;
      }
    }
  }
  std::cout << id << std::endl;
  return GetSongById(id);
}

int DataBase::getSongRowCount() const {
  std::string query = "SELECT COUNT(*) FROM SONG";
  sqlite3_stmt* statement;
  int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, nullptr);

  rc = sqlite3_step(statement);

  if (rc == SQLITE_ROW) {
    int rowCount = sqlite3_column_int(statement, 0);
    sqlite3_finalize(statement);

    return rowCount;
  } else {
    std::cerr << "Error executing COUNT statement: " << sqlite3_errmsg(db_)
              << std::endl;
    sqlite3_finalize(statement);
    return -1;
  }
}
