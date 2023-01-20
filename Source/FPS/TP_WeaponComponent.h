#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AFPSProjectile;
class AFPSCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<AFPSProjectile> _projectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* _fireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* _fireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector _muzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;
	
	UTP_WeaponComponent();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AFPSCharacter* TargetCharacter);
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

protected:
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	AFPSCharacter* character;
};
