#include <iostream>
#include <time.h>
#include "CameraComponent.h"
#include "CheckerboardComponent.h"
#include "CalibrationComponent.h"
#include "VisionEngine.h"

using namespace std;

UCameraComponent LeftCamera(ECameraInc::PointGrey, 0); // 0번째 Point Grey Camera Handler
UCameraComponent RightCamera(ECameraInc::PointGrey, 1); // 1번째 Point Grey Camera Handler
UCheckerboardComponent CheckerboardComponent(7, 7, 0.03f, 0.015f, 6); // 검출 대상이 되는 Checkerboard에 해당합니다.

Ptr<aruco::Board> Board = CheckerboardComponent.GetBoard();
Ptr<aruco::Dictionary> Dictionary = CheckerboardComponent.GetDictionary();
Ptr<aruco::DetectorParameters> DetectorParams = CheckerboardComponent.GetDetectorParams();
Ptr<aruco::CharucoBoard> CharucoBoard = CheckerboardComponent.GetCharucoBoard();

/** Vector를 입력받아 구성 성분을 출력합니다. */
template <typename T>
void PrintVector(std::vector<T>& Vector)
{
	cout << "[";
	int Idx = 0;
	for (const auto& Elem : Vector)
	{
		cout << Idx++ << " ";
		
		cout << Elem << endl;
	}
	cout << "]" << endl;

}

/** 카메라 Frame을 입력받아 검출된 모든 Marker Id를 출력합니다. */
void PrintDetectedIds(Mat& Frame, UCameraComponent& Camera)
{
	vector<int> MarkerIds, Ids;
	vector<vector<Point2f>> MarkerCorners, Rejected;
	vector<Point2f> Corners;
	Mat IntrinsicParams = Camera.GetIntrinsicParams();
	Mat DistortionParams = Camera.GetDistortionParams();

	aruco::detectMarkers(Frame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, Frame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
	}
	PrintVector(Ids);
}

/** 카메라 Frame을 입력받아 검출된 모든 Marker Id와 위치를 출력합니다. */
void PrintDetectedIdsAndPts(Mat& Frame, UCameraComponent& Camera)
{
	vector<int> MarkerIds, Ids;
	vector<vector<Point2f>> MarkerCorners, Rejected;	
	vector<Point2f> Corners;
	Mat IntrinsicParams = Camera.GetIntrinsicParams();
	Mat DistortionParams = Camera.GetDistortionParams();

	aruco::detectMarkers(Frame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, Frame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
	}
	PrintVector(Ids);
	PrintVector(Corners);
}

/** 입력된 이미지로부터 Checkerboard를 검출한 뒤 모서리 정보를 입력받은 Vector에 기록합니다. */
void DetectAndInsertCorners(vector<vector<Point3f>>& InObjPts, vector<vector<Point2f>>& InLeftImgPts, vector<vector<Point2f>>& InRightImgPts, Mat& LeftFrame, Mat& RightFrame)
{
	vector<int> MarkerIds, Ids;
	vector<vector<Point2f>> MarkerCorners, Rejected;
	vector<Point2f> Corners;
	Mat IntrinsicParams; 
	Mat DistortionParams; 

	vector<Point3f> ObjPts;
	
	// 좌측 카메라
	IntrinsicParams = LeftCamera.GetIntrinsicParams();
	DistortionParams = LeftCamera.GetDistortionParams();
	aruco::detectMarkers(LeftFrame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, LeftFrame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
		InLeftImgPts.push_back(Corners);
	}

	// 출력한 Checkerboard의 정보를 기반으로 Object Point를 계산한 뒤, 기록합니다.
	for (int Id : Ids)
	{
		Vec3d ObjPoint = CheckerboardComponent.GetCornerPosition(Id);
		ObjPts.push_back(Point3f(ObjPoint[0], ObjPoint[1], ObjPoint[2]));
	}
	InObjPts.push_back(ObjPts);

	// 우측 카메라
	IntrinsicParams = RightCamera.GetIntrinsicParams();
	DistortionParams = RightCamera.GetDistortionParams();
	aruco::detectMarkers(RightFrame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, RightFrame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
		InRightImgPts.push_back(Corners);
	}
}

/** Camera의 Parameter 정보를 입력받은 뒤, 저장합니다. */
bool SaveStereoCameraParameters(const std::string &Filename, const Mat &CamMat1, const Mat &DistCoeffs1, const Mat &CamMat2, const Mat &DistCoeffs2, const Mat &Rot, const Mat &Trs, const Mat &Essential, const Mat &Fund, double TotalAvgErr)
{
	// (AllObjPoints, LeftImgPts, RightImgPts, CamMat1, DistCoeffs1, CamMat2, DistCoeffs2, ImgSize, R, T, E, F, CV_CALIB_USE_INTRINSIC_GUESS);
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

	FS << "camera_matrix1" << CamMat1;
	FS << "distortion_coefficients1" << DistCoeffs1;

	FS << "camera_matrix2" << CamMat2;
	FS << "distortion_coefficients2" << DistCoeffs2;

	
	FS << "rot" << Rot;
	FS << "trs" << Trs;
	
	FS << "avg_reprojection_error" << TotalAvgErr;
	
	return true;
}

