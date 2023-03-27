#include "character.h"

character::character() {
	pos[0] = 0;
	characterSize[0] = 30;
	characterSize[1] = 60;
	speed = 15;
}

void character::setPos(float x, float y, float z) {
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
}

const float* character::getPos() const {
	return pos;
}

void character::setCharacterSize(float x, float y) {
	characterSize[0] = x;
	characterSize[1] = y;
}

const float* character::getCharacterSize() const {
	return characterSize;
}

void character::setSpeed(int s) {
	speed = s;
}
int character::getSpeed() const {
	return speed;
}

void character::draw(GLuint characterTex) const {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, characterTex);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(pos[0] - characterSize[0] / 2, pos[1] - characterSize[1], pos[2]);
	glTexCoord2f(0, 1);
	glVertex3f(pos[0] - characterSize[0] / 2, pos[1], pos[2]);
	glTexCoord2f(1, 1);
	glVertex3f(pos[0] + characterSize[0] / 2, pos[1], pos[2]);
	glTexCoord2f(1, 0);
	glVertex3f(pos[0] + characterSize[0] / 2, pos[1] - characterSize[1], pos[2]);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
