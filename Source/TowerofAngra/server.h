// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
// *******************************************************
#define WIN32_LEAN_AND_MEAN
// *******************************************************
#include "TowerofAngra.h"
#pragma comment(lib, "ws2_32.lib")
#include<WinSock2.h>
#include<iostream>
#include<map>
#include"Runtime/Core/Public/HAL/Runnable.h"

class ACharacterPlayerController;

using namespace std;

//Network Define
#define SOCKET_BUF_SIZE			4096
#define SERVERPROT				9000
#define SERVERIP				"127.0.0.1"

// 소켓 통신 구조체
struct SOCKETIONFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[SOCKET_BUF_SIZE];
	int				recvBytes;
	int				sendBytes;
};

// 주고받을 패킷 타입.
enum PacketType
{
	ENROLL_CHARACTER,
	SEND_CHARACTER,
	RECV_CHARACTER,
	ENTER_NEW_PLAYER,
	LOGOUT_CHARACTER,
	ENTER_NEW_MONSTER,
	SPAWN_MONSTER
};

class cPlayer
{
public:
	cPlayer() {};
	~cPlayer() {};

private:


public:
	//아이디
	int ClientID = 0;

	// PlayerType
	int clientPlayerType = 0;

	// 위치
	float X;
	float Y;
	float Z;
	// 회전값
	float Yaw;
	float Pitch;
	float Roll;

	// 속도
	float VX;
	float VY;
	float VZ;

	bool IsSkilling;
	bool IsAttacking;

	//패킷 직렬화 역직렬화
	friend ostream& operator<<(ostream &stream, cPlayer& info)
	{
		stream << info.ClientID << endl;
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;
		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;
		stream << info.IsSkilling << endl;
		stream << info.IsAttacking << endl;
		stream << info.clientPlayerType << endl;
		return stream;
	}

	friend istream& operator>>(istream& stream, cPlayer& info)
	{
		stream >> info.ClientID;
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;
		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;
		stream >> info.IsSkilling;
		stream >> info.IsAttacking;
		stream >> info.clientPlayerType;
		return stream;
	}
};


class cPlayerInfo
{
public:
	cPlayerInfo() {};
	~cPlayerInfo() {};

	map<int, cPlayer> players;

	friend ostream& operator<<(ostream &stream, cPlayerInfo& info)
	{
		stream << info.players.size() << endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream &operator>>(istream &stream, cPlayerInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		cPlayer Player;
		info.players.clear();

		stream >> nPlayers;
		for (int i = 0; i < nPlayers; i++)
		{
			stream >> SessionId;
			stream >> Player;
			info.players[SessionId] = Player;
		}

		return stream;
	}
};

class Monster
{
public:
	float X;
	float Y;
	float Z;

	int MonsterID;
	bool IsAttacking;
	bool IsAliving;
	float HP;

	friend ostream& operator<<(ostream &stream, Monster& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.MonsterID << endl;
		stream << info.IsAttacking << endl;
		stream << info.IsAliving << endl;
		stream << info.HP << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, Monster& info)
	{
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.MonsterID;
		stream >> info.IsAttacking;
		stream >> info.IsAliving;
		stream >> info.HP;

		return stream;
	}
};

class MonsterSet
{
public:
	map<int, Monster> monsters;

	friend ostream& operator<<(ostream &stream, MonsterSet& info)
	{
		stream << info.monsters.size() << endl;
		for (auto& kvp : info.monsters)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, MonsterSet& info)
	{
		int nMonsters = 0;
		int PrimaryId = 0;
		Monster monster;
		info.monsters.clear();

		stream >> nMonsters;
		for (int i = 0; i < nMonsters; i++)
		{
			stream >> PrimaryId;
			stream >> monster;
			info.monsters[PrimaryId] = monster;
		}

		return stream;
	}
};


class TOWEROFANGRA_API server : public FRunnable
{
private:
	SOCKET	Sock;						// 서버와 연결할 소켓	
	cPlayerInfo playerinfo;				// 케릭터 정보
	char 	recvBuffer[SOCKET_BUF_SIZE];		// 수신 버퍼 스트림
	cPlayer * RecvNewPlayer(stringstream &);
	cPlayer NewPlayer;
	ACharacterPlayerController* Controller;	// 플레이어 컨트롤러 정보
	MonsterSet	MonsterSetInfo;
	Monster		MonsterInfo;
public:
	server();
	virtual ~server();

public:
	int nTotalSockets = 0;
	SOCKETIONFO* SocketInfoArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];


	//소켓 초기화
	bool InitSock();
	bool Connect(const char*, int);

	void EnrollCharacterInfo(cPlayer&);
	void SendCharacterInfo(cPlayer&);
	void LogoutCharacter(int);
	void SendPlayer(cPlayer& info);
	MonsterSet* RecvMonsterSet(stringstream&);
	Monster* RecvMonster(stringstream&);
	// 스레드 시작 및 종료
	bool StartListen();
	void StopListen();

	void CloseSocket();
	cPlayerInfo* RecvCharacterInfo(stringstream&);

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	//FRunnable override 함수
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();
	//======================

	// 플레이어 컨트롤러 세팅
	void SetPlayerController(ACharacterPlayerController*);

	static server* GetSingleton()
	{
		static server ins;
		return &ins;
	}

};