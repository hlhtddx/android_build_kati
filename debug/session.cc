#include "session.h"
#include "log.h"

namespace Debug {

  Session* Session::g_session = nullptr;

  Session::Session(Controller& ctrl)
        : controller(ctrl)
        , breakable_file(nullptr)
        , next_lineno(0)
  {}

  void Session::Start(Controller& ctrl) {
    if (g_session != nullptr) {
      return;
    }
    g_session = new Session(ctrl);
  }

  Session* Session::GetCurrentSession() {
    return g_session;
  }

  void Session::Stop() {
    if (g_session) {
      delete g_session;
      g_session = nullptr;
    }
  }

  void Session::EnterFile(const Loc& loc) {
    if (!file_stack.empty()) {
      auto &current_file = file_stack.back();
      current_file.lineno = next_lineno;
    }
    file_stack.emplace_back(loc);
    CheckBreakPoint();
#ifdef DEBUG_DBG_MSG
    printf("Enter file\n");
    for(auto iter = file_stack.begin(); iter != file_stack.end(); iter++) {
      printf("%s:%d\n", iter->filename, iter->lineno);
    }
#endif
  }

  void Session::LeaveFile() {
    file_stack.pop_back();
    CheckBreakPoint();
#ifdef DEBUG_DBG_MSG
    printf("Leave file\n");
    for(auto iter = file_stack.begin(); iter != file_stack.end(); iter++) {
      printf("%s:%d\n", iter->filename, iter->lineno);
    }
#endif
  }

  void Session::SetNextLine(const Loc& loc) {
    next_lineno = loc.lineno;
    if (ShouldBreak()) {
      WARN_LOC(loc, "Break at breakpoint");
    }
  }

  void Session::CheckBreakPoint() {
    if (file_stack.empty()) return;
    auto &current_file = file_stack.back();
    auto iter = bp_map.find(current_file.filename);
    breakable_file = (iter == bp_map.end()) ? nullptr : &(iter->second);
  }

  bool Session::ShouldBreak() const {
    if (breakable_file == nullptr) return false;
    return breakable_file->ShouldBreak(next_lineno);
  }
}

void StartDebugSession(Debug::Controller& ctrl) {
  Debug::Session::Start(ctrl);
}

Debug::Session* GetCurrentDebugSession() {
  return Debug::Session::GetCurrentSession();
}

void StopDebugSession() {
  Debug::Session::Stop();
}
