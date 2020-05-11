/**
 * Original Code by
 * https://github.com/omair18/Socket-Programming-in-C
 * 
 * 파일을 보내는 클라이언트
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

void sendfile(FILE *fp, int sockfd);
ssize_t total=0;
int main(int argc, char* argv[])
{
    if (argc != 3) 
    {
        perror("usage:send_file filepath <IPaddress>");
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);
    if (inet_pton(AF_INET, argv[2], &serveraddr.sin_addr) < 0)
    {
        perror("IPaddress Convert Error");
        exit(1);
    }

    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }
    
    // 여기부터 봅시다
    // 클라이언트이자, 파일을 주는 역할.

    // 파일이름 저장한 뒤
    char *filename = basename(argv[1]); 
    if (filename == NULL)
    {
        perror("Can't get filename");
        exit(1);
    }
    
    // 버퍼만큼의 배열 0으로 초기화
    char buff[BUFFSIZE] = {0};
    // 파일이름 버퍼에 저장.
    strncpy(buff, filename, strlen(filename));
    // 파일 이름 먼저 보내기! -> 쉽지.
    if (send(sockfd, buff, BUFFSIZE, 0) == -1)
    {
        perror("Can't send filename");
        exit(1);
    }
    
    // 파일을 열고
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) 
    {
        perror("Can't open file");
        exit(1);
    }

    // 파일을 보낸다아아아아
    sendfile(fp, sockfd);
    //puts("Send Success");
    printf("Send Success, NumBytes = %ld\n", total);
    
    // 파일닫고 마무리
    fclose(fp);
    close(sockfd);
    return 0;
}

void sendfile(FILE *fp, int sockfd) 
{
    int n; 
    // sendline은 아마 한번에 보내는 양을 나타내는 거겠지?
    // 4096바이트씩 읽어서 보내나보다.
    char sendline[MAX_LINE] = {0}; 
    
    /**
     * 파일을 읽어주는 fread 함수
     * size_t fread(void* ptr, size_t size, size_t count, FILE* stream)
     * 파일스트림에서 count개만큼 size크기에 해당하는 정보를 읽어 ptr 배열에 담는다.
     * 성공시 읽은 바이트 수, 즉 size*count 반환.
     * 
    */
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
	    // total에 읽은 바이트 수가 차오르고
        total+=n;
        
        /**
         * 파일의 오류를 검사해주는 ferror 함수
         * 정상이면 0 리턴, 오류뜨면 0이 아닌 값 리턴.
         * 즉 라인을 다 안읽었으면서 파일에 오류가 떴다면 읽기 실패!
         * perror 함수로 오류를 출력해준다.
        */
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        /**
         * 오류가 아니라면 보내야겠지.
         * 소켓에 sendline과 크기n을 싣고 보낸다~~
         * 에러뜨면 마찬가지로 처리해주고
        */
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }

        // memset 함수로 초기화!
        memset(sendline, 0, MAX_LINE);
    }
}
