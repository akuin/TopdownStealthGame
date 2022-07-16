// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Materials/MaterialParameterCollectionInstance.h>
#include "Enemy.generated.h"

UCLASS()
class STEALTHTOPDOWN_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UFUNCTION()
	void AddDecal();

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UMaterialInterface* ConeDecal;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UMaterialParameterCollection* ConeOriginParameter;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UMaterialParameterCollectionInstance* ConeOriginInst;

	UPROPERTY(Replicated)
	AActor* CurrentTarget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
