//
// Created by 张航 on 2019-02-05.
//

#include <regex>
#include <memory>

using namespace std;

#include "debug_command.h"
#include "session.h"

namespace Debug {

  class ErrorCommand : public DebugCommand {
   public:
    explicit ErrorCommand() : DebugCommand("error") {}
  };

  class AddBreakPointCommand : public DebugCommand {
   public:
    explicit AddBreakPointCommand() : DebugCommand("bp") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      regex pattern_quoted(R"(\"([^\"]+)\"\s+(\d+))");
      regex pattern_unquoted(R"(([^\s\"]+)\s+(\d+))");
      smatch matched;
      string filename;
      int lineno = 0;
      if (regex_match(arguments, matched, pattern_quoted)) {
        filename = matched[1].str();
        lineno = stoi(matched[2].str());
      } else if (regex_match(arguments, matched, pattern_unquoted)) {
        filename = matched[1].str();
        lineno = stoi(matched[2].str());
      } else {
        return Message("error", "invalid parameter");
      }
      session->AddBreakPointer(filename.c_str(), lineno);
      return Message(tag_, "OK");
    }
  };

  class RemoveBreakPointCommand : public DebugCommand {
   public:
    explicit RemoveBreakPointCommand() : DebugCommand("br") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      if (arguments.empty()) {
        session->RemoveAllBreakPointer();
        return Message(tag_, "OK");
      }
      regex pattern(R"((\d+))");
      smatch matched;
      int index = 0;
      if (regex_match(arguments, matched, pattern)) {
        index = stoi(matched[1].str());
      } else {
        return Message("error", "invalid parameter");
      }

      if (!session->RemoveBreakPointer(index - 1)) {
        return Message("error", "failed to remove break pointer");
      }
      return Message(tag_, "OK");
    }
  };

  class ListBreakPointCommand : public DebugCommand {
   public:
    explicit ListBreakPointCommand() : DebugCommand("bl") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      if (!arguments.empty()) {
        return Message("error", "invalid parameter");
      }
      msg_.Clear();
      session->ForEachBreakPointer([this](int index, const string &path, int lineno) {
          stringstream line;
          line << '<' << index << '>' << path << ':' << lineno;
          msg_.WriteLine(tag_, line.str().c_str());
      });
      msg_.EndResponse();
      return msg_;
    }

   private:
    Message msg_;
  };

  class BreakCommand : public DebugCommand {
   public:
    explicit BreakCommand() : DebugCommand("break") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      session->Break();
      return Message(tag_, "OK");
    }
  };

  class StepOverCommand : public DebugCommand {
   public:
    explicit StepOverCommand() : DebugCommand("step") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      session->Step();
      return Message(tag_, "OK");
    }
  };

  class ContinueCommand : public DebugCommand {
   public:
    explicit ContinueCommand() : DebugCommand("cont") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      session->Continue();
      return Message(tag_, "OK");
    }
  };

  class BackTraceCommand : public DebugCommand {
   public:
    explicit BackTraceCommand() : DebugCommand("bt") {}

    Message Execute(SessionBase *session, const string &arguments) override {
      if (!arguments.empty()) {
        return Message("error", "invalid parameter");
      }
      msg_.Clear();
      session->ForEachFileInStack([this](int index, const string &path, int lineno) {
          stringstream line;
          line << '<' << index << '>' << path << ':' << lineno;
          msg_.WriteLine(tag_, line.str().c_str());
      });
      msg_.EndResponse();
      return msg_;
    }

   private:
    Message msg_;
  };

  CommandMap::CommandMap() {
    auto bp = make_shared<AddBreakPointCommand>();
    (*this)["bp"] = bp;
    (*this)["b"] = bp;

    auto br = make_shared<RemoveBreakPointCommand>();
    (*this)["br"] = br;
    (*this)["d"] = br;

    auto bl = make_shared<ListBreakPointCommand>();
    (*this)["bl"] = bl;

    auto brk = make_shared<BreakCommand>();
    (*this)["break"] = brk;

    auto next = make_shared<StepOverCommand>();
    (*this)["next"] = next;
    (*this)["n"] = next;

    auto cont = make_shared<ContinueCommand>();
    (*this)["cont"] = cont;
    (*this)["c"] = cont;

    auto bt = make_shared<BackTraceCommand>();
    (*this)["bt"] = bt;
    (*this)["backtrace"] = bt;

    auto error = make_shared<ErrorCommand>();
    (*this)["_error"] = error;
  }

}
