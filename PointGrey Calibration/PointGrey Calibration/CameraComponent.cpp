#include "CameraComponent.h"

#define RS_WIDTH 640
#define RS_HEIGHT 480
/** Camera 회사 종류와 Id를 입력받아 CameraComponent를 생성합니다. */
UCameraComponent::UCameraComponent(ECameraInc CameraInc, int Id)
{
	Inc = CameraInc;

	int DropFrameCount = 0;

	switch (CameraInc)
	{
		case ECameraInc::PointGrey:
			FError = FBusManager.GetCameraFromIndex(Id, &FPGRGuid);
			FError = FCamera.Connect(&FPGRGuid);
			if (FError != FlyCapture2::PGRERROR_OK)
			{
				std::cout << "Failed to connect to camera" << std::endl;
			}
			FError = FCamera.GetCameraInfo(&FCameraInfo);
			if (FError != FlyCapture2::PGRERROR_OK)
			{
				std::cout << "Failed to get camera info from camera" << std::endl;
			}
			std::cout << FCameraInfo.vendorName << " " << FCameraInfo.modelName << " " << FCameraInfo.serialNumber << std::endl;

			FError = FCamera.StartCapture();
			if (FError == FlyCapture2::PGRERROR_ISOCH_BANDWIDTH_EXCEEDED)
			{
				std::cout << "Bandwidth exceeded" << std::endl;
			}
			else if (FError != FlyCapture2::PGRERROR_OK)
			{
				std::cout << "Failed to start image capture" << std::endl;
			}

			break;
		case ECameraInc::RealSense:
			Config.enable_stream(RS2_STREAM_COLOR, RS_WIDTH, RS_HEIGHT, RS2_FORMAT_BGR8, 30); // To resize solution, change (640, 480) to (Width, Height)
			Pipe.start(Config);
			// Drop several first frames  to let auto-exposure stabilize
			for (DropFrameCount = 0; DropFrameCount < 30; DropFrameCount++)
			{
				RealSenseFrames = Pipe.wait_for_frames();
			}
			break;
		case ECameraInc::Sony:
			// falls through
		default:
			CameraHandler.open(Id);
			break;
	}

}

/** 생성한 CameraComponent를 반납합니다. */
UCameraComponent::~UCameraComponent()
{
	switch (Inc)
	{
	case ECameraInc::PointGrey:
		FError = FCamera.StopCapture();
		if (FError != FlyCapture2::PGRERROR_OK)
		{
			// This may fail when the camera was removed, so don't show an error message
		}
		FCamera.Disconnect();
		break;
	case ECameraInc::RealSense:
		Pipe.stop();
		break;
	case ECameraInc::Sony:
		// falls through
	default:
		CameraHandler.release();
		break;
	}

}


/** CameraComponent로부터 현재 Frame을 Update 합니다. */
void UCameraComponent::UpdateFrame()
{
	unsigned int RowBytes;

	switch (Inc)
	{
	case ECameraInc::PointGrey:
		FError = FCamera.RetrieveBuffer(&FImage);
		if (FError != FlyCapture2::PGRERROR_OK)
		{
			std::cout << "Capture error!" << std::endl;
		}
		FImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &ConvertedImage);
		RowBytes = (double)ConvertedImage.GetReceivedDataSize() / (double)ConvertedImage.GetRows();
		Frame = cv::Mat(ConvertedImage.GetRows(), ConvertedImage.GetCols(), CV_8UC3, ConvertedImage.GetData(), RowBytes).clone();
		break;
	case ECameraInc::RealSense:
		RealSenseFrames = Pipe.wait_for_frames();
		RealSenseColorFrame = RealSenseFrames.get_color_frame();
		Frame = Mat(Size(RS_WIDTH, RS_HEIGHT), CV_8UC3, (void*)RealSenseColorFrame.get_data(), Mat::AUTO_STEP);
		Frame = Frame.clone();
		break;

	case ECameraInc::Sony:
		// falls through
	default:
		CameraHandler.read(Frame);
		Frame = Frame.clone();
		break;
	}

}

/** 현재 Frame을 반환합니다. */
Mat UCameraComponent::GetFrame()
{
	return Frame.clone();

}