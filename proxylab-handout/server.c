#include"csapp.h"
void echo(int connfd){
    rio_t rio;
    Rio_readinitb(&rio,connfd);
    int n;
    char buf[MAXLINE];
    while((n=Rio_readlineb(&rio,buf,MAXLINE))>0){
        printf("recieve from client %s\n",buf);
        Rio_writen(rio.rio_fd,buf,n);
    }

}
int main(int argc ,char** argv ){
    if(argc!=2){
        unix_error("format error");
    }
    int listenfd=Open_listenfd(argv[1]);
    while(1){
        struct sockaddr clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        char host[MAXLINE];
        memset(&clientaddr,0,addrlen);
        int connfd=Accept(listenfd,&clientaddr,&addrlen);
        Getnameinfo(&clientaddr,addrlen,host,MAXLINE,NULL,0,0);
        printf("receive from client %s\n",host);

        echo(connfd);
        close(connfd);

    }
    exit(1);
}