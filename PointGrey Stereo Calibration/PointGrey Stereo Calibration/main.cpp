#include <iostream>
#include <time.h>
#include "CameraComponent.h"
#include "CheckerboardComponent.h"
#include "CalibrationComponent.h"
#include "VisionEngine.h"

using namespace std;

UCameraComponent LeftCamera(ECameraInc::PointGrey, 0); // 0��° Point Grey Camera Handler
UCameraComponent RightCamera(ECameraInc::PointGrey, 1); // 1��° Point Grey Camera Handler
UCheckerboardComponent CheckerboardComponent(7, 7, 0.03f, 0.015f, 6); // ���� ����� �Ǵ� Checkerboard�� �ش��մϴ�.

Ptr<aruco::Board> Board = CheckerboardComponent.GetBoard();
Ptr<aruco::Dictionary> Dictionary = CheckerboardComponent.GetDictionary();
Ptr<aruco::DetectorParameters> DetectorParams = CheckerboardComponent.GetDetectorParams();
Ptr<aruco::CharucoBoard> CharucoBoard = CheckerboardComponent.GetCharucoBoard();

/** Vector�� �Է¹޾� ���� ������ ����մϴ�. */
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

/** ī�޶� Frame�� �Է¹޾� ����� ��� Marker Id�� ����մϴ�. */
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

/** ī�޶� Frame�� �Է¹޾� ����� ��� Marker Id�� ��ġ�� ����մϴ�. */
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

/** �Էµ� �̹����κ��� Checkerboard�� ������ �� �𼭸� ������ �Է¹��� Vector�� ����մϴ�. */
void DetectAndInsertCorners(vector<vector<Point3f>>& InObjPts, vector<vector<Point2f>>& InLeftImgPts, vector<vector<Point2f>>& InRightImgPts, Mat& LeftFrame, Mat& RightFrame)
{
	vector<int> MarkerIds, Ids;
	vector<vector<Point2f>> MarkerCorners, Rejected;
	vector<Point2f> Corners;
	Mat IntrinsicParams; 
	Mat DistortionParams; 

	vector<Point3f> ObjPts;
	
	// ���� ī�޶�
	IntrinsicParams = LeftCamera.GetIntrinsicParams();
	DistortionParams = LeftCamera.GetDistortionParams();
	aruco::detectMarkers(LeftFrame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, LeftFrame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
		InLeftImgPts.push_back(Corners);
	}

	// ����� Checkerboard�� ������ ������� Object Point�� ����� ��, ����մϴ�.
	for (int Id : Ids)
	{
		Vec3d ObjPoint = CheckerboardComponent.GetCornerPosition(Id);
		ObjPts.push_back(Point3f(ObjPoint[0], ObjPoint[1], ObjPoint[2]));
	}
	InObjPts.push_back(ObjPts);

	// ���� ī�޶�
	IntrinsicParams = RightCamera.GetIntrinsicParams();
	DistortionParams = RightCamera.GetDistortionParams();
	aruco::detectMarkers(RightFrame, Dictionary, MarkerCorners, MarkerIds, DetectorParams, Rejected);
	if (MarkerIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(MarkerCorners, MarkerIds, RightFrame, CharucoBoard, Corners, Ids, IntrinsicParams, DistortionParams);
		InRightImgPts.push_back(Corners);
	}
}

/** Camera�� Parameter ������ �Է¹��� ��, �����մϴ�. */
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

/** ��/�� ī�޶� ������ Vector�� �Է¹޾� Stereo camera calibration�� �����մϴ�. */
void CalibrateCameras(vector<Mat>& LeftFrames, vector<Mat>& RightFrames)
{
	vector<vector<Point3f>> AllObjPoints; // Obj Points
	vector<vector<Point2f>> LeftImgPts; // Image Pt1
	vector<vector<Point2f>> RightImgPts; // Image Pt2

	// �̸� ������ Intrinsic Parameter�鿡 �ش��մϴ�.
	Mat CamMat1 = LeftCamera.GetIntrinsicParams();
	Mat CamMat2 = RightCamera.GetIntrinsicParams();
	Mat DistCoeffs1 = LeftCamera.GetDistortionParams();
	Mat DistCoeffs2 = RightCamera.GetDistortionParams();

	// Intrinsic Parameter���� �����ϴµ� ��������, CV_CALIB_FIX_INTRINSIC�� ����ϹǷ� �ش簪�� �������� �ʰ� �μ��θ� �����˴ϴ�.
	Size ImgSize = LeftFrames[0].size();

	// Homogeneous Transform �� Essential Matrix / Fundamental Matrix�� �ش��մϴ�.
	Mat R;
	Mat T;
	Mat E;
	Mat F;

	int DataLength = LeftFrames.size(); // ��ü Frame�� ������ �ش��մϴ�.

	Mat Frame;
	for (int Idx = 0; Idx < DataLength; Idx++)
	{
		DetectAndInsertCorners(AllObjPoints, LeftImgPts, RightImgPts, LeftFrames[Idx], RightFrames[Idx]);
	}

	double AvgRepErr = stereoCalibrate(AllObjPoints, LeftImgPts, RightImgPts, CamMat1, DistCoeffs1, CamMat2, DistCoeffs2, ImgSize, R, T, E, F, CV_CALIB_USE_INTRINSIC_GUESS);

	SaveStereoCameraParameters("StreoCalibration2.xml", CamMat1, DistCoeffs1, CamMat2, DistCoeffs2, R, T, E, F, AvgRepErr);
}

/** ����� Marker�� ������ ��ȯ�մϴ�. */
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

/** ��/�� ī�޶��� �������� �Է¹޾� ��� ��Ŀ�� ����Ǿ����� ������ ��ȯ�մϴ�. */
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

		if (LeftFrame.data == 0 || RightFrame.data == 0) // ��/�� ī�޶� �������� �ϳ��� �������� �ʴ� ���
		{
			continue; // �ݺ����� ó������ ȸ��
		}
		
		// ��/�� ī�޶� �������� ����մϴ�.
		imshow("L", LeftFrame);
		imshow("R", RightFrame);

		char Key = cv::waitKey(10);
		switch (Key)
		{
		case 'q': // Exit ���
			bExit = true;			
			break;
		case 'c': // Capture ���
			if (AllMarkerDetected(LeftFrame, RightFrame)) // ��� ��Ŀ�� ����Ǿ��� ���
			{
				// ��/�� ī�޶� �������� Vector�� ����մϴ�.
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
		default: // ������� ���� ���
			break;
		}

		if (bExit) // Exit ����� ����Ǿ��� ���
		{
			break;
		}

	}
	CalibrateCameras(LeftFrames, RightFrames);

	return 0;
}