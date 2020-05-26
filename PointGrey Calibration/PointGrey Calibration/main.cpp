#include <iostream>
#include "CameraComponent.h"
#include "CheckerboardComponent.h"
#include "CalibrationComponent.h"
#include "VisionEngine.h"

using namespace std;

int main()
{
	UCameraComponent CameraComponent(ECameraInc::PointGrey, 1); // 0¹øÂ° Point Grey Camera Handler
	UCheckerboardComponent CheckerboardComponent(7, 7, 0.03f, 0.015f, 6);
	UCalibrationComponent::UpdateCalibrationInfo(&CameraComponent, "PointGreyIR.xml", &CheckerboardComponent);

	return 0;

}