// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// AFPSCharacter

AFPSCharacter::AFPSCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	_firstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	_firstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	_firstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	_firstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	_mesh->SetOnlyOwnerSee(true);
	_mesh->SetupAttachment(_firstPersonCameraComponent);
	_mesh->bCastDynamicShadow = false;
	_mesh->CastShadow = false;
	//_mesh->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	_mesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	_weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	_weaponMesh->SetupAttachment(GetMesh1P());
}

void AFPSCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	_healthPoints = _maxHealthPoints;
}

float AFPSCharacter::TakeDamage(float damage, FDamageEvent const& damageEvent, AController* eventInstigator,
                                AActor* damageCauser)
{
	if (!HasAuthority())
	{
		return 0.0f;
	}

	const float damageToApply = FMath::Min(_healthPoints, damage);
	_healthPoints -= damageToApply;
	if (_healthPoints <= 0.0f)
	{
		Restart();
	}

	_healthPoints = FMath::Clamp(_healthPoints, 0.0f, _maxHealthPoints);

	return 0.0f;
}

void AFPSCharacter::Restart()
{
	Super::Restart();

	_healthPoints = _maxHealthPoints;

	TArray<AActor*> playerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStartActors);
	if (playerStartActors.Num() > 0)
	{
		const int32 index = FMath::RandRange(0, playerStartActors.Num() - 1);
		SetActorLocation(playerStartActors[index]->GetActorLocation());

		return;
	}

	SetActorLocation(FVector(0.0f, 0.0f, 200.0f));
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, _healthPoints);
}

void AFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(_defaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(_jumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(_jumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(_lookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);

		EnhancedInputComponent->BindAction(_shootAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Shoot);
	}
}


void AFPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSCharacter::OnRep_HealthPoints() { }

void AFPSCharacter::Shoot()
{
	if (!HasAuthority())
	{
		Server_Shoot();
		return;
	}

	FHitResult hit;
	const FVector start = _firstPersonCameraComponent->GetComponentLocation();
	const FVector end = start + _firstPersonCameraComponent->GetForwardVector() * 10000.0f;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params);

	if (hit.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(hit.GetActor(), _damagePerHit, GetController(), this, nullptr);
	}
}

void AFPSCharacter::Server_Shoot_Implementation()
{
	Shoot();
}

void AFPSCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFPSCharacter::GetHasRifle()
{
	return bHasRifle;
}
