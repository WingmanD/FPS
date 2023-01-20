#include "TP_WeaponComponent.h"
#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

UTP_WeaponComponent::UTP_WeaponComponent()
{
	_muzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UTP_WeaponComponent::Fire()
{
	if (character == nullptr || character->GetController() == nullptr)
	{
		return;
	}

	if (_projectileClass != nullptr)
	{
		UWorld* const world = GetWorld();
		if (world != nullptr)
		{
			const APlayerController* playerController = Cast<APlayerController>(character->GetController());
			const FRotator spawnRotation = playerController->PlayerCameraManager->GetCameraRotation();
			
			const FVector spawnLocation = GetOwner()->GetActorLocation() + spawnRotation.RotateVector(_muzzleOffset);
	
			FActorSpawnParameters actorSpawnParams;
			actorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			world->SpawnActor<AFPSProjectile>(_projectileClass, spawnLocation, spawnRotation, actorSpawnParams);
		}
	}
	
	if (_fireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _fireSound, character->GetActorLocation());
	}
	
	if (_fireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(_fireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AFPSCharacter* TargetCharacter)
{
	character = TargetCharacter;
	if (character == nullptr)
	{
		return;
	}

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	character->SetHasRifle(true);

	if (const APlayerController* playerController = Cast<APlayerController>(character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (character == nullptr)
	{
		return;
	}

	if (const APlayerController* playerController = Cast<APlayerController>(character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}