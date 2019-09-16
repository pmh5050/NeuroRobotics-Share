#include <iostream>
#include "CameraComponent.h"
#include "CheckerboardComponent.h"
#include "CalibrationComponent.h"
#include "VisionEngine.h"

using namespace std;

int main()
{
	UCameraComponent CameraComponent(ECameraInc::PointGrey, 0); // 0¹øÂ° Point Grey Camera Handler
	UCheckerboardComponent CheckerboardComponent(7, 9, 0.03f, 0.015f, 2);
	UCalibrationComponent::UpdateCalibrationInfo(&CameraComponent, "PointGrey.xml", &CheckerboardComponent);

	return 0;

}