#pragma once

#include "VisionEngine.h"
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

class UCheckerboardComponent
{
public:
	/** Checkerboard�� �Ӽ����� �Է¹޾� Checkerboard Component�� �����մϴ�. */
	UCheckerboardComponent(int NewSquaresX, int NewSquaresY, float NewSquareLength, float NewMarkerLength, int NewDictionaryId);
	/** CheckerBoard�� Dictionary ���� ��ȯ�մϴ�. */
	int GetDictionaryId();
	/** DetectorParams ��ü�� ��ȯ�մϴ�. */
	Ptr<aruco::DetectorParameters> GetDetectorParams();
	/** Dictionary ��ü�� ��ȯ�մϴ�. */
	Ptr<aruco::Dictionary> GetDictionary();
	/** CharucoBoard ��ü�� ��ȯ�մϴ�. */
	Ptr<aruco::CharucoBoard> GetCharucoBoard();
	/** Board ��ü�� ��ȯ�մϴ�. */
	Ptr<aruco::Board> GetBoard();
	/** �� ���̸� ��ȯ�մϴ�. */
	float GetAxisLength();
	/** ��ǥ���������� Checkerboard�� �߽����� ���� Offset�� ��ȯ�մϴ�. */
	Vec3d GetOffset();

private:
	int SquaresX;
	int SquaresY;
	float SquareLength;
	float MarkerLength;
	int DictionaryId;

	Ptr<aruco::DetectorParameters> DetectorParams;
	Ptr<aruco::Dictionary> Dictionary;
	Ptr<aruco::CharucoBoard> CharucoBoard;
	Ptr<aruco::Board> Board;
	float AxisLength;

};

