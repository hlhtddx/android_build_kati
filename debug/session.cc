#include <regex>

using namespace std;

#include <cassert>

#include "debug_command.h"
#include "session.h"
#include "log.h"

namespace Debug {

  class SessionImpl : public SessionBase {
   private:
    Controller controller;
    CommandMap command_map;

    //Break Pointers
    deque<Loc> file_stack;
    unique_ptr<BreakPoints> breakpoints_;

    //Break/Continue/Step Control Flags
    bool is_paused;
    mutex break_mutex_;
    condition_variable broken_cond_;

    //Command Parser
    string last_command_;

    void SetCurrentFile();

    Message ParseCommand(const Message &command);

    DebugCommand *GetErrorCommand();

    DebugCommand *GetCommand(const string &command);

   public:
    explicit SessionImpl(const char *client);

    void EnterFile(const Loc &loc) override;

    void LeaveFile() override;

    void SetNextLine(const Loc &loc) override;

    bool ShouldBreak(int lineno) override;

   public:
    void Break() override;

    void Step() override;

    void Continue() override;

    void AddBreakPointer(const char *path, int lineno) override;

    bool RemoveBreakPointer(int index) override;

    void RemoveAllBreakPointer() override;

    void ForEachBreakPointer(const loc_func& func) override;

    void ForEachFileInStack(const loc_func& func) override;

   public:
    Message OnCommand(const Message &command) override;
  };

  DebugCommand *SessionImpl::GetErrorCommand() {
    return command_map["_error"].get();

  }

  DebugCommand *SessionImpl::GetCommand(const string &command) {
    auto iter = command_map.find(command);
    if (iter != command_map.end()) {
      return iter->second.get();
    }
    return GetErrorCommand();
  }

  SessionImpl::SessionImpl(const char *client)
      : controller(*this), is_paused(true) {
    breakpoints_ = GetBreakPoints();
    Connector* connector = GetConnector(client);
    controller.SetConnector(connector);
  }

  void SessionImpl::EnterFile(const Loc &loc) {
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

  void SessionImpl::LeaveFile() {
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

  void SessionImpl::SetNextLine(const Loc &loc) {
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

  void SessionImpl::SetCurrentFile() {
    auto &current_file = file_stack.front();
    breakpoints_->SetCurrentFile(file_stack.empty() ? string() : current_file.filename);
  }

  bool SessionImpl::ShouldBreak(int lineno) {
    if (is_paused) {
      return true;
    }
    return breakpoints_->ShouldBreak(lineno);
  }

  void SessionImpl::Break() {
    unique_lock<mutex> lock(break_mutex_);
    if (is_paused) {
      return;
    }
    is_paused = true;
    broken_cond_.wait(lock);
  }

  void SessionImpl::Step() {
    unique_lock<mutex> lock(break_mutex_);
    is_paused = true;
    broken_cond_.notify_one();
    broken_cond_.wait(lock);
  }

  void SessionImpl::Continue() {
    unique_lock<mutex> lock(break_mutex_);
    if (!is_paused) {
      return;
    }
    is_paused = false;
    broken_cond_.notify_one();
  }

  void SessionImpl::AddBreakPointer(const char *path, int lineno) {
    unique_lock<mutex> lock(break_mutex_);
    breakpoints_->Insert(path, lineno);
    SetCurrentFile();
  }

  bool SessionImpl::RemoveBreakPointer(int index) {
    unique_lock<mutex> lock(break_mutex_);
    if (!breakpoints_->Remove(index)) {
      return false;
    }
    SetCurrentFile();
    return true;
  }

  void SessionImpl::RemoveAllBreakPointer() {
    unique_lock<mutex> lock(break_mutex_);
    breakpoints_->RemoveAll();
    SetCurrentFile();
  }

  void SessionImpl::ForEachBreakPointer(const loc_func& func) {
    unique_lock<mutex> lock(break_mutex_);
    int index = 0;
    for (const auto &bp: breakpoints_->GetList()) {
      func(++index, bp.filename, bp.lineno);
    }
  }

  void SessionImpl::ForEachFileInStack(const loc_func& func) {
    unique_lock<mutex> lock(break_mutex_);
    int index = 0;
    for (const auto &loc: file_stack) {
      func(++index, loc.filename, loc.lineno);
    }
  }

  Message SessionImpl::ParseCommand(const Message &command) {
    string msg = command.GetMessage();
    if (msg.empty() && !last_command_.empty()) {
      msg = last_command_;
    } else {
      last_command_ = msg;
    }

    regex cmd_pattern(R"(([a-zA-Z][^\s]*)(\s+(.+))?)");
    smatch cmd_match;

    DebugCommand *debug_cmd = nullptr;
    if (regex_match(msg, cmd_match, cmd_pattern)) {
      string cmd = cmd_match[1].str();
      debug_cmd = GetCommand(cmd);
      return debug_cmd->Execute(this, cmd_match[3].str());
    }
    debug_cmd = GetErrorCommand();
    return debug_cmd->Execute(this, "");
  }

  Message SessionImpl::OnCommand(const Message &command) {
    return ParseCommand(command);
  }

  Session *g_session = nullptr;

  void StartDebugSession() {
    const char *debug_flag = getenv("KATI_DEBUG_FLAG");
    const char *debug_client = getenv("KATI_DEBUG_CLIENT");
    bool debug_enabled = debug_flag != nullptr && (strcmp(debug_flag, "true") == 0 || strcmp(debug_flag, "TRUE") == 0);
    if (debug_enabled) {
      g_session = new SessionImpl(debug_client);
    } else {
      g_session = new Session();
    }
  }

  void StopDebugSession() {
    if (g_session != nullptr) {
      delete g_session;
      g_session = nullptr;
    }
  }

  Session *GetCurrentDebugSession() {
    return g_session;
  }
}
