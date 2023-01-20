#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FPSCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config = Game)
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter();

	virtual void PostInitProperties() override;

	virtual float TakeDamage(float damage, FDamageEvent const& damageEvent, AController* eventInstigator,
	                         AActor* damageCauser) override;

	virtual void Restart() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	USkeletalMeshComponent* GetMesh1P() const
	{
		return _mesh;
	}

	UCameraComponent* GetFirstPersonCameraComponent() const
	{
		return _firstPersonCameraComponent;
	}

protected:
	virtual void BeginPlay();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* _lookAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	// APawn
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* _mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* _firstPersonCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* _weaponMesh;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* _defaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* _jumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* _moveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* _shootAction;

	UPROPERTY(EditDefaultsOnly, Category = "FPS")
	float _maxHealthPoints = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_HealthPoints)
	float _healthPoints;

	UPROPERTY(EditDefaultsOnly, Category = "FPS")
	float _damagePerHit = 30.0f;

private:
	UFUNCTION()
	void OnRep_HealthPoints();

	void Shoot();

	UFUNCTION(Server, Unreliable)
	void Server_Shoot();
};
