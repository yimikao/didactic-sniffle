#ifndef _WNET_H_
#define _WNET_H_
// get_listener_or_die returns a file descriptor from all possible network interfaces or not
int get_listener_or_die(char *port);
#endif