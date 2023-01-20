#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AFPSProjectile::AFPSProjectile()
{
	_sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	_sphereCollision->InitSphereRadius(5.0f);
	_sphereCollision->BodyInstance.SetCollisionProfileName("Projectile");
	_sphereCollision->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);

	_sphereCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	_sphereCollision->CanCharacterStepUpOn = ECB_No;
	RootComponent = _sphereCollision;

	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	projectileMovement->UpdatedComponent = _sphereCollision;
	projectileMovement->InitialSpeed = 3000.f;
	projectileMovement->MaxSpeed = 3000.f;
	projectileMovement->bRotationFollowsVelocity = true;
	projectileMovement->bShouldBounce = true;

	InitialLifeSpan = 3.0f;
}

void AFPSProjectile::OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp,
                           FVector normalImpulse, const FHitResult& hit)
{
	if ((otherActor != nullptr) && (otherActor != this) && (otherComp != nullptr) && otherComp->IsSimulatingPhysics())
	{
		otherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}
