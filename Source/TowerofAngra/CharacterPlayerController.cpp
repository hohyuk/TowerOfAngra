// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Engine.h"
#include "CharacterPlayerController.h"
#include <string>
#include "TowerofAngraCharacter.h"
#include "HKayaCharacter.h"
#include "HAxeCharacter.h"
#include "HWarriorCharacter.h"
#include "HMonster.h"
#include "HVamp.h"
#include "HGolem.h"
#include "HTOAGameState.h"
#include "HResultUserWidget.h"
#include "Async.h"

ACharacterPlayerController::ACharacterPlayerController()
{
	SessionId = FMath::RandRange(1000, 2000);

	Socket = server::GetSingleton();
	Socket->InitSock();

	const char* ip = TCHAR_TO_ANSI(*IPaddress);
	bIsConnected = Socket->Connect(ip, 9000);


	bNewPlayerEntered = false;
	nPlayers = -1;
	MonsterNum = -1;
	NextStageMonsterNum = -1;
	MonsterSpawn = false;
	NextStageMonsterSpawn = false;
	DesMonster = false;
	PrimaryActorTick.bCanEverTick = true;
	TYPE = 1;

	MonsterDestroy = false;

	// UI
	static ConstructorHelpers::FClassFinder<UHResultUserWidget> UI_RESULT_C(TEXT("/Game/TowerofAngra/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
		ResultWidgetClass = UI_RESULT_C.Class;
}

void ACharacterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACharacterPlayerController::Possess(APawn * aPawn)
{
	Super::Possess(aPawn);
}

void ACharacterPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void ACharacterPlayerController::ShowResultUI()
{
	auto TOAGameState = Cast<AHTOAGameState>(UGameplayStatics::GetGameState(this));

	TOAGameState->AddGameScore(CurrentScore);

	ResultWidget->BindGameState(TOAGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (bIsConnected)
	{
		Socket->SetPlayerController(this);
	}
	ChangeInputMode(true);

	ResultWidget = CreateWidget<UHResultUserWidget>(this, ResultWidgetClass);

	// MyPlayer ĳ���� ���
	if (PlayerType == EPlayerType::KAYA)
	{
		MyPlayer = Cast<AHKayaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::AXE)
	{
		MyPlayer = Cast<AHAxeCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::WARRIOR)
	{
		MyPlayer = Cast<AHWarriorCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else
		return;

	if (!MyPlayer)
	{
		return;
	}

	MyPlayer->SessionId = SessionId;
	auto MyLocation = MyPlayer->GetActorLocation();
	auto MyRotation = MyPlayer->GetActorRotation();

	cPlayer cp;

	cp.ClientID = SessionId;
	cp.Location = MyLocation;

	cp.Rotation = MyRotation;

	cp.Velocity = FVector::ZeroVector;

	cp.HP = MyPlayer->CurrentHP;

	cp.StageLevel = CurrentStageLevel;

	cp.IsSkilling = MyPlayer->IsSkilling;
	cp.SkillType = int(MyPlayer->SKILL_TYPE);
	cp.IsAttacking = MyPlayer->IsServerSend_Attacking;
	cp.clientPlayerType = int(MyPlayer->CurrentPlayerType);



	Socket->EnrollCharacterInfo(cp);


	Socket->StartListen();

	GetWorldTimerManager().SetTimer(SendPlayerInfoHandle, this, &ACharacterPlayerController::SendPlayerInfo, 0.010f, true);
}

void ACharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

int ACharacterPlayerController::GetSessionId()
{
	return SessionId;
}

void ACharacterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bNewPlayerEntered)
	{
		UpdateNewPlayer();
	}

	UpdateWorldInfo();

	if (MonsterSpawn)
	{
		SpawnMonster();
	}
	if (NextStageMonsterSpawn)
	{
		NextStageSpawnMonster();
	}
	// ���� �ı�
	/*if (MonsterDestroy)
		DesTroyMonster();*/

	if (NextStageMonsterDestroy)
		NextStageDesTroyMonster();

	UpdateMonster();

	UpdateNextStageMonster();

}

void ACharacterPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Socket->LogoutCharacter(SessionId);
	Socket->CloseSocket();
	Socket->StopListen();
}

void ACharacterPlayerController::RecvWorldInfo(cPlayerInfo * pi)
{
	if (pi != nullptr)
		playerinfo = pi;
}

void ACharacterPlayerController::RecvNewPlayer(cPlayer * NewPlayer_)
{
	if (NewPlayer_ != nullptr)
	{
		bNewPlayerEntered = true;
		NewPlayer = NewPlayer_;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("RecvNewPlayer")));
	}
}
void ACharacterPlayerController::RecvNewMonster(MonsterSet* NewMonster_)
{
	if (NewMonster_ != nullptr)
	{
		TOAMonsterset = NewMonster_;

	}
}
void ACharacterPlayerController::RecvSpawnMonster(Monster* m)
{
	if (m != nullptr)
	{
		TOAMonster = m;
		MonsterSpawn = true;
	}
}
void ACharacterPlayerController::RecvDestroyMonster(Monster*m)
{
	if (m != nullptr)
	{
		TOAMonster = m;
		MonsterDestroy = true;
	}
}

