#pragma once

#include "CameraComponent.h"

class UCalibrationComponent
{
public:
	/** Calibration ������ Update �մϴ�. */
	static void UpdateCalibrationInfo(class UCameraComponent* CameraComponent, std::string FileName, class UCheckerboardComponent* CheckerboardComponent);
	/** Camera�� Parameter ������ �Է¹��� ��, �����մϴ�. */
	static bool SaveCameraParameters(const std::string &Filename, Size ImageSize, float AspectRatio, int Flags, const Mat &CameraMatrix, const Mat &DistCoeffs, double TotalAvgErr);

};

