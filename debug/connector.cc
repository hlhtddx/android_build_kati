//
// Created by 张航 on 2019-02-06.
//

#include <regex>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "command_interface.h"

namespace Debug {
  class DefaultConnector : public Connector {
   public:
    DefaultConnector() = default;

    virtual ~DefaultConnector() = default;

    bool WaitForConnection() override {
      // do nothing for console is always connected
      return true;
    }

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

  class SocketConnector : public Connector {
   public:
    explicit SocketConnector()
      : fd_(-1), listen_fd_(-1) {
    }

    virtual ~SocketConnector() {
      if (fd_ != -1){
        close(fd_);
      }
      if (listen_fd_ != -1){
        close(listen_fd_);
      }
    };

    bool WaitForConnection() override {
      while(listen_fd_ == -1) {
        listen_fd_ = CreateSocket();
      }
      sockaddr_in client_addr = {0};
      socklen_t client_addr_len = sizeof(client_addr);
      do {
        fd_ = accept(listen_fd_, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
      } while (fd_ == -1);
      return true;
    }

    void OnResponse(const Message &response) override {
      response.Write(GetOutputHandle());
    }

    int GetInputHandle() override {
      return fd_;
    }

    int GetOutputHandle() override {
      return fd_;
    }

   protected:
    virtual int CreateSocket() = 0;

    int fd_;
    int listen_fd_;
  };

  class TcpSocketConnector : public SocketConnector {
   public:
    explicit TcpSocketConnector(int port) : port_(port) {
    }

   protected:
    int CreateSocket() override {

      /* First call to socket() function */
      listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);

      if (listen_fd_ < 0) {
        return -1;
      }

      sockaddr_in server_addr = {
          .sin_family = AF_INET,
          .sin_addr.s_addr = INADDR_ANY,
          .sin_port = htons(port_),
      };

      if (::bind(listen_fd_, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        close(listen_fd_);
        return -1;
      }

      /* Now start listening for the clients, here
         * process will go in sleep mode and will wait
         * for the incoming connection
      */

      if (listen(listen_fd_, 1) < 0) {
        close(listen_fd_);
        return -1;
      }
      return listen_fd_;
    }
   private:
    int port_;
  };


  regex patterns[] = {
      regex(R"(tcp:(\d+))"),
      regex(R"(unix:(\w+))"),
      regex(R"(pipe:(\w+))"),
  };

  Connector *GetConnector(const char *connector) {
    if (connector == nullptr) {
      return new DefaultConnector;
    }
    string connect(connector);
    smatch matched;
    if (regex_match(connect, matched, patterns[0])) {
      int port = stoi(matched[1].str());
      return new TcpSocketConnector(port);
    }

    return new DefaultConnector;
  }

}
