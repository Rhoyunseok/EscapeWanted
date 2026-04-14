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
	virtual void BeginPlay() override;

protected:
	// 기존 변수들
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Audio Settings")
	int32 AudioIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Audio Settings")
	FString ServerIP = TEXT("172.16.15.134:8000");

	// --- [추가된 변수] ---
	// 0.0이면 무음, 1.0이 기본값, 그 이상은 소리를 증폭시킵니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float VolumeMultiplier = 1.0f;
	// ---------------------

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void RequestAudioFromServer(int32 Index);
	void OnAudioReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};