/** 좌/우 카메라 프레임 Vector를 입력받아 Stereo camera calibration을 수행합니다. */
void CalibrateCameras(vector<Mat>& LeftFrames, vector<Mat>& RightFrames)
{
	vector<vector<Point3f>> AllObjPoints; // Obj Points
	vector<vector<Point2f>> LeftImgPts; // Image Pt1
	vector<vector<Point2f>> RightImgPts; // Image Pt2

	// 미리 측정된 Intrinsic Parameter들에 해당합니다.
	Mat CamMat1 = LeftCamera.GetIntrinsicParams();
	Mat CamMat2 = RightCamera.GetIntrinsicParams();
	Mat DistCoeffs1 = LeftCamera.GetDistortionParams();
	Mat DistCoeffs2 = RightCamera.GetDistortionParams();

	// Intrinsic Parameter들을 추정하는데 사용되지만, CV_CALIB_FIX_INTRINSIC를 사용하므로 해당값은 참조되지 않고 인수로만 제공됩니다.
	Size ImgSize = LeftFrames[0].size();

	// Homogeneous Transform 및 Essential Matrix / Fundamental Matrix에 해당합니다.
	Mat R;
	Mat T;
	Mat E;
	Mat F;

	int DataLength = LeftFrames.size(); // 전체 Frame의 개수에 해당합니다.

	Mat Frame;
	for (int Idx = 0; Idx < DataLength; Idx++)
	{
		DetectAndInsertCorners(AllObjPoints, LeftImgPts, RightImgPts, LeftFrames[Idx], RightFrames[Idx]);
	}

	double AvgRepErr = stereoCalibrate(AllObjPoints, LeftImgPts, RightImgPts, CamMat1, DistCoeffs1, CamMat2, DistCoeffs2, ImgSize, R, T, E, F, CV_CALIB_USE_INTRINSIC_GUESS);

	SaveStereoCameraParameters("StreoCalibration2.xml", CamMat1, DistCoeffs1, CamMat2, DistCoeffs2, R, T, E, F, AvgRepErr);
}

/** 검출된 Marker의 개수를 반환합니다. */
int GetDetectedMarkerLength(Mat& Frame, UCameraComponent& Camera)
{
	vector<int> MarkerIds, Ids;
	vector<vector<Point2f>> MarkerCorners, Rejected;
	vector<Point2f> Corners;
	Mat IntrinsicParams = Camera.GetIntrinsicParams();
	Mat DistortionParams = Camera.GetDistortionParams();

	aruco::detectMarkers(Frame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, Frame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
	}
	return (int)Ids.size();
}

/** 좌/우 카메라의 프레임을 입력받아 모든 마커가 검출되었는지 유무를 반환합니다. */
bool AllMarkerDetected(Mat& LeftFrame, Mat& RightFrame)
{
	static int AllMarkerLength = CharucoBoard->chessboardCorners.size();
	if (AllMarkerLength == GetDetectedMarkerLength(LeftFrame, LeftCamera) && AllMarkerLength == GetDetectedMarkerLength(RightFrame, RightCamera))
	{
		return true;
	}

	return false;
}

int main()
{
	Mat LeftFrame;
	Mat RightFrame;
	vector<Mat> LeftFrames;
	vector<Mat> RightFrames;
	LeftCamera.ReadCameraParameters("PointGreyColor.xml");
	RightCamera.ReadCameraParameters("PointGreyIR.xml");

	bool bExit = false;
	while (true)
	{
		LeftCamera.UpdateFrame();
		RightCamera.UpdateFrame();
		LeftFrame = LeftCamera.GetFrame();
		RightFrame = RightCamera.GetFrame();

		if (LeftFrame.data == 0 || RightFrame.data == 0) // 좌/우 카메라 프레임이 하나라도 존재하지 않는 경우
		{
			continue; // 반복문의 처음으로 회귀
		}
		
		// 좌/우 카메라 프레임을 출력합니다.
		imshow("L", LeftFrame);
		imshow("R", RightFrame);

		char Key = cv::waitKey(10);
		switch (Key)
		{
		case 'q': // Exit 명령
			bExit = true;			
			break;
		case 'c': // Capture 명령
			if (AllMarkerDetected(LeftFrame, RightFrame)) // 모든 마커가 검출되었을 경우
			{
				// 좌/우 카메라 프레임을 Vector에 기록합니다.
				LeftFrames.push_back(LeftFrame);
				RightFrames.push_back(RightFrame);
				cout << "Capture Size : " << LeftFrames.size() << endl;
			}
			else
			{
				cout << "Missed Corner, try again" << endl;
			}
			break;
		case 'd':
			PrintDetectedIds(LeftFrame, LeftCamera);
			break;
		default: // 예약되지 않은 명령
			break;
		}

		if (bExit) // Exit 명령이 수행되었을 경우
		{
			break;
		}

	}
	CalibrateCameras(LeftFrames, RightFrames);

	return 0;
}