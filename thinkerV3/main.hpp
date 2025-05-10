#pragma once
#include <WinSock2.h>
#include "messageParser.hpp"
#include "TFHandler.hpp"

#define	VERSION		309
#define	TEXTINFO	"Othello Thinker V3.09(Alpha Go Based)"

void HandleInformationRequest(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
void HandleThinkRequest(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
void HandleGameFinished(MessageParser messageParser, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
int SendThinkAccept(int id, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
int SendThinkResponse(int id, unsigned char x, unsigned char y, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);
int SendThinkReject(int id, SOCKET sock, struct sockaddr_in from, int sockaddr_in_size);