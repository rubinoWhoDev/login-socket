#ifndef LOGIN_H
#include <string.h>
#define LOGIN_H
#define OPERATION_LENGTH 16
#define STR_LENGTH 128
#define DATA_FILE "data.txt"
#define LOGIN_OPERATION "login"
#define NEW_USER_OPERATION "register"
#define SUCCESS "success"
#define FAILURE "failure"
#define RESPONSE_LENGTH 7
#define ABORT "abort"

typedef struct {
	char operation[OPERATION_LENGTH];
	char name[STR_LENGTH];
	char password[STR_LENGTH];
} Package;

struct sockaddr_in initAddr(char* addr, char* port) {
	struct sockaddr_in res;
	memset(&res, 0, sizeof(struct sockaddr_in));
	res.sin_family = AF_INET;
	if (strlen(addr) > 0) inet_pton(AF_INET, addr, &(res.sin_addr));
	res.sin_port = htons(atoi(port));
	return res;
}

#endif
