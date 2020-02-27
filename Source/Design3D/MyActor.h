// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
// ��ȡ��������
#include "MyStaticMeshComponent.h"
#include "Manager.h"
#include "GameFramework/Actor.h"
#include "LevelStreamingLoad.h"
#include "LevelLoadCharacter.h"

#include "MyActor.generated.h"





UCLASS()
class DESIGN3D_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();


protected:
	// Called when the game starts or when spawned
	// �麯�������ã�ʵ�ֶԸ���ͬ�������ຯ�����ã�ͬʱʵ�ֶ�̬��
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ʹ���������� UProperty������ʾһ������
	// �߱������������Եñ��UPROPERTY(����UObject��һ�������Ƿ��ܻس�)
	UPROPERTY(EditAnywhere)
	ULevelStreamingLoad* LevelStreamingActor;

	UPROPERTY(EditAnywhere)
	class AMyactor* MyActor;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MyStaticMeshComp;

	UPROPERTY(EditAnywhere)
	class UMaterial* OnMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterial* OffMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterial* RMCMaterial;


	UPROPERTY(EditAnywhere)
	class UBoxComponent* MyBoxComp;

	UPROPERTY(EditAnywhere)
		USceneComponent* DefaultRootComponent;

	UPROPERTY(EditAnywhere)
	URuntimeMeshComponent* RMComp;

	UPROPERTY(EditAnywhere)
	UMyStaticMeshComponent* mUMyStaticMeshComp;
	// ������Щcomponent�������࣬���ܷ��ڹ��캯�����Ա������ȥ���壿����
	//UPROPERTY()
	//class UMyStaticMeshComponent* UMyStaticMeshComp;

	UPROPERTY(EditAnywhere)
	TArray<UMaterialInterface*> OutMaterials;
	
	UPROPERTY(EditAnywhere)
	TSet<FGuid> DependentResources;

	unsigned char *buffer;
	UTexture2D *m_texture;
	UMaterialInterface *ScreenMaterial;
	UMaterialInstanceDynamic *DynamicMaterial;

	Manager manager;
	FString PathOfMesh;
	FString PathOfTexture;

	// ���еĺ���������ָ��ΪUFUNCTION��UFUNCTIONʹ�øú������ܹ���C++��ʹ�ã����ܹ�����ͼ��ʹ��
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, 
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, 
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

	// ���ٶ���
	UFUNCTION()
	void DestroyActorFunction();

	// ��ȡActor����
	UFUNCTION()
	AActor* GetMyActor();

	// ��ȡ��������
	UFUNCTION()
	TArray<FVector> GetVerticesofMesh(UStaticMeshComponent* MyStaticMeshComponent);
	
	//UFUNCTION()
	//void GetMyMaterials(UStaticMeshComponent* MyStaticMeshComponent, TArray<UMaterialInterface*>& OutMaterials);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiplePass")
		UMaterialInterface* SecondPassMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiplePass")
		bool NeedSecondPass = false;

	// ���ظú�����ʹtick�����ڱ������п��Ա�����
	UFUNCTION()
	virtual	bool ShouldTickIfViewportsOnly() const override;



	UFUNCTION()
	void LoadActor();

	UPROPERTY(EditAnywhere, Category = "ActorForLoad")
	TArray<AActor*> ActorForLoad;
	UPROPERTY(EditAnywhere, Category = "ActorArray")
	TArray<AActor*> ActorArray;
	UPROPERTY(EditAnywhere, Category = "ActorinFiles")
	TArray<AActor*> ActorinFiles;
	UPROPERTY(EditAnywhere, Category = "actor")
	AActor *actor;
	UPROPERTY(EditAnywhere, Category = "obj")
	UWorld* obj;
	UPROPERTY(EditAnywhere, Category = "World")
	UWorld* World;
	UPROPERTY(EditAnywhere, Category = "Level")
	ULevel* Level;

	UPROPERTY()
		AActor* Acotrptr;
	UPROPERTY(EditAnywhere, Category = "ParentComponent")
		USceneComponent* ParentIComponent;
	UPROPERTY(EditAnywhere, Category = "ChildComponent")
		USceneComponent* ChildIComponent;
	UPROPERTY(EditAnywhere, Category = "levelLoadCharacter")
		TSubclassOf<class ALevelLoadCharacter> levelLoadCharacter;
	UPROPERTY(EditAnywhere, Category = "levelLoadactor")
		ALevelLoadCharacter* levelLoadactor;

protected:
	virtual void PostActorCreated() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

};
