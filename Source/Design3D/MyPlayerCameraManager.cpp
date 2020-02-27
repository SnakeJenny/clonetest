// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCameraManager.h"
#include "MyCharacter.h"

void AMyPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	/*if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
	{
		return;
	}

	FMinimalViewInfo OrigPOV = OutVT.POV;
	OutVT.POV.FOV = DefaultFOV;
	OutVT.POV.OrthoWidth = DefaultOrthoWidth;
	OutVT.POV.bConstrainAspectRatio = false;
	OutVT.POV.ProjectionMode = this->bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
	OutVT.POV.PostProcessBlendWeight = 1.0f;
	bool bDoNotApplyModifiers = false;

	ABZGame_PlayerController* BZGame_Controller = Cast<ABZGame_PlayerController>(GetOwningPlayerController());
	if (BZGame_Controller != NULL)
	{
		UCameraComponent* ViewCam = BZGame_Controller->GetViewCamera();
		OutVT.POV.Location = ViewCam->GetComponentLocation();
		OutVT.POV.Rotation = ViewCam->GetComponentRotation();
		OutVT.POV.FOV = ViewCam->FieldOfView;
		OutVT.POV.AspectRatio = ViewCam->AspectRatio;
		OutVT.POV.bConstrainAspectRatio = ViewCam->bConstrainAspectRatio;
		OutVT.POV.ProjectionMode = ViewCam->ProjectionMode;
		OutVT.POV.OrthoWidth = ViewCam->OrthoWidth;
		OutVT.POV.PostProcessBlendWeight = ViewCam->PostProcessBlendWeight;

		if (BZGame_Controller->GetViewCamera()->PostProcessBlendWeight > 0.0f)
		{
			OutVT.POV.PostProcessSettings = ViewCam->PostProcessSettings;
		}

		if (!bDoNotApplyModifiers || this->bAlwaysApplyModifiers)
		{
			ApplyCameraModifiers(DeltaTime, OutVT.POV);
		}

		SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);
		UpdateCameraLensEffects(OutVT);
	}
	else
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
	}*/
	//if (bFollowHmdOrientation)
	//{
	//	//�����VR�豸��ֱ����������ԭʼ����
	//	Super::UpdateViewTarget(OutVT, DeltaTime);
	//	return;
	//}
	//// Don't update outgoing viewtarget during an interpolation
	//if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
	//{
	//	return;
	//}
	//bool bDoNotApplyModifiers = false;

	//if (ACameraActor* CamActor = Cast<ACameraActor>(OutVT.Target))
	//{
	//	// Viewing through a camera actor.
	//	CamActor->GetCameraComponent()->GetCameraView(DeltaTime, OutVT.POV);
	//}
	//else
	//{
	//	if (CameraStyle == FName("SceneFixed"))
	//	{��������������//�����Ҹо�������PendingViewTarget������������������ת�ȽϺã����ǻ�û���Թ�
	//		//�Լ�����һ�������̶��ӽ�
	//		ADemoCharacter *Character = Cast<ADemoCharacter>(GetOwningPlayerController()->GetPawn());
	//		OutVT.POV.Location = Character->ViewTargetLocation;
	//		OutVT.POV.Rotation = Character->ViewTargetRotator;
	//		//DesiredView.FOV = FieldOfView;
	//		//DesiredView.AspectRatio = AspectRatio;
	//		// don't apply modifiers when using this debug camera mode
	//		bDoNotApplyModifiers = true;
	//	}
	//	else if (CameraStyle == FName("Default"))
	//	{
	//		//Ĭ�Ϸ�ʽ��ֱ��ȡ��������Ĳ���������FTViewTarget.pov,��������������ࡢSpringArm���ơ�
	//		UpdateViewTargetInternal(OutVT, DeltaTime);
	//	}
	//	else
	//	{
	//		Super::UpdateViewTarget(OutVT, DeltaTime);
	//	}
	//}
	//if (!bDoNotApplyModifiers || bAlwaysApplyModifiers)
	//{
	//	// Apply camera modifiers at the end (view shakes for example)
	//	ApplyCameraModifiers(DeltaTime, OutVT.POV);
	//}
	//// Synchronize the actor with the view target results
	//SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);
	//UpdateCameraLensEffects(OutVT);
}