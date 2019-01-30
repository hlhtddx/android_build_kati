#include "connector.h"
#include <unistd.h>

namespace Debug {

  Controller::Controller() : fd_in(-1), fd_out(-1) {

  }

  Controller::~Controller() {
    if (fd_in != -1) close(fd_in);
    if (fd_out != -1) close(fd_out);
  }

}
