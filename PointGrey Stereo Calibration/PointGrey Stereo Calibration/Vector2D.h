#pragma once

struct FVector2D 
{
	FVector2D() : X(0), Y(0) {}
	FVector2D(float NewX, float NewY) : X(NewX), Y(NewY) {}
	FVector2D(float Array[2]) : X(Array[0]), Y(Array[1]) {}

	void ToArray(float Array[2])
	{
		Array[0] = X;
		Array[1] = Y;
	}

	float X;
	float Y;

};