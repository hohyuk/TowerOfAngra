// Fill out your copyright notice in the Description page of Project Settings.

#include "HowlingCameraEffect.h"


UHowlingCameraEffect::UHowlingCameraEffect()
{
	//Set Duration Skaing Camera
	OscillationDuration = 1.f;

	//Set Shake Rotation Pitch
	RotOscillation.Pitch.Amplitude = 5.f;
	RotOscillation.Pitch.Frequency = 50.f;

	//Set Shake Rotaion Yaw
	RotOscillation.Yaw.Amplitude = 5.f;
	RotOscillation.Yaw.Frequency = 20.f;

	LocOscillation.X.Amplitude = 20.f;
	LocOscillation.X.Frequency = 5.f;
	LocOscillation.X.InitialOffset = EOO_OffsetZero;

	LocOscillation.Z.Amplitude = 10.f;
	LocOscillation.Z.Frequency = 20.f;
	LocOscillation.Z.InitialOffset = EOO_OffsetZero;


}

