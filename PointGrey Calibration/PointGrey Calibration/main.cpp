#include <iostream>
#include "CameraComponent.h"
#include "CheckerboardComponent.h"
#include "CalibrationComponent.h"
#include "VisionEngine.h"

using namespace std;

int main()
{
	UCameraComponent CameraComponent(ECameraInc::PointGrey, 0); // 0¹øÂ° Point Grey Camera Handler
	UCheckerboardComponent CheckerboardComponent(7, 7, 0.0214286f, 0.010714286f, 2);
	UCalibrationComponent::UpdateCalibrationInfo(&CameraComponent, "PointGrey.xml", &CheckerboardComponent);

	return 0;

}