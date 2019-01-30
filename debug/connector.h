//
// Created by Frank Zhang on 2019-01-26.
//

#ifndef DEBUG_CONNECTOR_H
#define DEBUG_CONNECTOR_H

namespace Debug {

  class Controller {
   private:
    int fd_in, fd_out; // File handler to communicate with client
   public:
    Controller();

    virtual ~Controller();
  };
}

#endif // DEBUG_CONNECTOR_H
