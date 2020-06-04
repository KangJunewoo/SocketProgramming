/**
 * Linux System Term Project
 * Mission 1 - client.c
 * 
 * Author : 2016312924 강준우
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 에러핸들링을 위한 함수
void error_handling(char* message);

int main(int argc, char* argv[]){
  // fd와 주소를 저장할 소켓과 주소변수 선언
  ssize_t sock;
  struct sockaddr_in serv_addr;
  
  // 메세지 버퍼와 해당 크기 선언.
  char message[30]="hello server!";
  int str_len;

  // IP와 포트번호가 제대로 입력되지 않은 경우 에러처리.
  if(argc!=3){
    printf("usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }
  


  // ****** 1단계 - socket() 함수를 통한 소켓생성 ******
  
  // IPV4, TCP를 사용함
  sock=socket(PF_INET, SOCK_STREAM, 0);
  if(sock==-1)
    error_handling("socket() error");

  // 서버주소 초기화 후, 주소체계(IPV4) & IP주소 & 포트번호 세팅
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
  serv_addr.sin_port=htons(atoi(argv[2]));


  // ****** 2단계 - connect() 함수를 통한 연결 요청 ******

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("connect() error");


  // ****** 3단계 - read()와 write() 함수를 이용한 데이터 송수신 ******
  // TODO : strlen vs sizeof
  // 서버한테 메세지 전송 후
  printf("Sending \"%s\" to server..\n", message);
  write(sock, message, strlen(message));
  
  // 서버한테서 메세지를 읽고 에러가 없다면 print
  str_len=read(sock, message, strlen(message));
  if(str_len==-1)
    error_handling("read() error");
  printf("Message from server : %s\n", message);
  
  // ****** 4단계 - close() 함수를 이용한 연결 종료 ******
  close(sock);
  
  // 프로그램 종료
  return 0;
}

void error_handling(char* message){
  fputs(message,stderr);
  fputc('\n',stderr);
  exit(1);
}
