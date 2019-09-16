#pragma once

#include "CameraComponent.h"

class UCalibrationComponent
{
public:
	/** Calibration 정보를 Update 합니다. */
	static void UpdateCalibrationInfo(class UCameraComponent* CameraComponent, std::string FileName, class UCheckerboardComponent* CheckerboardComponent);
	/** Camera의 Parameter 정보를 입력받은 뒤, 저장합니다. */
	static bool SaveCameraParameters(const std::string &Filename, Size ImageSize, float AspectRatio, int Flags, const Mat &CameraMatrix, const Mat &DistCoeffs, double TotalAvgErr);

};

