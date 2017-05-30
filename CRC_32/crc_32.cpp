#pragma warning (disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 10000					
#define BUFF_SIZE 100

void start(void);
void server(); //�۽����� ���� ����
void client(); //�������� Ŭ���̾�Ʈ ����
void send_messge(); //�۽����� ó��
void recv_message();//�������� ó��
long long encoding(long long num);
//�� ��Ͽ� ���� CRC-32 �������� �ڵ尡 �߰��� ���� ������ ����
long long decoding(long long num);
//�� ��Ͽ� ���� CRC-32 �����˻� ����
void print_binary(long long num);
//64byte ������ 2������ ����ϴ� �Լ�
//���並 ���� ��������ϴ�.

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
	//�۽��� �� ������ ������ �� ������ ��Ȱ�� ���մϴ�.
	if (is_server == 1)
		send_messge();
	//�۽��� ó�� �Դϴ�.
	else
		recv_message();
	//������ ó�� �Դϴ�.
	return 0;
}

void start(void) {
	int input;
	//system("cls");
	printf("                 Welcome to CRC32  \n\n\n");
	printf("                                         made in �Ǵ뿵\n\n");
	printf("-------------------------------------------------------\n\n");

	printf("���ϴ� ������ ��ȣ�� �Է��� �ּ���! \n\n");
	printf("(	1.���� ����		2.Ŭ���̾�Ʈ ����	)\n\n�Է� : ");
	scanf("%d", &input);
	//�۽��� �� ������ ������ �� ������ ��Ȱ�� ���մϴ�.

	if (input == 1) {
		server();
	}
	else if (input == 2) {
		client();
	}
	else
		printf("�ùٸ� �Է��� �ƴմϴ�.");
	return;
}

void server()
{//������ ����� �ϱ� ���� �۽���(����) ���� �����Դϴ�.
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		printf("\n   ������ ������ �ʱ�ȭ �ϴµ� ���� �߽��ϴ�!!\n");
		return;
	}
	else
		printf("\n   ������ ������ �ʱ�ȭ �߽��ϴ�.\n");

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) {
		printf("   ���� ������ �����ϴµ� ���� �߽��ϴ�.\n");
		return;
	}
	else
		printf("   ���� ������ �ʱ�ȭ �߽��ϴ�.\n");

	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	bind(server_sock, (SOCKADDR *)&server_addr, sizeof(server_addr));

	listen(server_sock, 5);

	printf("   Ŭ���̾�Ʈ�� ������ ��ٸ��ϴ�......\n");
	client_sock = accept(server_sock, NULL, NULL);
	printf("   Ŭ���̾�Ʈ�� ���� �߽��ϴ�.\n");

	is_server = 1;
	printf("   ");
	system("pause");

	return;
}

