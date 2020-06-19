/**
 * Linux System Term Project
 * Mission 2 - client.c
 * 
 * Author : 2016312924 강준우
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 버퍼사이즈 선언. 1460바이트씩 보낸다.
#define BUFF_SIZE 1460

// 에러핸들링을 위한 함수
void error_handling(char* message);

// 서버한테서 받은 파일을 write하는 함수
void writefile(int sock, FILE *fp);

// 전체 쓰여진 바이트 수
ssize_t total=0;

int main(int argc, char* argv[]){
  // fd를 저장할 소켓과 주소변수 선언
  int sock;
  struct sockaddr_in serv_addr;
  
  // 메세지와 파일이름을 저장할 버퍼 선언.
  // 처음에 보낼 메세지인 "Hello Server!"로 초기화.
  char message[30]="Hello Server!";
  char filename[30];

  // IP와 포트번호 제대로 입력되지 않은 경우 에러처리.
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

  // ****** 3단계 - 데이터 송수신 ******
  // 서버한테 Hello Server 전송 후
  printf("Sending \"%s\" to server..\n", message);
  write(sock, message, strlen(message)+1);
  
  // 서버한테서 파일이름을 받아 저장.
  if(read(sock, message, sizeof(message))==-1)
    error_handling("read() error");
  printf("Filename : %s\n\n", message);
  strcpy(filename, message);

  // 파일을 열고
  FILE *fp = fopen(filename, "wb");
  if(fp==NULL)
    error_handling("Cannot open file");
  
  // writefile 함수를 통해 파일데이터 저장.
  printf("Downloading in progress...\n");
  writefile(sock, fp);
  printf("Receive Success, %ld bytes received.\n", total);
  
  // 작업을 마친 후 파일 닫기
  fclose(fp);


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

void writefile(int sock, FILE *fp){
  // 읽어들인 바이트를 담을 변수와 버퍼 선언
  ssize_t bytes_read;
  char buff[BUFF_SIZE]={0};

  // 1460바이트만큼 계속 읽어들이며 버퍼에 저장
  while((bytes_read=read(sock,buff,BUFF_SIZE))>0){
    total+=bytes_read;
    if(bytes_read==-1)
      error_handling("Receiving Error");

    // 읽기 완료된 버퍼의 데이터를 파일에 write함.
    if(fwrite(buff, sizeof(char), bytes_read, fp)==-1)
      error_handling("File-Writing Error");

    // 다음 데이터를 읽기 전 버퍼 초기화
    memset(buff, 0, BUFF_SIZE);
  }
}