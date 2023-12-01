#include "Database.hpp"
#include <iostream>

Song:: Song(const std::string& artist, const std::string& album, const std::string& title, const int& id)
    :artist_(artist), album_(album), title_(title) ,song_id_(id) {}

std::string Song::getArtist() const {
    return artist_;
}

std::string Song::getAlbum() const {
    return album_;
}

std::string Song::getName() const {
    return title_;
}

int Song::getID() const {
    return song_id_;
}

Hash::Hash(const std::string& hash_value, const int& id): hash_value_(hash_value), id_(id) {}

std::string Hash::getHashValue() const {
    return hash_value_;
}

int Hash::getID() const {
    return id_;
}

DataBase::DataBase(const std::string& db_name): db_name_(db_name) {
    int rc = sqlite3_open(db_name.c_str(), &db_);

    if (rc != SQLITE_OK) {
        throw std::runtime_error(" Can't open database: " + std::string(sqlite3_errmsg(db_)));
    }
    
    // Create tables if they don't exist
    CreateTables();
}

DataBase::~DataBase() {
    sqlite3_close(db_);
}


void DataBase::CreateTables() {
    // Create SONG table with unique ID
    ExecuteQuery("CREATE TABLE IF NOT EXISTS SONG (ID INTEGER PRIMARY KEY UNIQUE, Artist TEXT, Album TEXT, Name TEXT)");
   
    // Create HASHES table with unique ID
    ExecuteQuery("CREATE TABLE IF NOT EXISTS HASHES (ID INTEGER PRIMARY KEY, SONGID INTEGER, HashValue TEXT)");
}

void DataBase::ExecuteQuery(const std::string& query) {
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, nullptr);

    if (rc != SQLITE_OK) {
        std::cout << query.c_str() << '\n';
        throw std::runtime_error("Query execution failed: " + std::string(sqlite3_errmsg(db_)));
    }
}

void DataBase::AddSong(const Song& song) {
    std::string query = "INSERT INTO SONG (ID, Artist, Album, Name) VALUES (" +
                    std::to_string(song.getID()) + ", '" + song.getArtist() + "', '" +
                    song.getAlbum() + "', '" + song.getName() + "')";

    ExecuteQuery(query);
}

void DataBase::AddHash(const Hash& hash) {
    std::string query = "INSERT INTO HASHES (SONGID, HashValue) VALUES (" +
                    std::to_string(hash.getID()) + ", \'" + hash.getHashValue() + "\')";
    
    ExecuteQuery(query);
}