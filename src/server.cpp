#include "libsshpp.hpp"
#include <iostream>
using namespace std;

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
  int port = 22;
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
  ssh_bind_accept (bind_obj, session_obj);
  return 0;
}
