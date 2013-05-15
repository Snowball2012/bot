#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "client.h"


/*
	Common Client Realization
*/
Client::Client(int port, const char * addr)
{
	printf("test");
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	if(!inet_aton(addr, &(address.sin_addr))) 
		throw ClientEx("invalid IP address");
	if(0 != connect(sockfd, (struct sockaddr *)&address, sizeof(address)))
		throw ClientEx("can't connect to server");
	printf("test");
}

Client::~Client()
{
	shutdown(sockfd, 2);
	close(sockfd);
}

Buffer Client::GetString()
{
	if(strlen(str.GetBuff()) == 0)
		GetStrFromServ();
	Buffer res = Buffer(str);
	str.Clear();
	return res;
}

void Client::GetStrFromServ()
{
	str.Clear();
	int size_read;
	char c;
	do {
		size_read = read(sockfd, &c, 1);
	} while (c!='&' && size_read==1);
	if(size_read == 0)
		throw ClientEx("connection closed");
	if(size_read == -1)
		throw ClientEx("failed to read new line");
	size_read = read(sockfd, &c, 1);
	while (c!='\n' && size_read==1) {
		str.AddChar(c);
		size_read = read(sockfd, &c, 1);
	}
	if(size_read == 0)
		throw ClientEx("connection closed");
	if(size_read == -1)
		throw ClientEx("failed to read new line");
	str.AddChar(c);
}

void Client::Send() 
{
	tosend.AddChar('\n');
	const char * send_str = tosend.GetBuff();
	size_t size_send = write(sockfd, send_str, strlen(send_str)+1);
	while(size_send < strlen(send_str)+1) {
		send_str += size_send;
		size_send = write(sockfd, send_str, strlen(send_str)+1);
	}
	tosend.Clear();
}

void Client::AddString(const char * word)
{
	if(*(tosend.GetBuff())!='\0')
		tosend.AddChar(' ');
	tosend.AddLine(word);
}

void Client::AddInt(int arg)
{
	tosend.AddChar(' ');
	char num[20];
	bool neg = arg < 0;
	num[18] = '0';
	num[19] = '\0';
	int i = 18;
	if(neg) {
		arg = - arg;
	}
	for(;abs(arg)>0;arg/=10, --i) {
		num[i] = '0' + arg%10;
	}
	if(neg) {
		num[i] = '-';
		i--;
	}
	tosend.AddLine(num + i);
}


int Client::GetInt()
{
	Buffer word = GetWord();
	return atoi(word.GetBuff());
}

Buffer Client::GetWord()
{
	const char * c;
	const char * string = str.GetBuff();
	c = string;
	while (!isalnum(*c)&&*c!='\0') {
		c++;
	}
	if (c == '\0')
		throw ClientEx("no word in string remains");
	Buffer word;
	do {
		word.AddChar(*c);
		c++;
	} while(isalnum(*c));
	Buffer remain;
	remain.AddLine(c);
	str = remain;
	return word;
}


GameClient::GameClient(int port, const char * addr, const char * aname, int game)
	:Client(port, addr)
{
		printf("%s",aname);
	printf("test");
	AddString(aname);
	printf("%s",aname);

	Send();
	AddString(".join");
	AddInt(game);
	printf("%s",aname);
	Send();
	printf("%s",aname);
	GetStrFromServ();
	Buffer start_word = GetWord();
	while(strcmp(start_word.GetBuff(), "START")) {
		GetStrFromServ();
		start_word = GetWord();
	}
	turn = 1;
	strcpy(name, aname);
	market.Clear();
	printf("connected\n");
}


void GameClient::Endturn()
{
	AddString("turn");
	Send();
	GetStrFromServ();
	Buffer new_turn = GetWord();
	while(strcmp(new_turn.GetBuff(), "ENDTURN")) {
		GetStrFromServ();
		new_turn = GetWord();
	}
	
}


void GameClient::Buy(int count, int val)
{
	AddString("buy");
	AddInt(count);
	AddInt(val);
	Send();
	if(WaitForWord("OK")) {
		throw ClientEx("failed to buy");
	}
}

bool GameClient::WaitForWord(const char * word)
{
	GetStrFromServ();
	Buffer c = GetWord();
	return strcmp(c.GetBuff(), word);
}

void GameClient::Sell(int count, int val)
{
	AddString("sell");
	AddInt(count);
	AddInt(val);
	Send();
	if(WaitForWord("OK")) {
		throw ClientEx("failed to sell");
	}
}


