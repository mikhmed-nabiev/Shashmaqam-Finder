#ifndef SHASHMAQAMFINDER_DATABASE_HPP_
#define SHASHMAQAMFINDER_DATABASE_HPP_

#include <stdexcept>
#include <sqlite3.h>
#include <string>

class Song {
public:
    Song(const std::string& artist, const std::string& album, const std::string& title, const int& id);

    // Getters
    std::string getArtist() const;
    std::string getAlbum() const;
    std::string getName() const;
    int getID() const;

private:
    std::string artist_;
    std::string album_;
    std::string title_;
    int song_id_;
};


class Hash {
public:
    Hash(const std::string& hashValue, const int& ID);

    // Getters
    std::string getHashValue() const;
    int getID() const;
    
private:
    std::string hash_value_;
    int id_;
};


class DataBase {
public:
    DataBase(const std::string& name);
    
    ~DataBase();

    void AddSong(const Song& song);
    void AddHash(const Hash& hash);

private:
    sqlite3* db_;
    std::string db_name_;

    void CreateTables();
    void ExecuteQuery(const std::string& query);
};

#endif //SHASHMAQAMFINDER_DATABASE_HPP_