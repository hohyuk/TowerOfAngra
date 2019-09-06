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

ACharacterPlayerController::ACharacterPlayerController()
{
	SessionId = FMath::RandRange(0, 100);

	Socket = server::GetSingleton();
	Socket->InitSock();

	const char* ip = TCHAR_TO_ANSI(*IPaddress);
	bIsConnected = Socket->Connect(ip, 9000);

	if (bIsConnected)
	{
		Socket->SetPlayerController(this);
	}
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

	ChangeInputMode(true);

	ResultWidget = CreateWidget<UHResultUserWidget>(this, ResultWidgetClass);

	// MyPlayer 캐릭터 등록
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
	// 몬스터 파괴
	if (MonsterDestroy)
		DesTroyMonster();
	if (NextStageMonsterDestroy)
		NextStageDesTroyMonster();
	if (CurrentStageLevel == 0)
		UpdateMonster();
	else if (CurrentStageLevel == 1)
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

//============================================================================================================다음 스테이지
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

//============================================================================================================다음 스테이지

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

	//	UpdatePlayerInfo(playerinfo->players[SessionId]);

	TArray<AActor*> AllActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TOA_OtherPlayerClass, AllActor);
	int StageLevel = -1;

	if (nPlayers == -1)
	{
		for (auto & player : playerinfo->players)
		{
			if (player.first == SessionId)
				continue;

			FVector SpawnLocation = player.second.Location;

			FRotator SpawnRotation = player.second.Rotation;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this->Instigator;
			SpawnParams.Name = FName(*FString(to_string(player.second.ClientID).c_str()));

			// 다른 플레이어 타입
			EPlayerType OtherPlayerType = EPlayerType(player.second.clientPlayerType);

			if (OtherPlayerType == EPlayerType::AXE)
			{
				TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

				AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SessionId = player.second.ClientID;
			}
			else if (OtherPlayerType == EPlayerType::WARRIOR)
			{
				TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();

				AHWarriorCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHWarriorCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SessionId = player.second.ClientID;
			}
			else
			{
				TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

				AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
				SpawnCharacter->SpawnDefaultController();
				SpawnCharacter->SessionId = player.second.ClientID;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error Player Type")));
			}
		}
		nPlayers = playerinfo->players.size();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("nPlayers Size : %d"), nPlayers));
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

			OtherCharacter->AddMovementInput(info->Velocity);
			OtherCharacter->SetActorRotation(info->Rotation);
			OtherCharacter->SetActorLocation(info->Location);
			OtherCharacter->ServerSetHP(info->HP);
			OtherCharacter->CurrentPlayerType = EPlayerType(info->clientPlayerType);
		}
	}
	return true;
}

void ACharacterPlayerController::UpdatePlayerInfo(const cPlayer & info)
{
	//향후 체력이나 다른플레이어 체력 등등 업뎃
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
	SpawnParams.Instigator = Instigator;
	SpawnParams.Name = FName(*FString(to_string(NewPlayer->ClientID).c_str()));

	// 다른 플레이어 타입
	EPlayerType OtherPlayerType = EPlayerType(NewPlayer->clientPlayerType);

	if (OtherPlayerType == EPlayerType::AXE)
	{
		TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

		AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->SessionId = NewPlayer->ClientID;
	}
	else if (OtherPlayerType == EPlayerType::WARRIOR)
	{
		TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("WARRIOR")));

		AHWarriorCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHWarriorCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->SessionId = NewPlayer->ClientID;
	}
	else
	{
		TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();

		AHAxeCharacter* SpawnCharacter = GetWorld()->SpawnActor<AHAxeCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
		SpawnCharacter->SpawnDefaultController();
		SpawnCharacter->SessionId = NewPlayer->ClientID;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error Player Type")));
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
		player.clientPlayerType = int(NewPlayer->clientPlayerType);

		playerinfo->players[NewPlayer->ClientID] = player;
	}

	UE_LOG(LogClass, Log, TEXT("Another Player Spawned."));

	bNewPlayerEntered = false;
	NewPlayer = nullptr;
}

//=============================================================================================================추가

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
		SpawnParams.Name = FName(*FString(to_string(NextStageTOAMonster->MonsterID).c_str()));

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
		SpawnParams.Name = FName(*FString(to_string(TOAMonster->MonsterID).c_str()));

		AHVamp* SpawnMonster = world->SpawnActor<AHVamp>(AHVamp::StaticClass(), SpawnLocation, MonsterRotation, SpawnParams);
		if (SpawnMonster)
		{
			SpawnMonster->SpawnDefaultController();
			SpawnMonster->MonsterID = TOAMonster->MonsterID;
			//		SpawnMonster->HP = TOAMonster->HP;
		}


		TOAMonster = nullptr;
		MonsterSpawn = false;
	}
}

