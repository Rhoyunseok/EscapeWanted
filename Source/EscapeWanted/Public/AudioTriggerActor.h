#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Sound/SoundWaveProcedural.h"
#include "AudioTriggerActor.generated.h"

UCLASS()
class ESCAPEWANTED_API AAudioTriggerActor : public AActor
{
	GENERATED_BODY()
    
public:    
	AAudioTriggerActor();

protected:
	// .cpp에서 BeginPlay를 정의했다면 여기서 선언이 필요합니다.
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Audio Settings")
	int32 AudioIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Audio Settings")
	FString ServerIP = TEXT("172.16.15.134:8000");

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void RequestAudioFromServer(int32 Index);
	void OnAudioReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};