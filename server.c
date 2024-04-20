#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "login.h"

Package readRequest(int sock, struct sockaddr_in* dest_addr, socklen_t* dest_len){
	Package pkg;
	//memset(dest_addr, 0, sizeof(struct sockaddr_in));
	*dest_len = sizeof(struct sockaddr_in);
	int n = 0;
	while (n == 0) n = recvfrom(sock, &pkg, sizeof(pkg), 0, (struct sockaddr *) dest_addr, dest_len);
	if (strcmp(pkg.operation, LOGIN_OPERATION) && 
	    strcmp(pkg.operation, NEW_USER_OPERATION) && strcmp(pkg.operation, ABORT))
		printf("Invalid request.\n");
	
	printf("REQUEST: %s FROM: %s %d.\n", pkg.operation, inet_ntoa(dest_addr->sin_addr), ntohs(dest_addr->sin_port));
	return pkg;
}

void login(Package rqst, const char* filename, int sock, struct sockaddr_in* dest_addr, socklen_t dest_len){
	FILE* f_data = fopen(filename, "r");

	if (f_data == NULL){
		sendto(sock, FAILURE, strlen(FAILURE) + 1, 0, (struct sockaddr *) dest_addr, dest_len);
		return;
	}
	
	char nameBuff[STR_LENGTH];
	char passBuff[STR_LENGTH];
	bool found = false;

	while (!feof(f_data)){
		if (fscanf(f_data, "%128s %128s\n", nameBuff, passBuff) != 2) continue;
		if (strcmp(rqst.name, nameBuff) == 0 && strcmp(rqst.password, passBuff) == 0) {
			found = true;	
			break;
		}
	}
	
	if (found) {
		sendto(sock, SUCCESS, strlen(SUCCESS) + 1, 0, (struct sockaddr *) dest_addr, dest_len);
		printf("Login success.\n");
	}
	else {
		sendto(sock, FAILURE, strlen(FAILURE) + 1, 0, (struct sockaddr *) dest_addr, dest_len);
		printf("Login failed.\n");
	}

	fclose(f_data);
}

void newAccount(Package rqst, const char* filename, int sock, struct sockaddr_in* dest_addr, socklen_t dest_len){
	FILE* f_data = fopen(filename, "r");
	if (f_data != NULL){
                char nameBuff[STR_LENGTH];
		char passBuff[STR_LENGTH];
		while (!feof(f_data)){
			if (fscanf(f_data, "%128s %128s\n", nameBuff, passBuff) != 2) continue;
			if (strcmp(nameBuff, rqst.name) == 0){
				sendto(sock, FAILURE, strlen(FAILURE) + 1, 0, (struct sockaddr *) dest_addr, dest_len);
				printf("User already exists.\n");
				fclose(f_data);
				return;
			}
		}
        }
	fclose(f_data);

	f_data = fopen(filename, "a");
	if (f_data == NULL){
		sendto(sock, FAILURE, strlen(FAILURE) + 1, 0, (struct sockaddr *) dest_addr, dest_len);
		printf("Error in writing on disk.\n");
		return;
	}
	fprintf(f_data, "%s %s\n", rqst.name, rqst.password);
	sendto(sock, SUCCESS, strlen(SUCCESS) + 1, 0, (struct sockaddr *) dest_addr, dest_len);
	printf("New user operation success.\n");
	fclose(f_data);
}

int main(int argc, char** argv){
	if (argc != 2){
		printf("Uso: server <porta>\n");
		return EXIT_SUCCESS;
	}
	int sockid = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockid < 0){
		printf("Errore nell'apertura del socket.\n");
		return EXIT_FAILURE;
	}
	struct sockaddr_in local_addr, remote_addr;
	local_addr = initAddr("", argv[1]);
	socklen_t local_len, remote_len;
	local_len = remote_len = sizeof(struct sockaddr_in);
	memset(&remote_addr, 0, remote_len);
	if (bind(sockid, (struct sockaddr *) &local_addr, local_len) < 0){
		printf("Errore nel binding della porta. Inserire un numero diverso.\n");
		return EXIT_FAILURE;
	}

	Package request;
	do{
		request = readRequest(sockid, &remote_addr, &remote_len);
		if (strcmp(request.operation, LOGIN_OPERATION) == 0) login(request, DATA_FILE, sockid, &remote_addr, remote_len);
		else if (strcmp(request.operation, NEW_USER_OPERATION) == 0) newAccount(request, DATA_FILE, sockid, &remote_addr, remote_len);
	} while (strcmp(request.operation, ABORT) != 0);
	
	close(sockid);
	return EXIT_SUCCESS; 
}
