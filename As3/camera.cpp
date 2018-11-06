#include "camera.h"
#include <GL/GL.h>
#include <GL/GLU.h>
#include "matrix.h"
void OrthographicCamera::glInit(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h)
		glOrtho(-size / 2.0, size / 2.0, -size * (float)h / (float)w / 2.0, size*(float)h / (float)w / 2.0, 0.5, 40.0);
	else
		glOrtho(-size * (float)w / (float)h / 2.0, size*(float)w / (float)h / 2.0, -size / 2.0, size / 2.0, 0.5, 40.0);
}

void OrthographicCamera::glPlaceCamera(void)
{
	gluLookAt(pos.x(), pos.y(), pos.z(),
		pos.x() + dir.x(), pos.y() + dir.y(), pos.z() + dir.z(),
		up.x(), up.y(), up.z());
}

void OrthographicCamera::dollyCamera(float dist)
{
	pos += dir * dist;
	glPlaceCamera();
}

void OrthographicCamera::truckCamera(float dx, float dy)
{
	pos += horizontal * dx + up * dy;
	glPlaceCamera();
}

void OrthographicCamera::rotateCamera(float rx, float ry)
{
	float tiltAngle = acos(world_up.Dot3(dir));
	if (tiltAngle - ry > 3.13)
		ry = tiltAngle - 3.13;
	else if (tiltAngle - ry < 0.01)
		ry = tiltAngle - 0.01;

	Matrix rotMat = Matrix::MakeAxisRotation(world_up, rx);
	rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

	rotMat.Transform(pos);
	rotMat.TransformDirection(dir);
	Vec3f::Cross3(horizontal, dir, world_up);
	Vec3f::Cross3(up, horizontal, dir);
	dir.Normalize(); horizontal.Normalize(); up.Normalize();

	glPlaceCamera();
}

void PerspectiveCamera::glInit(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle*180.0 / 3.14159, (float)w / float(h), 0.5, 40.0);
}

void PerspectiveCamera::glPlaceCamera(void)
{
	gluLookAt(center.x(), center.y(), center.z(),
		center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
		up.x(), up.y(), up.z());
}

void PerspectiveCamera::dollyCamera(float dist)
{
	center += direction * dist;
	glPlaceCamera();
}

void PerspectiveCamera::truckCamera(float dx, float dy)
{
	center += horizontal * dx + up * dy;
	glPlaceCamera();
}

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
	float tiltAngle = acos(world_up.Dot3(direction));
	if (tiltAngle - ry > 3.13)
		ry = tiltAngle - 3.13;
	else if (tiltAngle - ry < 0.01)
		ry = tiltAngle - 0.01;

	Matrix rotMat = Matrix::MakeAxisRotation(world_up, rx);
	rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

	rotMat.Transform(center);
	rotMat.TransformDirection(direction);
	Vec3f::Cross3(horizontal, direction, world_up);
	Vec3f::Cross3(up, horizontal, direction);
	direction.Normalize(); horizontal.Normalize(); up.Normalize();

	glPlaceCamera();
}
