#pragma once
#include "Material.h"
// 6개 종류의 풍선이 가지는 동작 설정
class balloon
{
public:
	balloon(int t);

	void setRadius(float r);
	float getRadius() const;
	void setAcceleration(int a);
	float getAcceleration() const;
	void setSlice(float sl);
	void setStack(float st);
	void setCenter(float x, float y, float z);
	const float* getCenter() const;
	void setVelocity(float x, float y, float z);
	const float* getVelocity() const;
	void move();
	void setMTL(const Material& m);
	int getType() const;
	int getMaxHeight() const;
	float getAngularSpeed() const;
	float getAngle() const;

	void draw() const;

public:
	float radius;
	int slice;
	int stack;
	float acceleration;
	int type;
	int maxHeight;
	float angularSpeed;
	float angle;

	float center[3];
	float velocity[3];
	Material mtl;
};

