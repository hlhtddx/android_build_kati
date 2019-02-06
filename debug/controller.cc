//
// Created by Frank Zhang on 2019-02-03.
//

#include "controller.h"
#include "session.h"

namespace Debug {
  Controller::Controller(Session &session)
        : session_(session)
  {
  }

  void Controller::OnCommand(const Message& message) {
    auto response = session_.OnCommand(message);
    SendResponse(response);
  }

  void Controller::OnResponse(const Message& message) {
    if (connector_ != nullptr) {
      connector_->OnResponse(message);
    }
  }
}
