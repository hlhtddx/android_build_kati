#include <regex>
using namespace std;

#include "debug_command.h"
#include "session.h"
#include "log.h"

namespace Debug {

  Session* Session::g_session = nullptr;

  Session::Session()
        : controller(*this)
        , breakable_file(nullptr)
        , next_lineno(0)
        , is_paused(false)
  {
  }

  void Session::Start() {
    if (g_session != nullptr) {
      return;
    }
    g_session = new Session();
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
    unique_lock<mutex> lock(break_mutex_);
    if (ShouldBreak()) {
      WARN_LOC(loc, "Break at breakpoint");
      break_cond_.wait(lock);
      WARN_LOC(loc, "Continue breakpoint");
    }
  }

  void Session::CheckBreakPoint() {
    if (file_stack.empty()) {
      return;
    }
    auto &current_file = file_stack.back();
    auto iter = bp_map.find(current_file.filename);
    breakable_file = (iter == bp_map.end()) ? nullptr : &(iter->second);
  }

  bool Session::ShouldBreak() {
    if (is_paused) {
      return true;
    }
    if (breakable_file == nullptr) {
      return false;
    }
    return breakable_file->ShouldBreak(next_lineno);
  }

  void Session::Break() {
    unique_lock<mutex> lock(break_mutex_);
    if (is_paused) {
      return;
    }
    is_paused = true;
  }

  void Session::Step() {
    unique_lock<mutex> lock(break_mutex_);
    is_paused = true;
    break_cond_.notify_all();
  }

  void Session::Continue() {
    unique_lock<mutex> lock(break_mutex_);
    if (!is_paused) {
      return;
    }
    is_paused = false;
    break_cond_.notify_all();
  }

  void Session::AddBreakPointer(const char *path, int lineno) {
    unique_lock<mutex> lock(break_mutex_);
    bp_map.AddBreakPointer(path, lineno);
    CheckBreakPoint();
  }

  void Session::RemoveBreakPointer(const char *path, int lineno) {
    unique_lock<mutex> lock(break_mutex_);
    bp_map.RemoveBreakPointer(path, lineno);
    CheckBreakPoint();
  }

  void Session::RemoveAllBreakPointer() {
    unique_lock<mutex> lock(break_mutex_);
    bp_map.RemoveAllBreakPoints();
    CheckBreakPoint();
  }

  Message Session::ParseCommand(const Message &command) {
    string msg = command.GetMessage();
    if (msg.empty() && !last_command_.empty()) {
      msg = last_command_;
    } else {
      last_command_ = msg;
    }

    regex cmd_pattern(R"(([a-zA-Z][^\s]*)(\s+(.+))?)");
    smatch cmd_match;

    DebugCommand* debug_cmd = nullptr;
    if (regex_match(msg, cmd_match, cmd_pattern)) {
      string cmd = cmd_match[1].str();
      debug_cmd = DebugCommand::GetCommand(cmd);
      return debug_cmd->Execute(this, cmd_match[3].str());
    }
    debug_cmd = DebugCommand::GetErrorCommand();
    return debug_cmd->Execute(this, "");
  }

  Message Session::OnCommand(const Message &command) {
    return ParseCommand(command);
  }
}

void StartDebugSession() {
  Debug::Session::Start();
}

Debug::Session* GetCurrentDebugSession() {
  return Debug::Session::GetCurrentSession();
}

void StopDebugSession() {
  Debug::Session::Stop();
}
