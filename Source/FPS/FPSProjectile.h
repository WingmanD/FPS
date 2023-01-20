#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config = Game)
class AFPSProjectile : public AActor
{
	GENERATED_BODY()

public:
	AFPSProjectile();

	USphereComponent* GetCollisionComp() const
	{
		return _sphereCollision;
	}

	UProjectileMovementComponent* GetProjectileMovement() const
	{
		return projectileMovement;
	}

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* _sphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* projectileMovement;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse,
	           const FHitResult& hit);
};
