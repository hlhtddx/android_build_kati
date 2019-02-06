//
// Created by 张航 on 2019-02-05.
//

#ifndef DEBUG_COMMAND_H
#define DEBUG_COMMAND_H

#include "command_interface.h"

namespace Debug {
  class Session;
  class DebugCommand {
   public:
    explicit DebugCommand(const char* tag) : tag_(tag) {}
    virtual bool IsValid() const { return true; }
    virtual Message Execute(Session* session, const string& arguments) {
      return Message("error");
    }
   protected:
    const char* tag_;
  };

  class ErrorCommand : public DebugCommand {
   public:
    explicit ErrorCommand() : DebugCommand("error") {}
    bool IsValid() const override {
      return false;
    }
  };

  class AddBreakPointCommand : public DebugCommand {
   public:
    explicit AddBreakPointCommand() : DebugCommand("bp") {}
    bool IsValid() const override {
      return true;
    }
    Message Execute(Session* session, const string& arguments) override;
  };

  class RemoveBreakPointCommand : public DebugCommand {
   public:
    explicit RemoveBreakPointCommand() : DebugCommand("br") {}
    bool IsValid() const override {
      return true;
    }
    Message Execute(Session* session, const string& arguments) override;
  };

  class ListBreakPointCommand : public DebugCommand {
   public:
    explicit ListBreakPointCommand() : DebugCommand("bl") {}
    bool IsValid() const override {
      return true;
    }
    Message Execute(Session* session, const string &arguments) override;
   private:
    Message msg_;
  };

  class BreakCommand : public DebugCommand {
   public:
    explicit BreakCommand() : DebugCommand("break") {}
    bool IsValid() const override {
      return true;
    }
    Message Execute(Session* session, const string &arguments) override;
  };

  class StepOverCommand : public DebugCommand {
   public:
    explicit StepOverCommand() : DebugCommand("step") {}
    bool IsValid() const override {
      return true;
    }
    Message Execute(Session* session, const string &arguments) override;
  };

  class ContinueCommand : public DebugCommand {
   public:
    explicit ContinueCommand() : DebugCommand("cont") {}
    bool IsValid() const override {
      return true;
    }
    Message Execute(Session* session, const string &arguments) override;
  };

}

#endif //DEBUG_COMMAND_H