void GameClient::Prod(int count)
{
	AddString("buy");
	AddInt(count);
	Send();
	if(WaitForWord("OK")) {
		throw ClientEx("failed to produce");
	}
}

void GameClient::Build(int count)
{
	while(count >0) {
		AddString("build");
		Send();
		if(WaitForWord("OK")) {
			throw ClientEx("failed to build");
		}
		count--;
	}
}

int GameClient::MyId()
{
	AddString("info");
	Send();
	int i = 0;
	bool found = false;
	while (WaitForWord("PLAYERS")) {
		Buffer name_str = GetWord();
		if(!found) {
			if(!strcmp(name_str.GetBuff(), name))
				found = true;
			else
				i++;
		}
	}
	if(!found)
		throw ClientEx("name not found");
	return i;
}


int GameClient::Players()
{
	int plc;
	AddString("info");
	while(WaitForWord("PLAYERS")) {}
	plc = GetInt();
	return plc;
}


int GameClient::ActPlayers()
{
	return Players();
}


int GameClient::Turn()
{
	return turn;
}


int GameClient::Supply()
{
	if(!market.act)
		Market();
	return market.supply;
}

void GameClient::Market()
{
	market.act = true;
	AddString("market");
	Send();
	if(WaitForWord("MARKET"))
		throw ClientEx("market");
	market.supply = GetInt();
	market.raw_price = GetInt();
	market.demand = GetInt();
	market.prod_price = GetInt();
}

int GameClient::RawPrice()
{
	if(!market.act)
		Market();
	return market.raw_price;
}


int GameClient::Demand()
{
	if(!market.act)
		Market();
	return market.demand;
}


int GameClient::ProdPrice()
{
	if(!market.act)
		Market();
	return market.prod_price;
}


int GameClient::Money(int id)
{
	int plc = Players();
	if(id>=plc)
		throw ClientEx("id is greater then player count");
	AddString("info");
	for (int i = 0; i<id-1; i++) {
		if(WaitForWord("INFO"))
			throw ClientEx("info");
	}
	if(WaitForWord("INFO"))	
		throw ClientEx("info");
	GetWord();
	GetInt();
	GetInt();
	int money = GetInt();
	while(WaitForWord("PLAYERS")) {}
	return money;
}



int GameClient::Raw(int id)
{
	int plc = Players();
	if(id>=plc)
		throw ClientEx("id is greater then player count");
	AddString("info");
	for (int i = 0; i<id-1; i++) {
		if(WaitForWord("INFO"))
			throw ClientEx("info");
	}
	if(WaitForWord("INFO"))	
		throw ClientEx("info");
	GetWord();
	int raw = GetInt();
	while(WaitForWord("PLAYERS")) {}
	return raw;
}


int GameClient::Production(int id)
{
	int plc = Players();
	if(id>=plc)
		throw ClientEx("id is greater then player count");
	AddString("info");
	for (int i = 0; i<id-1; i++) {
		if(WaitForWord("INFO"))
			throw ClientEx("info");
	}
	if(WaitForWord("INFO"))	
		throw ClientEx("info");
	GetWord();
	GetInt();
	int raw = GetInt();
	while(WaitForWord("PLAYERS")) {}
	return raw;
}


int GameClient::Facts(int id)
{
	int plc = Players();
	if(id>=plc)
		throw ClientEx("id is greater then player count");
	AddString("info");
	for (int i = 0; i<id-1; i++) {
		if(WaitForWord("INFO"))
			throw ClientEx("info");
	}
	if(WaitForWord("INFO"))	
		throw ClientEx("info");
	GetWord();
	GetInt();
	GetInt();
	GetInt();
	int raw = GetInt();
	while(WaitForWord("PLAYERS")) {}
	return raw;
}


int GameClient::AutoFacts(int id)
{
	int plc = Players();
	if(id>=plc)
		throw ClientEx("id is greater then player count");
	AddString("info");
	for (int i = 0; i<id-1; i++) {
		if(WaitForWord("INFO"))
			throw ClientEx("info");
	}
	if(WaitForWord("INFO"))	
		throw ClientEx("info");
	GetWord();
	GetInt();
	GetInt();
	GetInt();
	GetInt();
	int raw = GetInt();
	while(WaitForWord("PLAYERS")) {}
	return raw;
}


int GameClient::Manuf(int id)
{
	return 0;
}


int GameClient::RawSold(int id)
{
	return 0;
}


int GameClient::RawPrice(int id)
{
	return 0;
}


int GameClient::ProdBought(int id)
{
	return 0;
}


int GameClient::ProdPrice(int id)
{
	return 0;
}








