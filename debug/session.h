//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <functional>
#include <stack>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

using namespace std;

#include "loc.h"
#include "breakpoint.h"
#include "controller.h"
#include "command_interface.h"

namespace Debug {

  class DebugCommand;

  class CommandMap : public unordered_map<string, shared_ptr<DebugCommand>> {
   public:
    CommandMap();
    virtual ~CommandMap() = default;
  };


  class Session : public CommandHandler {
   private:
    Controller controller;
    CommandMap command_map;

    //Break Pointers
    stack<Loc> file_stack;
    unique_ptr<BreakPoints> breakpoints_;

    //Break/Continue/Step Control Flags
    bool is_paused;
    mutex break_mutex_;
    condition_variable break_cond_;

    //Command Parser
    string last_command_;

    static Session *g_session;

    void SetCurrentFile();

    Message ParseCommand(const Message &command);
    DebugCommand* GetErrorCommand();
    DebugCommand* GetCommand(const string& command);

   public:
    explicit Session();

    static void Start();

    static Session *GetCurrentSession();

    static void Stop();

    void EnterFile(const Loc &loc);

    void LeaveFile();

    void SetNextLine(const Loc &loc);

    bool ShouldBreak(int lineno);

   public:
    void Break();
    void Step();
    void Continue();
    void AddBreakPointer(const char *path, int lineno);
    bool RemoveBreakPointer(int index);
    void RemoveAllBreakPointer();
    typedef function<void(int,const BreakPoint&)> bp_func;
    void ForeachBreakPointer(bp_func func);

   public:
    Message OnCommand(const Message &command) override;
  };
}

void StartDebugSession();

Debug::Session *GetCurrentDebugSession();

void StopDebugSession();

#endif // DEBUG_SESSION_H
