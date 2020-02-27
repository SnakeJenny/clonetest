// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"

// ��ȡ�����Ϣ
#include "Components/BoxComponent.h"//    UBoxComponent ����
#include "DrawDebugHelpers.h"//    USphereComponent ����
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h" // ConstructorHelpers ����
#include "Engine/StaticMesh.h"	//staticmesh����
#include "Components/StaticMeshComponent.h"  //staticmeshcompoent����
#include "StaticMeshResources.h" //LOD resource����
#include <Engine/EngineTypes.h>
#include <Materials/MaterialInstanceDynamic.h> //Materials����
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
#include "Core/Public/HAL/RunnableThread.h"
// ��ȡUworld��Ϣ
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <Runtime/Engine/Public/EngineUtils.h>
#include <Runtime/Engine/Classes/Engine/StaticMeshActor.h>
#include "Kismet/GameplayStatics.h"

#include "Engine/LevelStreaming.h"




// Sets default values
AMyActor::AMyActor()
{
	// ��ò�Ҫ�ڹ�����ʵ�г�ʼ��������Ǳ�����ʼ����UE4�й����ִ��2��
 	// ����Ϊtrue����Actor�����֡���£�����Ϊfalse������΢�������ܡ�
	PrimaryActorTick.bCanEverTick = true;
	
	LevelStreamingActor = NewObject<ULevelStreamingLoad>();

	//GWorld ��ȡȫ��ָ�룬ͨ��������ʹ�ã�

	//PathOfMesh = FString(R"(D:\Github\master_szu\ue4_project\Design3D\3D_Models)");
	//PathOfTexture = FString(R"(D:\Github\master_szu\ue4_project\Design3D\3D_Models)");

	//manager.GetMaterial();
	//USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	//SphereComponent->SetupAttachment(RootComponent);

	//RootComponent = SphereComponent;
	////SphereComponent->InitSphereRadius(40.0f);
	////SphereComponent->SetCollisionProfileName(TEXT("ActorMeshLoader"));
	////SetRootComponent(RootComponent);

	//----------------------------------------------------------------------
	// ��ʼ��MyActor��components
	//----------------------------------------------------------------------
	// 1. ����buffer
	//buffer = new unsigned char[2048 * 2048 * 4];

	// 2. ��ʼ��staticmeshcomponent
	/* 
	1��CreateDefaultSubobject ue4�����������ͨ��������������ģ�
	2��CreateDefaultSubobject����д��Actor���޲ι��캯���У�����crash��
	3��CreateDefaultSubobject�е�TEXT����FName������ͬһ��Actor�в����ظ�������crash��
	*/
	MyStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyStaticComponent"));//���Ӿ�Ч��
	if (nullptr == MyStaticMeshComp)
		return;
	
	/*
	1����Ӹ�����RootComponent�����Ӿ�Ч����
	2�����staticmeshcomponent����rootcomponent�������AttachToComponent�󶨵�ĳ��sceneComponent֮�£�
	*/
	//RootComponent = MyStaticMeshComp;

	/*
	1��FObjectFinder��ConstructorHelpers���ڲ��࣬���Ǿ�̬��������ķ�ʽ
	2����������ù���ue4��ֵ���õõ���ַ����������·��
	*/
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MyStaticMeshCompAssetWzh(TEXT("/Game/StarterContent/Props/sz3D12000.sz3D12000"));//���زĿ�
	if (MyStaticMeshCompAssetWzh.Succeeded())//�ж��زĿ��Ƿ��
	{
		MyStaticMeshComp->SetStaticMesh(MyStaticMeshCompAssetWzh.Object);//�����ز� �������Ӿ�Ч��
		MyStaticMeshComp->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(0, 30, 0), FVector(0.8f)));//��ʼ��λ�á���ת�ͷŴ���   
		//MyStaticMeshComp->SetRelativeLocation(FVector(0.0f, 30.0f, 0.0f));//����λ��0,0,0Ϊ��ʼλ��    ��ʼλ��Ϊ��ֹλ�� z�����ƽ��һ�� �ڸ����û�и�����״ʱ                                                                                                                      
		//MyStaticMeshComp->SetWorldScale3D(FVector(0.8f));//SetWorldScale3D �������������ռ��еı任������ ������ԭ��ģ�ʹ�СΪ��׼
	}

	// 3. ��ʼ��Boxcomponent
	MyBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("MyBox"));
	MyBoxComp->InitBoxExtent(FVector(100, 100, 100));
	MyBoxComp->SetCollisionProfileName("Trigger");
	//MyBoxComp->SetupAttachment(RootComponent);

	// 4. ��ʼ��Materialcomponent��ע��UMaterialInterface��UMaterial�Ĳ���
	//MyAcotrptr->Materials
	//OnMaterial = CreateDefaultSubobject<UMaterial>(TEXT("OnMaterialComponent"));
	//OffMaterial = CreateDefaultSubobject<UMaterial>(TEXT("OffMaterialComponent"));
	RMCMaterial = CreateDefaultSubobject<UMaterial>(TEXT("RMCMaterialComponent"));

	
	// ��̬���ط�ʽ������ʼ��
	//static ConstructorHelpers::FObjectFinder<UMaterial>MyMaterialOn(TEXT("Material'/Game/StarterContent/Props/Materials/OnMaterial.OnMaterial'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial>MyMaterialOff(TEXT("Material'/Game/StarterContent/Props/Materials/OffMaterial.OffMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterial>MyRMCMaterial(TEXT("Material'/Game/StarterContent/Props/Materials/OffMaterial.OffMaterial'"));
	if (MyRMCMaterial.Succeeded()) {
		//OnMaterial = MyMaterialOn.Object;
		//OffMaterial = MyMaterialOff.Object;
		RMCMaterial = MyRMCMaterial.Object;
	}
	MyBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AMyActor::OnOverlapBegin);
	//GetVerticesofMesh(MyStaticMeshComp);

	// 5.����RMC
	RMComp = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMComp"));
	//RMComp->SetupAttachment(RootComponent);

	//RootComponent = RMComp;
	//UMyStaticMeshComp->DynamicDrawMesh(MyStaticMeshComp, RMComp);

}

