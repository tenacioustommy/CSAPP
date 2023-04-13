#include"csapp.h"
int main(int argc ,char** argv ){
    if(argc!=3){
        unix_error("format error");
    }
    char *hostname=argv[1];
    char *port=argv[2];
    int clientid=Open_clientfd(hostname,port);
    char buf[MAXLINE];
    char* rptr =NULL;
    rio_t rio;
    Rio_readinitb(&rio,clientid);
    while((rptr=Fgets(buf,MAXLINE,stdin))!=NULL){
        Rio_writen(rio.rio_fd,buf,strlen(buf));
        Rio_readlineb(&rio,buf,MAXLINE);
        printf("%s",buf);
    }
    exit(0);
}