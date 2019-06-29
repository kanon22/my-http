#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

void raise_error(char* description){
	perror(description);
	printf("errno: %d\n", errno);
}

/* strlenしてるからバイナリは送れないよ！！ */
int write_msg(int sock, char* msg){
	int len = strlen(msg);
	if(write(sock, msg, len) != len){
		raise_error("write message");
		return -1;
	}
	return len;
}

int write_file(int sock, char* file_path){
	int fd, len;
	char* buf[1024];
	buf[0] = '\0';

	fd = open(file_path, O_RDONLY);
	if(fd < 0){
		raise_error("opening file");
		fprintf(stderr, "cannot open file %s\n", file_path);
	}else{
		/* send file */
		while((len = read(fd, buf, 1024)) > 0){
			write(sock, buf, len);
		}
	}

	close(fd);
	return len;
}

int http(int wsock, char* request){
	char method[16];
	char uri[256];
	char version[64];
	char* file_uri;
	char content_type[64];
	char header[1024];
	int status_code;
	char* extension;

	sscanf(request, "%s %s %s", method, uri, version);
	//fprintf(stderr, "%s %s %s\n", method, uri, version);
	/* GET以外のリクエストをはじく */
	if (strcmp(method, "GET") != 0) {
		write_msg(wsock, "HTTP/1.1 501 Not Implemented\r\n");
		status_code = 501;
	}else{

		/* find file with uri */
		if(strcmp(uri, "/") == 0){
			file_uri = "index.html";
		}else{
			file_uri = uri + 1; // pathの最初の/を取り除く
		}

		/* check file existence */
		if(access(file_uri, F_OK) != 0){
			fprintf(stderr, "%s does not exist\n", file_uri);
			strcpy(header, "HTTP/1.1 404 Not Found\r\n");
			status_code = 404;
		}else if(access(file_uri, R_OK) != 0){
			strcpy(header, "HTTP/1.1 403 Forbidden\r\n");
			status_code = 403;
		}else{
			strcpy(header, "HTTP/1.1 200 OK\r\n");
			status_code = 200;
		}

		/* detect file type */
		extension = strrchr(file_uri, '.') + 1;
		if(strcmp(extension, "html") == 0){
			strcpy(content_type, "Content-Type: text/html\r\n");
		}else if(strcmp(extension, "png") == 0 || strcmp(extension, "ico") == 0){
			strcpy(content_type, "Content-Type: image/png\r\n");
		}else if(strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0){
			strcpy(content_type, "Content-Type: image/jpeg\r\n");
		}else{
			strcpy(content_type, "Content-Type: application/octet-stream\r\n");
		}

		/* send header */
		strcat(header, content_type);
	}

	write_msg(wsock, header);

	/* send body */
	if(status_code == 200){
		write_msg(wsock, "\r\n");
		write_file(wsock, file_uri);
	}

	return 0; //ステータスコード返したら楽しそう
}

int main(){
	int rsock, wsock;
	struct sockaddr_in addr, client;
	unsigned len;
	int buf_len;
	char buf[1024];

	/* ソケットの作成*/
	rsock = socket(AF_INET, SOCK_STREAM, 0);
	//rsock = socket(3000, 4000, 5000);
	if(rsock < 0){
		raise_error("socket");
		return 1;
	}

	/* socket setting */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = INADDR_ANY;

	/* binding socket */
	if(bind(rsock, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		raise_error("bind");
		return 1;
	}

	/* TCPクライアントからの接続要求を待てる状態にする = listen  */
	if(listen(rsock, 5) != 0){
		raise_error("listen");
		return 1;
	}

	/* 複数回の接続要求を受け付ける */
	while(1){
		buf[0] = '\0';
		/* accept TCP connection from client */
		len = sizeof(client);
		wsock = accept(rsock, (struct sockaddr *)&client, &len);
		if(wsock < 0){
			raise_error("accept");
			break;
		}

		/* 受信したhttpリクエストを処理する */
		if((buf_len = read(wsock, buf, 1023)) <= 0 ){
			raise_error("reading request");
		}else{
			buf[buf_len] = '\0';
#ifdef DEBUG
			fprintf(stderr, "%s\n", buf);
#endif
			http(wsock, buf);
		}

		/* close TCP session */
		close(wsock);
	}

	/* close listening socket */
	close(rsock);

	return 0;
}