void AMyActor::PostActorCreated()
{
	Super::PostActorCreated();
	UE_LOG(LogTemp, Error, TEXT("ATestActor::PostActorCreated"));
}

void AMyActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UE_LOG(LogTemp, Error, TEXT("ATestActor::OnConstruction"));

}

void AMyActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("ATestActor::PreInitializeComponents"));
}

void AMyActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("ATestActor::PostInitializeComponents"));
}

void AMyActor::LoadActor()
{


	
	static int k = 0;
	if (k == 0)
	{

		ActorArray.Empty();
		ActorinFiles.Empty();

		FRotator Rotation = FRotator(0, 0, 0);
		// Translation = {0,0,0,0)
		FVector Translation = FVector(0, 0, 0);
		// Scale3D = {1,1,1,0);
		FVector Scale3D = FVector(1, 1, 1);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = Instigator;

		for (int layer = 0; layer < 3; ++layer)
		{
			for (int col = 0; col < pow(2, layer + 1); ++col)
			{
				for (int row = 0; row < pow(2, layer); ++row)
				{
					FString longPackageName = "/Game/" + FString::FromInt(layer) + "/" + FString::FromInt(col) + "/u" + FString::FromInt(row);
					Tile tile;
					tile.layer = layer;
					tile.row = row;
					tile.col = col;
					if (!tile.IsLevelFileExist())
						continue;
					obj = (UWorld*)StaticLoadObject(UWorld::StaticClass(), NULL, *longPackageName);
					if (obj)
					{
						Level = obj->GetCurrentLevel();
						for (int num = 0; num < Level->Actors.Num(); ++num)
						{
							ActorinFiles.Add(Level->Actors[num]);
							if (Level->Actors[num]->GetActorLabel() == "Landscape")
							{
								//AActor* actorLoad = Level->Actors[num];
								//SpawnParams.Template = actorLoad;
								//AActor *actor = obj->SpawnActor<AActor>(actorLoad->GetClass(), Translation, Rotation, SpawnParams);
								//
								ActorArray.Add(Level->Actors[num]);
							}

						}

					}
				}
			}
		}


		ActorForLoad.Empty();
		World = GetWorld();

		for (int num = 0; num < ActorArray.Num(); ++num)
		{
			SpawnParams.Template = ActorArray[num];
			actor = World->SpawnActor<AActor>(ActorArray[num]->GetClass(), Translation, Rotation, SpawnParams);
			actor->AddToRoot();
			ActorForLoad.Add(actor);
		}
		//for (int num = 0; num < ActorinFiles.Num(); ++num)
		//{
		//	if (!ActorinFiles[num]->IsPendingKillPending())
		//		ActorinFiles[num]->MarkPendingKill();
		//}
		k++;

		World->AddToRoot();
		obj->AddToRoot();
		/*if (!World->bIsWorldInitialized)
		{
			World->InitWorld();
		}
		FCoreUObjectDelegates::PostLoadMapWithWorld.Broadcast(World);
		GWorld = World;*/
		//Super::PostRegisterAllComponents();
	}
	

	//UWorld* obj = (UWorld*)StaticLoadObject(UWorld::StaticClass(), NULL, *RootPath);
	//static int k = 0;
	//UWorld* World = GetWorld();
	//ULevel* Level = obj->GetCurrentLevel();
	//if (k == 0)
	//{
	//	for (int i = 0; i < Level->Actors.Num(); ++i)
	//	{
	//		// Find our player index
	//		int32 IterIndex = 0;
	//		int32 PlayerIndex = INDEX_NONE;
	//		bool bIsPaused = GetWorld()->IsPaused();
	//		if (Level->Actors[i]->GetActorLabel() == "Landscape")
	//		{

	//			Acotrptr = Level->Actors[i];



	//			//for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator, ++IterIndex)
	//			//{
	//			//	APlayerController* PlayerController = Iterator->Get();

	//			//	if (!bIsPaused || PlayerController->ShouldPerformFullTickWhenPaused())
	//			//	{
	//			//		levelLoadactor = World->SpawnActor<ALevelLoadCharacter>(ALevelLoadCharacter::StaticClass(), Translation, Rotation);
	//			//		PlayerController->Possess(levelLoadactor);
	//			//		break;
	//			//	}
	//			//}
	//		
	//			AActor* alandscape = World->SpawnActor<AActor>(Acotrptr->GetClass(), Translation, Rotation, SpawnParams);
	//			Level->Actors[i]->MarkPendingKill();
	//			k++;

	//			//return;
	//			/*
	//			levelLoadactor = World->SpawnActor<ALevelLoadCharacter>(ALevelLoadCharacter::StaticClass(), Translation, Rotation, SpawnParams);

	//			const FName actorComponentName = FName("ActorComponent");
	//			const FName loadComponentName = FName("loadComponent");
	//			UMyStaticMeshComponent* loadComponent = NewObject<UMyStaticMeshComponent>(levelLoadactor, actorComponentName);
	//			loadComponent->RegisterComponent();
	//			levelLoadactor->AddOwnedComponent(loadComponent);
	//			UMyStaticMeshComponent* actorComponent = NewObject<UMyStaticMeshComponent>(Acotrptr, actorComponentName);
	//			actorComponent->RegisterComponent();
	//			Acotrptr->AddOwnedComponent(actorComponent);
	//			*/
	//			//ParentIComponent = levelLoadactor->GetDefaultAttachComponent();
	//			//RootComponent = ParentIComponent;
	//			//ChildIComponent = Acotrptr->GetDefaultAttachComponent();
	//			//ChildIComponent->SetupAttachment(RootComponent);
	//			RootComponent->UpdateComponentToWorld(EUpdateTransformFlags::SkipPhysicsUpdate, ETeleportType::None);

	//			Acotrptr->bExchangedRoles = true;
	//			Acotrptr->bNetLoadOnClient = true;
	//			Acotrptr->bRelevantForLevelBounds = true;
	//			Acotrptr->bRelevantForNetworkReplays = true;
	//			Acotrptr->bCanBeDamaged = true;
	//			Acotrptr->bFindCameraComponentWhenViewTarget = true;
	//			Acotrptr->bEnableAutoLODGeneration = true;
	//			World->GetCurrentLevel()->Actors.Add(Acotrptr);
	//			//Acotrptr;
	//			RootComponent->UpdateComponentToWorld(EUpdateTransformFlags::SkipPhysicsUpdate, ETeleportType::None);

	//			RegisterAllComponents();
	//			RegisterAllActorTickFunctions(true, true); // register all tick functions
	//		}

	//		World->AddToRoot();
	//		if (!World->bIsWorldInitialized)
	//		{
	//			World->InitWorld();
	//		}
	//		FCoreUObjectDelegates::PostLoadMapWithWorld.Broadcast(World);
	//		GWorld = World;
	//		Super::PostRegisterAllComponents();
	//	}
	//}
}
// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	//ULevelStreamingLoad LevelStreaming = ULevelStreamingLoad();
//	LevelStreaming.LoadMap("StarterMap");
//	LevelStreaming.LoadMap("Advanced_Lighting");


	RegisterAllComponents();
	//DrawDebugBox(GetWorld(), GetActorLocation(), FVector(100, 100, 100), FColor::White, true, -1, 0, 10);

	//MyStaticMeshComp->SetMaterial(0, OffMaterial); ��̬���ظ�Ϊ��̬����
	//MyBoxComp->OnComponentEndOverlap.AddDynamic(this, &AMyActor::OnOverlapEnd);

	//UMyStaticMeshComp->SetRMCInfo(RootComponent, MyStaticMeshComp, RMComp, RMCMaterial);

	//UMyStaticMeshComponent* UMyStaticMeshComp = NewObject<UMyStaticMeshComponent>();
	//UMyStaticMeshComp->DynamicDrawMesh(MyStaticMeshComp, RMComp);

	// û����Ч��why��
	//FTimerHandle Timer;
	//GetWorldTimerManager().SetTimer(Timer, this, &AMyActor::DestroyActorFunction, 10);
}

