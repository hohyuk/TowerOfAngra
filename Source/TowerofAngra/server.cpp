// Fill out your copyright notice in the Description page of Project Settings.
#pragma warning(disable : 4996)

#include "server.h"
#include <sstream>
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/Async/Async.h"

#include "RunnableThread.h"
#include <algorithm>
#include <string>
#include "CharacterPlayerController.h"
#include "Async.h"

server::server() :StopTaskCounter(0)
{
}


server::~server()
{
	delete Thread;
	Thread = nullptr;

	closesocket(Sock);
	WSACleanup();
}

bool server::InitSock()
{
	WSADATA wsaData;
	// ���� ������ 2.2�� �ʱ�ȭ
	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (retval != 0)
	{
		return false;
	}

	// TCP ���� ����	
	Sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (Sock == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}
bool server::Connect(const char* pszIP, int nPort)
{
	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	stServerAddr.sin_addr.s_addr = inet_addr(pszIP);

	int nRet = connect(Sock, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}
void server::EnrollCharacterInfo(cPlayer& info)
{
	stringstream SendStream;

	SendStream << PacketType::ENROLL_CHARACTER << endl;;
	SendStream << info;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

// ĳ���� ����ȭ
void server::SendPlayer(cPlayer& info)
{
	// ĳ���� ���� ����ȭ

	stringstream SendStream;
	// ��û ����
	SendStream << PacketType::SEND_CHARACTER << endl;
	SendStream << info;

	// ĳ���� ���� ����
	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

// ĳ���� �α׾ƿ�
void server::LogoutCharacter(int SessionId)
{
	stringstream SendStream;
	SendStream << PacketType::LOGOUT_CHARACTER << endl;
	SendStream << SessionId << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);

	if (nSendLen == -1)
	{
		return;
	}

	closesocket(Sock);
	WSACleanup();
}
// ������ ���� �� ����
bool server::StartListen()
{
	// ������ ����
	if (Thread != nullptr)
	{
		cout << "Thread != nullptr" << endl;
		return false;
	}

	Thread = FRunnableThread::Create(this, TEXT("serversocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}
void server::CloseSocket()
{
	closesocket(Sock);
	WSACleanup();
}
void server::StopListen()
{
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}

cPlayerInfo* server::RecvCharacterInfo(stringstream& RecvStream)
{
	RecvStream >> playerinfo;
	return &playerinfo;
}

//FRunnable override �Լ�
bool server::Init()
{
	return true;

}

uint32 server::Run()
{
	FPlatformProcess::Sleep(0.03);
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;
		int PacketType;
		int nRecvLen = recv(Sock, (CHAR*)&recvBuffer, SOCKET_BUF_SIZE, 0);
		if (nRecvLen > 0)
		{
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			switch (PacketType)
			{
			case PacketType::RECV_CHARACTER:
			{
				Controller->RecvWorldInfo(RecvCharacterInfo(RecvStream));
			}
			break;
			case PacketType::ENTER_NEW_PLAYER:
			{
				Controller->RecvNewPlayer(RecvNewPlayer(RecvStream));
			}
			break;
			case PacketType::SPAWN_MONSTER:
			{

				Controller->RecvSpawnMonster(RecvMonster(RecvStream));
			}
			break;

			case PacketType::NEXT_LEVEL_STAGE_SPAWN_MONSTER:
			{


				Controller->NextStageRecvSpawnMonster(NextStageRecvMonster(RecvStream));
			}
			break;
			case PacketType::SYNCRO_MONSTER:
			{
				Controller->RecvNewMonster(RecvMonsterSet(RecvStream));
			}
			break;
			case PacketType::NEXT_LEVEL_STAGE_SYNCRO_MONSTER:
			{
				Controller->NextStageRecvNewMonster(NextStageRecvMonsterSet(RecvStream));
			}
			break;
			case PacketType::DESTROY_MONSTER:
			{
				Controller->RecvDestroyMonster(RecvMonster(RecvStream));
			}
			break;
			case PacketType::NEXT_LEVEL_STAGE_DESTROY_MONSTER:
			{
				Controller->NextStageRecvDestroyMonster(NextStageRecvMonster(RecvStream));
			}
			break;
			default:
				break;
			}
		}
	}
	return 0;
}
void server::Stop()
{
	StopTaskCounter.Increment();
}
void server::Exit()
{

}

MonsterSet* server::RecvMonsterSet(stringstream& RecvStream)
{
	RecvStream >> MonsterSetInfo;
	return &MonsterSetInfo;
}
Monster* server::RecvMonster(stringstream& RecvStream)
{
	RecvStream >> MonsterInfo;
	return &MonsterInfo;
}


//====================================================================================================================���� ��������
NextStageMonsterSet* server::NextStageRecvMonsterSet(stringstream& RecvStream)
{
	RecvStream >> NextStageMonsterSetInfo;
	return &NextStageMonsterSetInfo;
}
NextStageMonster* server::NextStageRecvMonster(stringstream& RecvStream)
{
	RecvStream >> NextStageMonsterInfo;
	return &NextStageMonsterInfo;
}
//====================================================================================================================���� ��������

cPlayer *server::RecvNewPlayer(stringstream & RecvStream)
{
	RecvStream >> NewPlayer;
	return &NewPlayer;
}

void server::SetPlayerController(ACharacterPlayerController* PlayerController)
{
	if (PlayerController)
	{
		Controller = PlayerController;
	}
}

void server::NextStageHitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie)
{
	// �������� �������� �� ���� ���� ����
	stringstream SendStream;
	SendStream << PacketType::HIT_NEXT_STAGE_MONSTER << endl;
	SendStream << MonsterID << endl;
	SendStream << Hp << endl;
	SendStream << damage << endl;
	SendStream << isDie << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);
}
//=====================================================2019-08-03
void server::HitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie)
{
	// �������� �������� �� ���� ���� ����
	stringstream SendStream;
	SendStream << PacketType::HIT_MONSTER << endl;
	SendStream << MonsterID << endl;
	SendStream << Hp << endl;
	SendStream << damage << endl;
	SendStream << isDie << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);
}
void server::HitPlayer(const int& SessionId)
{
	// �������� �������� �� ĳ���� ���� ����
	stringstream SendStream;
	SendStream << PacketType::HIT_PLAYER << endl;
	SendStream << SessionId << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);
}
//=====================================================2019-08-03