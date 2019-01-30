//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

#include "loc.h"
#include "breakpoint.h"
#include "controller.h"
#include "command_interface.h"

namespace Debug {
  class Session : public CommandHandler {
   private:
    Controller controller;

    //Break Pointers
    vector<Loc> file_stack;
    BreakPointMap bp_map;
    BreakPointFile *breakable_file;
    int next_lineno;

    //Break/Continue/Step Control Flags
    bool is_paused;
    mutex break_mutex_;
    condition_variable break_cond_;

    //Command Parser
    string last_command_;

    static Session *g_session;

    void CheckBreakPoint();

    Message ParseCommand(const Message &command);

   public:
    explicit Session();

    static void Start();

    static Session *GetCurrentSession();

    static void Stop();

    void EnterFile(const Loc &loc);

    void LeaveFile();

    void SetNextLine(const Loc &loc);

    bool ShouldBreak();

   public:
    void Break();
    void Step();
    void Continue();
    void AddBreakPointer(const char *path, int lineno);
    void RemoveBreakPointer(const char *path, int lineno);
    void RemoveAllBreakPointer();

   public:
    Message OnCommand(const Message &command) override;
  };
}

void StartDebugSession();

Debug::Session *GetCurrentDebugSession();

void StopDebugSession();

#endif // DEBUG_SESSION_H
