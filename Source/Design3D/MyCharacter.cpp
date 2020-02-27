// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "misc/Paths.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Camera/CameraPhotography.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "Landscape.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "LevelLoadCharacter.h"
// Sets default values

USpringArmComponent* SpringArm;
UCameraComponent* Camera;
UCameraComponent* InternalCamera;
FVector InternalCameraOrigin;
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("CharacterMeshCom")));
	//const FMatrix DefMeshLS = FRotationTranslationMatrix(SpringArm->RelativeRotation, SpringArm->RelativeLocation);
	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	//SpringArm->AttachTo(RootComponent);
	SpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SpringArm->TargetArmLength = 125.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	//Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;
}

bool isBebin = false;
bool bIsFirst = true;

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	isBebin = true;
	bIsFirst = true;
}

int32  tick = 0;
// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->bEnableOnScreenDebugMessagesDisplay = 0;
	/*fminimalviewinfo newpov = viewtarget.pov;
	fstring str = text("begin load level" + currentname + "!");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, str);*/

	++tick;
	levelpara.step = 25 * DeltaTime * levelpara.latlontoued;
	levelpara.step = 25 * DeltaTime * levelpara.latlontoued;
	ComputeLOD(levelpara);
	if (isBebin && tick == 10)
	{
		LoadLevelbyCameraInfo();
		tick = 0;
	}

}

int GetMaxPlayerCount()
{
	int max = 1;
	return max;
}


void AMyCharacter::SetFlyMode()
{
	if (GetCharacterMovement()->IsFlying() == false)
	{
		// If we are not flying already then set our movement mode so that we are  
		//MyCharacter->GetCharacterMovement()->bCheatFlying = true;
		//MyCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		//MyCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		// ���ö�����Z�����ƶ������Զ�ת��
		//MyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		//bUseControllerRotationYaw = false;
		MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
	else
	{
		// Set the new movement mode to walking  
		MyCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Flying", IE_Pressed, this, &AMyCharacter::Flying);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::TurnAround);//��MouseX��Z����ת    �����ӽ�
	PlayerInputComponent->BindAxis("Lookup", this, &AMyCharacter::LookUpAround);//��MouseY��Y����ת   �����ӽ�
	PlayerInputComponent->BindAxis("Zoom", this, &AMyCharacter::MoveUpDown);
	PlayerInputComponent->BindAxis("ScrollWheelUp", this, &AMyCharacter::OnScrollWheelUpPress);
	PlayerInputComponent->BindAxis("ScrollWheelDown", this, &AMyCharacter::OnScrollWheelDownPress);

}

void AMyCharacter::OnStartJump()
{
	bPressedJump = true;
}

//void AMyCharacter::OnStopJump()
//{
//	bPressedJump = false;
//}

void AMyCharacter::Flying()
{
	MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void AMyCharacter::MoveForward(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		//const FVector Direction = FRotationMatrix(levelpara.Rotator).GetScaledAxis(EAxis::X);
		//FVector NewLocation = GetActorLocation();
		//NewLocation += Direction * val * levelpara.step;
		//SetActorLocation(NewLocation);

		FRotator Rotation = Controller->GetControlRotation();
		FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		FVector Delta = FVector(0.0, 0.0, 0.0);
		//FVector Direction = FRotationMatrix(levelpara.Rotator).GetScaledAxis(EAxis::X);
		float movestep = levelpara.step < levelpara.levelsize ? levelpara.step : levelpara.levelsize;
		movestep = val * movestep;

		Delta = movestep * Direction;
		RootComponent->MoveComponent(Delta, GetActorQuat(), true);

		//AddMovementInput(Direction, val * levelpara.step);
	}
}

void AMyCharacter::MoveRight(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		//const FVector Direction = FRotationMatrix(levelpara.Rotator).GetScaledAxis(EAxis::Y);
		//FVector NewLocation = GetActorLocation();
		//NewLocation += Direction * val * levelpara.step;
		//SetActorLocation(NewLocation);
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		float movestep = levelpara.step < levelpara.levelsize ? levelpara.step : levelpara.levelsize;
		movestep = val * movestep;
		FVector Delta = movestep * Direction;
		RootComponent->MoveComponent(Delta, GetActorQuat(), true);

		//AddMovementInput(Direction, val * levelpara.step);
	}
}

void AMyCharacter::TurnAround(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		// Ϊ�˱�֤��ͬ������֡��ˢ������һ��GetWorld()->GetDeltaSeconds()
		AddControllerYawInput(val);
	}
}

void AMyCharacter::LookUpAround(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		AddControllerPitchInput(val);
	}
}

void AMyCharacter::MoveUpDown(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		FVector Delta = FVector(0.0, 0.0, 0.0);
		Delta.Z = val * levelpara.step;

		APlayerController* PlayerController = GetWorld()->GetPlayerControllerIterator()->Get();
		FVector Loc = PlayerController->PlayerCameraManager->GetCameraLocation();
		//Delta.Z = val * Loc.Z * 0.1;

		RootComponent->MoveComponent(Delta, GetActorQuat(), true);
	}

}

void AMyCharacter::OnScrollWheelUpPress(float val)
{
	FVector Delta = FVector(0.0, 0.0, 0.0);
	APlayerController* PlayerController = GetWorld()->GetPlayerControllerIterator()->Get();
	FVector Loc = PlayerController->PlayerCameraManager->GetCameraLocation();
	Delta.Z = val * Loc.Z * 0.1;
	RootComponent->MoveComponent(Delta, GetActorQuat(), true);
}

void AMyCharacter::OnScrollWheelDownPress(float val)
{

	FVector2D CursorPos;
	FVector WorldPos;
	FVector MoveDirction = FVector::ZeroVector;

	if (FMath::Abs(val) > KINDA_SMALL_NUMBER)
	{
		GetWorld()->GetFirstPlayerController()->GetMousePosition(CursorPos.X, CursorPos.Y);
		UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GWorld, 0), CursorPos, WorldPos, MoveDirction);
	}
	AddMovementInput(MoveDirction, val * levelpara.step);
}


