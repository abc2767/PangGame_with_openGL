#pragma once
#include <GL/glut.h>
// �������� ������ �ؽ��Ŀ� ���� ����
class item
{
public:
	item(int type, GLuint tex);
	void setCenter(float x, float y, float z);
	const float* getCenter() const;
	void setVelocity(float x, float y, float z);
	const float* getVelocity() const;
	int getType() const;
	float getSize() const;
	int getTimer() const;
	void setTimer(int t);
	void setTex(GLuint t);

	void move();
	void draw() const;

private:
	int type;
	float itemSize;
	GLuint texID;
	int itemTimer;

	float center[3];
	float velocity[3];

};

