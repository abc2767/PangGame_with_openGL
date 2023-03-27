#include "rope.h"

rope::rope() {
	harpoonSize[0] = 20;
	harpoonSize[1] = 20;
	ropeSize[0] = 10;
	ropeSize[1] = 390;
	isShot = false;
}

void rope::setHarpoonSize(float x, float y) {
	harpoonSize[0] = x;
	harpoonSize[1] = y;
}
const float* rope::getHarpoonSize() const {
	return harpoonSize;
}
void rope::setRopeSize(float x, float y) {
	ropeSize[0] = x;
	ropeSize[1] = y;
}
const float* rope::getRopeSize() const {
	return ropeSize;
}
void rope::setIsShot(bool b) {
	isShot = b;
}
bool rope::getIsShot() const {
	return isShot;
}
void rope::setPos(float x, float y, float z) {
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
}
const float* rope::getPos() const {
	return pos;
}
void rope::setVelocity(float x, float y, float z) {
	velocity[0] = x;
	velocity[1] = y;
	velocity[2] = z;
}
const float* rope::getVelocity() const {
	return velocity;
}
void rope::move() {
	pos[0] += velocity[0];
	pos[1] += velocity[1];
	pos[2] += velocity[2];
}
void rope::draw(GLuint ropeTex, GLuint harpoonTex) const {
	if (isShot) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, harpoonTex);
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0, 0);
		glVertex3f(pos[0] - harpoonSize[0] / 2, pos[1] - harpoonSize[1], pos[2]);
		glTexCoord2f(0.5, 1);
		glVertex3f(pos[0], pos[1], pos[2]);
		glTexCoord2f(1, 0);
		glVertex3f(pos[0] + harpoonSize[0] / 2, pos[1] - harpoonSize[1], pos[2]);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, ropeTex);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(pos[0] - (harpoonSize[0] - ropeSize[0]) / 2, pos[1] - harpoonSize[1] - ropeSize[1], pos[2]);
		glTexCoord2f(0, 1);
		glVertex3f(pos[0] - (harpoonSize[0] - ropeSize[0]) / 2, pos[1] - harpoonSize[1], pos[2]);
		glTexCoord2f(1, 1);
		glVertex3f(pos[0] + (harpoonSize[0] - ropeSize[0]) / 2, pos[1] - harpoonSize[1], pos[2]);
		glTexCoord2f(1, 0);
		glVertex3f(pos[0] + (harpoonSize[0] - ropeSize[0]) / 2, pos[1] - harpoonSize[1] - ropeSize[1], pos[2]);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}
