#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

void raise_error(char* description){
	perror(description);
	printf("errno: %d\n", errno);
}

int write_msg(int sock, char* msg){
	int len = strlen(msg);
	if(write(sock, msg, len) != len){
		raise_error("write");
		return -1;
	}
	return len;
}

int http(int wsock, char* request){
	char method[16];
	char uri[256];
	char version[64];

	sscanf(request, "%s %s %s", method, uri, version);
	//fprintf(stdout, "%s %s %s\n", method, uri, version);
	/* GET以外のリクエストをはじく */
	if (strcmp(method, "GET") != 0) {
		write_msg(wsock, "HTTP1.1 501 ");
		write_msg(wsock, "Not Implemented");
	}

	/* send message */
	write_msg(wsock, "HTTP1.1 200 OK");
	//write_msg(wsock, "HTTP1.1 302 FOUND\r\n");
	//write_msg(wsock, "Location: https://hikalium.com");
	return 0; //ステータスコード返したら楽しそう
}

int main(){
	int rsock, wsock;
	struct sockaddr_in addr, client;
	unsigned len;
	//int n;
	char buf[1024];

	/* ソケットの作成*/
	rsock = socket(AF_INET, SOCK_STREAM, 0);
	//rsock = socket(3000, 4000, 5000);
	if(rsock < 0){
		//perror("socket");
		//printf("%d\n", errno);
		//return 1;
		raise_error("socket");
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
			raise_error("accept");
			break;
		}

		/* 受信したhttpリクエストを処理する */
		if(read(wsock, buf, 1024) <= 0 ){
			raise_error("reading request");
		}else{
			fprintf(stdout, "%s\n", buf);
			http(wsock, buf);
		}

		/* close TCP session */
		close(wsock);
	}

	/* close listening socket */
	close(rsock);

	return 0;
}
