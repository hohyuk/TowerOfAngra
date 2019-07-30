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

ACharacterPlayerController::ACharacterPlayerController()
{
	SessionId = FMath::RandRange(0, 100);

	// 서버와 연결
	Socket = server::GetSingleton();
	Socket->InitSock();
	const char* ip = TCHAR_TO_ANSI(*IPaddress);
	bIsConnected = Socket->Connect(ip, 9000);
	if (bIsConnected)
	{
		UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		Socket->SetPlayerController(this);
	}
	bNewPlayerEntered = false;
	nPlayers = -1;
	MonsterNum = -1;
	MonsterSpawn = false;

	PrimaryActorTick.bCanEverTick = true;
}

void ACharacterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACharacterPlayerController::Possess(APawn * aPawn)
{
	Super::Possess(aPawn);
}

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터 등록
	ATowerofAngraCharacter* Player = nullptr;
	if (PlayerType == EPlayerType::KAYA)
	{
		Player = Cast<AHKayaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::AXE)
	{
		Player = Cast<AHAxeCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::WARRIOR)
	{
		Player = Cast<AHWarriorCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else
		return;

	if (!Player)
	{
		return;
	}
	auto MyLocation = Player->GetActorLocation();
	auto MyRotation = Player->GetActorRotation();

	cPlayer cp;

	cp.ClientID = SessionId;
	cp.Location = MyLocation;
	
	cp.Rotation = MyRotation;
	
	cp.Velocity = FVector::ZeroVector;
	
	cp.IsSkilling = Player->IsSkilling;
	cp.IsAttacking = Player->IsServerSend_Attacking;
	cp.clientPlayerType = int(Player->CurrentPlayerType);

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
		UpdateNewPlayer();

	if (MonsterSpawn)
		SpawnMonster();

	UpdateWorldInfo();
	UpdateMonster();
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
	playerinfo = pi;
}

void ACharacterPlayerController::RecvNewPlayer(cPlayer * NewPlayer_)
{
	if (NewPlayer_ != nullptr)
	{
		bNewPlayerEntered = true;
		NewPlayer = NewPlayer_;
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
void ACharacterPlayerController::SendPlayerInfo()
{
	ATowerofAngraCharacter* Player = nullptr;
	if (PlayerType == EPlayerType::KAYA)
	{
		Player = Cast<AHKayaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::AXE)
	{
		Player = Cast<AHAxeCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::WARRIOR)
	{
		Player = Cast<AHWarriorCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else
		return;

	if (!Player)
		return;

	const auto &MyLocation = Player->GetActorLocation();
	const auto &MyRotation = Player->GetActorRotation();
	const auto &Velocity = Player->GetVelocity();

	cPlayer cp;
	cp.ClientID = SessionId;
	
	cp.Location = MyLocation;
	
	cp.Rotation = MyRotation;
	
	cp.Velocity = Velocity;

	cp.IsSkilling = Player->IsSkilling;
	cp.IsAttacking = Player->IsServerSend_Attacking;
	cp.clientPlayerType = int(Player->CurrentPlayerType);

	Socket->SendCharacterInfo(cp);
}

bool ACharacterPlayerController::UpdateWorldInfo()
{
	UWorld* const world = GetWorld();
	if (world == nullptr)
		return false;
	if (playerinfo == nullptr)
		return false;

	UpdatePlayerInfo(playerinfo->players[SessionId]);

	TArray<AActor*> SpawnedCharacters;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TOA_OtherPlayerClass, SpawnedCharacters);

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
			}
			else if (OtherPlayerType == EPlayerType::WARRIOR)
			{
				TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();
			}
			else
			{
				TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();
			}

			ATowerofAngraCharacter* SpawnCharacter = world->SpawnActor<ATowerofAngraCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
			SpawnCharacter->SpawnDefaultController();

			SpawnCharacter->SessionId = player.second.ClientID;
			SpawnCharacter->IsSkilling = player.second.IsSkilling;
			SpawnCharacter->IsServerSend_Attacking = player.second.IsAttacking;


			SpawnCharacter->CurrentPlayerType = EPlayerType(player.second.clientPlayerType);
		}
		nPlayers = playerinfo->players.size();
	}
	else
	{
		for (const auto& Character : SpawnedCharacters)
		{
			ATowerofAngraCharacter * OtherCharacter = Cast<ATowerofAngraCharacter>(Character);

			if (!OtherCharacter || OtherCharacter->SessionId == -1 || OtherCharacter->SessionId == SessionId)
			{
				continue;
			}

			cPlayer * info = &playerinfo->players[OtherCharacter->SessionId];

			if (info->IsSkilling)
			{
				UE_LOG(LogClass, Log, TEXT("Skilling ANIM"));
				OtherCharacter->Skill();
			}
			else if (info->IsAttacking)
			{
				UE_LOG(LogClass, Log, TEXT("Attacking ANIM"));
				OtherCharacter->Attack();
			}

			FVector CharacterLocation = info->Location;
			FRotator CharacterRotation = info->Rotation;
		
			FVector CharacterVelocity = info->Velocity;

			// 다른 플레이어 타입
			EPlayerType OtherPlayerType = EPlayerType(info->clientPlayerType);
			if (OtherPlayerType == EPlayerType::AXE)
			{
				TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();
			}
			else if (OtherPlayerType == EPlayerType::WARRIOR)
			{
				TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();
			}
			else
			{
				TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();
			}
			//			SpawnCharacter->CurrentPlayerType = EPlayerType(player.second.clientPlayerType);
			//			UE_LOG(LogTemp, Log, TEXT("clientPlayerType :: &d"), int(NewPlayer->clientPlayerType));

			OtherCharacter->AddMovementInput(CharacterVelocity);
			OtherCharacter->SetActorRotation(CharacterRotation);
			OtherCharacter->SetActorLocation(CharacterLocation);
		}

	}
	return true;
}

