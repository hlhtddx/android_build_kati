//
// Created by Frank Zhang on 2019-02-03.
//

#include "controller.h"
#include "session.h"

namespace Debug {
  Controller::Controller(SessionBase &session)
        : session_(session)
  {
  }

  void Controller::SetConnector(Connector *connector) {
    connector_ = connector;
    connector->WaitForConnection();
    SetCommandHandle(connector_->GetInputHandle());
    Start();
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
