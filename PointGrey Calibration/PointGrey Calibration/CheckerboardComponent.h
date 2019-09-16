#pragma once

#include "VisionEngine.h"
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

class UCheckerboardComponent
{
public:
	/** Checkerboard의 속성들을 입력받아 Checkerboard Component를 생성합니다. */
	UCheckerboardComponent(int NewSquaresX, int NewSquaresY, float NewSquareLength, float NewMarkerLength, int NewDictionaryId);
	/** CheckerBoard의 Dictionary 값을 반환합니다. */
	int GetDictionaryId();
	/** DetectorParams 객체를 반환합니다. */
	Ptr<aruco::DetectorParameters> GetDetectorParams();
	/** Dictionary 객체를 반환합니다. */
	Ptr<aruco::Dictionary> GetDictionary();
	/** CharucoBoard 객체를 반환합니다. */
	Ptr<aruco::CharucoBoard> GetCharucoBoard();
	/** Board 객체를 반환합니다. */
	Ptr<aruco::Board> GetBoard();
	/** 축 길이를 반환합니다. */
	float GetAxisLength();
	/** 좌표시작점에서 Checkerboard의 중심으로 가는 Offset을 반환합니다. */
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

