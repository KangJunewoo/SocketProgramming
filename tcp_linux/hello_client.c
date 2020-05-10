#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char* message);

int main(int argc, char* argv[]){
  // fd 저장할 변수
  int sock;

  // serv_addr 저장할 주소
  struct sockaddr_in serv_addr;
  
  // 메세지 버퍼와 그걸 담을 크기.
  char message[30];
  int str_len;

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
  str_len=read(sock, message, sizeof(message)-1);
  if(str_len==-1)
    error_handling("read() error");
  printf("message from server:%s\n", message);
  
  // 4단계 - 연결 종료.
  close(sock);
  return 0;
}

void error_handling(char* message){
  fputs(message,stderr);
  fputc('\n',stderr);
  exit(1);
}
