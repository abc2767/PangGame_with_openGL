#pragma once
#include <GL/glut.h>
// ĳ���Ͱ� ������ ���� ����
class character
{
public:
	character();

	void setPos(float x, float y, float z);
	const float* getPos() const;
	void setCharacterSize(float x, float y);
	const float* getCharacterSize() const;
	void setSpeed(int s);
	int getSpeed() const;

	void draw(GLuint characterTex) const;

private:
	float pos[3];
	float characterSize[2];
	int speed;
};

