#include "libsshpp.hpp"
#include <iostream>
#include <string.h>
using namespace std;

int authenticate(ssh_session session);

static int auth_password(const char *user, const char *password){
  if(strcmp(user,"swap"))
    return 0;
  if(strcmp(password,"root"))
    return 0;
  return 1; // authenticated
}

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
  int port = 1333;
  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_BINDADDR, "localhost") < 0) {
    std::cout << "Can't use localhost"<<std::endl;
    return -1; 
  }
  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_BINDPORT, (void*)&port) < 0) {
    std::cout << "Can't use the specified port"<<std::endl;
    return -1; 
  }

  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_RSAKEY, "/root/.ssh/id_rsa") < 0) {
    std::cout << "Something wrong with the keys"<<std::endl;
    return -1;
  }
  int bindRes = ssh_bind_listen (bind_obj);
  if (ssh_bind_listen (bind_obj) < 0) {
    std::cout << "Error listening"<<std::endl;
    return -1;
  }
  std::cout << "All set!" << std::endl;
  ssh_session session_obj = ssh_new ();
  int r = ssh_bind_accept (bind_obj, session_obj);
  if(r == SSH_ERROR)
    std::cout << "error accepting connection" << std::endl;
  std::cout << "Connection accepted successfully" << std::endl;

  if (ssh_handle_key_exchange(session_obj)) {
    std::cout << "ssh_handle_key_exchange : " << ssh_get_error(session_obj);
    return 1;
  }

  int auth = 0;


  ssh_message msg;
  do{
    msg = ssh_message_get(session_obj);
    if(!msg)
      break;
    switch(ssh_message_type(msg)){
      case SSH_REQUEST_AUTH:
        std::cout << "inside SSH_REQUEST_AUTH " << std::endl;
        switch(ssh_message_subtype(msg)){
          case SSH_AUTH_METHOD_PUBLICKEY:
            //TODO : need to get key and compare
            std::cout << "Authenticate using public key" << std::endl;
            if(ssh_message_auth_pubkey(msg)){
              std::cout << "successfully authenticated" << std::endl;
              auth =1;
              ssh_message_auth_reply_success(msg,0);
            }
            break;
          case SSH_AUTH_METHOD_PASSWORD:
            std::cout <<  "inside SSH_AUTH_METHOD_PASSWORD " << std::endl;
            std::cout << "User " << ssh_message_auth_user(msg) << " wants to auth with pass " << ssh_message_auth_password(msg) << std::endl;
            if(auth_password(ssh_message_auth_user(msg), ssh_message_auth_password(msg))){
              auth=1;
              ssh_message_auth_reply_success(msg,0);
            }
            break;
          case SSH_AUTH_METHOD_NONE:
            std::cout << "inside auth method node" << std::endl;
          default:
            ssh_message_auth_set_methods(msg,SSH_AUTH_METHOD_PASSWORD);
            ssh_message_reply_default(msg);
            break;
        }
        break;
      default:
        ssh_message_reply_default(msg); 
        break;
    }
    ssh_message_free(msg);
  }while(!auth);


  //#TODO : Check for channel open request and spwan channel and read commands

  return 0;
}


