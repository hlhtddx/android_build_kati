#include <unistd.h>
#include "command_interface.h"

namespace Debug {
  Message::Message(const string &command) {
    msg_ = command;
  }

  Message::Message(const char *tag) {
    if (strcmp(tag, "<end>") != 0) {
      WriteLine(tag);
    }
    EndResponse();
  }

  Message::Message(const char *tag, const char *response) {
    WriteLine(tag, response);
    EndResponse();
  }

  class DefaultConnector : public Connector {
   public:
    DefaultConnector()
    {}

    virtual ~DefaultConnector() = default;

   protected:
    void OnResponse(const Message &response) override {
      response.Write(GetOutputHandle());
    }

    int GetInputHandle() override {
      return fileno(stdin);
    }

    int GetOutputHandle() override {
      return fileno(stdout);
    }
  };

  Connector* GetDefaultConnector() {
    return new DefaultConnector;
  }
}
