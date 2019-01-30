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
    explicit Message(const string& command);
    explicit Message(const char *tag);
    Message(const char *tag, const char *response);

   public:
    const string& GetMessage() const {
      return msg_;
    }

    void WriteLine(const char *tag, const char *response = nullptr) {
      msg_ += tag;
      if (response != nullptr) {
        msg_ += response;
      }
      msg_ += '\n';
    }

    void EndResponse() {
      WriteLine("<end>");
    }

    void Write(int fd) const {
      write(fd, msg_.c_str(), msg_.size());
    }

  };

  class Connector {
   public:
    virtual void OnResponse(const Message &response) = 0;

    virtual int GetInputHandle() = 0;

    virtual int GetOutputHandle() = 0;
  };

  Connector* GetDefaultConnector();
}

#endif // DEBUG_COMMAND_INTERFACE_H
