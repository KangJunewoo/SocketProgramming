/**
 * Linux System Term Project
 * Mission 2 - server.c
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
void error_handling(char *message);

// 클라이언트한테 파일을 보내기 위한 함수
void sendfile(int sock, FILE* fp);

// 전체 보낸 바이트 수
ssize_t total=0;

int main(int argc, char* argv[]){
  // fd를 저장할 서버소켓과 클라이언트 소켓 선언.
  ssize_t serv_sock;
  ssize_t clnt_sock;

  // 소켓 주소를 담는 구조체 역시 서버, 클라이언트 각각 선언
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;

  // 클라이언트 주소 크기
  socklen_t clnt_addr_size;

  // 메세지와 파일이름을 저장할 버퍼 선언.
  char message[30];
  char filename[30];

  // 포트번호가 입력되지 않았다면 에러처리.
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
  // 연결요청 queue size는 5로 잡음.
  if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");

  // 클라이언트 주소크기 저장
  clnt_addr_size=sizeof(clnt_addr);

  // ****** 4단계 - accept() 함수를 통한 연결 허용 ******
  clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
  if(clnt_sock==-1)
    error_handling("accept() error");

  // ****** 5단계 - 데이터 송수신 ******
  // 클라이언트한테서 메세지를 받아 출력
  if(read(clnt_sock, message, sizeof(message))==-1)
    error_handling("read() error");
  printf("Message from client : %s\n", message);

  // 파일이름을 입력받고, 클라이언트에게 파일 이름 전송
  printf("Enter filename to send : ");
  scanf("%s", filename);
  strcpy(message, filename);
  if(write(clnt_sock, message, sizeof(message))==-1)
    error_handling("cannot send filename");

  // 파일을 열고
  FILE *fp = fopen(filename, "rb");
  if(fp==NULL)
    error_handling("Cannot Open File");
  
  // sendfile 함수를 통해 파일데이터 전송.
  printf("Sending in progress...\n");
  sendfile(clnt_sock, fp);
  printf("Send Success, %ld bytes send\n", total);
  
  // 작업을 마친 후 파일 닫기
  fclose(fp);
  
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

void sendfile(int sock, FILE* fp){
  // 읽어들인 바이트를 담을 변수와 버퍼 선언
  ssize_t bytes_read;
  char buff[BUFF_SIZE]={0};

  // 1460바이트만큼 계속 읽어들이며 버퍼에 저장
  while((bytes_read=fread(buff, sizeof(char), BUFF_SIZE, fp))>0){
    total+=bytes_read;
    if(bytes_read!=BUFF_SIZE&&ferror(fp)){
      error_handling("File-Read Error");
    }

    // 읽기 완료된 버퍼의 데이터를 소켓을 통해 전송.
    if(write(sock, buff, bytes_read)==-1)
      error_handling("Cannot send file");

    // 다음 데이터를 읽기 전 버퍼 초기화
    memset(buff, 0, BUFF_SIZE);
  }
}