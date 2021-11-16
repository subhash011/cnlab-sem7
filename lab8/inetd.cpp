#include "common.h"

/* Some identifiers for clear output messages. */
#define ID "[INETD] "
#define ADD_ID "[ADD] "
#define SUB_ID "[SUB] "
#define MUL_ID "[MUL] "
#define IDIV_ID "[IDIV] "

int main(int argc, char const *argv[])
{
    /** 
     * Initialize sockets, address structures, ports
     * and the executables for each of the servers on h2.
     */
    int sockfds[4];
    sockaddr_in addrs[4];
    int ports[] = {ADD_PORT, SUB_PORT, MUL_PORT, IDIV_PORT};
    const char *progs[] = {"./inetd_add", "./inetd_sub", "./inetd_mul", "./inetd_idiv"};
    const char *ids[] = {ADD_ID, SUB_ID, MUL_ID, IDIV_ID};
    socklen_t addr_size = sizeof(addrs[0]);
    /** 
     * Create, bind and start listening on each of ports for each 
     * of the servers on h2.
     */
    for (int i = 0; i < 4; i++)
    {
        try_socket_creation(sockfds[i], true);
        get_addr(addrs[i], H2_IP, ports[i]);
        try_bind(sockfds[i], addrs[i]);
        listen(sockfds[i], 5);
    }
    /* store a set of file descripts for select. */
    fd_set readfds;
    /* clear all file descriptors in the set. */
    FD_ZERO(&readfds);
    int max_sd = *max_element(sockfds, sockfds + 4);
    while (1)
    {
        /**
         * We expect connection on any of the four ports.
         * So we add each of the file descriptors to the set.
         */
        for (int i = 0; i < 4; i++)
        {
            FD_SET(sockfds[i], &readfds);
        }
        cout << ID << "Waiting for client connections..." << endl;
        /** 
         * We wait for a connection on any of the four ports.
         * Here the __readfds is not NULL so it checks for fds that
         * are ready to be read.
         */
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            cerr << ID << "Error while calling select: " << activity << endl;
            exit(EXIT_FAILURE);
        }
        /* Since we are here, we have received a client connection request. */
        for (int i = 0; i < 4; i++)
        {
            /* Find the appropriate server that has received the connection request. */
            if (FD_ISSET(sockfds[i], &readfds))
            {
                int client_sd;
                /* Accept the connection request and store the file descriptor of the client. */
                try_accept(client_sd, sockfds[i], addrs[i], addr_size);
                string client_ip = inet_ntoa(addrs[i].sin_addr);
                cout << ids[i] << "Connected to client with IP: " << client_ip << endl;
                /* Create a new process for the server. */
                pid_t pid = fork();
                if (pid == 0)
                {
                    /** 
                     * Inside child process.
                     * Redirect all messages on the socket to the standard input
                     * and close all sockets.
                     */
                    for (int i = 0; i < 4; i++)
                    {
                        close(sockfds[i]);
                    }
                    dup2(client_sd, STDIN_FILENO);
                    close(client_sd);
                    /* Run the appropriate operation. */
                    execl(progs[i], progs[i], client_ip.c_str(), NULL);
                }
                else
                {
                    /** 
                     * Inside parent process.
                     * Close the socket and wait till the child exits.
                     */
                    close(client_sd);
                    waitpid(pid, NULL, 0);
                }
            }
        }
    }
    return 0;
}
