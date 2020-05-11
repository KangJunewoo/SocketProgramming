/**
 * Original Code by
 * https://github.com/omair18/Socket-Programming-in-C
 * 
 * 파일을 받는 서버
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

void writefile(int sockfd, FILE *fp);
ssize_t total=0;
int main(int argc, char *argv[]) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }
    
    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) 
    {
        perror("Bind Error");
        exit(1);
    }

    if (listen(sockfd, LINSTENPORT) == -1) 
    {
        perror("Listen Error");
        exit(1);
    }

    socklen_t addrlen = sizeof(clientaddr);
    int connfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
    if (connfd == -1) 
    {
        perror("Connect Error");
        exit(1);
    }
    close(sockfd); 

    // 여기부터 봅시다
    // 서버이자, 파일을 받는 역할.

    // 파일이름이 들어갈 버퍼를 만들고
    char filename[BUFFSIZE] = {0}; 
    
    // 파일이름을 받았다면, filename에 저장하고 아니면 에러띄우고.
    if (recv(connfd, filename, BUFFSIZE, 0) == -1) 
    {
        perror("Can't receive filename");
        exit(1);
    }

    // 받은 filename의 이름으로 파일을 열고
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) 
    {
        perror("Can't open file");
        exit(1);
    }
    
    /**
     * INET_ADDRSTRLEN은 ipv4의 길이, 즉 16이지.
     * filename에서 inet_ntop로 받겠다고 출력.
     * inet_ntop(ip정보, 주소정보, 저장될버퍼, 버퍼크기)
     * ipv4 혹은 ipv6 주소정보를 바이너리 -> 텍스트로 바꿔줌.
     * 즉 start receive file : merong.txt from 127.0.0.1 식으로 나옴.
    */
    char addr[INET_ADDRSTRLEN];
    printf("Start receive file: %s from %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
    
    // 파일 작성!!!
    writefile(connfd, fp);
    printf("Receive Success, NumBytes = %ld\n", total);
    

    // 파일닫고 마무리
    fclose(fp);
    close(connfd);
    return 0;
}

void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    // buff는 받은 파일이 저장될 버퍼겠지?
    // 4096바이트씩.
    char buff[MAX_LINE] = {0};
    
    /**
     * 파일을 받아주는 recv 함수
     * 소켓에서 받아 buff에 저장한다.
     * 근데 이게 반복문이 되는거구나.
    */
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) 
    {
	    // total에 읽은 바이트 수가 차오르고
        total+=n;
        
        // recv 리턴값이 -1이면 수신실패니까 끝내버리고
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }
        
        /**
         * 잘 받았으면 fwrite 함수로 fp에 저장.
         * 파일을 써주는 fwrite 함수, 찾아보지 않아도 fread랑 메커니즘이 같을듯.
         * size_t fwrite(void* ptr, size_t size, size_t count, FILE* stream)
         * 파일스트림에 count개만큼 size크기에 해당하는 정보를 ptr로부터 쓴다.
         * 성공시 읽은 바이트 수, 즉 size*count 반환. 실패하면 -1 반환할듯.
        */
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        
        // memset 함수로 버퍼 초기화!
        memset(buff, 0, MAX_LINE);
    }
}
