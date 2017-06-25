#include "libsshpp.hpp"
#include <iostream>

int main () {
  ssh::Session session;
  session.setOption (SSH_OPTIONS_HOST, "localhost");
  int port = 22;
  session.setOption (SSH_OPTIONS_PORT, &port);
  session.setOption (SSH_OPTIONS_USER, "abhi");
  session.connect ();
  if (session.getOpensshVersion () == SSH_INT_VERSION(6,6,1)) {
    std::cout << "version match" << std::endl;
  } else {
    std::cout << "version mismatch" << std::endl; 
  }

  return 0;
}
