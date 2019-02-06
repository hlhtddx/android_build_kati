#include <regex>
using namespace std;

#include "debug_command.h"
#include "session.h"
#include "log.h"

namespace Debug {

  DebugCommand *Session::GetErrorCommand() {
    return command_map["_error"].get();

  }

  DebugCommand *Session::GetCommand(const string &command) {
    auto iter = command_map.find(command);
    if (iter != command_map.end()) {
      return iter->second.get();
    }
    return GetErrorCommand();
  }

  Session* Session::g_session = nullptr;

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

  Session::Session()
        : controller(*this)
        , is_paused(true)
  {
    breakpoints_ = GetBreakPoints();
    controller.SetConnector(GetDefaultConnector());
  }

  void Session::EnterFile(const Loc& loc) {
    unique_lock<mutex> lock(break_mutex_);
    file_stack.push_front(loc);
    SetCurrentFile();
#ifdef DEBUG_DBG_MSG
    printf("Enter file\n");
    for(auto iter = file_stack.begin(); iter != file_stack.end(); iter++) {
      printf("%s:%d\n", iter->filename, iter->lineno);
    }
#endif
  }

  void Session::LeaveFile() {
    unique_lock<mutex> lock(break_mutex_);
    file_stack.pop_front();
    SetCurrentFile();
#ifdef DEBUG_DBG_MSG
    printf("Leave file\n");
    for(auto iter = file_stack.begin(); iter != file_stack.end(); iter++) {
      printf("%s:%d\n", iter->filename, iter->lineno);
    }
#endif
  }

  void Session::SetNextLine(const Loc& loc) {
    unique_lock<mutex> lock(break_mutex_);
    assert(!file_stack.empty());
    auto &current_file = file_stack.front();
    current_file.lineno = loc.lineno;
    if (ShouldBreak(loc.lineno)) {
      stringstream position;
      position << loc.filename << ':' << loc.lineno;
      Message msg("current", position.str().c_str());
      controller.SendResponse(msg);
      broken_cond_.notify_one();
      broken_cond_.wait(lock);
    }
  }

  void Session::SetCurrentFile() {
    assert (!file_stack.empty());
    auto &current_file = file_stack.front();
    breakpoints_->SetCurrentFile(current_file.filename);
  }

  bool Session::ShouldBreak(int lineno) {
    if (is_paused) {
      return true;
    }
    return breakpoints_->ShouldBreak(lineno);
  }

  void Session::Break() {
    unique_lock<mutex> lock(break_mutex_);
    if (is_paused) {
      return;
    }
    is_paused = true;
    broken_cond_.wait(lock);
  }

  void Session::Step() {
    unique_lock<mutex> lock(break_mutex_);
    is_paused = true;
    broken_cond_.notify_one();
    broken_cond_.wait(lock);
  }

  void Session::Continue() {
    unique_lock<mutex> lock(break_mutex_);
    if (!is_paused) {
      return;
    }
    is_paused = false;
    broken_cond_.notify_one();
  }

  void Session::AddBreakPointer(const char *path, int lineno) {
    unique_lock<mutex> lock(break_mutex_);
    breakpoints_->Insert(path, lineno);
    SetCurrentFile();
  }

  bool Session::RemoveBreakPointer(int index) {
    unique_lock<mutex> lock(break_mutex_);
    if (!breakpoints_->Remove(index)){
      return false;
    }
    SetCurrentFile();
    return true;
  }

  void Session::RemoveAllBreakPointer() {
    unique_lock<mutex> lock(break_mutex_);
    breakpoints_->RemoveAll();
    SetCurrentFile();
  }

  void Session::ForEachBreakPointer(loc_func func) {
    unique_lock<mutex> lock(break_mutex_);
    int index = 0;
    for (const auto& bp: breakpoints_->GetList()) {
      func(++index, bp.filename, bp.lineno);
    }
  }

  void Session::ForEachFileInStack(loc_func func) {
    unique_lock<mutex> lock(break_mutex_);
    int index = 0;
    for (const auto& loc: file_stack) {
      func(++index, loc.filename, loc.lineno);
    }
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
      debug_cmd = GetCommand(cmd);
      return debug_cmd->Execute(this, cmd_match[3].str());
    }
    debug_cmd = GetErrorCommand();
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
