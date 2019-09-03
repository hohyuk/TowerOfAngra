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

// ���� ��� ����ü
struct SOCKETIONFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[SOCKET_BUF_SIZE];
	int				recvBytes;
	int				sendBytes;
};

// �ְ���� ��Ŷ Ÿ��.
enum PacketType
{
	ENROLL_CHARACTER,
	SEND_CHARACTER,
	RECV_CHARACTER,
	ENTER_NEW_PLAYER,
	LOGOUT_CHARACTER,
	SPAWN_MONSTER,
	HIT_PLAYER,
	HIT_MONSTER,
	SYNCRO_MONSTER,
	DESTROY_MONSTER,
	NEXT_LEVEL_STAGE_SYNCRO_MONSTER,
	NEXT_LEVEL_STAGE_SPAWN_MONSTER
};

class cPlayer
{
public:
	cPlayer() {};
	~cPlayer() {};

private:


public:
	//���̵�
	int ClientID = 0;

	// PlayerType
	int clientPlayerType = 0;
	FVector Location;		// ��ġ
	FRotator Rotation;		// ȸ��
	FVector Velocity;		// �ӵ�

	float HP;
	bool IsSkilling;
	bool IsAttacking;
	bool IsAliving;
	// SkillTYPE
	int SkillType = 0; // 0 - None 1 - CommonSkill 2 - Skill



	//2019-08-30 �÷��̾�Ÿ���̶� ���̺� �ּ�
	int StageLevel;
	//��Ŷ ����ȭ ������ȭ
	friend ostream& operator<<(ostream &stream, cPlayer& info)
	{
		stream << info.ClientID << endl;

		stream << info.Location.X << endl;
		stream << info.Location.Y << endl;
		stream << info.Location.Z << endl;

		stream << info.Velocity.X << endl;
		stream << info.Velocity.Y << endl;
		stream << info.Velocity.Z << endl;

		stream << info.Rotation.Yaw << endl;
		stream << info.Rotation.Pitch << endl;
		stream << info.Rotation.Roll << endl;

		stream << info.HP << endl;

		stream << info.SkillType << endl;
		stream << info.StageLevel << endl;

		stream << info.IsSkilling << endl;
		stream << info.IsAttacking << endl;
		stream << info.clientPlayerType << endl;
		//stream << info.IsAliving << endl;
		return stream;
	}

	friend istream& operator>>(istream& stream, cPlayer& info)
	{
		stream >> info.ClientID;

		stream >> info.Location.X;
		stream >> info.Location.Y;
		stream >> info.Location.Z;

		stream >> info.Velocity.X;
		stream >> info.Velocity.Y;
		stream >> info.Velocity.Z;

		stream >> info.Rotation.Yaw;
		stream >> info.Rotation.Pitch;
		stream >> info.Rotation.Roll;

		stream >> info.HP;

		stream >> info.SkillType;
		stream >> info.StageLevel;

		stream >> info.IsSkilling;
		stream >> info.IsAttacking;
		stream >> info.clientPlayerType;
		//stream >> info.IsAliving;
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

	// MonsterTYPE
	int MonsterType = 0;



	friend ostream& operator<<(ostream &stream, Monster& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.MonsterID << endl;
		stream << info.IsAttacking << endl;
		stream << info.IsAliving << endl;
		stream << info.HP << endl;
		stream << info.MonsterType << endl;

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
		stream >> info.MonsterType;

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
//=======================================================================
class NextStageMonster
{
public:
	float X;
	float Y;
	float Z;
	float Movement;

	int MonsterID;
	bool IsAttacking;
	bool IsAliving;
	float HP;
	float AttackRange;
	float ChaseRange;
	bool Chasing;
	float AttackPower;


	// MonsterTYPE
	int MonsterType = 0;



	friend ostream& operator<<(ostream &stream, NextStageMonster& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.MonsterID << endl;
		stream << info.IsAttacking << endl;
		stream << info.IsAliving << endl;
		stream << info.HP << endl;
		stream << info.MonsterType << endl;
		return stream;
	}

	friend istream& operator>>(istream& stream, NextStageMonster& info)
	{
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.MonsterID;
		stream >> info.IsAttacking;
		stream >> info.IsAliving;
		stream >> info.HP;
		stream >> info.MonsterType;
		return stream;
	}
};
class NextStageMonsterSet
{
public:
	map<int, NextStageMonster> monsters;

	friend ostream& operator<<(ostream &stream, NextStageMonsterSet& info)
	{
		stream << info.monsters.size() << endl;
		for (auto& kvp : info.monsters)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, NextStageMonsterSet& info)
	{
		int nMonsters = 0;
		int PrimaryId = 0;
		NextStageMonster monster;
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
	SOCKET	Sock;						// ������ ������ ����	
	cPlayerInfo playerinfo;				// �ɸ��� ����
	char 	recvBuffer[SOCKET_BUF_SIZE];		// ���� ���� ��Ʈ��
	cPlayer * RecvNewPlayer(stringstream &);
	cPlayer NewPlayer;
	ACharacterPlayerController* Controller;	// �÷��̾� ��Ʈ�ѷ� ����
	MonsterSet	MonsterSetInfo;
	Monster		MonsterInfo;


	//========================================================================���� ��������
	NextStageMonsterSet		NextStageMonsterSetInfo;
	NextStageMonster		NextStageMonsterInfo;
	//========================================================================���� ��������

public:
	server();
	virtual ~server();

public:
	int nTotalSockets = 0;
	SOCKETIONFO* SocketInfoArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];


	//���� �ʱ�ȭ
	bool InitSock();
	bool Connect(const char*, int);

	void EnrollCharacterInfo(cPlayer&);
	void LogoutCharacter(int);
	void SendPlayer(cPlayer& info);
	MonsterSet* RecvMonsterSet(stringstream&);
	Monster* RecvMonster(stringstream&);

	//========================================================================���� ��������
	NextStageMonsterSet* NextStageRecvMonsterSet(stringstream&);
	NextStageMonster* NextStageRecvMonster(stringstream&);
	//========================================================================���� ��������

	// ������ ���� �� ����
	bool StartListen();
	void StopListen();

	void CloseSocket();
	cPlayerInfo* RecvCharacterInfo(stringstream&);

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	//FRunnable override �Լ�
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();
	//======================

	// �÷��̾� ��Ʈ�ѷ� ����
	void SetPlayerController(ACharacterPlayerController*);

	//-----------------------------------------------08-03
	void HitMonster(const int&, const float&);
	void HitPlayer(const int&);
	//-----------------------------------------------
	static server* GetSingleton()
	{
		static server ins;
		return &ins;
	}

};