//============================================================================================================���� ��������
void ACharacterPlayerController::NextStageRecvNewMonster(NextStageMonsterSet* NewMonster_)
{
	if (NewMonster_ != nullptr)
	{
		NextStageTOAMonsterset = NewMonster_;

	}
}
void ACharacterPlayerController::NextStageRecvSpawnMonster(NextStageMonster* m)
{
	if (m != nullptr)
	{
		NextStageTOAMonster = m;
		NextStageMonsterSpawn = true;
	}
}
void ACharacterPlayerController::NextStageRecvDestroyMonster(NextStageMonster*m)
{
	if (m != nullptr)
	{
		NextStageTOAMonster = m;
		NextStageMonsterDestroy = true;
	}
}

//============================================================================================================���� ��������

void ACharacterPlayerController::SendPlayerInfo()
{
	cPlayer cp;
	cp.ClientID = SessionId;

	cp.Location = MyPlayer->GetActorLocation();
	cp.Rotation = MyPlayer->GetActorRotation();
	cp.Velocity = MyPlayer->GetVelocity();


	cp.HP = MyPlayer->CurrentHP;

	cp.StageLevel = CurrentStageLevel;

	cp.SkillType = int(MyPlayer->SKILL_TYPE);
	cp.IsSkilling = MyPlayer->IsSkilling;
	cp.IsAttacking = MyPlayer->IsServerSend_Attacking;
	cp.clientPlayerType = int(MyPlayer->CurrentPlayerType);

	Socket->SendPlayer(cp);
}

