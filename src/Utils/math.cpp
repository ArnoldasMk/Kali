#include "math.h"


void Math::SinCos(float radians, float *sine, float *cosine)
{
	double __cosr, __sinr;
	__asm ("fsincos" : "=t" (__cosr), "=u" (__sinr) : "0" (radians));

	*sine = __sinr;
	*cosine = __cosr;
}

float Math::float_rand( float min, float max ) // thanks foo - https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c :^)
{
	float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
	return min + scale * ( max - min );      /* [min, max] */
}

void Math::AngleVectors(const QAngle &angles, Vector& forward)
{
	float sp, sy, cp, cy;

	Math::SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	Math::SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

void Math::VectorRotate(const float *in1, const matrix3x4_t& in2, float *out)
{
    out[0] = Math::DotProduct(in1, in2[0]);
    out[1] = Math::DotProduct(in1, in2[1]);
    out[2] = Math::DotProduct(in1, in2[2]);
}

void Math::VectorRotate(const Vector &in1, const Vector &in2, Vector &out)
{
    matrix3x4_t matRotate;
    AngleMatrix(in2, matRotate);
    Math::VectorRotate(in1, matRotate, out);
}

void Math::VectorRotate(const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
    Math::VectorRotate(&in1.x, in2, &out.x);
}

void Math::AngleVectors( const QAngle& angles, Vector* forward, Vector* right, Vector* up ) {
	float sr, sp, sy, cr, cp, cy;

	SinCos( DEG2RAD( angles[1] ), &sy, &cy );
	SinCos( DEG2RAD( angles[0] ), &sp, &cp );
	SinCos( DEG2RAD( angles[2] ), &sr, &cr );

	if ( forward ) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if ( right ) {
		right->x = ( -1 * sr * sp * cy + -1 * cr * -sy );
		right->y = ( -1 * sr * sp * sy + -1 * cr * cy );
		right->z = -1 * sr * cp;
	}

	if ( up ) {
		up->x = ( cr * sp * cy + -sr * -sy );
		up->y = ( cr * sp * sy + -sr * cy );
		up->z = cr * cp;
	}
}

void Math::AngleVectors( const Vector& angles, Vector* forward, Vector* right, Vector* up ) {
	float sr, sp, sy, cr, cp, cy;

	SinCos( DEG2RAD( angles[1] ), &sy, &cy );
	SinCos( DEG2RAD( angles[0] ), &sp, &cp );
	SinCos( DEG2RAD( angles[2] ), &sr, &cr );

	if ( forward ) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if ( right ) {
		right->x = ( -1 * sr * sp * cy + -1 * cr * -sy );
		right->y = ( -1 * sr * sp * sy + -1 * cr * cy );
		right->z = -1 * sr * cp;
	}

	if ( up ) {
		up->x = ( cr * sp * cy + -sr * -sy );
		up->y = ( cr * sp * sy + -sr * cy );
		up->z = cr * cp;
	}
}


void Math::NormalizeAngles(QAngle& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

void Math::NormalizeYaw( float& yaw ) {
	while ( yaw > 180.0f )
		yaw -= 360.0f;
	while ( yaw < -180.0f )
		yaw += 360.0f;
}

void Math::ClampAngles(QAngle& angle)
{
	if (angle.y > 180.0f)
		angle.y = 180.0f;
	else if (angle.y < -180.0f)
		angle.y = -180.0f;

	if (angle.x > 89.0f)
		angle.x = 89.0f;
	else if (angle.x < -89.0f)
		angle.x = -89.0f;

	angle.z = 0;
}

void Math::CorrectMovement(const QAngle &vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/forward move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

float Math::GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	NormalizeAngles(delta);

	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

void Math::VectorAngles(const Vector& forward, QAngle &angles)
{
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
		angles[1] = 0.0f;  //yaw left/right
	}
	else
	{
		angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
		angles[1] = atan2(forward[1], forward[0]) * 180 / M_PI;

		if (angles[1] > 90)
			angles[1] -= 180;
		else if (angles[1] < 90)
			angles[1] += 180;
		else if (angles[1] == 90)
			angles[1] = 0;
	}

	angles[2] = 0.0f;
}

float Math::DotProduct( const float *v1, const float *v2 ) {
    return v1 [ 0 ] * v2 [ 0 ] + v1 [ 1 ] * v2 [ 1 ] + v1 [ 2 ] * v2 [ 2 ];
}

float Math::DotProduct(const Vector &v1, const float* v2)
{
	return v1.x*v2[0] + v1.y*v2[1] + v1.z*v2[2];
}

void Math::VectorTransform (const Vector &in1, const matrix3x4_t& in2, Vector &out)
{
	out.x = DotProduct(in1, in2[0]) + in2[0][3];
	out.y = DotProduct(in1, in2[1]) + in2[1][3];
	out.z = DotProduct(in1, in2[2]) + in2[2][3];
}

void Math::CalcAngle(const Vector &src, const Vector &dst, QAngle& angle)
{
	Vector delta = src - dst;

	delta.Normalize();

	Math::VectorAngles(delta, angle);
}

QAngle Math::CalcAngle(const Vector &src, const Vector &dst)
{
	QAngle angle;
	Vector delta = src - dst;

	delta.Normalize();

	Math::VectorAngles(delta, angle);
	return angle;
}

float Math::SquareRoot(const float &x)
{
	if ( x == 0 || x ==1)
		return x;
		
	int i = 1, result = 1; 
    while (result <= x) 
    { 
      i++; 
      result = i * i; 
    } 
    return i - 1;
}

float Math::CalMaxDistance(const Vector &src, const Vector &dest)
{
	float	Ydistance = 0.f,
			ZDistance = 0.f;

	// Getting Y axis distance
	if (src.y > dest.y)
		Ydistance = src.y - dest.y;
	else if (src.y < dest.y)
		Ydistance = dest.y - src.y;

	// Calculate the z distance
	if (src.z > dest.z)
		ZDistance = src.z - dest.z;
	else if (src.z < dest.z)
		ZDistance = dest.z - src.z;	

	return Math::SquareRoot( square(Ydistance) + square(ZDistance) );
}

float Math::CalMaxDistance(const QAngle &src, const Vector &dest)
{
	float Ydistance = src.y - dest.y,
			ZDistance =src.z - dest.z,
			XDistance = src.x - dest.x, 
			tempdistance;

	return tempdistance = Math::SquareRoot( square(Ydistance) + square(ZDistance) );
	return Math::SquareRoot( square(tempdistance) + square(XDistance) );
}

void Math::AngleMatrix(const Vector angles, matrix3x4_t& matrix)
{
    float sr, sp, sy, cr, cp, cy;
    
    sy = sin(DEG2RAD(angles[1]));
    cy = cos(DEG2RAD(angles[1]));
    
    sp = sin(DEG2RAD(angles[0]));
    cp = cos(DEG2RAD(angles[0]));
    
    sr = sin(DEG2RAD(angles[2]));
    cr = cos(DEG2RAD(angles[2]));
    
    //matrix = (YAW * PITCH) * ROLL
    matrix[0][0] = cp * cy;
    matrix[1][0] = cp * sy;
    matrix[2][0] = -sp;
    
    float crcy = cr * cy;
    float crsy = cr * sy;
    float srcy = sr * cy;
    float srsy = sr * sy;
    
    matrix[0][1] = sp * srcy - crsy;
    matrix[1][1] = sp * srsy + crcy;
    matrix[2][1] = sr * cp;
    
    matrix[0][2] = (sp * crcy + srsy);
    matrix[1][2] = (sp * crsy - srcy);
    matrix[2][2] = cr * cp;
    
    matrix[0][3] = 0.0f;
    matrix[1][3] = 0.0f;
    matrix[2][3] = 0.0f;
}

void Math::VectorAngles(const Vector& forward, Vector& up, QAngle& angles)
{
		Vector left = up * forward;
		left.NormalizeInPlace();

		float forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI_F;
			angles.y = atan2f(forward.y, forward.x) * 180 / M_PI_F;

			float upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.z = atan2f(left.z, upZ) * 180 / M_PI_F;
		}
		else
		{
			angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI_F;
			angles.y = atan2f(-left.x, left.y) * 180 / M_PI_F;
			angles.z = 0;
		}
}