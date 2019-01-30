//
// Created by 张航 on 2019-02-05.
//

#include <unordered_map>
#include <regex>
using namespace std;
#include "debug_command.h"
#include "session.h"

namespace Debug {
  class CommandMap : public unordered_map<string, DebugCommand*> {
   public:
    CommandMap() {
      (*this)["bp"] = new AddBreakPointCommand();
      (*this)["br"] = new RemoveBreakPointCommand();
      (*this)["bl"] = new ListBreakPointCommand();
      (*this)["break"] = new BreakCommand();
      (*this)["step"] = new StepCommand();
      (*this)["cont"] = new ContinueCommand();
      (*this)["default_error"] = new ErrorCommand();
    }
    ~CommandMap() {
      for(const auto &iter: *this) {
        delete iter.second;
      }
    }
  };

  CommandMap command_map;

  DebugCommand* DebugCommand::GetErrorCommand() {
    return command_map["default_error"];

  }
  DebugCommand* DebugCommand::GetCommand(const string &command) {
    auto iter = command_map.find(command);
    if (iter != command_map.end()) {
      return iter->second;
    }
    return GetErrorCommand();
  }

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
    session->RemoveBreakPointer(filename.c_str(), lineno);
    return Message(tag_, "OK");
  }

  Message ListBreakPointCommand::Execute(Debug::Session *session, const string &arguments) {
    return Message(tag_);
  }

  Message BreakCommand::Execute(Debug::Session *session, const string &arguments) {
    session->Break();
    return Message(tag_, "OK");
  }

  Message StepCommand::Execute(Debug::Session *session, const string &arguments) {
    session->Step();
    return Message(tag_, "OK");
  }

  Message ContinueCommand::Execute(Debug::Session *session, const string &arguments) {
    session->Continue();
    return Message(tag_, "OK");
  }
}
