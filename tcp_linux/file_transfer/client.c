// server on
// client on, access server and send "hello server"
// server sends file to client by packet of 1460 bytes
// client receives packets, storing in buffer, write file at last.
// debug : compare contents, size of send file / receive file by cmp or diff.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "transfer_header.h"

void error_handling(char* message);
void writefile(int sockfd, FILE *fp);

int main(int argc, char* argv[]){
  // fd 저장할 변수
  int sock;

  // serv_addr 저장할 주소
  struct sockaddr_in serv_addr;
  
  // 이 메세지를 서버한테 보내는 게 목표!!
  char message[]="hello world!";

  // IP와 포트번호 제대로 입력해!
  if(argc!=3){
    printf("usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  // 1단계 - 소켓생성
  sock=socket(PF_INET, SOCK_STREAM, 0);
  if(sock==-1)
    error_handling("socket() error");

  // 서버주소 터미널에서 입력한대로 설정
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
  serv_addr.sin_port=htons(atoi(argv[2]));


  /**
   * 2단계 - 연결요청
   * 성공시 0, 실패시 -1 리턴.
   * 인자로 fd, 서버주소값, 서버주소크기 들어감.
  */
  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("connect() error");

  /**
   * 3단계 - 데이터 송수신
   * write : client fd, msg to send, msg크기 (for server)
   * read : client fd, msg to be saved, msg크기 (for client)
  */
  write(sock, message, sizeof(message));
  
  // TODO : client receives packets, storing in buffer, write file at last.

  // TODO : debug : compare contents, size of send file / receive file by cmp or diff.
  // 4단계 - 연결 종료.
  close(sock);
  return 0;
}

void error_handling(char* message){
  fputs(message,stderr);
  fputc('\n',stderr);
  exit(1);
}

void writefile(int sockfd, FILE *fp){
  ssize_t n;
  char buff[MAX_LINE]={0};

  while((n=recv(sockfd,buff,MAX_LINE,0))>0){
    total+=n;
    if(n==-1){
      error_handling("Receiving Error");
    }
    if(fwrite(buff, sizeof(char), n, fp)!=n){
      error_handling("File-Writing Error");
    }

    memset(buff, 0, MAX_LINE);
  }
}