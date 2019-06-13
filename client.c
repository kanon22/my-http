#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(){
	struct sockaddr_in serv;
	int sock;
	char buf[32];
	int n;

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	//rsock = socket(3000, 4000, 5000);
	if(sock < 0){
		perror("client socket");
		printf("%d\n", errno);
		return 1;
	}

	/* access setting */
	serv.sin_family = AF_INET;
	serv.sin_port = htons(12345);
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* connect to server */
	connect(sock, (struct sockaddr *)&serv, sizeof(serv));

	/* send data to server */
	memset(buf, 0, sizeof(buf));
	n = read(sock, buf, sizeof(buf));

	printf("%d, %s\n", n, buf);

	/* close socket */
	close(sock);

	return 0;
}
