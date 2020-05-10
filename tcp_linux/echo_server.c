#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char* message);

int main(int argc, char* argv[]){
  // fd를 저장할 변수 두 개 선언.
  int serv_sock, clnt_sock;
  char message[BUF_SIZE];
  int str_len, i;

  /**
   * sockaddr_in은 소켓주소 담는 구조체라고 생각하면 됨.
   * 생전 처음보는 자료형들이 가득하구먼..
   * 
   * sa_family_t sin_family : ipv4같은 주소체계.
   * uint16_t sin_port : unsigned short(부호없는 16비트짜리 int) -> 포트번호 16비트
   * struct in_addr sin_addr : IP주소 32비트
   * char sin_zero[8] : 안씀.
   * 
   * 여기서 stuct in_addr는
   * in_addr_t s_addr : uint32_t 즉 unsigned long(부호없는 32비트짜리 int)
  */
  
  struct sockaddr_in serv_addr, clnt_addr;
  socklen_t clnt_addr_size;

  // 만약 포트번호 안입력했다면 입력하라고 하자.
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

  /**
   * 3단계 - 연결요청 대기상태 진입
   * 성공시 0, 실패시 -1 반환.
   * 인자로 fd, 요청대기 큐의 크기 전달.
  */
  if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");

  clnt_addr_size=sizeof(clnt_addr);
  
  /**
   * 4,5,6단계 반복
   * 
   * 4단계 - 연결 허용 이제 진짜 연결되어서 clnt_addr 활성화됨.
   * 성공시 fd, 실패시 -1 반환.
   * 인자로 client fd, client 주소, 주소크기 들어감.
   * 
   * 5단계 - 데이터 송수신
   * write : client fd, msg to send, msg크기
   * read : client fd, msgto be saved, msg크기
   * 둘 다 성공시 송신/수신 바이트 수, 실패시 -1 반환.
   * 
   * 6단계 - 연결 종료
   * 성공시 0, 실패시 -1 리턴.
   * 인자로 fd 들어감.
  */
  for(i=0;i<5;i++){
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock==-1)
      error_handling("accept() error");
    else
      printf("Connected client %d\n", i+1);

    // 받으면 그대로 client로 echo
    while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
      printf("Message from client : %s\n", message);
      write(clnt_sock, message, str_len);

    close(clnt_sock);
  }
  
  close(serv_sock);
  return 0;
  
}

void error_handling(char* message){
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
