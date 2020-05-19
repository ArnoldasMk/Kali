#pragma once

#include "../SDK/vector.h"
#include "../SDK/VMatrix.h"
#include "../SDK/IInputSystem.h"

namespace Math {
    bool Cmpf( float f1, float f2, float epsilon = 0.005f );
	void inline SinCos(float radians, float *sine, float *cosine);
	void AngleVectors(const QAngle &angles, Vector &forward);
	void AngleVectors( const Vector& angles, Vector* forward, Vector* right, Vector* up );
	void NormalizeAngles(QAngle& angle);
	void NormalizeYaw( float& yaw );
	void ClampAngles(QAngle& angle);
	void CorrectMovement(const QAngle &vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	float GetFov(const QAngle &viewAngle, const QAngle &aimAngle);
	float DotProduct(const Vector &v1, const float* v2);
	float DotProduct( const float *v1, const float *v2 );
	void VectorAngles(const Vector &forward, QAngle &angles);
	void VectorTransform (const Vector &in1, const matrix3x4_t& in2, Vector &out);
	void VectorRotate(const float *in1, const matrix3x4_t& in2, float *out);
	void VectorRotate(const Vector &in1, const Vector &in2, Vector &out);
	void VectorRotate(const Vector& in1, const matrix3x4_t &in2, Vector &out);
	QAngle CalcAngle(const Vector &src, const Vector &dst);
	float float_rand( float min, float max );
	// static void AngleMatrix(const QAngle& angles, Vector& position, matrix3x4_t& matrix);
	// static void AngleMatrix(const QAngle& angles, matrix3x4_t& matrix);
	void AngleMatrix(const Vector angles, matrix3x4_t& matrix);
	float CalMaxDistance(const Vector &src, const Vector &dest);
	float CalMaxDistance(const QAngle &src, const Vector &dest);
	float SquareRoot(const float &x);
	// static void AngleMatrix(const Vector &angles, const Vector &position, matrix3x4_t& matrix_out);
}