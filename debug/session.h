//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <vector>

using std::vector;

#include "loc.h"
#include "breakpoint.h"
#include "connector.h"

namespace Debug {
  class Session {
   private:
    Controller &controller;
    vector<Loc> file_stack;
    BreakPointMap bp_map;
    BreakPointFile *breakable_file;
    int next_lineno;

    static Session* g_session;

    void CheckBreakPoint();

   public:
    explicit Session(Controller& ctrl);

    static void Start(Controller& ctrl);
    static Session* GetCurrentSession();
    static void Stop();

    void EnterFile(const Loc& loc);
    void LeaveFile();
    void SetNextLine(const Loc& loc);

    void AddBreakPointer(const char *path, int lineno) {
      bp_map.AddBreakPointer(path, lineno);
      CheckBreakPoint();
    }

    void RemoveBreakPointer(const char *path, int lineno) {
      bp_map.RemoveBreakPointer(path, lineno);
      CheckBreakPoint();
    }

    void RemoveAllBreakPointer() {
      bp_map.RemoveAllBreakPoints();
      CheckBreakPoint();
    }

    bool ShouldBreak() const;
  };
}

void StartDebugSession(Debug::Controller& ctrl);
Debug::Session* GetCurrentDebugSession();
void StopDebugSession();

#endif // DEBUG_SESSION_H