// ����actor
void AMyActor::DestroyActorFunction()
{

	if (MyActor != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Actor Destroy"));
	}
}


int second = 0;
// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LoadActor();

	UWorld* InWorld;
	InWorld = GWorld;
	//AActor* actor = GetMyActor();
	second++;
	if (second == 1000) {
		mUMyStaticMeshComp = NewObject<UMyStaticMeshComponent>();
		mUMyStaticMeshComp->SetRMCInfo(RootComponent, MyStaticMeshComp, RMComp, RMCMaterial);
		//UMyStaticMeshComp->DynamicDrawMesh(MyStaticMeshComp, RMComp);
		//second = 0;
	}
}

// �����޸�mesh��material��
void AMyActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ��ĳ�������Box���ʱ����

	// �����µĲ��ʲ��趨Ϊ������׸�����
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		// new UMaterialInstanceDynamic();
		if (UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(OnMaterial, this))
		{
			// ParameterFName, ParameterValue;(color.m_fR, color.m_fG, color.m_fB, color.m_fA);
			MatInstance->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.0f, 1.0f, 0.0f, 0.0f));
			// 0����get��MaterialInstance��index����Ϊ����Mesh�ж��Material������
			MyStaticMeshComp->SetMaterial(0, MatInstance);
		}
	}
}

