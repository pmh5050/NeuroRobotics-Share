#pragma once

struct FVector4
{
	FVector4() : X(0), Y(0), Z(0), W(0) {}
	FVector4(float NewX, float NewY, float NewZ, float NewW) : X(NewX), Y(NewY), Z(NewZ), W(NewW) {}
	FVector4(float Array[4]) : X(Array[0]), Y(Array[1]), Z(Array[2]), W(Array[3]) {}
	
	void ToArray(float Array[4])
	{
		Array[0] = X;
		Array[1] = Y;
		Array[2] = Z;
		Array[3] = W;
	}
	
	float X;
	float Y;
	float Z;
	float W;

};