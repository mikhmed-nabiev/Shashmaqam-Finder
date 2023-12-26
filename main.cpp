#include "UI/UIManager.hpp"
#include "Logger.hpp"

int main(int argc, char **argv) {

  Logger::SetLogLevel(LogLevel::kInfo);
  Logger::DisableFileStamp();

  if (argc == 2) {
    // TODO: 
    // 1. Loading folder
    // 2. Testing folder
  }

  AudioIdentifier app;
  app.Run();
}