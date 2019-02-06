#include <unistd.h>
#include <string.h>
#include "command_interface.h"

namespace Debug {
  Message::Message(const string &command) {
    msg_ = command;
  }

  Message::Message(const char *tag) {
    if (strcmp(tag, "end") != 0) {
      WriteLine(tag);
    }
    EndResponse();
  }

  Message::Message(const char *tag, const char *response) {
    WriteLine(tag, response);
    EndResponse();
  }
}
