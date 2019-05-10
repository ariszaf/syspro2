#include <cstring>
#include "NetworkFunctions.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>

void NetworkFunctions::sendMessage(int fd, char * msg) {
    int header = strlen(msg);
    int header_length = sizeof (header);

    char * payload = msg;
    int payload_length = strlen(msg);

    if (header_length > 0) {
        write(fd, ( void*) &(header), header_length);
    }

    if (payload_length > 0) {
        write(fd, ( void*) payload, payload_length);
    }
}
 char * NetworkFunctions::receiveMessage(int fd) {
    int header;                     
    int header_length = sizeof (header);                

    char* payload;
    int payload_length;

    if (read(fd, (void*) &(header), header_length) <= 0) {
        return NULL;
    }

    payload_length = header;

    payload = new char [header + 1]();
   // payload = (char*) calloc(header + 1, sizeof (char));
    if (read(fd, (void*) payload, payload_length) <= 0) {
        return NULL;
    }
    return payload;
}