//=============================================================================================================추가
void ACharacterPlayerController::UpdateNextStageMonster()
{
	if (NextStageTOAMonsterset == nullptr)
		return;
	TArray<AActor*> SpawnedMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHGolem::StaticClass(), SpawnedMonsters);

	UWorld* const world = GetWorld();

	//다음 스테이지 스폰할 몬스터들

	if (world && CurrentStageLevel == 1)
	{
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
				SpawnParams.Name = FName(*FString(to_string(monster->MonsterID).c_str()));

				AHGolem* SpawnMonster = world->SpawnActor<AHGolem>(AHGolem::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

				if (SpawnMonster)
				{
					SpawnMonster->SpawnDefaultController();
					SpawnMonster->MonsterID = monster->MonsterID;
					//	SpawnMonster->HP = monster->HP;
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

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("1stage monster x : %d"), MonsterInfo->X));

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

	TArray<AActor*> SpawnedMonsters;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHVamp::StaticClass(), SpawnedMonsters);

	UWorld* const world = GetWorld();

	if (world && CurrentStageLevel == 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHVamp::StaticClass(), SpawnedMonsters);
		if (MonsterNum == -1)
		{
			MonsterNum = TOAMonsterset->monsters.size();

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
				SpawnParams.Name = FName(*FString(to_string(monster->MonsterID).c_str()));

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("monster->X =  %d"), monster->X));

				AHVamp* SpawnMonster = world->SpawnActor<AHVamp>(AHVamp::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
				if (SpawnMonster)
				{
					SpawnMonster->SpawnDefaultController();
					SpawnMonster->MonsterID = monster->MonsterID;
					//	SpawnMonster->HP = monster->HP;
				}
			}
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

					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MonsterInfo->X =  %f"), MonsterInfo->X));


					//if (MonsterInfo->IsAttacking)
					//{
					//	monster->ServerAttack(EMonsterName::VAMP);

					//}
				}
			}
		}
	}
}
void ACharacterPlayerController::NextStageDesTroyMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("NextStageDesTroyMonster")));
	if (GetWorld())
	{
		// 스폰된 몬스터에서 찾아 파괴
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHGolem::StaticClass(), SpawnedMonsters);

		for (auto Actor : SpawnedMonsters)
		{
			AHGolem * Monster = Cast<AHGolem>(Actor);
			if (Monster && Monster->MonsterID == NextStageTOAMonster->MonsterID)
			{
				Monster->ServerSendDieOn(EMonsterName::GOLEM);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("NextStageDesTroyMonster()  %d"), Monster->MonsterID));
				break;
			}
		}

		// 업데이트 후 초기화
		NextStageTOAMonster = nullptr;
		NextStageMonsterDestroy = false;
	}
}
void ACharacterPlayerController::DesTroyMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("DesTroyMonster")));
	if (GetWorld())
	{
		// 스폰된 몬스터에서 찾아 파괴
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHVamp::StaticClass(), SpawnedMonsters);

		for (auto Actor : SpawnedMonsters)
		{
			AHVamp * Monster = Cast<AHVamp>(Actor);
			if (Monster && Monster->MonsterID == TOAMonster->MonsterID)
			{
				Monster->ServerSendDieOn(EMonsterName::VAMP);
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("DesTroyMonster()  %d"), Monster->MonsterID));
				break;
			}
		}

		// 업데이트 후 초기화
		TOAMonster = nullptr;
		MonsterDestroy = false;
	}
}

void ACharacterPlayerController::HitCharacter(const int & SessionId)
{
	//	UE_LOG(LogClass, Log, TEXT("Player Hit Called %d"), SessionId);

	Socket->HitPlayer(SessionId);
}

void ACharacterPlayerController::HitMonster(const int & MonsterId, const float& damage)
{
	//	UE_LOG(LogClass, Log, TEXT("Monster Hit Called %d"), MonsterId);

	Socket->HitMonster(MonsterId, damage);
}

void ACharacterPlayerController::NextStageHitMonster(const int & MonsterId, const float& damage)
{
	//	UE_LOG(LogClass, Log, TEXT("Monster Hit Called %d"), MonsterId);

	Socket->NextStageHitMonster(MonsterId, damage);
}