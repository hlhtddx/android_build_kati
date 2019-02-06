//
// Created by 张航 on 2019-02-05.
//

#ifndef DEBUG_COMMAND_H
#define DEBUG_COMMAND_H

#include "command_interface.h"
#include <unordered_map>

using namespace std;

namespace Debug {
  class SessionBase;

  class DebugCommand {
   public:
    explicit DebugCommand(const char *tag) : tag_(tag) {}

    virtual Message Execute(SessionBase *session, const string &arguments) {
      return Message("error");
    }

   protected:
    const char *tag_;
  };

  class CommandMap : public unordered_map<string, shared_ptr<DebugCommand>> {
   public:
    CommandMap();

    virtual ~CommandMap() = default;
  };

}

#endif //DEBUG_COMMAND_H
