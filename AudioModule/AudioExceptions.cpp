#include <stdexcept>

class PortAudioException : public std::exception {
 public:
  explicit PortAudioException(const std::string& message) : message_(message) {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
};

class AudioConverterException : public std::exception {
 public:
  explicit AudioConverterException(const std::string& message)
      : message_(message) {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
};
