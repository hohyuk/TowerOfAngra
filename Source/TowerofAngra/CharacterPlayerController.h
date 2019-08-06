// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/PlayerController.h"
#include "server.h"
#include "CharacterPlayerController.generated.h"

/**
 *
 */

UCLASS()
class TOWEROFANGRA_API ACharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void Possess(APawn* aPawn) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupInputComponent() override;


public:
	int GetSessionId();
	// 소켓으로부터 월드 정보 수신
	void RecvWorldInfo(cPlayerInfo *);
	// 새 플레이어 업데이트
	void RecvNewPlayer(cPlayer *);
	void RecvNewMonster(MonsterSet*);
	void RecvSpawnMonster(Monster*);
	void SpawnMonster();
	void UpdateMonster();
	void RecvDestroyMonster(Monster*);

	//------------------------------------2019-08-03
	void HitCharacter(const int&);
	void HitMonster(const int&);

	//------------------------------------
private:
	MonsterSet * TOAMonsterset;
	int MonsterNum;
	Monster * TOAMonster;
	bool MonsterSpawn;
	server* Socket;
	bool				bIsConnected;	// 서버와 접속 유무
	int					SessionId;		// 캐릭터의 세션 고유 아이디 (랜덤값)
	cPlayerInfo *		playerinfo;	// 다른 캐릭터의 정보

	bool DesMonster;

	void SendPlayerInfo();		// 플레이어 위치 송신
	bool UpdateWorldInfo();		// 월드 동기화
	void UpdatePlayerInfo(const cPlayer &);		// 플레이어 동기화	

	FTimerHandle SendPlayerInfoHandle;	// 동기화 타이머 핸들러

	// 새 플레이어 입장
	int	nPlayers;
	bool bNewPlayerEntered;
	cPlayer * NewPlayer;
	void UpdateNewPlayer();

public:
	TSubclassOf<class ATowerofAngraCharacter> TOA_OtherPlayerClass;
};
