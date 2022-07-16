// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialParameterCollectionInstance.h>
#include <Kismet/KismetRenderingLibrary.h>
#include <Engine/Canvas.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AIPerceptionComponent.h>
#include <Net/UnrealNetwork.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <TimerManager.h>

AEnemyController::AEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception component")));

	SightConfig->SightRadius = 700;
	SightConfig->LoseSightRadius = 800;
	SightConfig->PeripheralVisionAngleDegrees = 40;
	SightConfig->NearClippingRadius = 0;
	SightConfig->PointOfViewBackwardOffset = 0;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyController::OnEnemyPerceived);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		BehaviorTreeComp->StartTree(*BehaviorTree);
	}
}

// Called every frame
void AEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CreateCone();
	CreateCanvasTriangles();
	DrawTrianglesOnCanvas();
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledPawn = Cast<AEnemy>(GetPawn());
	if (Blackboard && BehaviorTree)
	{
		Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}

void AEnemyController::OnEnemyPerceived(const TArray<AActor*>& UpdatedActors)
{
	TArray<AActor*> OutActors;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(SightConfig->GetSenseImplementation(), OutActors);

	ACharacter* SpottedCharacter = nullptr;

	if (OutActors.Num() > 0)
		{
		GetWorldTimerManager().SetTimer(TimerHandle_VisionCheck, this, &AEnemyController::CheckEnemiesInVision, 0.2f, true);
		}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_VisionCheck);
		GetBlackboardComponent()->SetValueAsObject("Target", nullptr);
		ControlledPawn->CurrentTarget = nullptr;
	}
}

void AEnemyController::CreateCone()
{
	TraceResults.Empty();

	FHitResult OutResult, OutResultOffset;
	FVector TraceStart = GetPawn()->GetActorLocation();
	FVector TraceEnd;
	FCollisionQueryParams CollisionParameters;

	FVector	HeightOffsetVector = FVector(0, 0, ConeHeightOffset);
	//find obstacles
	for (int i = 0; i < NumTraces; ++i)
	{
		FRotator rot = FRotator(0.f, NumTraces * TraceDegrees / -2, 0.f);
		 TraceEnd = rot.RotateVector(GetPawn()->GetActorForwardVector());
		 TraceEnd = FRotator(0.f, TraceDegrees * i, 0.f).RotateVector(TraceEnd);
		 FVector RotatedVector = TraceEnd * TraceDistance;
		 TraceEnd = RotatedVector + TraceStart;
		
		GetWorld()->LineTraceSingleByChannel(OutResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParameters);

		if (OutResult.bBlockingHit)
		{
			//look up
			GetWorld()->LineTraceSingleByChannel(OutResultOffset, TraceStart, TraceEnd + HeightOffsetVector, ECollisionChannel::ECC_Visibility, CollisionParameters);
			if (OutResultOffset.bBlockingHit)
			{
				TraceResults.Add(OutResultOffset.Location);
			} 
			else
			{
				//find ledge
				for (int j = 1; j < 21; ++j)
				{
					FVector HeightOffsetEnd = ((TraceEnd + HeightOffsetVector) - OutResult.Location + FVector(0.f, 0.f, 100.f)) * (0.05 * j) + OutResult.Location + FVector(0.f, 0.f, 100.f);
					GetWorld()->LineTraceSingleByChannel(OutResultOffset, HeightOffsetEnd, HeightOffsetEnd + FVector(0.f, 0.f, -400.f), ECollisionChannel::ECC_Visibility, CollisionParameters);
					if (OutResultOffset.bBlockingHit)
					{
						if (j == 20)
						{
							TraceResults.Add(OutResult.TraceEnd);
						}
					} 
					else
					{
						TraceResults.Add(OutResultOffset.TraceEnd);
						break;
					}
				}
			}
		}
		else
		{
			TraceResults.Add(TraceEnd);
		}
	}
}

void AEnemyController::CreateCanvasTriangles()
{
	CanvasTriangles.Empty();

	for (int i = 0; i < TraceResults.Num() - 2; ++i)
	{
		FCanvasUVTri NewTriangle;

		FVector TrisVector1 = TraceResults[i] - GetPawn()->GetActorLocation() + FVector(HalfCanvasSize, HalfCanvasSize, 0);
		FVector TrisVector2 = TraceResults[i + 2] - GetPawn()->GetActorLocation() + FVector(HalfCanvasSize, HalfCanvasSize, 0);

		NewTriangle.V0_Pos = FVector2D(HalfCanvasSize, HalfCanvasSize);
		NewTriangle.V0_Color = FLinearColor::White;
		NewTriangle.V1_Pos = FVector2D(TrisVector1.X, TrisVector1.Y);
		NewTriangle.V1_Color = FLinearColor::White;
		NewTriangle.V2_Pos = FVector2D(TrisVector2.X, TrisVector2.Y);
		NewTriangle.V2_Color = FLinearColor::White;
		CanvasTriangles.Add(NewTriangle);
	}
}

void AEnemyController::DrawTrianglesOnCanvas()
{
	UCanvas* Canvas = nullptr;
	FVector2D SizeVector;
	FDrawToRenderTargetContext RenderContext;
	if (ConeRenderTarget)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), ConeRenderTarget, FLinearColor::Black);
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ConeRenderTarget, Canvas, SizeVector, RenderContext);
	}

	if (Canvas)
	{
		Canvas->K2_DrawTriangle(nullptr, CanvasTriangles);
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), RenderContext);
	}
}

void AEnemyController::CheckEnemiesInVision()
{
	TArray<AActor*> OutActors;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(SightConfig->GetSenseImplementation(), OutActors);

	for (AActor* Target : OutActors)
	{
		ACharacter* PerceivedTarget = Cast<ACharacter>(Target);
		if (PerceivedTarget && !PerceivedTarget->bIsCrouched)
		{
			ControlledPawn->CurrentTarget = PerceivedTarget;
			GetBlackboardComponent()->SetValueAsObject("Target", PerceivedTarget);
			return;
		}
		if (PerceivedTarget && PerceivedTarget->bIsCrouched && ControlledPawn->GetDistanceTo(PerceivedTarget) < 500)
		{
			ControlledPawn->CurrentTarget = PerceivedTarget;
			GetBlackboardComponent()->SetValueAsObject("Target", PerceivedTarget);
			return;
		}
	}

	GetBlackboardComponent()->SetValueAsObject("Target", nullptr);
	ControlledPawn->CurrentTarget = nullptr;
}
