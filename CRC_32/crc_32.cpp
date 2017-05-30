#pragma warning (disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 10000					
#define BUFF_SIZE 100

void start(void);
void server(); //송신자측 서버 연결
void client(); //수신자측 클라이언트 연결
void send_messge(); //송신자측 처리
void recv_message();//수신자측 처리
long long encoding(long long num);
//각 블록에 대해 CRC-32 오류검출 코드가 추가된 전송 프레임 생성
long long decoding(long long num);
//각 블록에 대한 CRC-32 오류검사 수행
void print_binary(long long num);
//64byte 변수를 2진수로 출력하는 함수
//검토를 위해 만들었습니다.

WSADATA wsdata;
SOCKET server_sock, client_sock;
SOCKADDR_IN server_addr;
char str[8];
char msg[BUFF_SIZE];
unsigned char buffer[BUFF_SIZE];
int is_server = 0;
long long dataword1, dataword2, codeword1, codeword2;

int main()
{
	start();
	//송신을 할 것인지 수신을 할 것인지 역활을 정합니다.
	if (is_server == 1)
		send_messge();
	//송신측 처리 입니다.
	else
		recv_message();
	//수신측 처리 입니다.
	return 0;
}

void start(void) {
	int input;
	//system("cls");
	printf("                 Welcome to CRC32  \n\n\n");
	printf("                                         made in 권대영\n\n");
	printf("-------------------------------------------------------\n\n");

	printf("원하는 동작의 번호를 입력해 주세요! \n\n");
	printf("(	1.서버 동작		2.클라이언트 동작	)\n\n입력 : ");
	scanf("%d", &input);
	//송신을 할 것인지 수신을 할 것인지 역활을 정합니다.

	if (input == 1) {
		server();
	}
	else if (input == 2) {
		client();
	}
	else
		printf("올바른 입력이 아닙니다.");
	return;
}

void server()
{//데이터 통신을 하기 위해 송신측(서버) 연결 과정입니다.
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		printf("\n   윈도우 소켓을 초기화 하는데 실패 했습니다!!\n");
		return;
	}
	else
		printf("\n   윈도우 소켓을 초기화 했습니다.\n");

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) {
		printf("   서버 소켓을 생성하는데 실패 했습니다.\n");
		return;
	}
	else
		printf("   서버 소켓을 초기화 했습니다.\n");

	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	bind(server_sock, (SOCKADDR *)&server_addr, sizeof(server_addr));

	listen(server_sock, 5);

	printf("   클라이언트의 접속을 기다립니다......\n");
	client_sock = accept(server_sock, NULL, NULL);
	printf("   클라이언트가 접속 했습니다.\n");

	is_server = 1;
	printf("   ");
	system("pause");

	return;
}

void client()
{//데이터 통신을 하기 위해 수신측(클라이언트) 연결 과정입니다.
	char server_ip[30];

	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		printf("   윈도우 소켓을 초기화 하는데 실패 했습니다!!\n");
		return;
	}
	else
		printf("   윈도우 소켓을 초기화 했습니다.\n");

	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) {
		printf("   클라이언트 소켓을 생성하는데 실패 했습니다.\n");
		return;
	}
	else
		printf("   클라이언트 소켓을 초기화 했습니다.\n");

	ZeroMemory(&server_addr, sizeof(server_addr));

	printf("   서버주소를 입력하세요 (ex.127.0.0.1): ");
	scanf("%s", server_ip);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	connect(client_sock, (SOCKADDR *)(&server_addr), sizeof(server_addr));

	is_server = 0;
	printf("   ");
	system("pause");

	return;
}

