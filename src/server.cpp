#include "libsshpp.hpp"

int _main () {
  ssh_bind bind_obj = ssh_bind_new ();
  ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_HOSTKEY, "~/.ssh/id_rsa");
  ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_BINDADDR, "localhost");
  ssh_bind_options_set (bind_obj, SSH_BIND_OPTIONS_BINDPORT_STR, "22");
  ssh_bind_listen (bind_obj);
  ssh_session session_obj = ssh_new ();
  ssh_bind_accept (bind_obj, session_obj);
  return 0;
}
