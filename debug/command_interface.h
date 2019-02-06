//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_COMMAND_INTERFACE_H
#define DEBUG_COMMAND_INTERFACE_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

#include <unistd.h>

namespace Debug {
  class Message {
   private:
    string msg_;
   public:
    explicit Message() = default;

    explicit Message(const string &command);

    explicit Message(const char *tag);

    Message(const char *tag, const char *response);

   public:
    const string &GetMessage() const {
      return msg_;
    }

    void Clear() {
      msg_.clear();
    }

    void WriteLine(const char *tag, const char *response = nullptr) {
      stringstream line;
      line << '<' << tag << '>';
      if (response != nullptr) {
        line << response;
      }
      line << endl;
      msg_ += line.str();
    }

    void EndResponse() {
      WriteLine("end");
    }

    void Write(int fd) const {
      write(fd, msg_.c_str(), msg_.size());
    }

  };

  class Connector {
   public:
    // Wait for client connected
    virtual bool WaitForConnection() = 0;

    // Callback when response comes
    virtual void OnResponse(const Message &response) = 0;

    // Get file handle for command (input stream)
    virtual int GetInputHandle() = 0;

    // Get file handle for response (output stream)
    virtual int GetOutputHandle() = 0;
  };

  Connector *GetConnector(const char *connector);
}

#endif // DEBUG_COMMAND_INTERFACE_H