void AMyCharacter::GetCameraInfo(FParameter& para)
{
	UWorld* CurWorld = GetWorld();
	if (!CurWorld)
	{
		return;
	}
	para.Resolution = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	// Find our player index
	int32 IterIndex = 0;
	int32 PlayerIndex = INDEX_NONE;
	bool bIsPaused = GetWorld()->IsPaused();

	for (FConstPlayerControllerIterator Iterator = CurWorld->GetPlayerControllerIterator(); Iterator; ++Iterator, ++IterIndex)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (!bIsPaused || PlayerController->ShouldPerformFullTickWhenPaused())
		{
			if (bIsFirst)
			{
				FMinimalViewInfo CurrentPOV = PlayerController->PlayerCameraManager->GetCameraCachePOV();
				CurrentPOV.Rotation = FRotator(0.0, -89.0, 0.0);
				CurrentPOV.Location.X = 28 * para.latlontoued;
				CurrentPOV.Location.Y = 87 * para.latlontoued;
				CurrentPOV.Location.Z = 1 * 760000.0;
				PlayerController->PlayerCameraManager->SetCameraCachePOV(CurrentPOV);
				MyCharacter = GetWorld()->SpawnActor<AMyCharacter>(AMyCharacter::GetClass(), CurrentPOV.Location, CurrentPOV.Rotation);
				PlayerController->Possess(MyCharacter);
				bIsFirst = false;
			}

			PlayerIndex = IterIndex;
			float CamFOV = PlayerController->PlayerCameraManager->GetFOVAngle();
			FVector Loc = PlayerController->PlayerCameraManager->GetCameraLocation();
			FRotator Rotator = PlayerController->PlayerCameraManager->GetCameraRotation();

			MyCharacter->GetCharacterMovement()->bCheatFlying = true;
			MyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
			MyCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
			MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

			para.CamFOV = CamFOV;
			para.Loc = Loc;
			para.Rotator = Rotator;
			break;
		}
	}
}


void AMyCharacter::GetFovMaxDistance(FParameter& para)
{
	// ��ǰ��Ļ��ȷ�����Ƭ����n=width/bitmap����Ƭ�ܸ���=pow(2,1+LOD)
	float SliceofWidth = para.Resolution.X / para.bitmapsize;
	para.lon_preload = ceil(SliceofWidth / 2.0);
	para.sliceofPitch = ceil(SliceofWidth) + 2 * para.lon_preload;
	float SliceofHeight = para.Resolution.Y / para.bitmapsize;
	para.lat_preload = ceil(SliceofHeight) / 2.0;
	para.sliceofRoll = ceil(SliceofHeight) + 2 * para.lat_preload;
}

bool AMyCharacter::GetFocusPosition(FParameter& para)
{
	para.radian = PI / 180;
	// ƫ��������ͼ�����÷�ΧΪ��-180~180����C++��ȡ��ΧΪ��0~360��
	// ����������ͼ�����÷�ΧΪ��-90~90����C++��ȡ��ΧΪ��0~90���ͣ�270-360��
	// �ӵ�Ĭ��λ�ã�ƫ����Ϊ0��������Ϊ0����Ұ����Ϊ����UE4��x��
	// ��Ұ��������ԣ�-90��90��Ϊһ����λ����ƫ���Ǽ��㣬��ʱ��Ϊ��ֵ��˳ʱ��Ϊ��ֵ
	// �ӵ㵽����ľ���Ϊ�����룬�Դ˾���Ϊ�뾶���ӵ�ΪԲ�Ļ�Բ��Բ������Ϊ��Ұ����
	float AngleYaw = para.Rotator.Yaw > 0 ? para.Rotator.Yaw : para.Rotator.Yaw + 360;
	float offsetYaw = para.radian * AngleYaw;

	// �����������µ���Ұ��������ֻ����x���򣩸������й�
	float CamYaw = para.CamFOV * para.radian / 2.0;
	// ��Զ����Ϊ�������˫���غ�����124��ʱ�ľ���
	float CamPitch = atan(tan(para.radian * para.CamFOV / 2.0) * para.Resolution.Y / para.Resolution.X);
	float CamFOVPitch = CamPitch / para.radian;
	// (x����)��Զ����Ϊ����߶ȳ�����Զ�����븩��������Ž�֮�һ�룩������ֵ
	//float AngleMaxPitch = para.AngleMax / 2.0 * para.Resolution.Y / para.Resolution.X;
	// ��xoyƽ��Ҫ��ͶӰ�������ǵĽǶ�Ҫ����validpitch��������-(90-(124/2-27))=-48,������ͷ���µĽǶ�Ҫ����48
	float ValidPitch = -(90 - (para.AngleMax / 2.0 - CamFOVPitch));
	float AnglePitch = para.Rotator.Pitch < 180 ? para.Rotator.Pitch : para.Rotator.Pitch - 360;
	if (AnglePitch > ValidPitch)
	{
		return false;
	}

	// segment��ʾ���������ʼ����xoyƽ�潹�㴦���߶ξ���
	// ��Զ����Ϊ�������˫���غ�����124��ʱ�ľ���
	// (x����)��Զ������xoyƽ���ͶӰ��Ϊ����߶ȳ�����Զ������ƫ��������Ž�֮�һ�룩������ֵ
	float segmentmax = para.Loc.Z / cos(para.radian * (para.AngleMax / 2.0 - CamFOVPitch));
	// ��̵��߶ξ���Ϊ�����Z����ľ��룬����ֱ���µ��ӽ�
	float segmentmin = para.Loc.Z;
	// ���ʵ�ʽ�����xoyƽ��ͶӰ�븩�����й�
	float radianPitch = para.radian * AnglePitch;
	float segment = para.Loc.Z / sin(abs(radianPitch));
	if (segment < segmentmin)
	{
		return false;
	}
	// projection���߶���xoyƽ���ͶӰ
	float projection = 0.0;
	projection = para.Loc.Z / tan(abs(radianPitch));
	// �����������ӽǣ���ô������Ϊ(90��- (para.AngleMax - CamFOVPitch)/2.0)
	if (segment > segmentmax)
	{
		segment = segmentmax;
		projection = para.Loc.Z * tan(para.radian * (para.AngleMax / 2.0 - CamFOVPitch));
	}
	para.fovmax = segment;

	para.focus.X = 0.0;
	para.focus.Y = 0.0;
	para.PitchisLon = true;
	if (AngleYaw >= 0 && AngleYaw < 45)
	{
		// lim deta(theta) {tan(theta-deta(theta)) - tan(theta)};
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X + projection * cos(offsetYaw);
		para.focus.Y = para.Loc.Y + projection * sin(offsetYaw);
	}
	else if (AngleYaw >= 45 && AngleYaw < 135)
	{
		offsetYaw = para.radian * (AngleYaw - 90);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		// ���λ���ұߣ�offsetYaw:0~45�㣩ΪX�ĸ�������ߣ�offsetYaw:-45��~0��ΪX��������
		para.focus.X = para.Loc.X - projection * sin(offsetYaw);
		para.focus.Y = para.Loc.Y + projection * cos(offsetYaw);
		para.PitchisLon = false;
	}
	else if (AngleYaw >= 135 && AngleYaw < 225)
	{
		offsetYaw = para.radian * (AngleYaw - 180);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X - projection * cos(offsetYaw);
		para.focus.Y = para.Loc.Y - projection * sin(offsetYaw);
	}
	else if (AngleYaw >= 225 && AngleYaw < 315)
	{
		offsetYaw = para.radian * (AngleYaw - 270);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X + projection * sin(offsetYaw);
		para.focus.Y = para.Loc.Y - projection * cos(offsetYaw);
		para.PitchisLon = false;
	}
	else if (AngleYaw >= 315 && AngleYaw <= 360)
	{
		offsetYaw = para.radian * (AngleYaw - 360);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X + projection * cos(offsetYaw);
		para.focus.Y = para.Loc.Y + projection * sin(offsetYaw);
	}
	return true;
}

