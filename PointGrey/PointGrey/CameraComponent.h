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
	/** Camera ȸ�� ������ Id�� �Է¹޾� CameraComponent�� �����մϴ�. */
	UCameraComponent(ECameraInc CameraInc, int Id);
	/** ������ CameraComponent�� �ݳ��մϴ�. */
	~UCameraComponent();
	/** CameraComponent�κ��� ���� Frame�� Update �մϴ�. */
	void UpdateFrame();
	/** ���� Frame�� ��ȯ�մϴ�. */
	Mat GetFrame();

private:
	FlyCapture2::Error FError;
	FlyCapture2::Camera FCamera;
	FlyCapture2::CameraInfo FCameraInfo;
	FlyCapture2::Image FImage;
	FlyCapture2::Image ConvertedImage;
	FlyCapture2::BusManager FBusManager;
	FlyCapture2::PGRGuid FPGRGuid;

	ECameraInc Inc; // ������ ����

	Mat Frame;

};