void send_messge()
{//송신측 처리
		int input;
		printf("\n전송할 문자를 입력해 주세요(8자) : ");
		scanf("%c", &str[0]);
		for (int i = 0; i < 8; i++)
			scanf("%c", &str[i]);
		//사용자로 부터 8개의 문자를 입력받습니다.

		for (int i = 0; i < 4; i++)
		{
			dataword1 += str[i];
			dataword1 = dataword1 << 8;
		}
		dataword1 = dataword1 << 24;

		for (int i = 4; i < 8; i++)
		{
			dataword2 += str[i];
			dataword2 = dataword2 << 8;
		}
		dataword2 = dataword2 << 24;
		/*8개의 문자를 4문자씩 2개의 블록으로 만듭니다.
		2진 계산을 위해 문자들을 8byte크기의 long long형 변수에 넣습니다.
		dataword1 <= ABCD
		dataword2 <= EFGH
		*/

		codeword1 = encoding(dataword1);
		codeword2 = encoding(dataword2);
		//각 블록에 대해 CRC-32오류 검출 코드가 추가된 전송프레임을 생성합니다.
		
		printf("\n해당 메세지 : ");
		for (int i = 0; i < 8; i++)
			printf("%c", str[i]);
		printf("를 어떻게 보내시겠습니까?\n");
		printf("(	1.정상전송		2.오류전송	)\n입력:");
		scanf("%d", &input);
		//인위적인 오류 삽입 유무 확인

		printf("\n생성된 CRC-32 오류검출 코드가 추가된 전송 프레임은 : \n");
		long long discode = 0;
		if (input == 1)
		{//오류없는 정상적인 전송
			for (int i = 0; i < 8; i++)
			{
				msg[i] = (codeword1 >> (56 - (i * 8)));
				printf("%c ", msg[i]);
				discode = msg[i];
				codeword1 = codeword1 - (discode << (56 - (i * 8)));
			}
			for (int i = 0; i < 8; i++)
			{
				msg[i + 8] = (codeword2 >> (56 - (i * 8)));
				printf("%c ", msg[i+8]);
				discode = msg[i + 8];
				codeword2 = codeword2 - (discode << (56 - (i * 8)));
			}
			/*
			CRC코드가 추가된 64bit 변수를 전송을 위해
			8bit 씩 char형 배열에 나눠 담는 과정입니다.
			*/
			printf("	입니다.\n\n\n");
			send(client_sock, msg, BUFF_SIZE, 0);
			//수신자에게 전송!
		}
		else if (input == 2)
		{//인위적인 전송 에러 발생 경우
			for (int i = 0; i < 8; i++)
			{
				msg[i] = (codeword1 >> (56 - (i * 8))) + 1;
				//인위적인 전송에러 발생 1bit씩 추가
				printf("%c ", msg[i]);
				discode = msg[i];
				codeword1 = codeword1 - (discode << (56 - (i * 8)));
			}
			for (int i = 0; i < 8; i++)
			{
				msg[i + 8] = (codeword2 >> (56 - (i * 8)));
				printf("%c ", msg[i]);
				discode = msg[i + 8];
				codeword2 = codeword2 - (discode << (56 - (i * 8)));
			}
			/*
			CRC코드가 추가된 64bit 변수를 전송을 위해
			8bit 씩 char형 배열에 나눠 담는 과정입니다.
			*/
			printf("	입니다.\n");
			printf("인위적인 전송에러를 발생시켰습니다.\n\n");
			send(client_sock, msg, BUFF_SIZE, 0);
			//수신자에게 전송!
		}
		else
			printf("올바른 입력이 아닙니다.\n");
		closesocket(server_sock);
		closesocket(client_sock);
}

