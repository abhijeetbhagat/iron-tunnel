#include "libsshpp.hpp"
#include <iostream>
#include <string.h>

int main () {
  ssh::Session session;
  session.setOption (SSH_OPTIONS_HOST, "localhost");
  int port = 1333;
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
  try{
    //authRes = session.userauthPublickeyAuto();
    authRes = session.userauthPassword("root");
  }catch(ssh::SshException e){ 
    std::cout << "error connection localhost "<< e.getError() << std::endl; 
    return -1;
  }

  

  /*int method = session.getAuthList();

  if (method & SSH_AUTH_METHOD_NONE)
  { // For the source code of function authenticate_none(),
    // refer to the corresponding example
    //rc = authenticate_none(session);
    //if (rc == SSH_AUTH_SUCCESS) return rc;
    std::cout << "user auth none" << std::endl;
  }
  if (method & SSH_AUTH_METHOD_PUBLICKEY)
  { // For the source code of function authenticate_pubkey(),
    // refer to the corresponding example
   // rc = authenticate_pubkey(session);
    //if (rc == SSH_AUTH_SUCCESS) return rc;
    std::cout << "user auth pub" << std::endl;
  }
  if (method & SSH_AUTH_METHOD_INTERACTIVE)
  { // For the source code of function authenticate_kbdint(),
    // refer to the corresponding example
    //rc = authenticate_kbdint(session);
    //if (rc == SSH_AUTH_SUCCESS) return rc;
    std::cout << "user auth inter" << std::endl;
  }
  if (method & SSH_AUTH_METHOD_PASSWORD)
  { // For the source code of function authenticate_password(),
    // refer to the corresponding example
   // rc = authenticate_password(session);
   // if (rc == SSH_AUTH_SUCCESS) return rc;
    std::cout << "user auth pass" << std::endl;
  } */

  std::cout << "Auth Res : " << authRes << std::endl;
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
  
  //#TODO : Request for channel and start executing commands
  ssh::Channel ssh_channel(session);

  try{
    ssh_channel.openSession();
    //ssh_channel.requestPty();
    //ssh_channel.changePtySize(30,60);
    ssh_channel.requestShell();
  }catch(ssh::SshException e){
    std::cout << "error connection localhost "<< e.getError() << std::endl; 
    return 1;
  }
  char s[2048] = "";
  std::cout << "Enter commands to be sent to server  " << std::endl;
  while(ssh_channel.isOpen() && !ssh_channel.isEof()){
    std::cin >> s;
    ssh_channel.write(&s,strlen(s));
    std::cout << "command  *" << s << " * , sent to server "<< std::endl;
  }

  std::cout << "opened channel and shell successfully " << std::endl;
  return 0;
}
