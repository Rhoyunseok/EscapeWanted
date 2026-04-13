

#include "Public/Actors/Door.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("CollisionComp"));
	RootComponent = BoxComp;
	BoxComp->SetCollisionProfileName(TEXT("Trigger")); // Change to Trigger to allow overlap
	BoxComp->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	
	TrimMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("TrimMesh"));
	TrimMesh->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempTrimMesh(TEXT("/Game/NZW/00_Maps/_GENERATED/user/SM_DoorTrim.SM_DoorTrim"));       
	if (TempTrimMesh.Succeeded()) TrimMesh->SetStaticMesh(TempTrimMesh.Object);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorMesh"));
	DoorMesh->SetupAttachment(TrimMesh); // Usually Door rotates relative to Trim 
	ConstructorHelpers::FObjectFinder<UStaticMesh>TempDoorMesh(TEXT("/Game/HospitalCorridor/Meshes/Objects/SM_HospitalDoor_01.SM_HospitalDoor_01")); 
	if (TempDoorMesh.Succeeded()) DoorMesh->SetStaticMesh(TempDoorMesh.Object);   
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpening)
	{
		FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
		FRotator TargetRot = FRotator(0.f, FinalTargetRot, 0.f); 
       
		if (!CurrentRotation.Equals(TargetRot, 0.1f))
		{
			FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRot, DeltaTime, OpenSpeed);
			DoorMesh->SetRelativeRotation(NewRotation);
		}
		else
		{
			DoorMesh->SetRelativeRotation(TargetRot);
			bIsOpening = false;
		}
	}
}

void ADoor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		if (bCanOpen)
		{
			FinalTargetRot = TargetRotation; // 설정해둔 열림 각도로 목표 설정!
			bIsOpening = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("문안열리는 소리"));
		}
	}
}

void ADoor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{		
		if (bCanOpen)
		{
			FinalTargetRot = 90.f;
			bIsOpening = true; 
			UE_LOG(LogTemp, Log, TEXT("자동문 클로즈!"));
		}
	}
}
