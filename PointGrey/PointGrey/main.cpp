#include <iostream>
#include "CameraComponent.h"
#include "VisionEngine.h"

using namespace std;

int main()
{
	UCameraComponent CameraComponent(ECameraInc::PointGrey, 0); // 0¹øÂ° Point Grey Camera Handler
	Mat Frame;
	cout << "Hello World!" << endl;

	while (true)
	{
		CameraComponent.UpdateFrame();
		Frame = CameraComponent.GetFrame();
		imshow("Image", Frame);
		char Key = cvWaitKey(10);
		if (Key == 'q' || Key == 'Q')
		{
			break;
		}
	}

	return 0;

}