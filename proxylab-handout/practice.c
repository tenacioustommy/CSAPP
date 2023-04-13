#include"csapp.h"
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void rio_readinitb(rio_t *rp, int fd); 
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
ssize_t	rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t	rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

//unbuffered
ssize_t rio_readn(int fd,void *usrbuf,size_t n){
    char *buf=usrbuf;
    size_t nread;
    size_t nleft =n;
    while(nleft>0){
       if((nread=read(fd,buf,nleft))<0){
        //interrupt
            if(errno==EINTR){
                nread=0;
            }else{
                return -1;
            }
       }else if(nread==0){  
            break; //end of file
       }else{
            buf+=nread;
            nleft-=nread;
       }
    }
    return n-nleft;
}
ssize_t rio_writen(int fd,void *usrbuf,size_t n){
    char* buf=usrbuf;
    size_t nwrite;
    size_t nleft=n;
    while(nleft>0){
        if((nwrite = write(fd,buf,nwrite))<0){
            if(errno==EINTR)
                nwrite=0;
            else 
                return -1;
        }
        nleft-=nwrite;
        buf+=nwrite; 
    }
    return n-nleft;
}
static ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n){
    while(rp->rio_cnt<=0){//noThing to read
        if(rp->rio_cnt=read(rp->rio_fd,rp->rio_buf,sizeof(rp->rio_buf))<0){
            if(errno==EINTR)
                rp->rio_cnt=0;
            else
                return -1;
        }else if(rp->rio_cnt==0){
            break;
        }else{
            rp->rio_bufptr=rp->rio_buf;
        }
    }
    int cnt;
    cnt=(rp->rio_cnt<n)?(rp->rio_cnt):n;
    memcpy(usrbuf,rp->rio_bufptr,cnt);
    rp->rio_bufptr+=cnt;
    rp->rio_cnt-=cnt;
}
ssize_t rio_readlineb(rio_t *rp,void *usrbuf,size_t maxlen){
    int rv;
    char c;
    char *buf=usrbuf;
    int i;
    for(i=0;i<maxlen;i++){
        if((rv=rio_read(rp,&c,1))==1){
            *buf++=c;
            if(c=='\n'){
                i++;
                break;
            }
        }else if(rv==0){
            if(i==1)
                return 0;
            break;
        }else 
            return -1;
    }
    *buf=0;
    return i-1;
}