void AMyCharacter::ComputeLOD(FParameter& para)
{
	// ��ǰ��Ļ��ȷ�����Ƭ����n=width/bitmap����Ƭ�ܸ���=pow(2,1+LOD)
	float SliceofWidth = para.Resolution.X / para.bitmapsize;
	float SilcesAngle = para.radian * para.CamFOV * para.DervativeofAngle * para.fovmax / para.latlontoued;
	float lod = log(360 * SliceofWidth / SilcesAngle) / log(2) - 1;
	para.LOD = ceil(lod) > 0 ? ceil(lod) : 0;
	//para.LOD = 4;
	para.levelsize = 180.0 / pow(2, para.LOD) * para.latlontoued / 100.0;
}
//
//void AMyCharacter::SetSliceMapOffset(FParameter& para, FLevelNameInfo levelName)
//{
//	int32 latNum = pow(2, levelName.lodName);
//	// ������Ƭ���ĵ�ʵ�ʾ�γ��
//	float lon = (levelName.lonName - latNum + 0.5) * 360 / (2 * latNum);
//	float lat = (levelName.latName - latNum / 2.0 + 0.5) * 180 / latNum;
//	if (levelName.lodName == 0)
//	{
//		lon = 180 * (levelName.lonName - 0.5);
//		lat = 0;
//	}
//
//	para.MapOffset.X = lat * 6300;
//	para.MapOffset.Y = lon * 6300;
//
//}

void AMyCharacter::SetSliceMapOffset(FParameter& para, int32 lonfileName, int32 latfileName)
{
	int32 latNum = pow(2, para.LOD);
	float lon = (lonfileName - latNum + 0.5) * 360 / (2 * latNum);
	float lat = (latfileName - latNum / 2.0 + 0.5) * 180 / latNum;
	if (para.LOD == 0)
	{
		lon = 0;
		lat = 0;
	}

	para.MapOffset.X = lat * para.latlontoued;
	para.MapOffset.Y = lon * para.latlontoued;

	//para.MapOffset.X = lat * para.latlontoued;
	//para.MapOffset.Y = lon * para.latlontoued;

	//para.MapScale.X = 360 * para.latlontoued / (pow(2, para.LOD) * para.heightmapsize);
	//para.MapScale.Y = 360 * para.latlontoued / (pow(2, para.LOD) * para.heightmapsize);
	//para.MapScale.Z = 360 * para.latlontoued / (pow(2, para.LOD) * para.heightmapsize * 5);
}

void AMyCharacter::GetLevelFile(FParameter& para)
{
	float Slicedegree = 360.0 / pow(2, 1 + para.LOD);
	float Slicesize = Slicedegree * para.latlontoued;
	//float lat_center = floor(para.focus.X / Slicesize);
	//float lon_center = floor(para.focus.Y / Slicesize);
	int32 lat_index = floor(para.focus.X / Slicesize) + pow(2, para.LOD - 1);
	int32 lon_index = floor(para.focus.Y / Slicesize) + pow(2, para.LOD);

	if (!para.PitchisLon)
	{
		int32 slicenum = para.sliceofPitch;
		para.sliceofPitch = para.sliceofRoll;
		para.sliceofRoll = slicenum;
	}

	//int32 lat_index = floor((lat_center + 90) / Slicedegree);
	//int32 lon_index = floor((lon_center + 180) / Slicedegree);
	int32 lat_start = lat_index - floor(para.sliceofRoll / 2.0);
	int32 lat_end = lat_index + ceil(para.sliceofRoll / 2.0);
	int32 lon_start = lon_index - floor(para.sliceofPitch / 2.0);
	int32 lon_end = lon_index + ceil(para.sliceofPitch / 2.0);

	para.lonName.Empty();
	para.latName.Empty();
	para.levelsName.Empty();
	for (int32 lon = lon_start; lon < lon_end; ++lon)
	{
		para.lonName.Add(lon);
		for (int32 lat = lat_start; lat < lat_end; ++lat)
		{
			FString levelName = FString::FromInt(para.LOD) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);

			if (lon == lon_start)
			{
				para.latName.Add(lat);
			}
			para.levelsName.Add(levelName);
		}
	}

}