bool ACharacterPlayerController::UpdateWorldInfo()
{
	if (playerinfo == nullptr)
		return false;
	TArray<AActor*> AllActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TOA_OtherPlayerClass, AllActor);


	if (nPlayers == -1)
	{

		nPlayers = playerinfo->players.size();
		for (auto & player : playerinfo->players)
		{
			if (player.first == SessionId)
				continue;

			FVector SpawnLocation = player.second.Location;

			FRotator SpawnRotation = player.second.Rotation;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this->Instigator;


			// �ٸ� �÷��̾� Ÿ��
			EPlayerType OtherPlayerType = EPlayerType(player.second.clientPlayerType);

			if (OtherPlayerType == EPlayerType::AXE)
			{
				TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

				AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(AHAxeCharacter::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SessionId = player.second.ClientID;
			}
			else if (OtherPlayerType == EPlayerType::WARRIOR)
			{
				TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();

				AHWarriorCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHWarriorCharacter>(AHWarriorCharacter::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SessionId = player.second.ClientID;
			}
		}
	}
	else
	{
		for (const auto& Character : AllActor)
		{
			ATowerofAngraCharacter * OtherCharacter = Cast<ATowerofAngraCharacter>(Character);

			if (!OtherCharacter || OtherCharacter->SessionId == -1 || OtherCharacter->SessionId == SessionId)
				continue;

			cPlayer * info = &playerinfo->players[OtherCharacter->SessionId];
			info->StageLevel = CurrentStageLevel;

			if (info->IsSkilling)
			{
				UE_LOG(LogClass, Log, TEXT("Skilling ANIM"));
				OtherCharacter->ServerRecvSkillCheck(ESkillType(info->SkillType));
			}
			else if (info->IsAttacking)
			{
				UE_LOG(LogClass, Log, TEXT("Attacking ANIM"));
				OtherCharacter->Attack();
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("nPlayers Size : %d"), playerinfo->players.size()));

			OtherCharacter->AddMovementInput(info->Velocity);
			OtherCharacter->SetActorRotation(info->Rotation);
			OtherCharacter->SetActorLocation(info->Location);
			OtherCharacter->ServerSetHP(info->HP);
		}
	}
	return true;
}

void ACharacterPlayerController::UpdatePlayerInfo(const cPlayer & info)
{
	auto Player = Cast<ATowerofAngraCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Player)
		return;

	UWorld* const world = GetWorld();
	if (!world)
		return;


	Player->ServerSetHP(info.HP);
	Player->CurrentPlayerType = EPlayerType(info.clientPlayerType);
}

