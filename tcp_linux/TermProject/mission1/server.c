/**
 * Linux System Term Project
 * Mission 1 - server.c
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
  // fd를 저장할 서버소켓과 클라이언트 소켓 선언.
  ssize_t serv_sock;
  ssize_t clnt_sock;
  int str_len;

  // 소켓 주소를 담는 구조체 역시 서버, 클라이언트 각각 선언
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  
  // 클라이언트 주소 크기
  socklen_t clnt_addr_size;

  // 메세지를 담을 버퍼. 여기 담긴 메세지를 클라이언트한테 보내는 것이 목표.
  char message[30];

  // 만약 포트번호가 입력되지 않았다면 에러처리.
  if(argc!=2){
    printf("usage : %s <port>\n", argv[0]);
    exit(1);
  }

  // ****** 1단계 - socket() 함수를 통한 소켓생성 ******
  
  // IPV4, TCP를 사용함.
  serv_sock=socket(PF_INET, SOCK_STREAM, 0);
  if(serv_sock==-1)
    error_handling("socket() error");

  // 서버주소 초기화 후, 주소체계(IPV4) & IP주소 & 포트번호 세팅
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port=htons(atoi(argv[1]));

  // ****** 2단계 - bind() 함수를 통한 소켓 주소 할당 ******

  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");

  // ****** 3단계 - listen() 함수를 통한 연결요청 대기상태 진입 ******

  if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");

  clnt_addr_size=sizeof(clnt_addr);
  
  // ****** 4단계 - accept() 함수를 통한 연결 허용 ******

  clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
  if(clnt_sock==-1)
    error_handling("accept() error");


  // ****** 5단계 - read()와 write() 함수를 이용한 데이터 송수신 ******

  // 클라이언트한테서 메세지를 읽고 에러가 없다면 print
  str_len = read(clnt_sock, message, sizeof(message));
  if(str_len==-1)
    error_handling("read() error");
  printf("Message from client : %s\n", message);

  // 그 뒤 클라이언트에게 메세지 전송
  strcpy(message, "hello client!");
  printf("Sending \"%s\" to client..\n", message);
  write(clnt_sock, message, strlen(message));
  
  // ****** 6단계 - close() 함수를 이용한 연결 종료 ******
  
  close(clnt_sock);
  close(serv_sock);
  
  // 프로그램 종료
  return 0;
}

void error_handling(char* message){
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
