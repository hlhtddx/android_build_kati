//
// Created by Frank Zhang on 2019-02-03.
//

#include <unistd.h>
#include <poll.h>
#include "log.h"
#include "handler.h"

#define PIPE_FD_READ  0
#define PIPE_FD_WRITE 1
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

namespace Debug {
  Handler::Handler()
      : is_running_(true)
      , loop_thread_([this]() { Loop(); }) {
    pipe(fd_response_);
  }

  Handler::~Handler() {
    is_running_ = false;
    close(fd_response_[PIPE_FD_READ]);
    close(fd_response_[PIPE_FD_WRITE]);
    loop_thread_.join();
  }

  void Handler::WaitForStart() {
    char c;
    while(true) {
      ssize_t nread = read(fd_response_[PIPE_FD_READ], &c, sizeof(c));
      if (nread >=0) {
        return;
      }
    }
  }

  void Handler::Start() {
    char c = 'S';
    while(true) {
      ssize_t nwrite = write(fd_response_[PIPE_FD_WRITE], &c, sizeof(c));
      if (nwrite >=0) {
        return;
      }
    }
  }

  void Handler::Loop() {
    WaitForStart();
    while (is_running_) {
      pollfd fds[] = {
          {
            .fd = fd_command_,
            .events = POLLIN,
            .revents = 0
          },
          {
            .fd = fd_response_[PIPE_FD_READ],
            .events = POLLIN,
            .revents = 0
          }
      };
      int npfds = poll(fds, ARRAY_SIZE(fds), 10000);
      if (npfds > 0) {
        if ((fds[0].revents & POLLIN) != 0 && !ProcessCommand()) {
          WARN("Failed to process commands. Error=(%s)\n", strerror(errno));
          return;
        }
        if ((fds[1].revents & POLLIN) != 0 && !ProcessResponse()) {
          WARN("Failed to process response. Error=(%s)\n", strerror(errno));
          return;
        }
      } else if (npfds == 0) {
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
      } if (p < buffer + nread - 1 && *p == '\r' && *(++p) == '\n') {
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
