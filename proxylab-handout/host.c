#include"csapp.h"
// struct addrinfo
// {
//   int ai_flags;			/* Input flags.  */
//   int ai_family;		/* Protocol family for socket.  */
//   int ai_socktype;		/* Socket type.  */
//   int ai_protocol;		/* Protocol for socket.  */
//   socklen_t ai_addrlen;		/* Length of socket address.  */
//   struct sockaddr *ai_addr;	/* Socket address for socket.  */
//   char *ai_canonname;		/* Canonical name for service location.  */
//   struct addrinfo *ai_next;	/* Pointer to next in list.  */
// };
// ./host www.google.com
int main(int argc,char **argv){
    if(argc!=2){
        unix_error("format ./host www.example.com");
    }
    struct addrinfo hints;
    struct addrinfo* res,*cur;
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    Getaddrinfo(argv[1],NULL,&hints,&res);
    char hostname[MAXLINE];
    int flags=NI_NUMERICHOST;
    for(cur=res;cur!=NULL;cur=cur->ai_next){
        Getnameinfo(cur->ai_addr,cur->ai_addrlen,hostname,MAXLINE,NULL,0,flags);
        printf("%s,%s\n",cur->ai_canonname,hostname);

    }
    Freeaddrinfo(res);
    exit(0);
}