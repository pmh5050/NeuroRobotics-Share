#include "CameraComponent.h"

/** Camera 회사 종류와 Id를 입력받아 CameraComponent를 생성합니다. */
UCameraComponent::UCameraComponent(ECameraInc CameraInc, int Id)
{
	Inc = CameraInc;

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
	case ECameraInc::Sony:
		// falls through
	default:
		CameraHandler.release();
		break;
	}

}

/** File name을 입력받아 카메라의 내부 파라미터 및 왜곡 계수를 설정합니다 */
void UCameraComponent::ReadCameraParameters(std::string FileName)
{
	FileStorage File(FileName, FileStorage::READ);
	if (!File.isOpened())
	{
		std::cout << "File system error detected!" << std::endl;
		return;
	}

	File["camera_matrix"] >> IntrinsicParams;
	File["distortion_coefficients"] >> DistortionParams;

	double* MatrixAccessor;

	MatrixAccessor = IntrinsicParams.ptr<double>(0);
	FocalLength.X = MatrixAccessor[0];
	PrincipalPoint.X = MatrixAccessor[2];

	MatrixAccessor = IntrinsicParams.ptr<double>(1);
	FocalLength.Y = MatrixAccessor[1];
	PrincipalPoint.Y = MatrixAccessor[2];

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

/** 내부 파라미터를 반환합니다. */
Mat UCameraComponent::GetIntrinsicParams()
{
	return IntrinsicParams.clone();

}

/** 왜곡 계수를 반환합니다. */
Mat UCameraComponent::GetDistortionParams()
{
	return DistortionParams.clone();

}

/** 초점 거리를 반환합니다. */
FVector2D UCameraComponent::GetFocalLength()
{
	return FocalLength;

}

/** 주점을 반환합니다. */
FVector2D UCameraComponent::GetPrincipalPoint()
{
	return PrincipalPoint;

}