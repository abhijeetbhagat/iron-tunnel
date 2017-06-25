#include "libsshpp.hpp"
#include <iostream>

int main () {
  ssh::Session session;
  session.setOption (SSH_OPTIONS_HOST, "localhost");
  int port = 22;
  session.setOption (SSH_OPTIONS_PORT, &port);
  session.setOption (SSH_OPTIONS_USER, "abhi");
  try {
    session.connect ();
  } catch (ssh::SshException & e) {
    return -1;
  }

  if (session.getOpensshVersion () == SSH_VERSION_INT (6,6,0)) {
    std::cout << "version match" << std::endl;
  } else {
    std::cout << "version mismatch" << std::endl; 
  }

  //this is a mandatory step to check the authenticity of the server
  switch (session.isServerKnown ()) {
    case SSH_SERVER_KNOWN_OK:
      std::cout << "known server\n";
      break;
    default:
      std::cout << "unknown server\n"; 
      return -1;
  }

  return 0;
}
