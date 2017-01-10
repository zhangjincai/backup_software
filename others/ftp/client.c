#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define N 256

typedef struct sockaddr SA;

void commd_help();
void commd_exit();
void commd_ls(struct sockaddr_in, char *);
void commd_get(struct sockaddr_in , char *);
void commd_put(struct sockaddr_in , char *);

int main(int argc, char *argv[])
{
    char commd[N];
    struct sockaddr_in addr;
    int len;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8989);
    len = sizeof(addr);

    while(1)
    {
        printf("ftp>");
        bzero(commd,N);
        if(fgets(commd,N,stdin) == NULL)
        {
            printf("Fgets Error!\n");
            return -1;
        }

        commd[strlen(commd)-1]='\0';

        printf("Input Command Is [ %s ]\n",commd);

        if(strncmp(commd,"help",4) == 0)
        {
            commd_help();
        }else if(strncmp(commd, "exit",4) == 0)
        {
            commd_exit();
            exit(0);
        }else if(strncmp(commd, "ls" , 2) == 0)
        {
            commd_ls(addr, commd);
        }else if(strncmp(commd, "get" , 3) == 0)
        {
            commd_get(addr, commd);
        }else if(strncmp(commd, "put", 3) ==0 )
        {
            commd_put(addr, commd);
        }else
        {
            printf("Command Is Error!Please Try Again!\n");
        }

    }    
    return 0;
}

void commd_help()
{
    
    printf("\n=------------------- Welcome to Use the Ftp ----------------=\n");
    printf("|                                                           |\n");
    printf("|  help : Display All Command for the Server                |\n");
    printf("|                                                           |\n");
    printf("|   exit: Quit The Sever                                    |\n");
    printf("|                                                           |\n");
    printf("|   ls : Display All file On the Ftp Server                 |\n");
    printf("|                                                           |\n");
    printf("| get <file>: Download FIle from the Ftp Server             |\n");
    printf("|                                                           |\n");
    printf("| put <file>: Upload FIle to the Ftp Server                 |\n");
    printf("|                                                           |\n");
    printf("=-----------------------------------------------------------=\n");

    return ;
}

void commd_exit()
{
    printf("Byte!\n");
}

void commd_ls(struct sockaddr_in addr, char *commd)
{
    int sockfd;

    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }    

    if(connect(sockfd, (SA *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    
    if(write(sockfd, commd, N) < 0)
    {
        printf("Write Error!\n");
        exit(1);
    }

    while(read(sockfd, commd, N) > 0)
    {
        printf(" %s ",commd);
    }
    printf("\n");

    close(sockfd);
    return ;
}

void commd_get(struct sockaddr_in addr, char *commd)
{
    int fd;
    int sockfd;
    char buffer[N];
    int nbytes;

    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }

    if(connect(sockfd, (SA *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    
    if(write(sockfd, commd, N) < 0)
    {
        printf("Write Error!At commd_get 1\n");
        exit(1);
    }

    if(read(sockfd, buffer, N) < 0)
    {
        printf("Read Error!At commd_get 1\n");
        exit(1);
    }

    if(buffer[0] =='N')
    {
        close(sockfd);
        printf("Can't Open The File!\n");
        return ;
    }

    if((fd=open(commd+4, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
    {
        printf("Open Error!\n");
        exit(1);
    }

    while((nbytes=read(sockfd, buffer, N)) > 0)
    {
        if(write(fd, buffer, nbytes) < 0)
        {
            printf("Write Error!At commd_get 2");
        }
    }

    close(fd);
    close(sockfd);

    return ;

}

void commd_put(struct sockaddr_in addr, char *commd)
{
    int fd;
    int sockfd;
    char buffer[N];
    int nbytes;

    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error!\n");
        exit(1);
    }

    if(connect(sockfd, (SA *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect Error!\n");
        exit(1);
    }
    
    if(write(sockfd, commd, N)<0)
    {
        printf("Wrtie Error!At commd_put 1\n");
        exit(1);
    }

    if((fd=open(commd+4, O_RDONLY)) < 0)
    {
        printf("Open Error!\n");
        exit(1);
    }

    while((nbytes=read(fd, buffer, N)) > 0)
    {
        if(write(sockfd, buffer, nbytes) < 0)
        {
            printf("Write Error!At commd_put 2");
        }
    }

    close(fd);
    close(sockfd);

    return ;
}
