#include "libsshpp.hpp"
#include <iostream>
#include <stdio.h>
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

  ssh_channel chan=0;
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

  if (ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_ECDSAKEY, "/root/.ssh/id_ecdsa") < 0) {
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
              std::cout << "successfully authenticated sending success reply" << std::endl;
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
  if(!auth){
    std::cout << "auth error: " << ssh_get_error(session_obj) << std::endl;
    ssh_disconnect(session_obj);
    return 1;
  }


  //check for new channel request
  do {
    msg=ssh_message_get(session_obj);
    if(msg){
      switch(ssh_message_type(msg)){
        case SSH_REQUEST_CHANNEL_OPEN:
          if(ssh_message_subtype(msg)==SSH_CHANNEL_SESSION){
            std::cout << "channel open request " << std::endl;
            chan=ssh_message_channel_request_open_reply_accept(msg);
            break;
          }
        default:
          ssh_message_reply_default(msg);
      }
      ssh_message_free(msg);
    }
  } while(msg && !chan);
  

  if(!chan){
    std::cout << "channel opening error : " << ssh_get_error(session_obj) << std::endl;
    ssh_finalize();
    return 1;
  }

  do {
    msg=ssh_message_get(session_obj);
    if(msg && ssh_message_type(msg)==SSH_REQUEST_CHANNEL &&
        ssh_message_subtype(msg)==SSH_CHANNEL_REQUEST_SHELL){
      std::cout << "channel request shell "<< std::endl;
      ssh_message_channel_request_reply_success(msg);
      break;
    }else 
      ssh_message_reply_default(msg);
    ssh_message_free(msg);
  } while (msg);
  

  std::cout << "it works !" << std::endl;
  int i=0;
  char buf[2048] = "";
  do{
    i=ssh_channel_read(chan,buf, 2048, 0);
    if(i>0) {
   /*   ssh_channel_write(chan, buf, i);
      if (write(1,buf,i) < 0) {
        std::cout << "error writing to buffer" << std::endl;
        return 1;
      }*/
      buf[i] = '\0';
      std::cout <<"command received from client : " << buf << std::endl;
      memset(&buf,0,2048);
    }
  } while (i>0);

  ssh_disconnect(session_obj);
  ssh_bind_free(bind_obj);
  ssh_finalize();
  return 0;
}


