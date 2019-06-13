#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int main(){
	int rsock, wsock;
	struct sockaddr_in addr, client;
	int len, n;

	/* ソケットの作成*/
	rsock = socket(AF_INET, SOCK_STREAM, 0);
	//rsock = socket(3000, 4000, 5000);
	if(rsock < 0){
		perror("socket");
		printf("%d\n", errno);
		return 1;
	}

	/* socket setting */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = INADDR_ANY;

	/* binding socket */
	if(bind(rsock, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		perror("bind");
		printf("%d\n", errno);
		return 1;
	}

	/* TCPクライアントからの接続要求を待てる状態にする = listen  */
	if(listen(rsock, 5) != 0){
		perror("listen");
		printf("%d\n", errno);
		return 1;
	}

	/* 複数回の接続要求を受け付ける */
	while(1){
		/* accept TCP connection from client */
		len = sizeof(client);
		wsock = accept(rsock, (struct sockaddr *)&client, &len);
		if(wsock < 0){
			perror("accept");
			break;
		}



		/* send message */
		//n = write(wsock, "HELLO", 5);
		//n = write(wsock, "HTTP1.1 200 OK", 14);
		n = write(wsock, "HTTP1.1 302 FOUND\r\n", 19);
		n = write(wsock, "Location: https://hikalium.com", 30);
		if(n < 1){
			perror("write");
			break;
		}

		/* close TCP session */
		close(wsock);
	}

	/* close listening socket */
	close(rsock);

	return 0;
}
