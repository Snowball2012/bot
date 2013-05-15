#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include "buffer.h"

class Client
{
	int sockfd;
	Buffer str; //last accepted string
	Buffer tosend;
public:
	void GetStrFromServ();
	Client(int port, const char * addr);
	~Client();
	int GetInt();
	Buffer GetWord();
	Buffer GetString();
	void Send();
	void AddString(const char * word);
	void AddInt(int arg);
};

class GameClient:public Client
{
	int turn;
	bool WaitForWord(const char * word);
	void Market();
	char name[128];
	struct MarketState {
		int supply;
		int raw_price;
		int demand;
		int prod_price;
		bool act;
		void Clear() {
			supply = 0;
			raw_price = 0;
			demand = 0;
			prod_price = 0;
			act = false;
		}
	} market;
public:
	GameClient(int port, const char * addr, const char * name, int game);
	void Endturn();
	void Buy(int count, int value);
	void Sell(int count, int value);
	void Prod(int count);
	void Build(int count);
	//functions
	int MyId();
	int Players();
	int ActPlayers();
	int Turn();
	int Supply();
	int RawPrice();
	int Demand();
	int ProdPrice();
	int Money(int id);
	int Raw(int id);
	int Production(int id);
	int Facts(int id);
	int AutoFacts(int id);
	int Manuf(int id);
	int RawSold(int id);
	int RawPrice(int id);
	int ProdBought(int id);
	int ProdPrice(int id);
};


class ClientEx
{
	const char * msg;
public:
	ClientEx(const char * amsg): msg(amsg) {}
	void Print() const 
	{
		printf("Client error: %s\n", msg);
	}
};

#endif
