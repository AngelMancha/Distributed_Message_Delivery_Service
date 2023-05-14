#include <unistd.h>
#include "comunicacion.h"

int sendMessage(int socket, char *buffer, int len);
ssize_t readLine(int fd, void *buffer, size_t n);