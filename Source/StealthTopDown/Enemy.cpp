// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>
#include <GameFramework/CharacterMovementComponent.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

void AEnemy::AddDecal()
{
	if (ConeDecal)
	{
		DecalComp = UGameplayStatics::SpawnDecalAttached(ConeDecal, FVector(1000.f, 1000.f, 1000.f), GetMesh(), EName::NAME_None, FVector(EForceInit::ForceInit), FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	}

	if (ConeOriginParameter)
	{
		ConeOriginInst = GetWorld()->GetParameterCollectionInstance(ConeOriginParameter);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AddDecal();
}

void AEnemy::Tick(float DeltaTime)
{
		FLinearColor LinearPosition;

		LinearPosition.R = GetActorLocation().X;
		LinearPosition.G = GetActorLocation().Y;
		LinearPosition.B = GetActorLocation().Z;

		if (ConeOriginInst)
		{
			ConeOriginInst->SetVectorParameterValue(FName("EnemyPosition"), LinearPosition);
		}
		if (CurrentTarget)
		{
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), GetActorLocation(), CurrentTarget->GetActorLocation(), FLinearColor::Red, 0.f, 10.f);
		}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, CurrentTarget);
}