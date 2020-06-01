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
#include <libgen.h>
#include <netinet/in.h>
#define MAX_LINE 1460
#define BUFFSIZE 1460
void error_handling(char *message);
void sendfile(FILE *fp, int sockfd);
ssize_t total=0;

int main(int argc, char* argv[]){
  int serv_sock;
  int clnt_sock;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;

  socklen_t clnt_addr_size;

  // 메세지 버퍼와 그걸 담을 크기.
  char message[30];
  int str_len;

  if(argc!=2){
    printf("usage : %s <port>\n", argv[0]);
    exit(1);
  }

  /**
   * 1단계 - 소켓생성
   * 성공 시 fd, 실패시 -1 반환
   * 
   * 인자1 PF_INET : ipv4에서 돌리겠다.
   * 인자2,3 : TCP or UDP 결정.
   * TCP : SOCK_STREAM, IPPROTO_TCP
   * UDP : SOCK_DGRAM, IPPROTO_UDP
  */

  serv_sock=socket(PF_INET, SOCK_STREAM, 0);
  if(serv_sock==-1)
    error_handling("socket() error");

  /**
   * memset으로 serv_addr를 0으로 싹 초기화해주고
   * 주소체계는 ipv4로
   * ip주소 세팅 : host_order to network_order, for long. 자기 IP주소를 네트워크 바이트 형태로 바꿔 할당.
   * 포트번호 세팅 : host_order to network_order, for short. argument를 int로 바꾸고 네트워크 바이트 형태로 바꿔 할당.
  */

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port=htons(atoi(argv[1]));

  /**
   * 2단계 - 소켓 주소 할당
   * 성공시 0, 실패시 -1 리턴.
   * 인자로 fd, 서버주소, 주소값의 길이정보 들어감.
  */

  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");

  if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");

  clnt_addr_size=sizeof(clnt_addr);

  /**
   * 4단계 - 연결 허용 이제 진짜 연결되어서 clnt_addr 활성화됨.
   * 성공시 fd, 실패시 -1 반환.
   * 인자로 client fd, client 주소, 주소크기 들어감.
  */
  clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
  if(clnt_sock==-1)
    error_handling("accept() error");

  /**
   * 5단계 - 데이터 송수신
   * write : client fd, msg to send, msg크기 (for server)
   * read : client fd, msgto be saved, msg크기 (for client)
  */
  /*
  str_len=read(clnt_sock, message, sizeof(message)-1);
  if(str_len==-1)
    error_handling("read() error");
  printf("message from client:%s\n", message);
  */
  // TODO : server sends file to client by packet of 1460 bytes

  // FIXME : 지금은 임시로 test.mp4로 설정함. 이거 argv로 받아야해.
  char* filename = "test.mp4";
  if(filename==NULL)
    error_handling("cannot get filename");

  char buff[BUFFSIZE]={0};
  strncpy(buff, filename, strlen(filename));
  if(send(clnt_sock, buff, BUFFSIZE, 0)==-1)
    error_handling("cannot send filename");

  // FIXME : 여기도 마찬가지.
  FILE *fp = fopen("test.mp4", "rb");
  if(fp==NULL)
    error_handling("Cannot Open File");

  sendfile(fp, clnt_sock);
  printf("Send Success, %ld bytes send\n", total);
  fclose(fp);
  /**
   * 6단계 - 연결 종료
   * 성공시 0, 실패시 -1 리턴.
   * 인자로 fd 들어감.
  */
  close(clnt_sock);
  close(serv_sock);
  return 0;
}

void error_handling(char* message){
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

void sendfile(FILE *fp, int sockfd){
  int n;
  char sendline[MAX_LINE]={0};

  while((n=fread(sendline, sizeof(char), MAX_LINE, fp))>0){
    total+=n;

    if(n!=MAX_LINE&&ferror(fp)){
      error_handling("File-Read Error");
    }

    if(send(sockfd, sendline, n, 0)==-1)
      error_handling("Cannot send file");

    memset(sendline, 0, MAX_LINE);
  }
}