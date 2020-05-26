#include "CheckerboardComponent.h"

#include <iostream>

#define DEBUG_CHECKERBOARD_COMPONENT false

/** Checkerboard의 속성들을 입력받아 Checkerboard Component를 생성합니다. */
UCheckerboardComponent::UCheckerboardComponent(int NewSquaresX, int NewSquaresY, float NewSquareLength, float NewMarkerLength, int NewDictionaryId)
{
	SquaresX = NewSquaresX;
	SquaresY = NewSquaresY;
	SquareLength = NewSquareLength;
	MarkerLength = NewMarkerLength;
	DictionaryId = NewDictionaryId;

	DetectorParams = aruco::DetectorParameters::create();
	Dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(DictionaryId));
	CharucoBoard = aruco::CharucoBoard::create(SquaresX, SquaresY, SquareLength, MarkerLength, Dictionary);
	Board = CharucoBoard.staticCast<aruco::Board>();
	AxisLength = 0.1f * ((float)min(SquaresX, SquaresY) * (SquareLength));

}

/** CheckerBoard의 Dictionary 값을 반환합니다. */
int UCheckerboardComponent::GetDictionaryId()
{
	return DictionaryId;

}

/** DetectorParams 객체를 반환합니다. */
Ptr<aruco::DetectorParameters> UCheckerboardComponent::GetDetectorParams()
{
	return DetectorParams;

}

/** Dictionary 객체를 반환합니다. */
Ptr<aruco::Dictionary> UCheckerboardComponent::GetDictionary()
{
	return Dictionary;

}

/** CharucoBoard 객체를 반환합니다. */
Ptr<aruco::CharucoBoard> UCheckerboardComponent::GetCharucoBoard()
{
	return CharucoBoard;

}

/** Board 객체를 반환합니다. */
Ptr<aruco::Board> UCheckerboardComponent::GetBoard()
{
	return Board;

}

/** 축 길이를 반환합니다. */
float UCheckerboardComponent::GetAxisLength()
{
	return AxisLength;

}

/** 좌표시작점에서 Checkerboard의 중심으로 가는 Offset을 반환합니다. */
Vec3d UCheckerboardComponent::GetOffset()
{
	Vec3d Offset;
	Offset[0] = SquareLength * SquaresX / 2;
	Offset[1] = SquareLength * SquaresY / 2;
	Offset[2] = 0;

	return Offset;

}

/** Marker Corner의 Index를 입력받은 뒤, Board Coordinate 기준 좌표계로 반환합니다. */
Vec3d UCheckerboardComponent::GetCornerPosition(int Id)
{
	int PosX = Id % (SquaresX - 1);
	int PosY = Id / (SquaresX - 1);

	double OffsetX = -(SquaresX - 2) * SquareLength * 0.5;
	double OffsetY = -(SquaresY - 2) * SquareLength * 0.5;

	Vec3d CornerPosition(PosX * SquareLength + OffsetX, PosY * SquareLength + OffsetY, 0.0);

#if DEBUG_CHECKERBOARD_COMPONENT
	std::cout << "(Id, x, y) : " << "(" << Id << ", " << CornerPosition[0] << ", " << CornerPosition[1] << ")" << std::endl;
#endif

	return CornerPosition;

}