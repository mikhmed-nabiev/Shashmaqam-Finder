#include "Database.hpp"

/*-------------------Song-------------------*/
Song::Song(const std::string& artist, const std::string& title,
           const std::string& album, const int64_t song_id)
    : artist_(artist), title_(title), album_(album), song_id_(song_id) {}

// Костыль
Song::Song(const std::string& file_path, const int64_t song_id)
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

std::string Song::getTitle() const { return title_; }

int64_t Song::getID() const { return song_id_; }

/*-------------------Hash-------------------*/

Hash::Hash(const std::string& hash_value, const int64_t offset,
           const int64_t id)
    : hash_value_(hash_value), offset_(offset), id_(id) {}

std::string Hash::getHashValue() const { return hash_value_; }

int64_t Hash::getID() const { return id_; }

int64_t Hash::getOffset() const { return offset_; }

/*-------------------DataBase-------------------*/
DataBase::DataBase(const std::string& db_name) : db_name_(db_name) {
  int64_t rc = sqlite3_open(db_name.c_str(), &db_);

  if (rc != SQLITE_OK) {
    throw std::runtime_error("Can't open database: " +
                             std::string(sqlite3_errmsg(db_)));
  }

  CreateTables();
}

DataBase::~DataBase() { sqlite3_close(db_); }

sqlite3* DataBase::GetDb() const { return db_; }

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
  int64_t rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, nullptr);

  if (rc != SQLITE_OK) {
    std::cerr << query.c_str() << '\n';
    throw std::runtime_error("Query execution failed: " +
                             std::string(sqlite3_errmsg(db_)));
  }
}

void DataBase::AddSong(const Song& song) {
  std::string query = "INSERT INTO SONG (ID, Artist, Album, Name) VALUES (" +
                      std::to_string(song.getID()) + ", '" + song.getArtist() +
                      "', '" + song.getAlbum() + "', '" + song.getTitle() +
                      "')";

  ExecuteQuery(query);
}

void DataBase::AddHash(const Hash& hash) {
  std::string query =
      "INSERT INTO HASHES (SONGID, Offset, HashValue) VALUES (" +
      std::to_string(hash.getID()) + "," + std::to_string(hash.getOffset()) +
      ", '" + hash.getHashValue() + "')";

  ExecuteQuery(query);
}

std::optional<Song> DataBase::GetSongById(const int64_t id) const {
  std::string query = "SELECT * FROM SONG WHERE ID = " + std::to_string(id);
  sqlite3_stmt* statement = nullptr;

  if (sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, nullptr) ==
      SQLITE_OK) {
    if (sqlite3_step(statement) == SQLITE_ROW) {
      Song song = ExtractSongFromStatement(statement);
      sqlite3_finalize(statement);
      return song;
    } else {
      std::cerr << "No song found with ID: " << id << std::endl;
    }
  } else {
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db_)
              << std::endl;
  }

  return std::nullopt;
}

Song DataBase::ExtractSongFromStatement(sqlite3_stmt* statement) const {
  const char* ptr_artist =
      reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
  const char* ptr_album =
      reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
  const char* ptr_title =
      reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));

  std::string artist = (ptr_artist != nullptr) ? ptr_artist : "";
  std::string title = (ptr_title != nullptr) ? ptr_title : "";
  std::string album = (ptr_album != nullptr) ? ptr_album : "";
  int64_t song_id = sqlite3_column_int(statement, 0);

  return Song(artist, title, album, song_id);
}

