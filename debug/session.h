//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <functional>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

using namespace std;

#include "loc.h"
#include "breakpoint.h"
#include "controller.h"
#include "command_interface.h"
#include "debug_command.h"

namespace Debug {

  class Session {
   public:
    virtual void EnterFile(const Loc &loc) {}

    virtual void LeaveFile() {}

    virtual void SetNextLine(const Loc &loc) {}

    virtual bool ShouldBreak(int lineno) { return false; }

  };

 class SessionBase : public Session, public CommandHandler {
   public:
    virtual void Break()= 0;

    virtual void Step()= 0;

    virtual void Continue()= 0;

    virtual void AddBreakPointer(const char *path, int lineno)= 0;

    virtual bool RemoveBreakPointer(int index)= 0;

    virtual void RemoveAllBreakPointer()= 0;

    typedef function<void(int, const string &, int)> loc_func;

    virtual void ForEachBreakPointer(const loc_func &func)= 0;

    virtual void ForEachFileInStack(const loc_func &func)= 0;

    virtual Message OnCommand(const Message &command)= 0;
  };

  void StartDebugSession();

  void StopDebugSession();

  Session *GetCurrentDebugSession();
}

#endif // DEBUG_SESSION_H
