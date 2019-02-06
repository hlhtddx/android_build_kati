//
// Created by 张航 on 2019-02-05.
//

#include <unordered_map>
#include <regex>
#include <memory>

using namespace std;

#include "debug_command.h"
#include "session.h"

namespace Debug {

  Message AddBreakPointCommand::Execute(Debug::Session *session, const string &arguments) {
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

  Message RemoveBreakPointCommand::Execute(Debug::Session *session, const string &arguments) {
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

  Message ListBreakPointCommand::Execute(Debug::Session *session, const string &arguments) {
    if (!arguments.empty()) {
      return Message("error", "invalid parameter");
    }
    msg_.Clear();
    session->ForeachBreakPointer([this](int index, const BreakPoint& bp)
      {
        stringstream line;
        line << '<' << index << '>' << bp.filename << ' ' << bp.lineno;
        msg_.WriteLine(tag_, line.str().c_str());
      });
    msg_.EndResponse();
    return msg_;
  }

  Message BreakCommand::Execute(Debug::Session *session, const string &arguments) {
    session->Break();
    return Message(tag_, "OK");
  }

  Message StepOverCommand::Execute(Debug::Session *session, const string &arguments) {
    session->Step();
    return Message(tag_, "OK");
  }

  Message ContinueCommand::Execute(Debug::Session *session, const string &arguments) {
    session->Continue();
    return Message(tag_, "OK");
  }
}