FTransform GetTransform(FParameter& para)
{
	int rowNum = pow(2, para.LOD);
	FVector2D size;
	FVector2D center;
	size.X = 180.0 / rowNum;
	size.Y = size.X;
	center.X = para.focus.X - size.X / 2.0f;
	center.Y = para.focus.Y - size.Y / 2.0f;

	UGeographicMath* geographicMath = NewObject<UGeographicMath>();
	geographicMath->AddToRoot();
	FTransform geoTransform = geographicMath->transform;

	/*
	size.X = size.X * this->TileScale;
	size.Y = size.Y * this->TileScale;
	center.X = center.X * this->TileScale + this->OffsetXY.X;
	center.Y = center.Y * this->TileScale + this->OffsetXY.Y;
	*/
	float initTileSize = 63.0f;
	float scale = size.X / initTileSize;

	center.X = center.X * geoTransform.GetScale3D().X;
	center.Y = center.Y * geoTransform.GetScale3D().Y;

	FTransform transform(geoTransform);

	float earthRadius = 6378.2 * 1000; // m
	float c = 2 * PI * earthRadius;
	float meterPerDegree = c / 360.0f;

	float UedPerDegree = 100.0f;

	float meterPerR16 = 5.0f / 128.0f;

	float zOffset = 0;//215.9375f;
	transform.MultiplyScale3D(FVector(scale, scale, 2 * 1.0f / (meterPerDegree * meterPerR16)));
	transform.AddToTranslation(FVector(center, zOffset));

	FTransform a;
	return transform;
}

void TransformLevel(FParameter& para, ULevel *Level)
{
	FTransform LevelTransform = GetTransform(para);
	for (int32 ActorIndex = 0; ActorIndex < Level->Actors.Num(); ActorIndex++)
	{
		AActor* Actor = Level->Actors[ActorIndex];

		// Don't want to transform children they should stay relative to there parents.
		if (Actor && Actor->GetAttachParentActor() == NULL)
		{
			// Has to modify root component directly as GetActorPosition is incorrect this early
			USceneComponent *RootComponent = Actor->GetRootComponent();
			if (RootComponent)
			{

				RootComponent->SetRelativeLocationAndRotation(LevelTransform.GetLocation(), LevelTransform.Rotator());
				RootComponent->SetRelativeScale3D(LevelTransform.GetScale3D());

			}
		}
	}
}


/*
// Ĭ�ϼ���3��level�ķ���
void AMyCharacter::GetWorldLevels(FParameter& para)
{
	para.levelInfoMap.Empty();
	para.levelInfo.levelsName.Empty();

	float Slicedegree = 360.0 / pow(2, 1 + para.LOD);
	float Slicesize = Slicedegree * para.latlontoued;
	int32 lat_index = floor(para.focus.X / Slicesize) + pow(2, para.LOD - 1);
	int32 lon_index = floor(para.focus.Y / Slicesize) + pow(2, para.LOD);
	int32 lodscale = 0;
	int32 levelnum = 0;
	FString levelName;
	FLevelNameInfo landScapeInfo;

	if (para.LOD < 3)
	{
		for (int32 lon = 0; lon < pow(2, para.LOD + 1); ++lon)
		{
			for (int32 lat = 0; lat < pow(2, para.LOD); ++lat)
			{
				landScapeInfo.lodName = para.LOD;
				landScapeInfo.lonName = lon;
				landScapeInfo.latName = lat;
				para.levelInfoMap.Add(levelnum, landScapeInfo);
				levelnum += 1;

				levelName = FString::FromInt(para.LOD) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);
				para.levelInfo.levelsName.Add(levelName);
			}
		}
	}
	else
	{
		int32 lon_focus = 2 * lon_index;
		int32 lat_focus = 2 * lat_index;
		int32 lon_start = lon_index;
		int32 lat_start = lat_index;
		for (int32 lod = para.LOD; lod >= 0; --lod)
		{
			//�������ۻ��ķ�ʽ����������ۻ��ĳ���2�ͻ����
			//lodscale += 1;
			lon_focus = lon_focus / 2;
			lat_focus = lat_focus / 2;
			lon_start = lon_start / 2;
			lat_start = lat_start / 2;

			landScapeInfo.lodName = lod;
			if (lod == para.LOD)
			{
				for (int32 lon = lon_start * 2; lon < lon_start * 2 + 2; ++lon)
				{
					for (int32 lat = lat_start * 2; lat < lat_start * 2 + 2; ++lat)
					{
						landScapeInfo.lonName = lon;
						landScapeInfo.latName = lat;
						para.levelInfoMap.Add(levelnum, landScapeInfo);
						levelnum += 1;

						levelName = FString::FromInt(lod) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);
						para.levelInfo.levelsName.Add(levelName);
					}
				}
			}
			else
			{
				for (int32 lon = lon_start * 2; lon < lon_start * 2 + 2; ++lon)
				{
					for (int32 lat = lat_start * 2; lat < lat_start * 2 + 2; ++lat)
					{
						if (!(lon == lon_focus && lat == lat_focus))
						{
							landScapeInfo.lonName = lon;
							landScapeInfo.latName = lat;
							para.levelInfoMap.Add(levelnum, landScapeInfo);
							levelnum += 1;

							levelName = FString::FromInt(lod) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);
							para.levelInfo.levelsName.Add(levelName);
						}
 					}
				}
			}

		}
	}

}*/

