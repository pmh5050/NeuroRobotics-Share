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
	/** Camera ȸ�� ������ Id�� �Է¹޾� CameraComponent�� �����մϴ�. */
	UCameraComponent(ECameraInc CameraInc, int Id);
	/** ������ CameraComponent�� �ݳ��մϴ�. */
	~UCameraComponent();
	/** File name�� �Է¹޾� ī�޶��� ���� �Ķ���� �� �ְ� ����� �����մϴ� */
	void ReadCameraParameters(std::string FileName);
	/** CameraComponent�κ��� ���� Frame�� Update �մϴ�. */
	void UpdateFrame();
	/** ���� Frame�� ��ȯ�մϴ�. */
	Mat GetFrame();
	/** ���� �Ķ���͸� ��ȯ�մϴ�. */
	Mat GetIntrinsicParams();
	/** �ְ� ����� ��ȯ�մϴ�. */
	Mat GetDistortionParams();
	/** ���� �Ÿ��� ��ȯ�մϴ�. */
	FVector2D GetFocalLength();
	/** ������ ��ȯ�մϴ�. */
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

	Mat IntrinsicParams; // ���� �Ķ����
	Mat DistortionParams; // �ְ� ���

	FVector2D FocalLength; // ���� �Ÿ�
	FVector2D PrincipalPoint; // ����

	ECameraInc Inc; // ������ ����

	Mat Frame;

};

