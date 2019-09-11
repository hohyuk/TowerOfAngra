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

	void ChangeInputMode(bool bGameMode = true);
	void ShowResultUI();

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
	//=======================================================================================================
	void NextStageSpawnMonster();
	void NextStageDesTroyMonster();
	//=============================================================================================
	void UpdateMonster();
	void UpdateNextStageMonster();
	void RecvDestroyMonster(Monster*);
	void DesTroyMonster();
	//------------------------------------2019-08-03
	void HitCharacter(const int&);
	void HitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie);
	int TYPE;
	//------------------------------------

	void NextStageHitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie);
	//==============================================================================================���� ��������
	void NextStageRecvDestroyMonster(NextStageMonster*);

	void NextStageRecvNewMonster(NextStageMonsterSet*);
	void NextStageRecvSpawnMonster(NextStageMonster*);
	//==================================================================================================
private:
	int MonsterNum;
	int NextStageMonsterNum;
	Monster * TOAMonster;


	//================================================================================================================
	NextStageMonster * NextStageTOAMonster;
	//==============================================================================================================
	bool MonsterSpawn;
	bool NextStageMonsterSpawn;


	server* Socket;
	bool				bIsConnected;	// ������ ���� ����
	int					SessionId;		// ĳ������ ���� ���� ���̵� (������)
	cPlayerInfo *		playerinfo;	// �ٸ� ĳ������ ����
	bool MonsterDestroy;
	bool DesMonster;

	bool NextStageMonsterDestroy;
	bool NextStageDesMonster;

	void SendPlayerInfo();		// �÷��̾� ��ġ �۽�
	bool UpdateWorldInfo();		// ���� ����ȭ
	void UpdatePlayerInfo(const cPlayer &);		// �÷��̾� ����ȭ	

	FTimerHandle SendPlayerInfoHandle;	// ����ȭ Ÿ�̸� �ڵ鷯

	// �� �÷��̾� ����
	int	nPlayers;
	bool bNewPlayerEntered;
	cPlayer * NewPlayer{ nullptr };
	void UpdateNewPlayer();

public:
	TSubclassOf<class ATowerofAngraCharacter> TOA_OtherPlayerClass;
	TSubclassOf<class AHMonster> TOA_OtherMonsterClass;


	/*���â UI*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UHResultUserWidget> ResultWidgetClass;
private:
	UPROPERTY()
		class ATowerofAngraCharacter* MyPlayer{ nullptr };
	UPROPERTY()
		class AHTOAGameState* TOAPlayerState;

	UPROPERTY()
		class UHResultUserWidget* ResultWidget;

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;

public:
	MonsterSet* TOAMonsterset;

	//================================================================================���� ��������
	NextStageMonsterSet* NextStageTOAMonsterset;
};
