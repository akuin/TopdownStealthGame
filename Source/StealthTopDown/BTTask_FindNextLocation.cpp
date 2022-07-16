// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindNextLocation.h"
#include <NavigationSystem.h>
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindNextLocation::UBTTask_FindNextLocation()
{
	NodeName = TEXT("Find next location");
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindNextLocation, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindNextLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation Location;

	AEnemyController* AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	const APawn* ControlledPawn = AIController->GetPawn();

	const FVector Origin = ControlledPawn->GetActorLocation();

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(Origin, 1000.f, Location))
	{
		AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName,
			Location.Location);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}

FString UBTTask_FindNextLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}