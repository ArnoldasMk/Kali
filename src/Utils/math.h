#include "../SDK/vector.h"
#include "../SDK/VMatrix.h"
#include "../SDK/IInputSystem.h"

#define square(x) (x * x)

namespace Math
{
	void SinCos(float radians, float *sine, float *cosine);
	void AngleVectors(const QAngle &angles, Vector &forward);
	void AngleVectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up);
	void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
	void NormalizeAngles(QAngle &angle);
	void NormalizeYaw(float &yaw);
	void ClampAngles(QAngle &angle);
	float Clamp(float x, float min, float max);
	void CorrectMovement(const QAngle &vOldAngles, CUserCmd *pCmd, float fOldForward, float fOldSidemove);
	float GetFov(const QAngle &viewAngle, const QAngle &aimAngle);
	float DotProduct(const Vector &v1, const float *v2);
	float DotProduct(const float *v1, const float *v2);
	void VectorAngles(const Vector &forward, QAngle &angle);
	void VectorAngles(const Vector &forward, Vector &up, QAngle &angles);
	void VectorTransform(const Vector &in1, const matrix3x4_t &in2, Vector &out);
	void VectorRotate(const float *in1, const matrix3x4_t &in2, float *out);
	void VectorRotate(const Vector &in1, const Vector &in2, Vector &out);
	void VectorRotate(const Vector &in1, const matrix3x4_t &in2, Vector &out);
	void CalcAngle(const Vector &src, const Vector &dst, QAngle &angle);
	QAngle CalcAngle(const Vector &src, const Vector &dst);
	float float_rand(float min, float max);
	void AngleMatrix(const Vector angles, matrix3x4_t &matrix);
	float CalMaxDistance(const Vector &src, const Vector &dest);
	float CalMaxDistance(const QAngle &src, const Vector &dest);
	float SquareRoot(const float &x);
	float ClampYaw(float val);
}