void recv_message()
{//수신측 처리
		printf("메세지를 기다리는 중입니다.\n");
		recv(client_sock, msg, BUFF_SIZE, 0);
		//수신측에서 메세지를 수신합니다.
		
		printf("\n전달 받은 데이터 : ");
		for (int i = 0; i < 16; i++)
		{
			buffer[i] = msg[i];
			printf("%c ", buffer[i]);
		}
		printf("\n\n");
		codeword1 = 0;
		codeword2 = 0;

		for (int i = 0; i < 7;i++)
		{
			codeword1 = codeword1 +(long long)buffer[i];
			codeword1 = codeword1 << 8;
		}
		codeword1 = codeword1 + buffer[7];
		for (int i = 8; i < 15;i++)
		{
			codeword2 = codeword2 + (long long)buffer[i];
			codeword2 = codeword2 << 8;
		}
		codeword2 = codeword2 + buffer[15];;
		/*
		전송을 위해 char(8bit)형으로 나눴던 데이터 블록들을
		다시 2진 연산을 위해 64bit 짜리 long long형 변수에
		이어 붙여 만듭니다.
		*/

		print_binary(codeword1);
		print_binary(codeword2);
		//전송받은 블록을 2진수로 출력(검토)

		dataword1 = decoding(codeword1);
		dataword2 = decoding(codeword2);
		//각각의 블록에 대해 CRC-32오류 검사를 합니다.

		if (dataword1 != 0 && dataword2 != 0)
		{
			printf("오류검출 테스트 결과 오류 없음!!\n\n");
			printf("전달받은 데이터는 : ");
			long long discord = 0;
			for (int i = 0; i < 4; i++)
			{
				discord = dataword1 >> (24 - (i * 8));
				printf("%c ", discord);
				dataword1 -= discord << (24 - (i * 8));
			}
			for (int i = 0; i < 4; i++)
			{
				discord = dataword2 >> (24 - (i * 8));
				printf("%c ", discord);
				dataword2 -= discord << (24 - (i * 8));
			}
			printf("입니다.\n\n");
		}
		closesocket(client_sock);
}

long long encoding(long long num)
{
	long long result = 0;
	long long remainder = num;
	//나머지
	long long divisor = 4374732215;
	//CRC-32 표준다항식
	divisor = divisor << 30;
	//dataword와 자리수를 맞춰주기 위해 비트 이동

	int i = 1;
	while(remainder > 4294967296)
	{//표준다항식으로 더이상 나눌 수 없을 때 까지
		if ((remainder >> (63 - i)) == 1)
		{//만약 몫이 1이라면
			remainder = remainder ^ divisor;
			//XOR 연산
			divisor = divisor >> 1;
			//표준다항식 한 계수 감소
		}
		else
		{//몫이 0일때
			divisor = divisor >> 1;
			//표준다항식 한 계수 감소
		}
		i++;
	}
	result = num + remainder;
	//나머지(검사비트)를 dataword에 추가하여
	//codeword를 만듭니다.

	return result;
}

long long decoding(long long num)
{
	long long result = 0;
	long long remainder = num;
	//나머지
	long long divisor = 4374732215;
	//CRC-32 표준다항식
	divisor = divisor << 30;
	//dataword와 자리수를 맞춰주기 위해 비트 이동

	int i = 1;
	while (remainder > 4294967296)
	{//표준다항식으로 더이상 나눌 수 없을 때 까지
		if ((remainder >> (63 - i)) == 1)
		{//만약 몫이 1이라면
			remainder = remainder ^ divisor;
			//XOR 연산
			divisor = divisor >> 1;
			//표준다항식 한 계수 감소
		}
		else
		{//몫이 0일때
			divisor = divisor >> 1;
			//표준다항식 한 계수 감소
		}
		i++;
	}

	if (remainder == 0)
	/*
	나머지가 0이라면 전송받은 데이터에 에러가 없음으로
	Codeword에서 검사를 위해 넣었던 CRC-32오류 검출코드를
	제거하고 원래 전송하고자 했던 dataword를 return합니다.
	*/
		return num >> 32;
	else
	{
	/*
	나머지가 0이 아니라면 전송도중 에러가 발생한 것으로서
	재전송을 요청합니다.
	*/
		printf("오류가 발생하였습니다. 재전송을 요청합니다.\n");
		return 0;
	}
}

void print_binary(long long num)
{//검토를 위해 만든 64bit long long형 변수의 2진수 출력 함수.
	for (int i = 63; i >= 0; --i)
	{
		if ((i + 1) % 8 == 0)
			printf(" ");
		printf("%d", (num >> i) & 1);
	}
	printf("\n");
}