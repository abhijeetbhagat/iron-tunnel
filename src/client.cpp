#include "libsshpp.hpp"
#include <iostream>

int main () {
  ssh::Session session;
  session.setOption (SSH_OPTIONS_HOST, "localhost");
  int port = 25;
  session.setOption (SSH_OPTIONS_PORT, &port);
  session.setOption (SSH_OPTIONS_USER, "swap");



  try {
    session.connect ();
  } catch (ssh::SshException & e) {
    std::cout << "Unable to connect to server " << e.getError() << std::endl;
    return -1;
  }

  if (session.getOpensshVersion () == SSH_VERSION_INT (7,2,0)) {
    std::cout << "version match" << std::endl;
  } else {
    std::cout << "version mismatch" << std::endl; 
  }

  //this is a mandatory step to check the authenticity of the server
  switch (session.isServerKnown ()) {
    case SSH_SERVER_KNOWN_OK:
      std::cout << "known server"<< std::endl;
      break;
    default:
      std::cout << "unknown server"<< std::endl; 
      std::cout << "unknown server"<< std::endl << "Do you want to add to trusted server (y/n) : ";  
      char ch;
      std::cin >> ch;
      std::cout << std::endl;
      switch(ch){
        case 'y':
        case 'Y':
          session.writeKnownhost();
          break;
        case 'n':
        case 'N':
          std::cout << "Not writing to the server to known host"<< std::endl;
          break;
      } 
  }
  int authRes;
  /*try{
    authRes = session.userauthPublickeyAuto();
  }catch(ssh::SshException e){ 
    std::cout << "error connection localhost "<< e.getError() << std::endl; 
    return -1;
  }

  */
  //authRes = session.userauthNone();
  authRes = session.userauthPassword("root");
  std::cout << "user auth none "<<authRes;

  switch(authRes){
    case SSH_AUTH_SUCCESS:
      std::cout << "Authentication successfull "<< std::endl;
      break;
    case SSH_AUTH_PARTIAL:
      std::cout << "Partical Authentication successfull " << std::endl;
      break;
    case SSH_AUTH_DENIED :
      std::cout << "Authentication denied "<< std::endl; 
      break;
    default:
      std::cout << "Unknown authentication message" << std::endl;
  }
  return 0;
}