/*
void AMyCharacter::GetFovMaxDistance(FParameter& para)
{
	// ��ǰ��Ļ��ȷ�����Ƭ����n=width/bitmap����Ƭ�ܸ���=pow(2,1+LOD)
	float SliceofWidth = para.Resolution.X / para.bitmapsize;
	para.lon_preload = ceil(SliceofWidth / 2.0);
	para.sliceofPitch = ceil(SliceofWidth) + 2 * para.lon_preload;
	float SliceofHeight = para.Resolution.Y / para.bitmapsize;
	para.lat_preload = ceil(SliceofHeight) / 2.0;
	para.sliceofRoll = ceil(SliceofHeight) + 2 * para.lat_preload;
}

bool AMyCharacter::GetFocusPosition(FParameter& para)
{
	para.radian = PI / 180;
	// ƫ��������ͼ�����÷�ΧΪ��-180~180����C++��ȡ��ΧΪ��0~360��
	// ����������ͼ�����÷�ΧΪ��-90~90����C++��ȡ��ΧΪ��0~90���ͣ�270-360��
	// �ӵ�Ĭ��λ�ã�ƫ����Ϊ0��������Ϊ0����Ұ����Ϊ����UE4��x��
	// ��Ұ��������ԣ�-90��90��Ϊһ����λ����ƫ���Ǽ��㣬��ʱ��Ϊ��ֵ��˳ʱ��Ϊ��ֵ
	// �ӵ㵽����ľ���Ϊ�����룬�Դ˾���Ϊ�뾶���ӵ�ΪԲ�Ļ�Բ��Բ������Ϊ��Ұ����
	float AngleYaw = para.Rotator.Yaw > 0 ? para.Rotator.Yaw : para.Rotator.Yaw + 360;
	float offsetYaw = para.radian * AngleYaw;

	// �����������µ���Ұ��������ֻ����x���򣩸������й�
	float CamYaw = para.CamFOV * para.radian / 2.0;
	// ��Զ����Ϊ�������˫���غ�����124��ʱ�ľ���
	float CamPitch = atan(tan(para.radian * para.CamFOV / 2.0) * para.Resolution.Y / para.Resolution.X);
	float CamFOVPitch = CamPitch / para.radian;
	// (x����)��Զ����Ϊ����߶ȳ�����Զ�����븩��������Ž�֮�һ�룩������ֵ
	//float AngleMaxPitch = para.AngleMax / 2.0 * para.Resolution.Y / para.Resolution.X;
	// ��xoyƽ��Ҫ��ͶӰ�������ǳ��µĽǶ�Ҫ����validpitch
	float ValidPitch = -(90 - para.AngleMax / 2.0 - CamFOVPitch);
	float AnglePitch = para.Rotator.Pitch < 180 ? para.Rotator.Pitch : para.Rotator.Pitch - 360;
	if (AnglePitch > ValidPitch)
	{
		return false;
	}

	// ��Զ����Ϊ�������˫���غ�����124��ʱ�ľ���
	// (x����)��Զ������xoyƽ���ͶӰ��Ϊ����߶ȳ�����Զ������ƫ��������Ž�֮�һ�룩������ֵ
	float fovmax = para.Loc.Z * tan(para.radian * ((para.AngleMax - CamFOVPitch) / 2.0));
	// ��̽�����xoyƽ���ͶӰ��Ϊ�����ڸô���ͶӰ
	float fovmin = 0;
	// ���ʵ�ʽ�����xoyƽ��ͶӰ�븩�����й�
	float offsetPitch = para.radian * AnglePitch;
	float fovpitch = para.Loc.Z / tan(abs(offsetPitch));
	fovpitch = fovpitch < fovmin ? fovmin : fovpitch;
	para.fovmax = fovpitch < fovmax ? fovpitch : fovmax;

	para.focus.X = 0.0;
	para.focus.Y = 0.0;
	para.PitchisLon = true;
	if (AngleYaw >= 0 && AngleYaw < 45)
	{
		// lim deta(theta) {tan(theta-deta(theta)) - tan(theta)};
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X + para.fovmax * cos(offsetYaw);
		para.focus.Y = para.Loc.Y + para.fovmax * sin(offsetYaw);
	}
	else if (AngleYaw >= 45 && AngleYaw < 135)
	{
		offsetYaw = para.radian * (AngleYaw - 90);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		// ���λ���ұߣ�offsetYaw:0~45�㣩ΪX�ĸ�������ߣ�offsetYaw:-45��~0��ΪX��������
		para.focus.X = para.Loc.X - para.fovmax * sin(offsetYaw);
		para.focus.Y = para.Loc.Y + para.fovmax * cos(offsetYaw);
		para.PitchisLon = false;
	}
	else if (AngleYaw >= 135 && AngleYaw < 225)
	{
		offsetYaw = para.radian * (AngleYaw - 180);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X - para.fovmax * cos(offsetYaw);
		para.focus.Y = para.Loc.Y - para.fovmax * sin(offsetYaw);
	}
	else if (AngleYaw >= 225 && AngleYaw < 315)
	{
		offsetYaw = para.radian * (AngleYaw - 270);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X + para.fovmax * sin(offsetYaw);
		para.focus.Y = para.Loc.Y - para.fovmax * cos(offsetYaw);
		para.PitchisLon = false;
	}
	else if (AngleYaw >= 315 && AngleYaw <= 360)
	{
		offsetYaw = para.radian * (AngleYaw - 360);
		para.DervativeofAngle = 1.0 / pow(cos(offsetYaw), 2);
		para.focus.X = para.Loc.X + para.fovmax * cos(offsetYaw);
		para.focus.Y = para.Loc.Y + para.fovmax * sin(offsetYaw);
	}
	return true;
}

void AMyCharacter::ComputeLOD(FParameter& para)
{
	// ��ǰ��Ļ��ȷ�����Ƭ����n=width/bitmap����Ƭ�ܸ���=pow(2,1+LOD)
	float SliceofWidth = para.Resolution.X / para.bitmapsize;
	float SilcesAngle = para.radian * para.CamFOV * para.DervativeofAngle * para.fovmax / para.latlontoued;
	float lod = log(360 * SliceofWidth / SilcesAngle) / log(2) - 1;
	para.LOD = ceil(lod) > 0 ? ceil(lod) : 0;
	//para.LOD = 4;
	para.levelsize = 180.0 / pow(2, para.LOD) * para.latlontoued;
}


void AMyCharacter::SetSliceMapOffset(FParameter& para, FLevelNameInfo levelName)
{
	int32 latNum = pow(2, levelName.lodName);
	// ������Ƭ���ĵ�ʵ�ʾ�γ��
	float lon = (levelName.lonName - latNum) * 360 / (2 * latNum);
	float lat = (levelName.latName - latNum / 2.0) * 180 / latNum;
	if (levelName.lodName == 0)
	{
		lon = 180 * (levelName.lonName);
		lat = 0;
	}


	para.MapOffset.X = lat * 6300;
	para.MapOffset.Y = lon * 6300;
	//para.MapOffset.X = lat * para.latlontoued * 63 / 64;
	//para.MapOffset.Y = lon * para.latlontoued * 63 / 64;

	//para.MapScale.X = 360 * para.latlontoued / (pow(2, para.LOD) * para.heightmapsize);
	//para.MapScale.Y = 360 * para.latlontoued / (pow(2, para.LOD) * para.heightmapsize);
	//para.MapScale.Z = 360 * para.latlontoued / (pow(2, para.LOD) * para.heightmapsize * 5);
}


// �����λ����Ϊlod��ʼ�㣬�õ�level����Ϊ16
void AMyCharacter::GetWorldLevels(FParameter& para)
{
	para.levelInfoMap.Empty();
	para.levelInfo.levelsName.Empty();

	float Slicedegree = 360.0 / pow(2, 1 + para.LOD);
	float Slicesize = Slicedegree * para.latlontoued;
	int32 lat_index = floor(para.focus.X / Slicesize) + pow(2, para.LOD - 1);
	int32 lon_index = floor(para.focus.Y / Slicesize) + pow(2, para.LOD);
	int32 lodscale = 0;
	int32 levelnum = 0;
	FString levelName;
	FLevelNameInfo landScapeInfo;

	if (para.LOD < 3)
	{
		for (int32 lon = 0; lon < pow(2, para.LOD + 1); ++lon)
		{
			for (int32 lat = 0; lat < pow(2, para.LOD); ++lat)
			{
				landScapeInfo.lodName = para.LOD;
				landScapeInfo.lonName = lon;
				landScapeInfo.latName = lat;
				para.levelInfoMap.Add(levelnum, landScapeInfo);
				levelnum += 1;

				levelName = FString::FromInt(para.LOD) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);
				para.levelInfo.levelsName.Add(levelName);
			}
		}
	}
	else
	{
		int32 lon_focus = 2 * lon_index;
		int32 lat_focus = 2 * lat_index;
		int32 lon_start = lon_index;
		int32 lat_start = lat_index;
		for (int32 lod = para.LOD; lod >= 0; --lod)
		{
			//�������ۻ��ķ�ʽ����������ۻ��ĳ���2�ͻ����
			//lodscale += 1;
			lon_focus = lon_focus / 2;
			lat_focus = lat_focus / 2;
			lon_start = lon_start / 2;
			lat_start = lat_start / 2;

			landScapeInfo.lodName = lod;
			if (lod == para.LOD)
			{
				for (int32 lon = lon_start * 2; lon < lon_start * 2 + 2; ++lon)
				{
					for (int32 lat = lat_start * 2; lat < lat_start * 2 + 2; ++lat)
					{
						landScapeInfo.lonName = lon;
						landScapeInfo.latName = lat;
						para.levelInfoMap.Add(levelnum, landScapeInfo);
						levelnum += 1;

						levelName = FString::FromInt(lod) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);
						para.levelInfo.levelsName.Add(levelName);
					}
				}
			}
			else
			{
				for (int32 lon = lon_start * 2; lon < lon_start * 2 + 2; ++lon)
				{
					for (int32 lat = lat_start * 2; lat < lat_start * 2 + 2; ++lat)
					{
						if (!(lon == lon_focus && lat == lat_focus))
						{
							landScapeInfo.lonName = lon;
							landScapeInfo.latName = lat;
							para.levelInfoMap.Add(levelnum, landScapeInfo);
							levelnum += 1;

							levelName = FString::FromInt(lod) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);
							para.levelInfo.levelsName.Add(levelName);
						}
					}
				}
			}

		}
	}

}*/

