//
// Created by Frank Zhang on 2019-02-03.
//

#ifndef DEBUG_CONTROLLER_H
#define DEBUG_CONTROLLER_H

#include "handler.h"

namespace Debug {
  class Session;

  class CommandHandler {
   public:
    virtual Message OnCommand(const Message &command) = 0;
  };

  class Controller : public Handler {
   private:
    Session &session_;
    Connector *connector_;

   public:
    explicit Controller(Session &session);

    void SetConnector(Connector* connector) {
      connector_ = connector;
      SetCommandHandle(connector_->GetInputHandle());
    }

    void OnCommand(const Message &message) override;

    void OnResponse(const Message &message) override;
  };

}
#endif //DEBUG_CONTROLLER_H
