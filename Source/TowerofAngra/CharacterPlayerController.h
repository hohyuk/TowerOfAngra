// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/PlayerController.h"
#include "server.h"
#include "CharacterPlayerController.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	KAYA,
	AXE,
	NONE
};

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
	// �������κ��� ���� ���� ����
	void RecvWorldInfo(cPlayerInfo *);
	// �� �÷��̾� ������Ʈ
	void RecvNewPlayer(cPlayer *);
	void RecvNewMonster(MonsterSet*);
	void RecvSpawnMonster(Monster*);
	void SpawnMonster();
	void UpdateMonster();
private:
	MonsterSet * TOAMonsterset;
	int MonsterNum;
	Monster * TOAMonster;
	bool MonsterSpawn;
	server* Socket;
	bool				bIsConnected;	// ������ ���� ����
	int					SessionId;		// ĳ������ ���� ���� ���̵� (������)
	cPlayerInfo *		playerinfo;	// �ٸ� ĳ������ ����

	void SendPlayerInfo();		// �÷��̾� ��ġ �۽�
	bool UpdateWorldInfo();		// ���� ����ȭ
	void UpdatePlayerInfo(const cPlayer &);		// �÷��̾� ����ȭ	

	FTimerHandle SendPlayerInfoHandle;	// ����ȭ Ÿ�̸� �ڵ鷯

	// �� �÷��̾� ����
	int	nPlayers;
	bool bNewPlayerEntered;
	cPlayer * NewPlayer;
	void UpdateNewPlayer();

public:
	EPlayerType PlayerType{ EPlayerType::NONE };

	TSubclassOf<class ATowerofAngraCharacter> TOA_PlayerClass;
};
