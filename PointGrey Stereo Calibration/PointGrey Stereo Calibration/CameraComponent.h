#pragma once

#include "VisionEngine.h"
#include "FlyCapture2.h"

#include <iostream>

enum ECameraInc
{
	PointGrey, Sony, Common
};

class UCameraComponent
{
public:
	/** Camera 회사 종류와 Id를 입력받아 CameraComponent를 생성합니다. */
	UCameraComponent(ECameraInc CameraInc, int Id);
	/** 생성한 CameraComponent를 반납합니다. */
	~UCameraComponent();
	/** File name을 입력받아 카메라의 내부 파라미터 및 왜곡 계수를 설정합니다 */
	void ReadCameraParameters(std::string FileName);
	/** CameraComponent로부터 현재 Frame을 Update 합니다. */
	void UpdateFrame();
	/** 현재 Frame을 반환합니다. */
	Mat GetFrame();
	/** 내부 파라미터를 반환합니다. */
	Mat GetIntrinsicParams();
	/** 왜곡 계수를 반환합니다. */
	Mat GetDistortionParams();
	/** 초점 거리를 반환합니다. */
	FVector2D GetFocalLength();
	/** 주점을 반환합니다. */
	FVector2D GetPrincipalPoint();

private:
	FlyCapture2::Error FError;
	FlyCapture2::Camera FCamera;
	FlyCapture2::CameraInfo FCameraInfo;
	FlyCapture2::Image FImage;
	FlyCapture2::Image ConvertedImage;
	FlyCapture2::BusManager FBusManager;
	FlyCapture2::PGRGuid FPGRGuid;

	VideoCapture CameraHandler; // OpenCV Web Camera handler

	Mat IntrinsicParams; // 내부 파라미터
	Mat DistortionParams; // 왜곡 계수

	FVector2D FocalLength; // 초점 거리
	FVector2D PrincipalPoint; // 주점

	ECameraInc Inc; // 제조사 종류

	Mat Frame;

};

