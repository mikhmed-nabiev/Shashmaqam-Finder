#include <string>
// #include "UI/UIManager.hpp"
#include "Logger.hpp"
#include "DB/Utils.hpp"

using namespace Utils;

int main() {

  Logger::SetLogLevel(LogLevel::kTrace);
  Logger::DisableFileStamp();
  Logger::SetStream(OutputStream::kBoth);
  Logger::SetLogFile("20Songs.txt");
  // std::string DBTEST_folderPath = "music/DBTEST";
  // std::string Fulls_folderPath = "../music/Fulls";
  std::string Samples_folderPath = "../music/Samples";
  // std::string SamplesTest_folderPath = "../music/SamplesTEST";

  DataBase db("20SongsCP.db");

  try {
    // LoadFolder(db, Fulls_folderPath);
    TestFolder(db, Samples_folderPath);
    // TestFolder(db, SamplesTest_folderPath);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
