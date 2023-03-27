#pragma once
#include <GL/glut.h>
// 로프가 가지는 텍스쳐와 동작 설정
class rope
{
public:
	rope();

	void setPos(float x, float y, float z);
	const float* getPos() const;
	void setVelocity(float x, float y, float z);
	const float* getVelocity() const;
	void setHarpoonSize(float x, float y);
	const float* getHarpoonSize() const;
	void setRopeSize(float x, float y);
	const float* getRopeSize() const;
	void setIsShot(bool b);
	bool getIsShot() const;
	void move();

	void draw(GLuint ropeTex, GLuint harpoonTex) const;

private:
	float pos[3];
	float velocity[3];
	float harpoonSize[2];
	float ropeSize[2];
	bool isShot;
};

