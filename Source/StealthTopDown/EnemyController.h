// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy.h"
#include <Engine/TextureRenderTarget2D.h>
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHTOPDOWN_API AEnemyController : public AAIController
{
	GENERATED_BODY()

	AEnemyController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY()
	AEnemy* ControlledPawn;

	UPROPERTY()
	AActor* CurrentTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AI)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI)
	UBlackboardComponent* BlackboardComponent;

private:

	UPROPERTY()
	int NumTraces = 40;

	UPROPERTY()
	float TraceDegrees = 2;

	UPROPERTY()
	float ConeHeightOffset = 400.f;

	UPROPERTY()
	TArray<FVector> TraceResults;

	UPROPERTY()
	float TraceDistance = 700.f;

	UPROPERTY()
	int HalfCanvasSize = 1024;

	UPROPERTY()
	TArray<FCanvasUVTri> CanvasTriangles;

	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UTextureRenderTarget2D* ConeRenderTarget;

	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnEnemyPerceived(const TArray<AActor*>& UpdatedActors);

	FTimerHandle TimerHandle_VisionCheck;

	UFUNCTION()
	void CheckEnemiesInVision();

	UFUNCTION()
	void CreateCone();

	UFUNCTION()
	void CreateCanvasTriangles();

	UFUNCTION()
	void DrawTrianglesOnCanvas();
};
