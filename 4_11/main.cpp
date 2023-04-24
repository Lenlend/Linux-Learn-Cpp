#include <iostream>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>

#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/select.h>

#include <pthread.h>

#define BUFFER_LENGTH 10
#define POLL_SIZE 1024

typedef void*(*void_func_ptr)(void*);

void* client_thread(void *arg)
{
    int clientfd = *(int*)arg;

    char buffer[BUFFER_LENGTH] = {0};
    int ret = recv(clientfd, buffer, BUFFER_LENGTH, 0);
    printf("ret:%d, buffer:%s\n",ret, buffer);

    send(clientfd, buffer, ret, 0);

    return nullptr;

}


int main()
{
    //lei si open io
    //AF_INET:
    //SOCK_STREAM
    //0:
    //fd:
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(struct sockaddr_in)); //192.168.2.123
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0
    servaddr.sin_port = htons(9999); //duan kou

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));

    // if( -1 == bind(sockfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in)));
    // {
    //     printf("failed to bind socket error:%s\n", strerror(errno));

    //     return -1;
    // }

    listen(sockfd, 10);

    // sleep(10);

    // int flags = fcntl(sockfd, F_GETFL, 0); //huo qu io flags
    // flags |= O_NONBLOCK;
    // fcntl(sockfd, F_SETFL, flags); // she zhi fei zu sai biao zhi 

    // struct sockaddr_in clinetaddr;
    // socklen_t len = sizeof(clinetaddr);

    // while(1)
    // {
    //     int clientfd = accept(sockfd, (struct sockaddr*)&clinetaddr, &len);

    //     printf("Accept %d\n", clientfd);
    //     pthread_t thread_id;
    //     void_func_ptr func;
    //     func = client_thread;
    //     pthread_create(&thread_id, NULL, func, &clientfd);
    // }
    

    // while(1)
    // {

    //     int clientfd = accept(sockfd, (struct sockaddr*)&clinetaddr, &len);
    //     // 0 / 1 / 2 3/4
    //     printf("clinetfd: %d\n sockfd:%d\n", clientfd, sockfd);
    //     char buffer[1024] = {0};
    //     int ret = recv(clientfd, buffer, sizeof(buffer) / sizeof(char), 0);

    //     printf("buffer: %s\n", buffer);
    //     send(clientfd, buffer, ret, 0);
    // }
    
    // fd_set rfds, rfd_set;
    // FD_ZERO(&rfds);
    // FD_SET(sockfd, &rfds);


    // struct sockaddr clientaddr;
    // uint32_t len = sizeof(clientaddr);
    // int clinetfd;

    // int max_fd = sockfd;
    // while(1)
    // {
    //     rfd_set = rfds;
    //     int ready = select(max_fd + 1, &rfd_set, NULL, NULL, NULL);

    //     if(FD_ISSET(sockfd, &rfd_set))
    //     {
    //         clinetfd = accept(sockfd, &clientaddr, &len);

    //         FD_SET(clinetfd, &rfds);
    //         if(clinetfd > max_fd)
    //             max_fd = clinetfd;

    //         printf("sockfd: %d\n, clinetfd:%d \n", sockfd, clinetfd);
            
    //     }

    //     int i = 0;
    //     for(i = sockfd + 1; i <= max_fd; i++)
    //     {
    //         if( FD_ISSET(i, &rfd_set) )
    //         {
    //             char buffer[BUFFER_LENGTH] = {0};
    //             int ret_len = recv(clinetfd, buffer, BUFFER_LENGTH, 0);

    //             if(ret_len == 0)
    //             {
    //                 close(clinetfd);
    //             }

    //             printf("recv:%s\n", buffer);
    //             send(clinetfd, buffer, ret_len, 0);
    //         }
            
    //     }
    // }


    // sockaddr clientaddr;
    // socklen_t len;

    // struct pollfd fds[POLL_SIZE] = {0};

    // fds[sockfd].fd = sockfd;
    // fds[sockfd].events = POLLIN;

    // int maxfd = sockfd;
    // int clientfd = 0;

    // while(1)
    // {
    //     int nready = poll(fds, maxfd + 1, - 1);

    //     if(fds[sockfd].revents & POLLIN)
    //     {
    //         clientfd = accept(sockfd, &clientaddr, &len);
    //         printf("accpet:%d\n", clientfd);
    //         fds[clientfd].fd = clientfd;
    //         fds[clientfd].events = POLLIN;

    //         if(clientfd > maxfd) maxfd = clientfd;
    //         if(--nready == 0) continue;
    //     }

    //     for(int i = 0; i <= maxfd; i++)
    //     {
    //         if(fds[i].revents & POLLIN)
    //         {
    //             char buffer[BUFFER_LENGTH] = {0};
    //             int ret_len = 0;
    //             ret_len = recv(fds[i].fd, buffer, BUFFER_LENGTH, 0);
    //             if(ret_len == 0 )
    //             {
    //                 fds[clientfd].fd = -1;
    //                 fds[clientfd].events = 0;
    //                 close(clientfd);
    //                 break;
    //             }
    //             printf("ret: %d, buffer:%s\n", ret_len, buffer);
    //             send(clientfd, buffer, ret_len, 0);
    //         }
    //     }
    // }

    int flags = fcntl(sockfd, F_GETFL, 0); //huo qu io flags
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags); // she zhi fei zu sai biao zhi 

    int epfd = epoll_create(1); // 1000

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    struct epoll_event events[1024] = {0};

    sockaddr clientaddr;
    socklen_t len;

    while(1)
    {
        int nready = epoll_wait(epfd, events, 1024, -1); //deng dai miao shu fu shi jian , bing fan hui shi jian lie biao 
        if(nready < 0) continue;

        for(int i = 0; i < nready; i++)
        {
            int connfd = events[i].data.fd;
            if(sockfd == connfd)
            {
                int clientfd = accept(sockfd, &clientaddr, &len);
                if(clientfd < 0)
                    continue;
                printf("clientfd:%d\n", clientfd);

                //ev.events = EPOLLIN | EPOLLET;  //shui ping chu fa
                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
            }
            else if(events[i].events & EPOLLIN)
            {
                char buffer[BUFFER_LENGTH] = {0};
                short len = 0;

                char buf[1024] = {0};
                
                int num = 0;
                int n = 0;
                do
                {
                   n = recv(connfd, buffer, BUFFER_LENGTH, 0);

                   printf("**********************************%d\n",n);
                   
                   if(n > 0)
                   {
                        memcpy(buf + num, buffer, n);    
                        num += n;
                        printf("%s\n",buf);
                   }
                   else if(n <= 0)
                   {
                        printf("close\n");
                        epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                        close(connfd);
                   }
                } while (n > 0);

                

                printf("recv:%s\n", buf);

            }
        }
    }



    return 0;

}