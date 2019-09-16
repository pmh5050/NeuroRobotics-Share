#pragma once

#include "VisionEngine.h"
#include "FlyCapture2.h"

#include <iostream>

enum ECameraInc
{
	PointGrey, Common
};

class UCameraComponent
{
public:
	/** Camera 회사 종류와 Id를 입력받아 CameraComponent를 생성합니다. */
	UCameraComponent(ECameraInc CameraInc, int Id);
	/** 생성한 CameraComponent를 반납합니다. */
	~UCameraComponent();
	/** CameraComponent로부터 현재 Frame을 Update 합니다. */
	void UpdateFrame();
	/** 현재 Frame을 반환합니다. */
	Mat GetFrame();

private:
	FlyCapture2::Error FError;
	FlyCapture2::Camera FCamera;
	FlyCapture2::CameraInfo FCameraInfo;
	FlyCapture2::Image FImage;
	FlyCapture2::Image ConvertedImage;
	FlyCapture2::BusManager FBusManager;
	FlyCapture2::PGRGuid FPGRGuid;

	ECameraInc Inc; // 제조사 종류

	Mat Frame;

};