void DataBase::ProcessHashValuesBatch(
    const std::vector<std::pair<std::string, int>>& hashValues,
    std::unordered_map<int, std::vector<int>>& songMatches) const {
  std::string query = "SELECT SONGID, Offset FROM HASHES WHERE HashValue IN (";
  for (size_t i = 0; i < hashValues.size(); ++i) {
    query += "?";
    if (i < hashValues.size() - 1) {
      query += ",";
    }
  }
  query += ")";

  sqlite3_stmt* statement;
  if (sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, nullptr) !=
      SQLITE_OK) {
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db_)
              << std::endl;
    return;
  }

  for (size_t i = 0; i < hashValues.size(); ++i) {
    if (sqlite3_bind_text(statement, i + 1, hashValues[i].first.c_str(), -1,
                          SQLITE_STATIC) != SQLITE_OK) {
      std::cerr << "Error binding SQL statement: " << sqlite3_errmsg(db_)
                << std::endl;
      return;
    }
  }

  while (sqlite3_step(statement) == SQLITE_ROW) {
    int64_t songId = sqlite3_column_int(statement, 0);
    int64_t offset = sqlite3_column_int(statement, 1);
    for (const auto& hashValue : hashValues) {
      songMatches[songId].push_back(offset - hashValue.second);
    }
  }

  sqlite3_finalize(statement);
}

void DataBase::BatchHashValues(
    const std::vector<std::pair<std::string, int>>& hashValues,
    std::function<void(const std::vector<std::pair<std::string, int>>&)>&&
        processBatch) const {
  for (size_t i = 0; i < hashValues.size(); i += BATCH_SIZE) {
    size_t batchSize = std::min(BATCH_SIZE, hashValues.size() - i);
    std::vector<std::pair<std::string, int>> batch(
        hashValues.begin() + i, hashValues.begin() + i + batchSize);

    processBatch(batch);
  }
}

std::unordered_map<int, std::vector<int>> DataBase::GetMatches(
    const std::vector<std::pair<std::string, int>>& hashValues,
    int64_t threshold) const {
  std::unordered_map<int, std::vector<int>> songMatches;

  BatchHashValues(hashValues,
                  [&](const std::vector<std::pair<std::string, int>>& batch) {
                    ProcessHashValuesBatch(batch, songMatches);
                  });

  return songMatches;
}

int ScoreMatch(const std::vector<int>& offsets) {
  const double bin_width = 0.5;

  int min_tk = *std::min_element(offsets.begin(), offsets.end());
  int max_tk = *std::max_element(offsets.begin(), offsets.end());

  int num_bins = static_cast<int>((max_tk - min_tk) / bin_width) + 1;
  std::vector<int> hist(num_bins, 0);
  int hist_mx = 0;
  for (const auto& tk : offsets) {
    int bin_index = static_cast<int>((tk - min_tk) / bin_width);
    hist[bin_index]++;
  }

  return *std::max_element(hist.begin(), hist.end());
}

std::optional<Song> DataBase::getBestMatch(
    const std::vector<std::pair<std::string, int>>& hashValues) const {
  auto matches = GetMatches(hashValues);
  int best_score = 0;
  int64_t id = -1;
  for (const auto& match : matches) {
    int song_id = match.first;
    const auto& offsets = match.second;
    if (offsets.size() < best_score) {
      continue;
    }
    int score = ScoreMatch(offsets);

    std::optional<Song> song = GetSongById(song_id);

    std::cout << song->getArtist() << " " << song->getTitle() << " - " << score
              << std::endl;

    if (score > best_score) {
      best_score = score;
      id = song_id;
    }
  }

  return GetSongById(id);
}

void DataBase::BeginTransaction() { ExecuteQuery("BEGIN TRANSACTION;"); }

void DataBase::CommitTransaction() { ExecuteQuery("COMMIT;"); }

void DataBase::RollbackTransaction() { ExecuteQuery("ROLLBACK;"); }

int64_t DataBase::getSongRowCount() const {
  std::string query = "SELECT COUNT(*) FROM SONG";
  sqlite3_stmt* statement;
  int64_t rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &statement, nullptr);

  rc = sqlite3_step(statement);

  if (rc == SQLITE_ROW) {
    int64_t rowCount = sqlite3_column_int(statement, 0);
    sqlite3_finalize(statement);

    return rowCount;
  } else {
    std::cerr << "Error executing COUNT statement: " << sqlite3_errmsg(db_)
              << std::endl;
  }
  sqlite3_finalize(statement);
  return -1;
}