void client()
{//������ ����� �ϱ� ���� ������(Ŭ���̾�Ʈ) ���� �����Դϴ�.
	char server_ip[30];

	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		printf("   ������ ������ �ʱ�ȭ �ϴµ� ���� �߽��ϴ�!!\n");
		return;
	}
	else
		printf("   ������ ������ �ʱ�ȭ �߽��ϴ�.\n");

	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) {
		printf("   Ŭ���̾�Ʈ ������ �����ϴµ� ���� �߽��ϴ�.\n");
		return;
	}
	else
		printf("   Ŭ���̾�Ʈ ������ �ʱ�ȭ �߽��ϴ�.\n");

	ZeroMemory(&server_addr, sizeof(server_addr));

	printf("   �����ּҸ� �Է��ϼ��� (ex.127.0.0.1): ");
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
{//�۽��� ó��
		int input;
		printf("\n������ ���ڸ� �Է��� �ּ���(8��) : ");
		scanf("%c", &str[0]);
		for (int i = 0; i < 8; i++)
			scanf("%c", &str[i]);
		//����ڷ� ���� 8���� ���ڸ� �Է¹޽��ϴ�.

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
		/*8���� ���ڸ� 4���ھ� 2���� ������� ����ϴ�.
		2�� ����� ���� ���ڵ��� 8byteũ���� long long�� ������ �ֽ��ϴ�.
		dataword1 <= ABCD
		dataword2 <= EFGH
		*/

		codeword1 = encoding(dataword1);
		codeword2 = encoding(dataword2);
		//�� ��Ͽ� ���� CRC-32���� ���� �ڵ尡 �߰��� ������������ �����մϴ�.
		
		printf("\n�ش� �޼��� : ");
		for (int i = 0; i < 8; i++)
			printf("%c", str[i]);
		printf("�� ��� �����ðڽ��ϱ�?\n");
		printf("(	1.��������		2.��������	)\n�Է�:");
		scanf("%d", &input);
		//�������� ���� ���� ���� Ȯ��

		printf("\n������ CRC-32 �������� �ڵ尡 �߰��� ���� �������� : \n");
		long long discode = 0;
		if (input == 1)
		{//�������� �������� ����
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
			CRC�ڵ尡 �߰��� 64bit ������ ������ ����
			8bit �� char�� �迭�� ���� ��� �����Դϴ�.
			*/
			printf("	�Դϴ�.\n\n\n");
			send(client_sock, msg, BUFF_SIZE, 0);
			//�����ڿ��� ����!
		}
		else if (input == 2)
		{//�������� ���� ���� �߻� ���
			for (int i = 0; i < 8; i++)
			{
				msg[i] = (codeword1 >> (56 - (i * 8))) + 1;
				//�������� ���ۿ��� �߻� 1bit�� �߰�
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
			CRC�ڵ尡 �߰��� 64bit ������ ������ ����
			8bit �� char�� �迭�� ���� ��� �����Դϴ�.
			*/
			printf("	�Դϴ�.\n");
			printf("�������� ���ۿ����� �߻����׽��ϴ�.\n\n");
			send(client_sock, msg, BUFF_SIZE, 0);
			//�����ڿ��� ����!
		}
		else
			printf("�ùٸ� �Է��� �ƴմϴ�.\n");
		closesocket(server_sock);
		closesocket(client_sock);
}

void recv_message()
{//������ ó��
		printf("�޼����� ��ٸ��� ���Դϴ�.\n");
		recv(client_sock, msg, BUFF_SIZE, 0);
		//���������� �޼����� �����մϴ�.
		
		printf("\n���� ���� ������ : ");
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
		������ ���� char(8bit)������ ������ ������ ��ϵ���
		�ٽ� 2�� ������ ���� 64bit ¥�� long long�� ������
		�̾� �ٿ� ����ϴ�.
		*/

		print_binary(codeword1);
		print_binary(codeword2);
		//���۹��� ����� 2������ ���(����)

		dataword1 = decoding(codeword1);
		dataword2 = decoding(codeword2);
		//������ ��Ͽ� ���� CRC-32���� �˻縦 �մϴ�.

		if (dataword1 != 0 && dataword2 != 0)
		{
			printf("�������� �׽�Ʈ ��� ���� ����!!\n\n");
			printf("���޹��� �����ʹ� : ");
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
			printf("�Դϴ�.\n\n");
		}
		closesocket(client_sock);
}

long long encoding(long long num)
{
	long long result = 0;
	long long remainder = num;
	//������
	long long divisor = 4374732215;
	//CRC-32 ǥ�ش��׽�
	divisor = divisor << 30;
	//dataword�� �ڸ����� �����ֱ� ���� ��Ʈ �̵�

	int i = 1;
	while(remainder > 4294967296)
	{//ǥ�ش��׽����� ���̻� ���� �� ���� �� ����
		if ((remainder >> (63 - i)) == 1)
		{//���� ���� 1�̶��
			remainder = remainder ^ divisor;
			//XOR ����
			divisor = divisor >> 1;
			//ǥ�ش��׽� �� ��� ����
		}
		else
		{//���� 0�϶�
			divisor = divisor >> 1;
			//ǥ�ش��׽� �� ��� ����
		}
		i++;
	}
	result = num + remainder;
	//������(�˻��Ʈ)�� dataword�� �߰��Ͽ�
	//codeword�� ����ϴ�.

	return result;
}

long long decoding(long long num)
{
	long long result = 0;
	long long remainder = num;
	//������
	long long divisor = 4374732215;
	//CRC-32 ǥ�ش��׽�
	divisor = divisor << 30;
	//dataword�� �ڸ����� �����ֱ� ���� ��Ʈ �̵�

	int i = 1;
	while (remainder > 4294967296)
	{//ǥ�ش��׽����� ���̻� ���� �� ���� �� ����
		if ((remainder >> (63 - i)) == 1)
		{//���� ���� 1�̶��
			remainder = remainder ^ divisor;
			//XOR ����
			divisor = divisor >> 1;
			//ǥ�ش��׽� �� ��� ����
		}
		else
		{//���� 0�϶�
			divisor = divisor >> 1;
			//ǥ�ش��׽� �� ��� ����
		}
		i++;
	}

	if (remainder == 0)
	/*
	�������� 0�̶�� ���۹��� �����Ϳ� ������ ��������
	Codeword���� �˻縦 ���� �־��� CRC-32���� �����ڵ带
	�����ϰ� ���� �����ϰ��� �ߴ� dataword�� return�մϴ�.
	*/
		return num >> 32;
	else
	{
	/*
	�������� 0�� �ƴ϶�� ���۵��� ������ �߻��� �����μ�
	�������� ��û�մϴ�.
	*/
		printf("������ �߻��Ͽ����ϴ�. �������� ��û�մϴ�.\n");
		return 0;
	}
}

void print_binary(long long num)
{//���並 ���� ���� 64bit long long�� ������ 2���� ��� �Լ�.
	for (int i = 63; i >= 0; --i)
	{
		if ((i + 1) % 8 == 0)
			printf(" ");
		printf("%d", (num >> i) & 1);
	}
	printf("\n");
}