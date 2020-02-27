// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyStaticMeshComponent.generated.h"
/**
 * 
 */


UENUM()
enum class EPathType : uint8
{
	Absolute,
	Relative
};

USTRUCT()
struct FMeshInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FVector> Vertices;

	UPROPERTY()
		TArray<int32> Triangles;

	UPROPERTY()
		TArray<FVector> Normals;

	UPROPERTY()
		TArray<FVector2D> UV0;

	// procedural meshʹ��FLinearColor
	UPROPERTY()
		TArray<FLinearColor> VertexColors;

	// RMCʹ��FColor
	UPROPERTY()
		TArray<FColor> VertexFColors;

	UPROPERTY()
		TArray<FRuntimeMeshTangent> Tangents;

	UPROPERTY()
		FTransform RelativeTransform;
};

USTRUCT()
struct FReturnedData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		bool bSuccess;

	UPROPERTY()
		int32 NumMeshes;

	UPROPERTY()
		TArray<FMeshInfo> meshInfo;

	UPROPERTY()
		int32 Id;

	bool bTextureSuccess;

	TArray<uint8> PixelData;

	int32 InSizeX;

	int32 InSizeY;

	EPixelFormat InFormat;
};


UCLASS()
class DESIGN3D_API UMyStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	// ʹ��Ĭ�Ϲ��캯������������
	UMyStaticMeshComponent(const FObjectInitializer& ObjectInitializer);
	UMyStaticMeshComponent();
	~UMyStaticMeshComponent();

