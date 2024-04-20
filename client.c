#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "login.h"

int menu(){
	printf("- - - M E N U - - -\n\n1. Effettua login\n2. Registrati\n0. Esci\n\nScelta: ");
	int scelta;
	scanf("%d", &scelta);
	printf("\n");
	return scelta;
}

void inputData(char* name, char* password){
	printf("Inserire nome utente: ");
        scanf("%128s", name);
        printf("Inserire password: ");
        scanf("%128s", password);
}

void newRequest(const char* operation_type, bool need_data, int sock, struct sockaddr_in addr){
	Package request;
	strcpy(request.operation, operation_type);
	if (need_data) inputData(request.name, request.password);
	else {
		strcpy(request.name, "");
		strcpy(request.password, "");
	}
	//printf("Sto inviando il pacchetto: %s%s%s\n", request.operation, request.name, request.password);
	sendto(sock, &request, sizeof(Package), 0, (struct sockaddr *) &addr, sizeof(addr));
}

char* readResponse(int sock, struct sockaddr_in addr){
	char* response = malloc(sizeof(char) * RESPONSE_LENGTH);
	socklen_t len = sizeof(struct sockaddr_in);
	int n = 0;
	while (n == 0) {
		n = recvfrom(sock, response, RESPONSE_LENGTH, 0, (struct sockaddr *) &addr, &len);
		//printf("n = %d\n", n);
	}
	return response;
}

int main(int argc, char** argv){
	if (argc != 3){
		printf("Uso: client <indirizzo ip> <porta>\n");
		return EXIT_SUCCESS;
	}

	int sockid = socket(PF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in dest_addr;
	
	if (sockid < 0){
		printf("Errore nell'apertura del socket.\n");
		return EXIT_FAILURE;
	}
	
	dest_addr = initAddr(argv[1], argv[2]);
	
	int scelta;
	char* response;
	do {
		scelta = menu();
		switch (scelta){
			case 1:
				newRequest(LOGIN_OPERATION, true, sockid, dest_addr);
				response = readResponse(sockid, dest_addr);
				if (strcmp(response, SUCCESS) == 0) printf("Accesso eseguito con successo.\n\n");
				else printf("Impossibile accedere.\n\n");
				break;
			case 2:
				newRequest(NEW_USER_OPERATION, true, sockid, dest_addr);
				response = readResponse(sockid, dest_addr);
				if (strcmp(response, SUCCESS) == 0) printf("Registrazione avvenuta con successo.\n\n");
				else printf("Impossibile registrare il nuovo utente.\n\n");
				break;
			case 0:
				printf("Chiusura server in corso.\n\n");
				newRequest(ABORT, false, sockid, dest_addr);
				break;
			default:
				printf("Inserire un valore valido.\n\n");
				break;
		}
	} while (scelta != 0);
	
	close(sockid);
	return EXIT_SUCCESS;
}
