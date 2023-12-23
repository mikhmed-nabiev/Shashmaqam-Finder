#include <string>
#include "DB/Utils.hpp"

using namespace Utils;

int main() {
  // std::string DBTEST_folderPath = "music/DBTEST";
  // std::string Fulls_folderPath = "music/Fulls";
  std::string Samples_folderPath = "music/Samples";
  // std::string SamplesTest_folderPath = "music/SamplesTEST";

  DataBase db("TESTDB.db");

  try {
    // LoadFolder(db, folderPath);
    TestFolder(db, Samples_folderPath);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
