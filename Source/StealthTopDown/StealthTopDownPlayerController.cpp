// Copyright Epic Games, Inc. All Rights Reserved.

#include "StealthTopDownPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "StealthTopDownCharacter.h"
#include "Engine/World.h"

AStealthTopDownPlayerController::AStealthTopDownPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AStealthTopDownPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		//MoveToMouseCursor();
	}
}

void AStealthTopDownPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AStealthTopDownPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AStealthTopDownPlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &AStealthTopDownPlayerController::OnCrouchPressed);

}

void AStealthTopDownPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AStealthTopDownPlayerController::MoveToMouseCursor()
{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
}

void AStealthTopDownPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AStealthTopDownPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Hit.ImpactPoint);
		// We hit something, move there
		if (!HasAuthority())
		{
			ServerMovetoLocation(Hit.ImpactPoint);
		}
	}
}

void AStealthTopDownPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void AStealthTopDownPlayerController::OnCrouchPressed()
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (ControlledCharacter)
	{
		if (ControlledCharacter->bIsCrouched)
			ControlledCharacter->UnCrouch();
		else
			ControlledCharacter->Crouch();
	}
}

void AStealthTopDownPlayerController::ServerMovetoLocation_Implementation(FVector NewLocation)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NewLocation);
}

bool AStealthTopDownPlayerController::ServerMovetoLocation_Validate(FVector NewLocation)
{
	return true;
}