#include "libsshpp.hpp"
#include <iostream>
using namespace std;

int authenticate(ssh_session session);

int main () {
  if (ssh_init () != 0) {
    std::cout << "Error initing ssh"<<std::endl;
    return -1;
  }
  ssh_bind bind_obj = ssh_bind_new ();
  if (bind_obj == NULL) {
    std::cout << "Error creating a bind object"<<std::endl;
    return -1;
  }
  int port = 25;
  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_BINDADDR, "localhost") < 0) {
    std::cout << "Can't use localhost"<<std::endl;
    return -1; 
  }
  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_BINDPORT, (void*)&port) < 0) {
    std::cout << "Can't use the specified port"<<std::endl;
    return -1; 
  }

  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_RSAKEY, "/etc/ssh/ssh_host_rsa_key") < 0 ||
      ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_DSAKEY, "/etc/ssh/ssh_host_dsa_key") < 0) {
    std::cout << "Something wrong with the keys"<<std::endl;
    return -1;
  }
  int bindRes = ssh_bind_listen (bind_obj);
  std::cout<< "bindresult : "<<bindRes;
  if (ssh_bind_listen (bind_obj) < 0) {
    std::cout << "Error listening"<<std::endl;
    return -1;
  }
  std::cout << "All set!"<<std::endl;
  ssh_session session_obj = ssh_new ();
  int r = ssh_bind_accept (bind_obj, session_obj);
  if(r == SSH_ERROR)
    std::cout << "error accepting connection" << std::endl;
  std::cout << "Connection accepted successfully" << std::endl;

  if (ssh_handle_key_exchange(session_obj)) {
    std::cout << "ssh_handle_key_exchange : " << ssh_get_error(session_obj);
    return 1;
  }
  return 0;
}


int authenticate(ssh_session session){
  ssh_message msg;
  do{
    msg = ssh_message_get(session);
    if(!msg)
      break;
    switch(ssh_message_type(msg)){
      case SSH_REQUEST_AUTH:
        switch(ssh_message_subtype(msg)){
          case SSH_AUTH_METHOD_PUBLICKEY:
            //TODO : need to get key and compare
            break;
        }
    }
  }while(1);
  return 0;
}
