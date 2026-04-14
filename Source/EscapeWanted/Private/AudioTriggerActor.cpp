#include "AudioTriggerActor.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

class USoundWaveProceduralEx : public USoundWaveProcedural
{
public:
    void SetAudioParams(int32 InSampleRate, int32 InNumChannels)
    {
        // 상속받은 자식 클래스에서는 부모의 protected 변수에 직접 접근이 가능합니다.
        this->SampleRate = InSampleRate;
        this->NumChannels = InNumChannels;
    }
};

// WAV 표준 44바이트 헤더 구조체
#pragma pack(push, 1)
struct FWavHeader {
    uint8  RIFF[4]; uint32 ChunkSize; uint8  WAVE[4]; uint8  fmt[4];
    uint32 SubChunk1Size; uint16 AudioFormat; uint16 NumChannels;
    uint32 SampleRate; uint32 ByteRate; uint16 BlockAlign;
    uint16 BitsPerSample; uint8  Data[4]; uint32 SubChunk2Size;
};
#pragma pack(pop)

AAudioTriggerActor::AAudioTriggerActor()
{
    PrimaryActorTick.bCanEverTick = false;
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAudioTriggerActor::OnOverlapBegin);
}

void AAudioTriggerActor::BeginPlay()
{
    Super::BeginPlay();
}

void AAudioTriggerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        RequestAudioFromServer(AudioIndex);
        TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 중복 실행 방지
    }
}

void AAudioTriggerActor::RequestAudioFromServer(int32 Index)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(FString::Printf(TEXT("http://%s/audio/%d"), *ServerIP, Index));
    Request->SetVerb("GET");
    Request->OnProcessRequestComplete().BindUObject(this, &AAudioTriggerActor::OnAudioReceived);
    Request->ProcessRequest();
}

void AAudioTriggerActor::OnAudioReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        TArray<uint8> RawData = Response->GetContent();
        if (RawData.Num() < 44) return;

        // 1. WAV 헤더 분석
        FWavHeader* Header = (FWavHeader*)RawData.GetData();

        // 2. 사운드 객체 생성
        USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>(this);
        
        // --- [수정된 부분] ---
        // 일반 사운드 웨이브를 방금 만든 '우회 클래스'로 잠시 형변환하여 
        // 꽁꽁 숨겨진 변수들을 강제로 세팅합니다.
        USoundWaveProceduralEx* SoundWaveEx = static_cast<USoundWaveProceduralEx*>(SoundWave);
        if (SoundWaveEx)
        {
            SoundWaveEx->SetAudioParams(Header->SampleRate, Header->NumChannels);
        }
        // ---------------------

        int32 PCMDataSize = RawData.Num() - 44;
        SoundWave->Duration = (float)PCMDataSize / (Header->SampleRate * Header->NumChannels * (Header->BitsPerSample / 8));

        // 3. 버퍼에 데이터 밀어넣기
        SoundWave->QueueAudio(RawData.GetData() + 44, PCMDataSize);

        // 4. 재생
        UGameplayStatics::PlaySound2D(this, SoundWave);

        UE_LOG(LogTemp, Warning, TEXT("플러그인 없이 실시간 재생 성공! (샘플레이트: %d)"), Header->SampleRate);
    }
}