void ACharacterPlayerController::UpdateNewPlayer()
{
	if (NewPlayer->ClientID == SessionId)
	{
		bNewPlayerEntered = false;
		NewPlayer = nullptr;
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("UpdateNewPlayer SessionId : %d"), SessionId));
	FVector SpawnLocation = NewPlayer->Location;
	FRotator SpawnRotation = NewPlayer->Rotation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this->Instigator;


	// �ٸ� �÷��̾� Ÿ��
	EPlayerType OtherPlayerType = EPlayerType(NewPlayer->clientPlayerType);

	if (OtherPlayerType == EPlayerType::AXE)
	{
		TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

		AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(AHAxeCharacter::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->SessionId = NewPlayer->ClientID;
	}
	else if (OtherPlayerType == EPlayerType::WARRIOR)
	{
		TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();

		AHWarriorCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHWarriorCharacter>(AHWarriorCharacter::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->SessionId = NewPlayer->ClientID;
	}
	else
	{
		TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

		AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->SessionId = NewPlayer->ClientID;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error Player Type")));
	}

	if (playerinfo != nullptr)
	{
		cPlayer player;

		player.ClientID = NewPlayer->ClientID;

		player.Location = NewPlayer->Location;
		player.Rotation = NewPlayer->Rotation;
		player.Velocity = NewPlayer->Velocity;

		player.HP = NewPlayer->HP;

		player.SkillType = int(NewPlayer->SkillType);
		player.IsSkilling = NewPlayer->IsSkilling;
		player.IsAttacking = NewPlayer->IsAttacking;
		player.StageLevel = CurrentStageLevel;
		player.clientPlayerType = int(NewPlayer->clientPlayerType);

		playerinfo->players[NewPlayer->ClientID] = player;
	}

	UE_LOG(LogClass, Log, TEXT("Another Player Spawned."));

	bNewPlayerEntered = false;
	NewPlayer = nullptr;
}

//=============================================================================================================�߰�

void ACharacterPlayerController::NextStageSpawnMonster()
{
	UWorld* const world = GetWorld();
	if (world && CurrentStageLevel == 1)
	{
		FVector SpawnLocation;
		SpawnLocation.X = NextStageTOAMonster->X;
		SpawnLocation.Y = NextStageTOAMonster->Y;
		SpawnLocation.Z = NextStageTOAMonster->Z;

		FRotator MonsterRotation;
		MonsterRotation.Yaw = 0;
		MonsterRotation.Pitch = 0;
		MonsterRotation.Roll = 0;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;


		AHGolem* SpawnMonster = world->SpawnActor<AHGolem>(AHGolem::StaticClass(), SpawnLocation, MonsterRotation, SpawnParams);
		if (SpawnMonster)
		{
			SpawnMonster->SpawnDefaultController();
			SpawnMonster->MonsterID = NextStageTOAMonster->MonsterID;
			SpawnMonster->HP = NextStageTOAMonster->HP;
		}


		NextStageTOAMonster = nullptr;
		NextStageMonsterSpawn = false;
	}
}
void ACharacterPlayerController::SpawnMonster()
{
	UWorld* const world = GetWorld();
	if (world && CurrentStageLevel == 0)
	{
		FVector SpawnLocation;
		SpawnLocation.X = TOAMonster->X;
		SpawnLocation.Y = TOAMonster->Y;
		SpawnLocation.Z = TOAMonster->Z;

		FRotator MonsterRotation;
		MonsterRotation.Yaw = 0;
		MonsterRotation.Pitch = 0;
		MonsterRotation.Roll = 0;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		AHVamp* SpawnMonster = world->SpawnActor<AHVamp>(AHVamp::StaticClass(), SpawnLocation, MonsterRotation, SpawnParams);
		if (SpawnMonster)
		{
			SpawnMonster->SpawnDefaultController();
			SpawnMonster->MonsterID = TOAMonster->MonsterID;
			SpawnMonster->HP = TOAMonster->HP;
		}


		TOAMonster = nullptr;
		MonsterSpawn = false;
	}
}

//=============================================================================================================�߰�
void ACharacterPlayerController::UpdateNextStageMonster()
{
	if (NextStageTOAMonsterset == nullptr)
		return;

	UWorld* const world = GetWorld();

	//���� �������� ������ ���͵�

	if (world && CurrentStageLevel == 1)
	{
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHGolem::StaticClass(), SpawnedMonsters);

		if (NextStageMonsterNum == -1)
		{
			NextStageMonsterNum = NextStageTOAMonsterset->monsters.size();

			for (const auto& kvp : NextStageTOAMonsterset->monsters)
			{
				const  NextStageMonster * monster = &kvp.second;
				FVector SpawnLocation;
				SpawnLocation.X = monster->X;
				SpawnLocation.Y = monster->Y;
				SpawnLocation.Z = monster->Z;

				FRotator SpawnRotation(0, 0, 0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;

				AHGolem* SpawnMonster = world->SpawnActor<AHGolem>(AHGolem::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

				if (SpawnMonster)
				{
					SpawnMonster->SpawnDefaultController();
					SpawnMonster->MonsterID = monster->MonsterID;
					SpawnMonster->HP = monster->HP;
				}
			}
		}
		else
		{
			for (auto actor : SpawnedMonsters)
			{
				AHGolem * monster = Cast<AHGolem>(actor);

				if (monster)
				{
					const  NextStageMonster * MonsterInfo = &NextStageTOAMonsterset->monsters[monster->MonsterID];

					FVector MonsterLocation;
					MonsterLocation.X = MonsterInfo->X;
					MonsterLocation.Y = MonsterInfo->Y;
					MonsterLocation.Z = MonsterInfo->Z;

					monster->MoveToLocation(MonsterLocation);

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d MonsterInfo->X =  %f"), MonsterInfo->MonsterID, MonsterInfo->X));

					if (MonsterInfo->IsAttacking)
					{
						monster->ServerAttack(EMonsterName::GOLEM);
					}
				}
			}
		}
	}
}
void ACharacterPlayerController::UpdateMonster()
{
	if (TOAMonsterset == nullptr)
		return;

	UWorld* const world = GetWorld();

	if (world && CurrentStageLevel == 0)
	{
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHVamp::StaticClass(), SpawnedMonsters);

		if (MonsterNum == -1)
		{


			for (const auto& kvp : TOAMonsterset->monsters)
			{
				const Monster * monster = &kvp.second;
				FVector SpawnLocation;
				SpawnLocation.X = monster->X;
				SpawnLocation.Y = monster->Y;
				SpawnLocation.Z = monster->Z;

				FRotator SpawnRotation(0, 0, 0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("monster->X =  %d"), monster->X));

				AHVamp* SpawnMonster = world->SpawnActor<AHVamp>(AHVamp::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
				if (SpawnMonster)
				{
					SpawnMonster->SpawnDefaultController();
					SpawnMonster->MonsterID = monster->MonsterID;
					SpawnMonster->HP = monster->HP;
				}
			}
			MonsterNum = TOAMonsterset->monsters.size();
		}
		else
		{
			for (auto actor : SpawnedMonsters)
			{
				AHVamp * monster = Cast<AHVamp>(actor);

				if (monster)
				{
					const Monster * MonsterInfo = &TOAMonsterset->monsters[monster->MonsterID];

					FVector MonsterLocation;
					MonsterLocation.X = MonsterInfo->X;
					MonsterLocation.Y = MonsterInfo->Y;
					MonsterLocation.Z = MonsterInfo->Z;

					monster->MoveToLocation(MonsterLocation);



					if (MonsterInfo->IsAttacking)
					{
						monster->ServerAttack(EMonsterName::VAMP);

					}
				}
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("monster size =  %d"), TOAMonsterset->monsters.size()));

	}
}
void ACharacterPlayerController::NextStageDesTroyMonster()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("NextStageDesTroyMonster")));
	if (GetWorld())
	{
		// ������ ���Ϳ��� ã�� �ı�
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHGolem::StaticClass(), SpawnedMonsters);

		for (auto Actor : SpawnedMonsters)
		{
			AHGolem * Monster = Cast<AHGolem>(Actor);
			if (Monster && Monster->MonsterID == NextStageTOAMonster->MonsterID)
			{
				Monster->ServerSendDieOn(EMonsterName::GOLEM);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("NextStageDesTroyMonster()  %d"), Monster->MonsterID));
				break;
			}
		}

		// ������Ʈ �� �ʱ�ȭ
		NextStageTOAMonster = nullptr;
		NextStageMonsterDestroy = false;
	}
}
void ACharacterPlayerController::DesTroyMonster()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("DesTroyMonster")));
	if (GetWorld())
	{
		// ������ ���Ϳ��� ã�� �ı�
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHVamp::StaticClass(), SpawnedMonsters);

		for (auto Actor : SpawnedMonsters)
		{
			AHVamp * Monster = Cast<AHVamp>(Actor);
			if (Monster && Monster->MonsterID == TOAMonster->MonsterID)
			{
				Monster->ServerSendDieOn(EMonsterName::VAMP);
				TOAMonsterset->monsters.erase(TOAMonster->MonsterID);
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("DesTroyMonster()  %d"), Monster->MonsterID));
				break;
			}
		}

		// ������Ʈ �� �ʱ�ȭ
		TOAMonster = nullptr;
		MonsterDestroy = false;
	}
}

void ACharacterPlayerController::HitCharacter(const int & SessionId)
{
	//	UE_LOG(LogClass, Log, TEXT("Player Hit Called %d"), SessionId);

	Socket->HitPlayer(SessionId);
}

void ACharacterPlayerController::HitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie)
{
	//	UE_LOG(LogClass, Log, TEXT("Monster Hit Called %d"), MonsterId);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("HitMonster Hp =  %f"), Hp));
	Socket->HitMonster(MonsterID, Hp, damage, isDie);
}

void ACharacterPlayerController::NextStageHitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie)
{
	//	UE_LOG(LogClass, Log, TEXT("Monster Hit Called %d"), MonsterId);

	Socket->NextStageHitMonster(MonsterID, Hp, damage, isDie);
}