int32 UniqueLevelInstanceId = 0;
void AMyCharacter::LoadLevelbyName(FParameter& para, FString levelName, FString LongPackageName, bool& bOutSuccess)
{
	bOutSuccess = false;

	UWorld* const World = GEngine->GetWorldFromContextObject(GWorld, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	FString str = TEXT("Begin Load level" + levelName + "!");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, str);

	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;
	UniqueLevelPackageName += TEXT("_LevelInstance_") + FString::FromInt(++UniqueLevelInstanceId);

	// Setup streaming level object that will load specified map
	ULevelStreamingDynamic* StreamingLevel = NewObject<ULevelStreamingDynamic>(World, ULevelStreamingDynamic::StaticClass(), NAME_None, RF_Transient, NULL);
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));

	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(true);
	StreamingLevel->bShouldBlockOnLoad = false;
	StreamingLevel->bInitiallyLoaded = true;
	StreamingLevel->bInitiallyVisible = true;
	// Transform
	//StreamingLevel->LevelTransform = FTransform(FRotator::ZeroRotator, para.MapOffset, para.MapScale);

	//if (StreamingLevel->IsLevelLoaded())
	//{
	//	TransformLevel(para, StreamingLevel->GetLoadedLevel());
	//}
	//else
	//{
	//	World->FlushLevelStreaming(); // ȷ�����سɹ�����֤���غ���ж��
	//	if (StreamingLevel->IsLevelLoaded())
	//	{
	//		TransformLevel(para, StreamingLevel->GetLoadedLevel());
	//	}
	//}


	// Map to Load
	StreamingLevel->PackageNameToLoad = FName(*LongPackageName);


	// Add the new level to world.
	World->AddStreamingLevel(StreamingLevel);
	//UGameplayStatics::LoadStreamLevel(World, FName(*levelName), true, true, FLatentActionInfo());
	para.levelStreamingMap.Add(levelName, StreamingLevel);
	para.levelsloaded.Add(levelName);
	bOutSuccess = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Load level finished!"));
}

void AMyCharacter::UnLoadLevelbyName(FParameter& para, FString levelName)
{
	FString str = TEXT("Begin Unload level" + levelName + "!");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, str);
	if (para.levelStreamingMap[levelName])
	{
		para.levelStreamingMap[levelName]->SetIsRequestingUnloadAndRemoval(true);
		para.levelStreamingMap.Remove(levelName);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Unload level finished!"));

}