void AMyActor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp,	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex)
{
	// ��ĳ�����뿪Box���ʱ����

	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// new UMaterialInstanceDynamic();
		if (UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(OffMaterial, this))
		{
			// ParameterFName, ParameterValue;(color.m_fR, color.m_fG, color.m_fB, color.m_fA);
			MatInstance->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.0f, 0.0f, 0.0f));
			// 0����get��MaterialInstance��index����Ϊ����Mesh�ж��Material������
			MyStaticMeshComp->SetMaterial(0, MatInstance);

		}
	}
}

AActor* AMyActor::GetMyActor()
{
	//----------------------------------------------------------------------
	// �����Զ�����AMyActor���Ѿ������Actors
	//----------------------------------------------------------------------
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		AMyActor* Acotrptr = Cast<AMyActor>(Actor);
		if (Acotrptr)
		{
			// ���ձ༭������������ͼ�е�tag[MyActor]��ȡactor����
			if (Acotrptr->GetName().Contains("MyActor", ESearchCase::CaseSensitive))
			{
				// �ڱ༭�������Ͻ���ʾ��target is found��
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("target is found"));
				return Acotrptr;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("not found"));
			}
		}
	}

	return nullptr;
}

TArray<FVector> AMyActor::GetVerticesofMesh(UStaticMeshComponent* MyStaticMeshComponent)
{
	//----------------------------------------------------------------------
	// ��ȡstaticmesh�Ķ�������
	//----------------------------------------------------------------------
	TArray<FVector> vertices = TArray<FVector>();
	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MyStaticMeshComponent);
	if (!StaticMeshComponent)
		return vertices;
	AActor* Actor = GetMyActor();

	if (Actor != nullptr)
	{
		UWorld* World = Actor->GetWorld();
		// GetFeatureLevel���������㼶SM5��������FSceneView��FMaterial��FSkeletalMeshObject��
		ERHIFeatureLevel::Type SceneFeatureLevel = World->Scene->GetFeatureLevel();
		//const FMaterial* Material = MaterialRenderProxy->GetMaterial(View.GetFeatureLevel());   const FViewInfo& View,
		if (!SceneFeatureLevel)
			return vertices;
	}
	//const auto FeatureLevel = MyStaticMeshComponent->GetWorld()->FeatureLevel;


	// RenderData�������Բ�����GetStaticMeshȥ��UStaticMesh�Ľṹ����thisָ��鿴����
	UStaticMesh* MyStaticMesh = StaticMeshComponent->GetStaticMesh();
	if (!MyStaticMesh)
		return vertices;

	//StaticMeshRender.cpp 111�У��鿴Դ���룬�°汾������Get��ȡ���ݣ�ע����web�ο����ϵĲ��
	FStaticMeshRenderData* RenderData = MyStaticMesh->RenderData.Get();
	if (!RenderData)
		return vertices;

	// StaticMesh.cpp 968��
	int32 LODIndex = 0;
	FStaticMeshLODResources& LODModel = RenderData->LODResources[LODIndex];

	// StaticMesh.h 221-250��
	// ����buffer�����ݺ�λ��
	FStaticMeshVertexBuffers& VertexBuffer = LODModel.VertexBuffers;
	FStaticMeshVertexBuffer& StaticMeshVertexBuffer = VertexBuffer.StaticMeshVertexBuffer;
	FPositionVertexBuffer& PositionVertexBuffer = VertexBuffer.PositionVertexBuffer;

	if (&VertexBuffer)
	{
		const int32 VertexCount = LODModel.GetNumVertices();
		for (int32 Index = 0; Index < VertexCount; Index++)
		{
			//This is in the Static Mesh Actor Class, so it is location and tranform of the SMActor
			/*��ο�ʾ����ʽ��
			TotalScale3D.X = FVector(LocalToWorld.TransformVector(FVector(1, 0, 0))).Size();
			TotalScale3D.Y = FVector(LocalToWorld.TransformVector(FVector(0, 1, 0))).Size();
			TotalScale3D.Z = FVector(LocalToWorld.TransformVector(FVector(0, 0, 1))).Size();
			*/
			// PositionVertexBuffer.h 73��
			const FVector WorldSpaceVertexLocation = GetActorLocation() + GetTransform().TransformVector(PositionVertexBuffer.VertexPosition(Index));
			//add to output FVector array
			vertices.Add(WorldSpaceVertexLocation);
		}
	}

	return vertices;
}

bool AMyActor::ShouldTickIfViewportsOnly() const
{
	return true;
}