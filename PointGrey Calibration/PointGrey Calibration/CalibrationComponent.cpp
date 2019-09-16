#include "CalibrationComponent.h"
#include "CheckerboardComponent.h"
#include "opencv2/calib3d.hpp"
#include "opencv2/aruco/charuco.hpp"
#include "opencv2/imgproc.hpp"

#include <ctime>

/** Calibration 정보를 Update 합니다. */
void UCalibrationComponent::UpdateCalibrationInfo(class UCameraComponent* CameraComponent, std::string FileName, class UCheckerboardComponent* CheckerboardComponent)
{
	bool bRefindStrategy = true;

	Ptr<aruco::DetectorParameters> DetectorParams = CheckerboardComponent->GetDetectorParams();
	Ptr<aruco::Dictionary> Dictionary = CheckerboardComponent->GetDictionary();

	Ptr<aruco::CharucoBoard> Charucoboard = CheckerboardComponent->GetCharucoBoard();
	Ptr<aruco::Board> Board = CheckerboardComponent->GetBoard();

	// Collect data from each frame
	std::vector< std::vector< std::vector< Point2f > > > AllCorners;
	std::vector< std::vector< int > > AllIds;
	std::vector< Mat > AllImgs;
	Size ImgSize;

	int CalibrationFlags = 0;
	float AspectRatio = 1;

	while (true)
	{
		Mat Image;
		Mat CopyImage;

		std::vector< int > Ids;
		std::vector< std::vector< Point2f > > Corners, Rejected;

		CameraComponent->UpdateFrame();
		Image = CameraComponent->GetFrame();

		aruco::detectMarkers(Image, Dictionary, Corners, Ids, DetectorParams, Rejected);

		// Refind strategy to detect more markers
		if (bRefindStrategy)
		{
			aruco::refineDetectedMarkers(Image, Board, Corners, Ids, Rejected);
		}

		// Interpolate charuco corners
		Mat CurrentCharucoCorners, CurrentCharucoIds;
		if (Ids.size() > 0)
		{
			aruco::interpolateCornersCharuco(Corners, Ids, Image, Charucoboard, CurrentCharucoCorners, CurrentCharucoIds);
		}

		// Draw results
		Image.copyTo(CopyImage);
		if (Ids.size() > 0)
		{
			aruco::drawDetectedMarkers(CopyImage, Corners);
		}

		if (CurrentCharucoCorners.total() > 0)
		{
			aruco::drawDetectedCornersCharuco(CopyImage, CurrentCharucoCorners, CurrentCharucoIds);
		}

		putText(CopyImage, "Press 'c' to add current frame. 'ESC' to finish and calibrate", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);

		imshow("Frame", CopyImage);

		char key = (char)waitKey(10);
		if (key == 27) break;
		if (key == 'c' && Ids.size() > 4)
		{
			std::cout << "Frame captured" << std::endl;
			AllCorners.push_back(Corners);
			AllIds.push_back(Ids);
			AllImgs.push_back(Image);
			ImgSize = Image.size();
		}
	}

	if (AllIds.size() < 1)
	{
		std::cerr << "Not enough captures for calibration" << std::endl;
		return;
	}

	Mat CameraMatrix, DistCoeffs;
	std::vector< Mat > Rvecs, Tvecs;
	double RepError;

	if (CalibrationFlags & CALIB_FIX_ASPECT_RATIO)
	{
		CameraMatrix = Mat::eye(3, 3, CV_64F);
		CameraMatrix.at< double >(0, 0) = AspectRatio;
	}

	// Prepare data for calibration
	std::vector< std::vector< Point2f > > AllCornersConcatenated;
	std::vector< int > AllIdsConcatenated;
	std::vector< int > MarkerCounterPerFrame;
	MarkerCounterPerFrame.reserve(AllCorners.size());
	for (unsigned int Row = 0; Row < AllCorners.size(); Row++)
	{
		MarkerCounterPerFrame.push_back((int)AllCorners[Row].size());
		for (unsigned int Col = 0; Col < AllCorners[Row].size(); Col++)
		{
			AllCornersConcatenated.push_back(AllCorners[Row][Col]);
			AllIdsConcatenated.push_back(AllIds[Row][Col]);
		}
	}

	// Calibrate camera using aruco markers
	double ArucoRepErr;
	ArucoRepErr = aruco::calibrateCameraAruco(AllCornersConcatenated, AllIdsConcatenated, MarkerCounterPerFrame, Board, ImgSize, CameraMatrix, DistCoeffs, noArray(), noArray(), CalibrationFlags);

	// Prepare data for charuco calibration
	int NFrames = (int)AllCorners.size();
	std::vector< Mat > AllCharucoCorners;
	std::vector< Mat > AllCharucoIds;
	std::vector< Mat > FilteredImages;
	AllCharucoCorners.reserve(NFrames);
	AllCharucoIds.reserve(NFrames);

	for (int i = 0; i < NFrames; i++)
	{
		// Interpolate using camera parameters
		Mat CurrentCharucoCorners, CurrentCharucoIds;
		aruco::interpolateCornersCharuco(AllCorners[i], AllIds[i], AllImgs[i], Charucoboard, CurrentCharucoCorners, CurrentCharucoIds, CameraMatrix, DistCoeffs);

		AllCharucoCorners.push_back(CurrentCharucoCorners);
		AllCharucoIds.push_back(CurrentCharucoIds);
		FilteredImages.push_back(AllImgs[i]);
	}

	if (AllCharucoCorners.size() < 4)
	{
		std::cerr << "Not enough corners for calibration" << std::endl;
		return;
	}

	// Calibrate camera using charuco
	RepError = aruco::calibrateCameraCharuco(AllCharucoCorners, AllCharucoIds, Charucoboard, ImgSize, CameraMatrix, DistCoeffs, Rvecs, Tvecs, CalibrationFlags);

	bool bSaveOk = SaveCameraParameters(FileName, ImgSize, AspectRatio, CalibrationFlags, CameraMatrix, DistCoeffs, RepError);
	if (!bSaveOk)
	{
		std::cerr << "Cannot save output file" << std::endl;
		return;
	}

	std::cout << "Rep Error: " << RepError << std::endl;
	std::cout << "Rep Error Aruco: " << ArucoRepErr << std::endl;
	std::cout << "Calibration saved to " << FileName << std::endl;

	return;
}

/** Camera의 Parameter 정보를 입력받은 뒤, 저장합니다. */
bool UCalibrationComponent::SaveCameraParameters(const std::string &Filename, Size ImageSize, float AspectRatio, int Flags, const Mat &CameraMatrix, const Mat &DistCoeffs, double TotalAvgErr)
{
	FileStorage FS(Filename, FileStorage::WRITE);
	if (!FS.isOpened())
	{
		return false;
	}

	time_t SystemTime;
	time(&SystemTime);

	struct tm *LocalTime = localtime(&SystemTime);
	char Buffer[1024];
	strftime(Buffer, sizeof(Buffer) - 1, "%c", LocalTime);

	FS << "calibration_time" << Buffer;

	FS << "image_width" << ImageSize.width;
	FS << "image_height" << ImageSize.height;

	if (Flags & CALIB_FIX_ASPECT_RATIO) FS << "aspectRatio" << AspectRatio;

	if (Flags != 0)
	{
		sprintf(Buffer, "flags: %s%s%s%s",
			Flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
			Flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
			Flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
			Flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
	}

	FS << "flags" << Flags;

	FS << "camera_matrix" << CameraMatrix;
	FS << "distortion_coefficients" << DistCoeffs;

	FS << "avg_reprojection_error" << TotalAvgErr;

	return true;
}