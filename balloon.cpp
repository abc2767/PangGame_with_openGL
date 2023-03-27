#include "balloon.h"

balloon::balloon(int t) {
	if (t == 0) {
		radius = 30;
		slice = 20;
		stack = 10;
		acceleration = -0.5;
		type = t;
		maxHeight = 200;
	}
	if (t == 1) {
		radius = 20;
		slice = 15;
		stack = 10;
		acceleration = -0.5;
		type = t;
		maxHeight = 150;
	}
	if (t == 2) {
		radius = 10;
		slice = 10;
		stack = 10;
		acceleration = -0.5;
		type = t;
		maxHeight = 70;
	}
	if (t == 3) {
		radius = 30;
		slice = 6;
		stack = 1;
		acceleration = 0;
		type = t;
		angularSpeed = 1;
		angle = 0;
	}
	if (t == 4) {
		radius = 20;
		slice = 6;
		stack = 1;
		acceleration = 0;
		type = t;
		angularSpeed = 1;
		angle = 0;
	}
	if (t == 5) {
		radius = 10;
		slice = 6;
		stack = 1;
		acceleration = 0;
		type = t;
		angularSpeed = 1;
		angle = 0;
	}
}

void balloon::setRadius(float r) {
	radius = r;
}
float balloon::getRadius() const {
	return radius;
}
void balloon::setAcceleration(int a) {
	acceleration = a;
}
float balloon::getAcceleration() const {
	return acceleration;
}
void balloon::setSlice(float sl) {
	slice = sl;
}
void balloon::setStack(float st) {
	stack = st;
}

void balloon::setCenter(float x, float y, float z) {
	center[0] = x;
	center[1] = y;
	center[2] = z;
}
const float* balloon::getCenter() const {
	return center;
}
void balloon::setVelocity(float x, float y, float z) {
	velocity[0] = x;
	velocity[1] = y;
	velocity[2] = z;
}
const float* balloon::getVelocity() const {
	return velocity;
}
int balloon::getMaxHeight() const {
	return maxHeight;
}
int balloon::getType() const {
	return type;
}
float balloon::getAngularSpeed() const {
	return angularSpeed;
}
float balloon::getAngle() const {
	return angle;
}
void balloon::move() {
	velocity[1] += acceleration;
	center[0] += velocity[0];
	center[1] += velocity[1];
	center[2] += velocity[2];
	angle += angularSpeed;
}
void balloon::setMTL(const Material& m) {
	mtl = m;
}
void balloon::draw() const {
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtl.getAmbient());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtl.getDiffuse());
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtl.getSpecular());
	glMaterialfv(GL_FRONT, GL_SHININESS, mtl.getShininess());
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	if (type / 3 == 0)
		glutSolidSphere(radius, slice, stack);
	if (type / 3 == 1) {
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		glutSolidCone(radius,1,slice,stack);
	}
	glPopMatrix();
}
