//
// Created by Frank Zhang on 2019-02-03.
//

#include <unistd.h>
#include <poll.h>
#include "log.h"
#include "handler.h"

#define PIPE_FD_READ  0
#define PIPE_FD_WRITE 1
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

namespace Debug {
  Handler::Handler()
        : is_running_(true), loop_func_([this]() { Loop(); }) {
    pipe(fd_response_);
  }

  Handler::~Handler() {
    is_running_ = false;
    close(fd_response_[PIPE_FD_READ]);
    close(fd_response_[PIPE_FD_WRITE]);
    loop_func_.join();
  }

  void Handler::Loop() {
    while (is_running_) {
      pollfd fds[] = {{fd_command_,                POLLIN, 0},
                      {fd_response_[PIPE_FD_READ], POLLIN, 0}};
      int npfds = poll(fds, ARRAY_SIZE(fds), 1000);
      if (npfds > 0) {
        if (fds[0].revents & POLLIN) {
          if (!ProcessCommand()) {
            WARN("Failed to process commands. Error=(%s)\n", strerror(errno));
            return;
          }
        }
        if (fds[1].revents & POLLIN) {
          if (!ProcessResponse()) {
            WARN("Failed to process response. Error=(%s)\n", strerror(errno));
            return;
          }
        }
      } else if (npfds == 0) {
//        WARN("Polling timeout\n");
      } else {
        WARN("Error occurs when polling on commands. Error=(%s)\n", strerror(errno));
        return;
      }
    }
  }

  bool Handler::ProcessCommand() {
    char buffer[1024];
    ssize_t nread = read(fd_command_, buffer, sizeof(buffer));
    if (nread <= 0) {
      return false;
    }
    for (char *p = buffer; p < buffer + nread; p++) {
      if (*p == '\n') {
        OnCommand(Message(current_command_));
        current_command_.clear();
      } else {
        current_command_.push_back(*p);
      }
    }
    return true;
  }

  void Handler::SendResponse(Debug::Message &message) {
    static char c = 0;
    unique_lock<mutex> lock(message_mutex_);
    responses_.push_back(std::move(message));
    write(fd_response_[PIPE_FD_WRITE], &(++c), sizeof(c));
  }

  bool Handler::ProcessResponse() {
    unique_lock<mutex> lock(message_mutex_);
    char buffer[1024];
    ssize_t nread = read(fd_response_[PIPE_FD_READ], buffer, sizeof(buffer));
    if (nread <= 0) {
      return false;
    }
    for (auto &message : responses_) {
      OnResponse(message);
    }
    responses_.clear();
    return true;
  }
}
