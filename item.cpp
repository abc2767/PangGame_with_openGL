#include "item.h"

item::item(int t, GLuint tex) {
	type = t;
	texID = tex;
	itemSize = 10;
	velocity[0] = 0.0f;
	velocity[1] = -2.0f;
	velocity[2] = 0.0f;
	itemTimer = 0;
}
void item::setCenter(float x, float y, float z) {
	center[0] = x;
	center[1] = y;
	center[2] = z;
}
const float* item::getCenter() const {
	return center;
}
void item::setVelocity(float x, float y, float z) {
	velocity[0] = x;
	velocity[1] = y;
	velocity[2] = z;
}
const float* item::getVelocity() const {
	return velocity;
}
int item::getType() const {
	return type;
}
float item::getSize() const {
	return itemSize;
}
int item::getTimer() const {
	return itemTimer;
}
void item::setTimer(int t) {
	itemTimer = t;
}
void item::setTex(GLuint t) {
	texID = t;
}
void item::move() {
	center[0] += velocity[0];
	center[1] += velocity[1];
	center[2] += velocity[2];
}
void item::draw() const {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texID);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(center[0] - itemSize, center[1] - itemSize, center[2]);
	glTexCoord2f(0, 1);
	glVertex3f(center[0] - itemSize, center[1] + itemSize, center[2]);
	glTexCoord2f(1, 1);
	glVertex3f(center[0] + itemSize, center[1] + itemSize, center[2]);
	glTexCoord2f(1, 0);
	glVertex3f(center[0] + itemSize, center[1] - itemSize, center[2]);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
