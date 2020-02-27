#include <opencv2/highgui.hpp>
#include <opencv2/aruco/charuco.hpp>

using namespace cv;

namespace 
{
	const char* About = "Create a ChArUco board image";
	const char* Keys =
	"{@outfile |<none> | Output image }"
	"{w        |       | Number of squares in X direction }"
	"{h        |       | Number of squares in Y direction }"
	"{sl       |       | Square side length (in pixels) }"
	"{ml       |       | Marker side length (in pixels) }"
	"{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
	"DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
	"DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
	"DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
	"{m        |       | Margins size (in pixels). Default is (squareLength-markerLength) }"
	"{bb       | 1     | Number of bits in marker borders }"
	"{si       | false | show generated image }";
}

int main(int argc, char *argv[]) 
{
	int SquaresX = 7; // ���ι��� ��Ŀ ����
	int SquaresY = 7; // ���ι��� ���� ����
	int SquareLength = 300; // ������ �׵θ� ������ ���簢���� �Ѻ� ����, �ȼ�����
	int MarkerLength = 150; // ��Ŀ �� ���� ����, �ȼ�����
	int DictionaryId = 6; // ��� ��Ŀ�� ����
	int Margins = 0; // ChArUco board�� A4���� ������ ��� ���� ũ��, �ȼ�����
	int BorderBits = 1; // ������ �׵θ� ũ��
	bool bShowImage = true; // ������ Checkerboard�� ������ �� ����
	String FileName = "Checkerboard.png";

	Ptr<aruco::Dictionary> Dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(DictionaryId));
	Size ImageSize; 
	ImageSize.width = SquaresX * SquareLength + 2 * Margins;
	ImageSize.height = SquaresY * SquareLength + 2 * Margins;

	Ptr<aruco::CharucoBoard> Board = aruco::CharucoBoard::create(SquaresX, SquaresY, (float)SquareLength, (float)MarkerLength, Dictionary);
	Mat BoardImage;
	Board->draw(ImageSize, BoardImage, Margins, BorderBits);
	if (bShowImage) 
	{
		imshow("Checkerboard", BoardImage);
		waitKey(0);
	}
	imwrite(FileName, BoardImage);

	return 0;
}
