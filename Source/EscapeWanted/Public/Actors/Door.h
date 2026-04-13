
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class ESCAPEWANTED_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* TrimMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOpening = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetRotation = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FinalTargetRot = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OpenSpeed = 2.f;
	
};
