//
// Created by Frank Zhang on 2019-02-03.
//

#ifndef DEBUG_HANDLER_H
#define DEBUG_HANDLER_H

#include <thread>
#include <mutex>
#include <vector>
using namespace std;

#include "command_interface.h"

namespace Debug {

  class Handler {
   private:
    thread loop_thread_;
    mutex message_mutex_;
    bool is_running_;
    int fd_response_[2];
    int fd_command_;
    string current_command_;
    vector<Message> responses_;

   protected:
    void WaitForStart();
    void Start();
    void Loop();
    bool ProcessCommand();
    bool ProcessResponse();

   public:
    explicit Handler();
    virtual ~Handler();
    virtual void OnCommand(const Message& message) = 0;
    virtual void OnResponse(const Message& message) = 0;

    void SetCommandHandle(int fd) {
      fd_command_ = fd;
    }
    void SendResponse(Message& message);
  };

}


#endif //DEBUG_HANDLER_H
