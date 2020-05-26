#pragma once

struct FVector 
{
	FVector() : X(0), Y(0), Z(0) {}
	FVector(float NewX, float NewY, float NewZ) : X(NewX), Y(NewY), Z(NewZ) {}
	FVector(float Array[3]) : X(Array[0]), Y(Array[1]), Z(Array[2]) {}

	void ToArray(float Array[3])
	{
		Array[0] = X;
		Array[1] = Y;
		Array[2] = Z;
	}

	/** 크기는 유지하고 반대방향으로 방향을 반전합니다. */
	void DirectionFlip()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
	}

	FVector operator * (float Src)
	{
		return FVector(X * Src, Y * Src, Z * Src);
	}

	FVector operator / (float Src)
	{
		return FVector(X / Src, Y / Src, Z / Src);
	}

	float Norm()
	{
		return sqrt(X * X + Y * Y + Z * Z);
	}

	float X;
	float Y;
	float Z;

};