void ACharacterPlayerController::UpdatePlayerInfo(const cPlayer & info)
{
	ATowerofAngraCharacter* Player = nullptr;
	if (PlayerType == EPlayerType::KAYA)
	{
		Player = Cast<AHKayaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::AXE)
	{
		Player = Cast<AHAxeCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else if (PlayerType == EPlayerType::WARRIOR)
	{
		Player = Cast<AHWarriorCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	}
	else
		return;

	if (!Player)
		return;

	UWorld* const world = GetWorld();
	if (!world)
		return;

	//향후 체력이나 다른플레이어 체력 등등 업뎃

}

void ACharacterPlayerController::UpdateNewPlayer()
{
	UWorld* const world = GetWorld();

	if (NewPlayer->ClientID == SessionId)
	{
		bNewPlayerEntered = false;
		NewPlayer = nullptr;
		return;
	}

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
	}
	else if (OtherPlayerType == EPlayerType::WARRIOR)
	{
		TOA_OtherPlayerClass = AHWarriorCharacter::StaticClass();
	}
	else
	{
		TOA_OtherPlayerClass = AHAxeCharacter::StaticClass();
	}

	ATowerofAngraCharacter* SpawnCharacter = world->SpawnActor<ATowerofAngraCharacter>(TOA_OtherPlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
	SpawnCharacter->SpawnDefaultController();

	if (SpawnCharacter == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("! Failed to spawn AprojectlevelCharacter Pawn."));
	}
	SpawnCharacter->SessionId = NewPlayer->ClientID;

	if (playerinfo != nullptr)
	{
		cPlayer player;
		player.ClientID = NewPlayer->ClientID;
		player.Location = NewPlayer->Location;
		
		player.Rotation = NewPlayer->Rotation;
		
		player.Velocity = NewPlayer->Velocity;

		player.IsSkilling = NewPlayer->IsSkilling;
		player.IsAttacking = NewPlayer->IsAttacking;
		player.clientPlayerType = int(NewPlayer->clientPlayerType);



		playerinfo->players[NewPlayer->ClientID] = player;
	}

	UE_LOG(LogClass, Log, TEXT("Another Player Spawned."));

	bNewPlayerEntered = false;
	NewPlayer = nullptr;
}

//Monster
void ACharacterPlayerController::SpawnMonster()
{
	UWorld* const world = GetWorld();

	if (world)
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

		AHMonster* SpawnMonster = world->SpawnActor<AHMonster>(AHMonster::StaticClass(), SpawnLocation, MonsterRotation, SpawnParams);

		TOAMonster = nullptr;
		MonsterSpawn = false;
	}
}
void ACharacterPlayerController::UpdateMonster()
{
	if (TOAMonsterset == nullptr)
		return;

	UWorld* const world = GetWorld();
	if (world)
	{
		TArray<AActor*> SpawnedMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHMonster::StaticClass(), SpawnedMonsters);

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

				AHMonster* SpawnMonster = world->SpawnActor<AHMonster>(AHMonster::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
				if (SpawnMonster)
				{
					SpawnMonster->MonsterID = monster->MonsterID;

				}
			}
		}
		else
		{
			for (auto actor : SpawnedMonsters)
			{
				AHMonster * monster = Cast<AHMonster>(actor);
				if (monster)
				{
					const Monster * MonsterInfo = &TOAMonsterset->monsters[monster->MonsterID];

					FVector Location;
					Location.X = MonsterInfo->X;
					Location.Y = MonsterInfo->Y;
					Location.Z = MonsterInfo->Z;



					if (MonsterInfo->IsAttacking)
					{

					}
				}
			}
		}
	}
}