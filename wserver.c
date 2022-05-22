#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */

#include "wrapsock.h"
#include "ws_helpers.h"


#define MAXCLIENTS 10

int handleClient(struct clientstate *cs, char *line);

// You may want to use this function for initial testing
//void write_page(int fd);


int read_line(int client_index, struct clientstate *client, fd_set all_fds, fd_set listen_fds) {
    
    int fd = client[client_index].sock;
    char buf[MAXLINE + 1];

    int num_read = read(fd, &buf, MAXLINE);

    if (num_read < 0){
        return fd;
    }


    if (strcmp(buf, "\r\n\r\n") > 0){

        int client_result = handleClient(&client[client_index], buf);

        if (client_result < 0){
            return fd;
        }

        char buf2[MAXLINE + 1];
        read(fd, &buf2, MAXLINE);

        if (strcmp(buf2, "\r\n\r\n")){

            int pd = processRequest(&client[client_index]);

            if (pd < 0){
                return fd;
            }

            int n = 0;
            
            char *output = malloc(sizeof(char) * MAXPAGE + 1 );

            n = read(pd, output, MAXPAGE+1);

            

            if (n < 0){
                return fd;
            }

            client[client_index].output = output;
            client[client_index].optr += n;

            if (num_read == 0){
                client[client_index].sock = -1;
                return fd;
            }


            int con = 0;

            for(; con < MAXCLIENTS; con++){
                
                if (FD_ISSET(client[con].sock, &listen_fds) && client[con].sock > -1) {

                    int n = strlen(client[con].output);

                    printOK(client[con].sock, client[con].output, n);

                    int closed = client[con].sock;
                    FD_CLR(closed, &all_fds);
                    resetClient(&client[con]);
                    
                    printf("Client %d disconnected\n", closed);

                }
            }
        }
    }
    return 0;
}


int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: wserver <port>\n");
        exit(1);
    }
    unsigned short port = (unsigned short)atoi(argv[1]);
    int sockfd;
    struct clientstate client[MAXCLIENTS];


    // Set up the socket to which the clients will connect
    sockfd = setupServerSocket(port);

    initClients(client, MAXCLIENTS);

    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sockfd, &all_fds);


    int max_fd = sockfd;

    printf("Starting...\n");

    while(1){

        fd_set listen_fds = all_fds;


        // 5 minutes interval (5*60 secs = 300)
        struct timeval interval = {300, 0};


        if (Select(max_fd + 1, &listen_fds, NULL, NULL, &interval) == -1) {
            perror("server: select");
            exit(1);
        }

        // #1: SOCKET

        if (FD_ISSET(sockfd, &listen_fds)) {

            int client_index = 0;

            while (client_index < MAXCLIENTS && client[client_index].sock != -1){
                client_index++;
            }

            if (client_index == MAXCLIENTS) {
                fprintf(stderr, "max connections\n");
                return -1;
            }

            int client_fd = Accept(sockfd, NULL, NULL);
            
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }

            FD_SET(client_fd, &all_fds);

            client[client_index].sock = client_fd;

            printf("%s %d\n", "Accepted connection", client_fd);

        }

        int i = 0;

        for (; i < MAXCLIENTS; i++) {
            if ( (client[i].sock > -1) && FD_ISSET(client[i].sock, &listen_fds)) {
                
                int closed = read_line(i, client, all_fds, listen_fds);

                if (closed > 0) {
                    FD_CLR(closed, &all_fds);
                    resetClient(&client[i]);
                    printf("Client %d disconnected\n", closed);
                } 
                
                // else {
                //     printf("PRINTING FROM\n");
                // }
            }
        }

    }
    return 0;
}

/* Update the client state cs with the request input in line.
 * Intializes cs->request if this is the first read call from the socket.
 * Note that line must be null-terminated string.
 *
 * Return 0 if the get request message is not complete and we need to wait for
 *     more data
 * Return -1 if there is an error and the socket should be closed
 *     - Request is not a GET request
 *     - The first line of the GET request is poorly formatted (getPath, getQuery)
 * 
 * Return 1 if the get request message is complete and ready for processing
 *     cs->request will hold the complete request
 *     cs->path will hold the executable path for the CGI program
 *     cs->query will hold the query string
 *     cs->output will be allocated to hold the output of the CGI program
 *     cs->optr will point to the beginning of cs->output
 */
int handleClient(struct clientstate *cs, char *line) {

    if (strstr(line, "GET") == NULL){
        return -1;
    }  

    if (cs -> request == NULL){

        char* request = malloc(sizeof(char) * strlen(line)+1 );

        strncpy(request, line,  strlen(line)+1 );
        cs -> request = request;
        strcpy(cs ->request, line);

    }

    char *path =  malloc(sizeof(char) * strlen(line)+1);
    strcpy(path, getPath(line));

    char *query=  malloc(sizeof(char) * strlen(line)+1 );
    strcpy(query, getQuery(line));

    cs->path = path;
    cs->query_string = query;
 
    // If the resource is favicon.ico we will ignore the request
    if(strcmp("favicon.ico", cs->path) == 0){
        // A suggestion for debugging output
        fprintf(stderr, "Client: sock = %d\n", cs->sock);
        fprintf(stderr, "        path = %s (ignoring)\n", cs->path);
		printNotFound(cs->sock);
        return -1;
    }


    // A suggestion for printing some information about each client. 
    // You are welcome to modify or remove these print statements
    fprintf(stderr, "Client: sock = %d\n", cs->sock);
    fprintf(stderr, "        path = %s\n", cs->path);
    fprintf(stderr, "        query_string = %s\n", cs->query_string);

    return 1;
}

