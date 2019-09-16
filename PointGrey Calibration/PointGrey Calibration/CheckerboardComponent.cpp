#include "CheckerboardComponent.h"

/** Checkerboard�� �Ӽ����� �Է¹޾� Checkerboard Component�� �����մϴ�. */
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

/** CheckerBoard�� Dictionary ���� ��ȯ�մϴ�. */
int UCheckerboardComponent::GetDictionaryId()
{
	return DictionaryId;

}

/** DetectorParams ��ü�� ��ȯ�մϴ�. */
Ptr<aruco::DetectorParameters> UCheckerboardComponent::GetDetectorParams()
{
	return DetectorParams;

}

/** Dictionary ��ü�� ��ȯ�մϴ�. */
Ptr<aruco::Dictionary> UCheckerboardComponent::GetDictionary()
{
	return Dictionary;

}

/** CharucoBoard ��ü�� ��ȯ�մϴ�. */
Ptr<aruco::CharucoBoard> UCheckerboardComponent::GetCharucoBoard()
{
	return CharucoBoard;

}

/** Board ��ü�� ��ȯ�մϴ�. */
Ptr<aruco::Board> UCheckerboardComponent::GetBoard()
{
	return Board;

}

/** �� ���̸� ��ȯ�մϴ�. */
float UCheckerboardComponent::GetAxisLength()
{
	return AxisLength;

}

/** ��ǥ���������� Checkerboard�� �߽����� ���� Offset�� ��ȯ�մϴ�. */
Vec3d UCheckerboardComponent::GetOffset()
{
	Vec3d Offset;
	Offset[0] = SquareLength * SquaresX / 2;
	Offset[1] = SquareLength * SquaresY / 2;
	Offset[2] = 0;

	return Offset;

}