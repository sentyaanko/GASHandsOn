// (C) Sentya Anko 2021


#include "Projectiles/GHOProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGHOProjectile::AGHOProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

//// Called when the game starts or when spawned
//void AGHOProjectile::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void AGHOProjectile::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