/*
void AMyCharacter::GetLevelFile(FParameter& para)
{
	float Slicedegree = 360.0 / pow(2, 1 + para.LOD);
	float Slicesize = Slicedegree * para.latlontoued;
	//float lat_center = floor(para.focus.X / Slicesize);
	//float lon_center = floor(para.focus.Y / Slicesize);
	int32 lat_index = floor(para.focus.X / Slicesize) + pow(2, para.LOD - 1);
	int32 lon_index = floor(para.focus.Y / Slicesize) + pow(2, para.LOD);

	if (!para.PitchisLon)
	{
		int32 slicenum = para.sliceofPitch;
		para.sliceofPitch = para.sliceofRoll;
		para.sliceofRoll = slicenum;
	}

	//int32 lat_index = floor((lat_center + 90) / Slicedegree);
	//int32 lon_index = floor((lon_center + 180) / Slicedegree);
	int32 lat_start = lat_index - floor(para.sliceofRoll / 2.0);
	int32 lat_end = lat_index + ceil(para.sliceofRoll / 2.0);
	int32 lon_start = lon_index - floor(para.sliceofPitch / 2.0);
	int32 lon_end = lon_index + ceil(para.sliceofPitch / 2.0);

	para.lonName.Empty();
	para.latName.Empty();
	para.levelsName.Empty();
	for (int32 lon = lon_start; lon < lon_end; ++lon)
	{
		para.lonName.Add(lon);
		for (int32 lat = lat_start; lat < lat_end; ++lat)
		{
			FString levelName = FString::FromInt(para.LOD) + R"(\)" + FString::FromInt(lon) + R"(\u)" + FString::FromInt(lat);

			if (lon == lon_start)
			{
				para.latName.Add(lat);
			}
			para.levelsName.Add(levelName);
		}
	}

}
*/
/*
int32 UniqueLevelInstanceId = 0;
void AMyCharacter::LoadLevelbyName(FParameter& para, FString levelName, FString LongPackageName, bool& bOutSuccess)
{
	bOutSuccess = false;

	UWorld* const World = GEngine->GetWorldFromContextObject(GWorld, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	FString str = TEXT("Begin Load level" + levelName + "!");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, str);

	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;
	UniqueLevelPackageName += TEXT("_LevelInstance_") + FString::FromInt(++UniqueLevelInstanceId);

	// Setup streaming level object that will load specified map
	ULevelStreamingDynamic* StreamingLevel = NewObject<ULevelStreamingDynamic>(World, ULevelStreamingDynamic::StaticClass(), NAME_None, RF_Transient, NULL);
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(true);
	StreamingLevel->bShouldBlockOnLoad = false;
	StreamingLevel->bInitiallyLoaded = true;
	StreamingLevel->bInitiallyVisible = true;
	// Transform
	StreamingLevel->LevelTransform = FTransform(FRotator::ZeroRotator, para.MapOffset, para.MapScale);
	// Map to Load
	StreamingLevel->PackageNameToLoad = FName(*LongPackageName);
	// Add the new level to world.
	World->AddStreamingLevel(StreamingLevel);

	para.levelStreamingMap.Add(levelName, StreamingLevel);
	para.levelsloaded.Add(levelName);

	bOutSuccess = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Load level finished!"));
}

void AMyCharacter::UnLoadLevelbyName(FParameter& para, FString levelName)
{
	FString str = TEXT("Begin Unload level" + levelName + "!");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, str);
	if (para.levelStreamingMap[levelName])
	{
		para.levelStreamingMap[levelName]->SetIsRequestingUnloadAndRemoval(true);
		para.levelStreamingMap.Remove(levelName);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Unload level finished!"));

}
*/
/*
void AMyCharacter::LoadLevelbyCameraInfo()
{
	GetCameraInfo(levelpara);
	GetFovMaxDistance(levelpara);
	GetFocusPosition(levelpara);

	if (levelpara.isFirstFrame)
	{
		levelpara.levelsloaded.Empty();
		ComputeLOD(levelpara);
		levelpara.preLOD = levelpara.LOD;
		levelpara.isFirstFrame = false;
	}

	GetLevelFile(levelpara);

	bool bSuccess;
	for (int32 lon = 0; lon < levelpara.sliceofPitch; ++lon)
	{
		for (int32 lat = 0; lat < levelpara.sliceofRoll; ++lat)
		{
			FString filepath = levelpara.levelsName[lon * levelpara.sliceofRoll + lat];
			FString levelmapPath = RootPath + filepath + ".umap";
			int32 lonName = levelpara.lonName[lon];
			int32 latName = levelpara.latName[lat];
			FString currentName = levelpara.levelsName[lon * levelpara.sliceofRoll + lat];
			FString longPackageName = "/Game/" + FString::FromInt(levelpara.LOD) + "/" + FString::FromInt(lonName) + "/u" + FString::FromInt(latName);

			//if (!FPaths::FileExists(levelmapPath))
			//{
			//	// ���ؿչؿ�
			//	longPackageName = "/Game/" + FString::FromInt(levelpara.LOD) + "/emptylevel";
			//}

			//// UE�ĳ����Ƶ��޶�Ϊ��/����������������Ƶ�Ŀ¼�������ظ���ѡ���һ������
			//if (levelpara.levelsloaded.Contains(currentName) && !levelpara.levelsName.Contains(currentName))
			//{
			//	UnLoadLevelbyName(levelpara, currentName);
			//}
			//if (!levelpara.levelsloaded.Contains(currentName))
			//{
			//	SetSliceMapOffset(levelpara, lonName, latName);
			//	LoadLevelbyName(levelpara, currentName, longPackageName, bSuccess);
			//}



			if (FPaths::FileExists(levelmapPath))
			{
				// UE�ĳ����Ƶ��޶�Ϊ��/����������������Ƶ�Ŀ¼�������ظ���ѡ���һ������
				if (levelpara.levelsloaded.Contains(currentName) && !levelpara.levelsName.Contains(currentName))
				{
					UnLoadLevelbyName(levelpara, currentName);
				}
				if (!levelpara.levelsloaded.Contains(currentName))
				{
					SetSliceMapOffset(levelpara, lonName, latName);
					LoadLevelbyName(levelpara, currentName, longPackageName, bSuccess);
				}
			}

		}
	}

	ComputeLOD(levelpara);
	if (levelpara.preLOD != levelpara.LOD)
	{
		int32 levelnum = levelpara.levelsloaded.Num();
		levelpara.isFirstFrame = true;
		for (int32 i = levelnum - 1; i >= 0; --i)
		{
			FString currentName = levelpara.levelsloaded[i];
			UnLoadLevelbyName(levelpara, currentName);
		}
	}
}
*/