public:

	//~ Begin UPrimitiveComponent Interface
	// ԭ�����ļ��У��ú�������д������麯��
	UFUNCTION()
	void GetMyMaterials(UStaticMeshComponent* MyStaticMeshComponent, TArray<UMaterialInterface*>& OutMaterials);
	
	// GetUsedMaterials��BuildTextureStreamingData����
	UFUNCTION()
	void BuildMyTextureStreamingData(UWorld* InWorld, UStaticMeshComponent* MyStaticMeshComponent);

	static bool EditorBuildTextureStreaming(UWorld* InWorld, EViewModeIndex SelectedViewMode = VMI_Unknown);

	// �Ӵ����ж�ȡͼƬ��������
	UFUNCTION()
	static void LoadTextureFromImage(const FString& ImagePath, FReturnedData& result);
	
	// ����IImageWrapperModule
	static IImageWrapperModule& ImageWrapperModule;

	// ʹ��assimp��ȡģ������
	UFUNCTION()
	static FReturnedData LoadMeshFromFile(FString filepath, EPathType type = EPathType::Absolute);

	// ��ȡRMC�������Ϣ
	UFUNCTION()
	bool SetRMCInfo(USceneComponent* RootComponent, UStaticMeshComponent* MyStaticMeshComponent, URuntimeMeshComponent* RMComponent, UMaterial* RMCMaterial);

	// ��̬����mesh
	bool DynamicDrawMesh(UStaticMeshComponent* MyStaticMeshComponent, URuntimeMeshComponent* RMComponent);

	// �Ӵ����ļ��л�ȡ����
	UTexture2D* GetTexture2DFromDiskFile(const FString& FilePath, int meshid);

	template <typename ParameterType, typename ExpressionType>
	//bool UpdateParameterSet(TArray<ParameterType>& Parameters, UMaterial* ParentMaterial);
	bool UpdateParameterSet(TArray<ParameterType>& Parameters, UMaterial* ParentMaterial)
	{
		bool bChanged = false;

		// Loop through all of the parameters and try to either establish a reference to the 
		// expression the parameter represents, or check to see if the parameter's name has changed.
		for (int32 ParameterIdx = 0; ParameterIdx < Parameters.Num(); ParameterIdx++)
		{
			bool bTryToFindByName = true;

			ParameterType& Parameter = Parameters[ParameterIdx];

			if (Parameter.ExpressionGUID.IsValid())
			{
				ExpressionType* Expression = nullptr;
				FindClosestExpressionByGUIDRecursive<ExpressionType>(Parameter.ParameterInfo.Name, Parameter.ExpressionGUID, ParentMaterial->Expressions, Expression);

				// Check to see if the parameter name was changed.
				if (Expression)
				{
					bTryToFindByName = false;

					if (Parameter.ParameterInfo.Name != Expression->ParameterName)
					{
						Parameter.ParameterInfo.Name = Expression->ParameterName;
						bChanged = true;
					}
				}
			}

			// No reference to the material expression exists, so try to find one in the material expression's array if we are in the editor.
			//if (bTryToFindByName && GIsEditor && !FApp::IsGame())
			{
				for (UMaterialExpression* Expression : ParentMaterial->Expressions)
				{
					//if (Expression->IsA<ExpressionType>())
					if (true)
					{
						ExpressionType* ParameterExpression = CastChecked<ExpressionType>(Expression);
						if (ParameterExpression->ParameterName == Parameter.ParameterInfo.Name)
						{
							Parameter.ExpressionGUID = ParameterExpression->ExpressionGUID;
							bChanged = true;
							break;
						}
					}
					else if (UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(Expression))
					{
						if (FunctionCall->MaterialFunction && FunctionCall->MaterialFunction->UpdateParameterSet<ParameterType, ExpressionType>(Parameter))
						{
							bChanged = true;
							break;
						}
					}
					else if (UMaterialExpressionMaterialAttributeLayers* LayersExpression = Cast<UMaterialExpressionMaterialAttributeLayers>(Expression))
					{
						const TArray<UMaterialFunctionInterface*> Layers = LayersExpression->GetLayers();
						const TArray<UMaterialFunctionInterface*> Blends = LayersExpression->GetBlends();

						for (auto* Layer : Layers)
						{
							if (Layer && Layer->UpdateParameterSet<ParameterType, ExpressionType>(Parameter))
							{
								bChanged = true;
								break;
							}
						}

						if (!bChanged)
						{
							for (auto* Blend : Blends)
							{
								if (Blend && Blend->UpdateParameterSet<ParameterType, ExpressionType>(Parameter))
								{
									bChanged = true;
									break;
								}
							}
						}

						if (bChanged)
						{
							break;
						}
					}
				}
			}
		}

		return bChanged;
	}
	// ��ģ�����ݼ��ص�RMC
	//UFUNCTION()
	//static FReturnedData AddMesh(FReturnedData& result);


	// ��unpacked����Ϣ��������������
	//static void PackMyStreamingTextureData(ULevel* Level, TArray<FStreamingTexturePrimitiveInfo>& UnpackedData, TArray<FStreamingTextureBuildInfo>& Out_StreamingTextureData, const FBoxSphereBounds& RefBounds);


	//UFUNCTION()
	//void BuildMyTextureStreamingData(UStaticMeshComponent* MyStaticMeshComponent);

	// ԭ�����ļ��У��ú�������д������麯��
	// δʶ������ 'ENUM or STRUCT' - ���ͱ����� UCLASS, USTRUCT or UENUM ". 
	// �Ƴ�UFUNCTION()���Ա���ͨ��,������ʱ����uobject�Ķϵ���󣬸�����ʵ����һ����why��
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//UFUNCTION()
	//FPrimitiveSceneProxy* CreateMySceneProxy(UStaticMeshComponent* MyStaticMeshComponent);
	//~ End UPrimitiveComponent Interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiplePass")
	UMaterialInterface* SecondPassMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiplePass")
	bool NeedSecondPass;

	// ��ʼ��mesh����ر���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RMCVariable")
	int32 NumMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RMCVariable")
	bool ManualMode;

private:
	int32 MeshLoaded;
	bool needRelease;
	TMap<int32, TFuture<FReturnedData>> ReturnedDataMap;
	TMap<int32, URuntimeMeshComponent *> MeshComponentMap;
	UMaterial* BaseMat;

private:

	// 3D Model��·��
	FString PathOfMesh;

	// ��Mesh��Texture
	bool BindMeshAndTexture(FReturnedData mesh, URuntimeMeshComponent * meshComponent, int meshid);
	TFuture<bool>  BindMeshAndTextureAsync(UObject* Outer, FReturnedData & mesh, URuntimeMeshComponent * meshComponent, int meshid, TFunction<void()> CompletionCallback);

	// ����mesh
	TFuture<FReturnedData> LoadMeshFromDiskAsync(UObject* Outer, const FString& MeshPath, TFunction<void()> CompletionCallback);
	//TFuture<FReturnedData> LoadMeshFromDiskAsync(const FString& MeshPath);
	UTexture2D* CreateTexture(UObject* Outer, const TArray<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, FName BaseName);
};