void AMyCharacter::LoadLevelbyCameraInfo()
{
	UWorld* World = GetWorld();
	GetCameraInfo(levelpara);
	GetFovMaxDistance(levelpara);

	if (!GetFocusPosition(levelpara))
	{
		return;
	}
	ComputeLOD(levelpara);

	levelpara.waitforload.Empty();
	levelpara.waitforunload.Empty();
	if (levelpara.isFirstFrame)
	{
		levelpara.levelsloaded.Empty();
		levelpara.preLOD = levelpara.LOD;
		levelpara.isFirstFrame = false;
	}
	else
	{
		for (int32 num = 0; num < levelpara.levelsloaded.Num(); ++num)
		{
			levelpara.waitforload.Add(levelpara.levelsloaded[num]);
		}
	}

	bool bSuccess;
	GetLevelFile(levelpara);
	for (int32 lon = 0; lon < levelpara.sliceofPitch; ++lon)
	{
		for (int32 lat = 0; lat < levelpara.sliceofRoll; ++lat)
		{
			FString filepath = levelpara.levelsName[lon * levelpara.sliceofRoll + lat];
			FString levelmapPath = RootPath + filepath + ".umap";
			int32 lonName = levelpara.lonName[lon];
			int32 latName = levelpara.latName[lat];
			FString currentName = levelpara.levelsName[lon * levelpara.sliceofRoll + lat];

			FString longPackageName = "/Game/" + FString::FromInt(levelpara.LOD) + "/" + FString::FromInt(lonName) + "/u" + FString::FromInt(latName);

			//currentName = FString::FromInt(2) + R"(\)" + FString::FromInt(5) + R"(\u)" + FString::FromInt(2);
			//longPackageName = "/Game/" + FString::FromInt(2) + "/" + FString::FromInt(5) + "/u" + FString::FromInt(2);


			if (FPaths::FileExists(levelmapPath))
			{
				// UE�ĳ����Ƶ��޶�Ϊ��/����������������Ƶ�Ŀ¼�������ظ���ѡ���һ������
				// �����ǰδ���أ����ǵȴ����ص��¹ؿ������ȼ���
				if (!levelpara.levelsloaded.Contains(currentName) && levelpara.waitforload.Contains(currentName))
				{
					SetSliceMapOffset(levelpara, lonName, latName);
					LoadLevelbyName(levelpara, currentName, longPackageName, bSuccess);
					//World->FlushLevelStreaming(); // ȷ�����سɹ�����֤���غ���ж��
				}
				// �����ǰδ���أ������
				else if (!levelpara.levelsloaded.Contains(currentName))
				{
					SetSliceMapOffset(levelpara, lonName, latName);
					LoadLevelbyName(levelpara, currentName, longPackageName, bSuccess);
				}
			}
			else
			{
				levelpara.waitforload.Remove(currentName);
			}
		}

		World->FlushLevelStreaming(); // ȷ�����سɹ�����֤���غ���ж��

		// ж��ȫ��֮ǰ�Ĺؿ�
		for (int32 num = 0; num < levelpara.levelsloaded.Num(); ++num)
		{
			levelpara.waitforunload.Add(levelpara.levelsloaded[num]);
		}
		for (int32 num = 0; num < levelpara.waitforunload.Num(); ++num)
		{
			FString currentName = levelpara.waitforunload[num];
			// �����ǰ�ؿ�û�У����Ѿ������ˣ���Ҫж��
			if (!levelpara.levelsName.Contains(currentName) && levelpara.levelsloaded.Contains(currentName))
			{
				UnLoadLevelbyName(levelpara, currentName);
				levelpara.levelsloaded.Remove(currentName);
			}
		}
	}
}

/*
void AMyCharacter::LoadLevelbyCameraInfo()
{
	UWorld* World = GetWorld();
	GetCameraInfo(levelpara);
	GetFovMaxDistance(levelpara);

	if (!GetFocusPosition(levelpara))
	{
		return;
	}
	ComputeLOD(levelpara);

	levelpara.levelInfo.waitforload.Empty();
	levelpara.levelInfo.waitforunload.Empty();
	if (levelpara.isFirstFrame)
	{
		levelpara.levelInfo.levelsloaded.Empty();
		levelpara.preLOD = levelpara.LOD;
		levelpara.isFirstFrame = false;
	}
	else
	{
		for (int32 num = 0; num < levelpara.levelInfo.levelsloaded.Num(); ++num)
		{
			levelpara.levelInfo.waitforload.Add(levelpara.levelInfo.levelsloaded[num]);
		}
	}

	bool bSuccess;
	GetLevelFile(levelpara);
	for (int32 num = 0; num < levelpara.levelInfoMap.Num(); ++num)
	{
		int32 lodName = levelpara.levelInfoMap[num].lodName;
		int32 lonName = levelpara.levelInfoMap[num].lonName;
		int32 latName = levelpara.levelInfoMap[num].latName;
	
		FString currentName = levelpara.levelInfo.levelsName[num];
		levelpara.levelInfo.waitforload.Add(currentName);
		FString levelmapPath = RootPath + currentName + ".umap";
		FString longPackageName = "/Game/" + FString::FromInt(lodName) + "/" + FString::FromInt(lonName) + "/u" + FString::FromInt(latName);
		if (FPaths::FileExists(levelmapPath))
		{
			// UE�ĳ����Ƶ��޶�Ϊ��/����������������Ƶ�Ŀ¼�������ظ���ѡ���һ������
			// �����ǰδ���أ����ǵȴ����ص��¹ؿ������ȼ���
			if (!levelpara.levelInfo.levelsloaded.Contains(currentName) && levelpara.levelInfo.waitforload.Contains(currentName))
			{
				SetSliceMapOffset(levelpara, levelpara.levelInfoMap[num]);
				LoadLevelbyName(levelpara, currentName, longPackageName, bSuccess);
				//World->FlushLevelStreaming(); // ȷ�����سɹ�����֤���غ���ж��
			}
			// �����ǰδ���أ������
			else if (!levelpara.levelInfo.levelsloaded.Contains(currentName))
			{
				SetSliceMapOffset(levelpara, levelpara.levelInfoMap[num]);
				LoadLevelbyName(levelpara, currentName, longPackageName, bSuccess);
			}
		}
		else
		{
			levelpara.levelInfo.waitforload.Remove(currentName);
		}
	}

	World->FlushLevelStreaming(); // ȷ�����سɹ�����֤���غ���ж��

	// ж��ȫ��֮ǰ�Ĺؿ�
	for (int32 num = 0; num < levelpara.levelInfo.levelsloaded.Num(); ++num)
	{
		levelpara.levelInfo.waitforunload.Add(levelpara.levelInfo.levelsloaded[num]);
	}
	for (int32 num = 0; num < levelpara.levelInfo.waitforunload.Num(); ++num)
	{
		FString currentName = levelpara.levelInfo.waitforunload[num];
		// �����ǰ�ؿ�û�У����Ѿ������ˣ���Ҫж��
		if (!levelpara.levelInfo.levelsName.Contains(currentName) && levelpara.levelInfo.levelsloaded.Contains(currentName))
		{
			UnLoadLevelbyName(levelpara, currentName);
			levelpara.levelInfo.levelsloaded.Remove(currentName);
		}
	}


}*/
