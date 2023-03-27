#include <iostream>
#include <FreeImage.h>
#include <time.h>
#include <vector>
#include <cstdlib>
#include <string>
#include "balloon.h"
#include "Light.h"
#include "rope.h"
#include "character.h"
#include "item.h"

// 상수 설정
#define WIDTH 640
#define HEIGHT 640
#define LEFT_END -280
#define RIGHT_END 280
#define TOP_END 280
#define BOTTOM_END -120
#define BLOCK_SIZE 40
#define HORIZONTAL_VELOCITY 5
#define VERTICAL_VELOCITY 5
#define HARPOON_SPEED 10

using namespace std;

vector<balloon> iBall; // 풍선들을 저장하는 벡터
vector<rope> ropeVec1, ropeVec2; // ropeVec1은 발사하기 전의 작살, ropeVec2는 발사하여 날아가는 중인 작살
vector<item> itemVec1; // 필드 위에 생성된 아이템을 저장하는 벡터
vector<Material> MTL;
character iCharacter; // 캐릭터의 정보를 저장하는 변수

static GLuint textureID[6]; // 텍스쳐를 담기 위한 배열
GLubyte* textureData[6];
static GLuint itemTexID[6];
GLubyte* itemTexData[6];
int imageWidth, imageHeight;

Light light(800, 800, 30, GL_LIGHT0); // 빛 위치 설정
int life; // 목숨 변수 설정
bool ignore; // ignore이 ture이면 풍선에 피격 후 캐릭터가 깜빡이며 무적상태.
bool isPause = false; // P를 누르면 일시정지
int timer; // 캐릭터 피격시 무적시간 설정
int itemTimer; // 아이템 드랍시 아이템 습득 가능시간 설정
int gameState = 10; // 초기 게임 상태 10
bool t = true; // 깜빡이는 글씨 표시를 위한 타이머
bool isSlow = false; // 모래시계 아이템 습득 시 풍선 느려짐.
int slow = 0; // 모래시계 아이템으로 느려지는 시간 설정

clock_t tstart = clock(); // FPS 조절을 위한 변수
clock_t tend;

// 텍스쳐 설정을 위한 함수
FIBITMAP* createBitmap(char const* filename) {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);
	if (format == -1)
		exit(-1);
	if (format == FIF_UNKNOWN) {
		format = FreeImage_GetFIFFromFilename(filename);
		if (!FreeImage_FIFSupportsReading(format))
			exit(-1);
	}
	FIBITMAP* bitmap = FreeImage_Load(format, filename);
	int bitsPerPixel = FreeImage_GetBPP(bitmap);
	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32) {
		bitmap32 = bitmap;
	}
	else {
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}
	return bitmap32;
}
// 텍스쳐 설정을 위한 함수
void generateTexture(char const* filename, GLuint &tID, GLubyte* data) {
	FIBITMAP* bitmap32 = createBitmap(filename);
	imageWidth = FreeImage_GetWidth(bitmap32);
	imageHeight = FreeImage_GetHeight(bitmap32);
	data = FreeImage_GetBits(bitmap32);
	glGenTextures(1, &tID);
	glBindTexture(GL_TEXTURE_2D, tID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
}
// 텍스쳐 설정을 위한 함수
void initTexture() {
	generateTexture("block.png", textureID[0], textureData[0]);
	generateTexture("background.png", textureID[1], textureData[1]);
	generateTexture("longrope.png", textureID[2], textureData[2]);
	generateTexture("harpoon.png", textureID[3], textureData[3]);
	generateTexture("character.png", textureID[4], textureData[4]);
	generateTexture("life.png", textureID[5], textureData[5]);
	generateTexture("itemrope.png", itemTexID[0], itemTexData[0]);
	generateTexture("dynamite.png", itemTexID[1], itemTexData[1]);
	generateTexture("sandglass.png", itemTexID[2], itemTexData[2]);
}
// 문자열 표시를 위한 함수.
void drawCharacters(void* font, const char* c, float x, float y) {
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(x, y);
	for (int i = 0; i < strlen(c); i++)
		glutBitmapCharacter(font, c[i]);
}
// 정수 표시를 위한 함수
void drawInteger(void* font, int i, float x, float y) {
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(x, y);
	glutBitmapCharacter(font, i + 48);
}
// 초기 설정
void initialize() {
	light.setAmbient(1.0, 1.0, 1.0, 1.0);
	light.setDiffuse(1.0, 1.0, 1.0, 1.0);
	light.setSpecular(1.0, 1.0, 1.0, 1.0);

	Material mtl1;
	mtl1.setAmbient(0.05, 0.0, 0.0, 1.0);
	mtl1.setDiffuse(0.5, 0.4, 0.4, 1.0);
	mtl1.setSpecular(0.7, 0.04, 0.04, 1.0);
	mtl1.setShininess(10);
	Material mtl2;
	mtl2.setAmbient(0.0, 0.05, 0.0, 1.0);
	mtl2.setDiffuse(0.4, 0.5, 0.4, 1.0);
	mtl2.setSpecular(0.04, 0.7, 0.04, 1.0);
	mtl2.setShininess(10);
	Material mtl3;
	mtl3.setAmbient(0.0, 0.05, 0.05, 1.0);
	mtl3.setDiffuse(0.4, 0.5, 0.5, 1.0);
	mtl3.setSpecular(0.04, 0.7, 0.7, 1.0);
	mtl3.setShininess(10);
	Material mtl4;
	mtl4.setAmbient(0.3, 0.1, 0.1, 1.0);
	mtl4.setDiffuse(0.5, 0.0, 0.0, 1.0);
	mtl4.setSpecular(0.7, 0.6, 0.6, 1.0);
	mtl4.setShininess(32);
	Material mtl5;
	mtl5.setAmbient(0.1, 0.3, 0.1, 1.0);
	mtl5.setDiffuse(0.1, 0.35, 0.1, 1.0);
	mtl5.setSpecular(0.45, 0.55, 0.45, 1.0);
	mtl5.setShininess(32);
	Material mtl6;
	mtl6.setAmbient(0.1, 0.1, 0.3, 1.0);
	mtl6.setDiffuse(0.0, 0.50980392, 0.50980392, 1.0);
	mtl6.setSpecular(0.50196078, 0.50196078, 0.50196078, 1.0);
	mtl6.setShininess(32);
	MTL.push_back(mtl1);
	MTL.push_back(mtl2);
	MTL.push_back(mtl3);
	MTL.push_back(mtl4);
	MTL.push_back(mtl5);
	MTL.push_back(mtl6);

	life = 3;
}
// 각 스테이지 초기 설정
void setStage1() {
	life = 3;
	balloon ball(0);
	ball.setCenter(0, ball.getMaxHeight(), 0);
	ball.setMTL(MTL[0]);
	ball.setVelocity(HORIZONTAL_VELOCITY, 0, 0);
	iBall.push_back(ball);
	ignore = false;
	timer = 0;

	iCharacter.setPos(0, iCharacter.getCharacterSize()[1] + BOTTOM_END, 0);

	ropeVec1.clear();
	ropeVec2.clear();
	rope r;
	r.setVelocity(0, HARPOON_SPEED, 0);
	ropeVec1.push_back(r);

	itemVec1.clear();

	isSlow = false;
	slow = 0;
}
void setStage2() {
	balloon ball(3);
	ball.setCenter(-100, 200, 0);
	ball.setMTL(MTL[3]);
	ball.setVelocity(HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
	iBall.push_back(ball);

	ball.setCenter(100, 200, 0);
	ball.setMTL(MTL[3]);
	ball.setVelocity(-HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
	iBall.push_back(ball);

	ignore = false;
	timer = 0;

	iCharacter.setPos(0, iCharacter.getCharacterSize()[1] + BOTTOM_END, 0);

	ropeVec1.clear();
	ropeVec2.clear();
	rope r;
	r.setVelocity(0, HARPOON_SPEED, 0);
	ropeVec1.push_back(r);

	itemVec1.clear();

	isSlow = false;
	slow = 0;
}
void setStage3() {
	balloon ball(0);
	ball.setCenter(-150, ball.getMaxHeight(), 0);
	ball.setMTL(MTL[0]);
	ball.setVelocity(-HORIZONTAL_VELOCITY, 0, 0);
	iBall.push_back(ball);

	balloon ball2(3);
	ball2.setCenter(-100, 200, 0);
	ball2.setMTL(MTL[3]);
	ball2.setVelocity(HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
	iBall.push_back(ball2);

	ball2.setCenter(100, 200, 0);
	ball2.setMTL(MTL[3]);
	ball2.setVelocity(-HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
	iBall.push_back(ball2);

	ignore = false;
	timer = 0;

	iCharacter.setPos(0, iCharacter.getCharacterSize()[1] + BOTTOM_END, 0);

	ropeVec1.clear();
	ropeVec2.clear();
	rope r;
	r.setVelocity(0, HARPOON_SPEED, 0);
	ropeVec1.push_back(r);
	itemVec1.clear();

	isSlow = false;
	slow = 0;
}
// 게임 실행 시 테두리 설정
void createBlock() {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	for (int i = 0; i < 16; i++) { // top edge
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(LEFT_END + BLOCK_SIZE * (i - 1), TOP_END, 0);
		glTexCoord2f(0, 1);
		glVertex3f(LEFT_END + BLOCK_SIZE * (i - 1), TOP_END + BLOCK_SIZE, 0);
		glTexCoord2f(1, 1);
		glVertex3f(LEFT_END + BLOCK_SIZE * i, TOP_END + BLOCK_SIZE, 0);
		glTexCoord2f(1, 0);
		glVertex3f(LEFT_END + BLOCK_SIZE * i, TOP_END, 0);
		glEnd();
	}
	for (int i = 0; i < 16; i++) { // bottom edge
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(LEFT_END + BLOCK_SIZE * (i - 1), BOTTOM_END - BLOCK_SIZE, 0);
		glTexCoord2f(0, 1);
		glVertex3f(LEFT_END + BLOCK_SIZE * (i - 1), BOTTOM_END, 0);
		glTexCoord2f(1, 1);
		glVertex3f(LEFT_END + BLOCK_SIZE * i, BOTTOM_END, 0);
		glTexCoord2f(1, 0);
		glVertex3f(LEFT_END + BLOCK_SIZE * i, BOTTOM_END - BLOCK_SIZE, 0);
		glEnd();
	}
	for (int i = 0; i < 10; i++) { // left edge
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(LEFT_END - BLOCK_SIZE, BOTTOM_END + BLOCK_SIZE * i, 0);
		glTexCoord2f(0, 1);
		glVertex3f(LEFT_END - BLOCK_SIZE, BOTTOM_END + BLOCK_SIZE * (i + 1), 0);
		glTexCoord2f(1, 1);
		glVertex3f(LEFT_END, BOTTOM_END + BLOCK_SIZE * (i + 1), 0);
		glTexCoord2f(1, 0);
		glVertex3f(LEFT_END, BOTTOM_END + BLOCK_SIZE * i, 0);
		glEnd();
	}
	for (int i = 0; i < 10; i++) { // right edge
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(RIGHT_END, BOTTOM_END + BLOCK_SIZE * i, 0);
		glTexCoord2f(0, 1);
		glVertex3f(RIGHT_END, BOTTOM_END + BLOCK_SIZE * (i + 1), 0);
		glTexCoord2f(1, 1);
		glVertex3f(RIGHT_END + BLOCK_SIZE, BOTTOM_END + BLOCK_SIZE * (i + 1), 0);
		glTexCoord2f(1, 0);
		glVertex3f(RIGHT_END + BLOCK_SIZE, BOTTOM_END + BLOCK_SIZE * i, 0);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}
// 게임 시작시 배경 설정
void createBackground() {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, textureID[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(LEFT_END, BOTTOM_END, 0);
	glTexCoord2f(0, 1);
	glVertex3f(LEFT_END, TOP_END, 0);
	glTexCoord2f(1, 1);
	glVertex3f(RIGHT_END, TOP_END, 0);
	glTexCoord2f(1, 0);
	glVertex3f(RIGHT_END, BOTTOM_END, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
float pow(float num) {
	return num * num;
}
// 풍선과 캐릭터가 부딪혔는지 판단하는 함수.
void hit() {
	bool isHit1;
	bool isHit2;
	for (auto i = 0; i < iBall.size(); i++) {
		isHit1 = (sqrt(pow(iBall[i].getCenter()[0] - iCharacter.getPos()[0]) + pow(iBall[i].getCenter()[1] - (iCharacter.getPos()[1] - iCharacter.getCharacterSize()[1] / 2 + 5))) < iBall[i].getRadius() + iCharacter.getCharacterSize()[0] / 2 - 9);
		isHit2 = (sqrt(pow(iBall[i].getCenter()[0] - iCharacter.getPos()[0]) + pow(iBall[i].getCenter()[1] - (iCharacter.getPos()[1] - iCharacter.getCharacterSize()[1] * 3 / 4))) < iBall[i].getRadius() + iCharacter.getCharacterSize()[0] / 2);
		if (!(ignore)&&(isHit1||isHit2)) {
			life--;
			timer = 0;
			ignore = true;
		}
	}
}
int genRandInRange(int min, int max) {
	return rand() % (max - min + 1) + min;
}
// 풍선을 터뜨렸을때 아이템이 생성되는지 결정하는 함수.
void generateItem(balloon b) {
	int num = genRandInRange(0, 9);
	if (num == 0) {
		item i(num,itemTexID[num]);
		i.setCenter(b.getCenter()[0], b.getCenter()[1], b.getCenter()[2]);
		itemVec1.push_back(i);
	}
	if (num == 1) {
		item i(num, itemTexID[num]);
		i.setCenter(b.getCenter()[0], b.getCenter()[1], b.getCenter()[2]);
		itemVec1.push_back(i);
	}
	if (num == 2) {
		item i(num, itemTexID[num]);
		i.setCenter(b.getCenter()[0], b.getCenter()[1], b.getCenter()[2]);
		itemVec1.push_back(i);
	}
}
// 풍선이 작살과 닿았을 때의 동작 구현
void split() {
	for (auto i = 0; i < ropeVec2.size(); i++) {
		for (auto j = 0; j < iBall.size(); j++) {
			if ((abs(iBall[j].getCenter()[0] - ropeVec2[i].getPos()[0]) < (iBall[j].getRadius() + ropeVec2[i].getRopeSize()[0] / 2)) && (iBall[j].getCenter()[1] < (ropeVec2[i].getPos()[1] - ropeVec2[i].getHarpoonSize()[1] / 2))) {
				if (iBall[j].getType() % 3 != 2) {
					balloon b(iBall[j].getType() + 1);
					b.setCenter(iBall[j].getCenter()[0], iBall[j].getCenter()[1], 0);
					b.setMTL(MTL[iBall[j].getType() + 1]);
					if (iBall[j].getType() / 3 == 0) {
						if (iBall[j].getCenter()[1] > b.getMaxHeight()) { // 풍선의 높이가 하위 풍선의 최대높이보다 높을 경우 우선 땅으로 추락
							b.setVelocity(HORIZONTAL_VELOCITY, iBall[j].getVelocity()[1], 0);
							iBall.push_back(b);
							b.setVelocity(-HORIZONTAL_VELOCITY, iBall[j].getVelocity()[1], 0);
							iBall.push_back(b);
						}
						else { // 에너지 보존 법칙을 이용하여 높이가 일정하게 속도 설정
							b.setVelocity(HORIZONTAL_VELOCITY, sqrt(2 * (-iBall[j].getAcceleration()) * (b.getMaxHeight() - b.getCenter()[1])), 0);
							iBall.push_back(b);
							b.setVelocity(-HORIZONTAL_VELOCITY, sqrt(2 * (-iBall[j].getAcceleration()) * (b.getMaxHeight() - b.getCenter()[1])), 0);
							iBall.push_back(b);
						}
						generateItem(iBall[j]);
						iBall.erase(iBall.begin() + j);
					}
					else {
						b.setVelocity(HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
						iBall.push_back(b);
						b.setVelocity(-HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
						iBall.push_back(b);
						generateItem(iBall[j]);
						iBall.erase(iBall.begin() + j);
					}
				}else if (iBall[j].getType() % 3 == 2) {
					generateItem(iBall[j]);
					iBall.erase(iBall.begin() + j);
				}
				ropeVec2[i].setIsShot(false);
				ropeVec1.push_back(ropeVec2[i]);
				ropeVec2.erase(ropeVec2.begin() + i);
				break;
			}
		}
	}
}
// 작살의 움직임 설정
void ropeControl() {
	for (auto i = 0; i < ropeVec2.size(); i++) {
		ropeVec2[i].move();
		if (ropeVec2[i].getPos()[1] > TOP_END) {
			ropeVec2[i].setIsShot(false);
			ropeVec1.push_back(ropeVec2[i]);
			ropeVec2.erase(ropeVec2.begin() + i);
		}
	}

}
// 공의 움직임 설정
void ballControl() {
	for (auto i = 0; i < iBall.size(); i++) {
		if (iBall[i].getCenter()[0] + iBall[i].getRadius() > RIGHT_END) {
			iBall[i].setVelocity(-iBall[i].getVelocity()[0], iBall[i].getVelocity()[1], iBall[i].getVelocity()[2]);
		}
		if (iBall[i].getCenter()[0] - iBall[i].getRadius() < LEFT_END) {
			iBall[i].setVelocity(-iBall[i].getVelocity()[0], iBall[i].getVelocity()[1], iBall[i].getVelocity()[2]);
		}
		if (iBall[i].getType() / 3 == 0) {
			if ((iBall[i].getVelocity()[1] < 0) && (iBall[i].getCenter()[1] - iBall[i].getRadius() <= BOTTOM_END)) {
				iBall[i].setVelocity(iBall[i].getVelocity()[0], sqrt(2 * (-iBall[i].getAcceleration()) * (iBall[i].getMaxHeight() - iBall[i].getCenter()[1])) - iBall[i].getAcceleration(), iBall[i].getVelocity()[2]);
			}
		}
		else {
			if (iBall[i].getCenter()[1] + iBall[i].getRadius() > TOP_END) {
				iBall[i].setVelocity(iBall[i].getVelocity()[0], -iBall[i].getVelocity()[1], iBall[i].getVelocity()[2]);
			}
			if (iBall[i].getCenter()[1] - iBall[i].getRadius() < BOTTOM_END) {
				iBall[i].setVelocity(iBall[i].getVelocity()[0], -iBall[i].getVelocity()[1], iBall[i].getVelocity()[2]);
			}
		}
		iBall[i].move();
	}
}
// 아이템 드랍시 바닥에서 깜빡이는 시간 설정
void itemControl() {
	for (auto i = 0; i < itemVec1.size(); i++) {
		if (itemVec1[i].getCenter()[1] - itemVec1[i].getSize() > BOTTOM_END) {
			itemVec1[i].move();
		}
		else {
			itemVec1[i].setTimer(itemVec1[i].getTimer() + 1);
		}
		if (itemVec1[i].getTimer() > 240) {
			itemVec1.erase(itemVec1.begin() + i);
		}
	}
}
// 아이템을 습득했을 때의 동작 설정
void pickItem() {
	for (auto i = 0; i < itemVec1.size(); i++) {
		if (itemVec1[i].getCenter()[1] < BOTTOM_END + iCharacter.getCharacterSize()[1] + itemVec1[i].getSize() && abs(itemVec1[i].getCenter()[0] - iCharacter.getPos()[0]) < itemVec1[i].getSize() + iCharacter.getCharacterSize()[0] / 2){
			if (itemVec1[i].getType() == 0 && (ropeVec1.size() + ropeVec2.size() < 2)) { // 이중작살 아이템
				rope r;
				r.setVelocity(0, HARPOON_SPEED, 0);
				ropeVec1.push_back(r);
			}
			if (itemVec1[i].getType() == 1) { // 다이너마이트 아이템
				int num = 0;
				while (num != iBall.size()) {
					num = 0;
					for (int i = 0; i < iBall.size(); i++) {
						if (iBall[i].getType() % 3 != 2) {
							balloon b(iBall[i].getType() + 1);
							b.setCenter(iBall[i].getCenter()[0], iBall[i].getCenter()[1], 0);
							b.setMTL(MTL[iBall[i].getType() + 1]);
							if (iBall[i].getType() / 3 == 0) {
								if (iBall[i].getCenter()[1] > b.getMaxHeight()) {
									b.setVelocity(HORIZONTAL_VELOCITY, iBall[i].getVelocity()[1], 0);
									iBall.push_back(b);
									b.setVelocity(-HORIZONTAL_VELOCITY, iBall[i].getVelocity()[1], 0);
									iBall.push_back(b);
								}
								else {
									b.setVelocity(HORIZONTAL_VELOCITY, sqrt(2 * (-iBall[i].getAcceleration()) * (b.getMaxHeight() - b.getCenter()[1])), 0);
									iBall.push_back(b);
									b.setVelocity(-HORIZONTAL_VELOCITY, sqrt(2 * (-iBall[i].getAcceleration()) * (b.getMaxHeight() - b.getCenter()[1])), 0);
									iBall.push_back(b);
								}
								iBall.erase(iBall.begin() + i);
							}
							else {
								b.setVelocity(HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
								iBall.push_back(b);
								b.setVelocity(-HORIZONTAL_VELOCITY, VERTICAL_VELOCITY, 0);
								iBall.push_back(b);
								iBall.erase(iBall.begin() + i);
							}
						}
						else {
							num++;
						}
					}
				}
			}
			if (itemVec1[i].getType() == 2) { // 모래시계 아이템
				isSlow = true;
				slow += 300;
			}
			itemVec1.erase(itemVec1.begin() + i);
		}
	}
}
void idle() {
	if (gameState % 2 == 1  && !isPause) { // FPS 컨트롤
		tend = clock();
		if (tend - tstart > 1000 / 60) {
			hit();
			split();
			if (slow % 2 == 0) {
				ballControl();
			}
			ropeControl();
			itemControl();
			pickItem();
			if (ignore) { // 무적 설정
				timer++;
				if (timer == 100)
					ignore = false;
			}
			if (isSlow && slow > 0) { // 모래시계 아이템 동작 설정
				slow--;
			}
			tstart = tend;
		}
	}
	if (gameState % 2 == 0) { // 깜빡이는 글자 설정
		tend = clock();
		if (tend - tstart > 1000 / 2) {
			t = !t;
			tstart = tend;
		}
	}
	if (life == 0) { // 게임 종료 조건 설정
		gameState = 8;
	}
	glutPostRedisplay();
}
void specialKeyDown(int key, int x, int y) { // 키보드 좌우 움직임 설정
	if (gameState % 2 == 1 && !isPause) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			if ((iCharacter.getPos()[0] + iCharacter.getCharacterSize()[0] / 2) < RIGHT_END)
				iCharacter.setPos(iCharacter.getPos()[0] + iCharacter.getSpeed(), iCharacter.getPos()[1], iCharacter.getPos()[2]);
			break;
		case GLUT_KEY_LEFT:
			if ((iCharacter.getPos()[0] - iCharacter.getCharacterSize()[0] / 2) > LEFT_END)
				iCharacter.setPos(iCharacter.getPos()[0] - iCharacter.getSpeed(), iCharacter.getPos()[1], iCharacter.getPos()[2]);
			break;
		}
	}
}
void keyboardDown(unsigned char key, int x, int y) {
	switch (key) { // space 키 동작 설정
	case 32:
		if (gameState % 2 == 1 && !isPause) {
			if (ropeVec1.size() > 0) {
				ropeVec1[ropeVec1.size() - 1].setIsShot(true);
				ropeVec2.push_back(ropeVec1[ropeVec1.size() - 1]);
				ropeVec1.pop_back();
				ropeVec2.back().setPos(iCharacter.getPos()[0], iCharacter.getPos()[1] - iCharacter.getCharacterSize()[1], iCharacter.getPos()[2]);
			}
		}
		if (gameState == 0) {
			setStage1();
			gameState++;
		}
		if (gameState == 2) {
			setStage2();
			gameState++;
		}
		if (gameState == 4) {
			setStage3();
			gameState++;
		}
		if (gameState == 6) {
			gameState = 10;
		}
		if (gameState == 8) {
			life = 3;
			gameState = 10; 
			iBall.clear();
		}
		if (gameState == 10) {
			gameState = 0;
		}
		break;
	case 'p': // p 동작 설정 (일시정지)
	case 'P':
		if (gameState % 2 == 1)
			isPause = !isPause;
		break; // ESC 누르면 종료
	case 27:
		exit(0);
	}

}
void renderScene() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (gameState == 10) { // 처음과 끝, 중간의 메시지 출력
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "PANG!", -40, 20);
		if (t) 
			drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Press SPACE BAR to start", -130, -140);
	}
	if (gameState == 0) {
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Stage 1", -40, 20);
		if (t)
			drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Press SPACE BAR to start", -130, -140);
	}
	if (gameState == 2) {
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Stage 2", -40, 20);
		if (t)
			drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Press SPACE BAR to start", -130, -140);
	}
	if (gameState == 4) {
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Stage 3", -40, 20);
		if (t)
			drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Press SPACE BAR to start", -130, -140);
	}
	if (gameState == 6) {
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Clear", -35, 20);
		if (t)
			drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Press SPACE BAR to play again", -160, -140);
	}
	if (gameState == 8) {
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Failed", -37, 20);
		if (t)
			drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "Press SPACE BAR to restart", -145, -140);
	}
	if (gameState % 2 == 1) { // 게임이 진행되는 동안의 작동함수
		createBackground();

		if (timer % 2 == 0) {
			iCharacter.draw(textureID[4]);
		}
		for (auto i = 0; i < itemVec1.size(); i++) {
			if (itemVec1[i].getTimer() < 120 || itemVec1[i].getTimer() % 2 == 0) 
				itemVec1[i].draw();
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		light.draw();

		for (auto i = 0; i < iBall.size(); i++) {
			iBall[i].draw();
		}

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		for (auto i = 0; i < ropeVec2.size(); i++) {
			ropeVec2[i].draw(textureID[2], textureID[3]);
		}

		glColor3f(0.f, 0.f, 0.f);
		glBegin(GL_QUADS);
		glVertex2f(-WIDTH / 2, -HEIGHT / 2);
		glVertex2f(-WIDTH / 2, BOTTOM_END - BLOCK_SIZE);
		glVertex2f(WIDTH / 2, BOTTOM_END - BLOCK_SIZE);
		glVertex2f(WIDTH / 2, -HEIGHT / 2);
		glEnd();
		createBlock();

		for (int i = 0; i < life; i++) {
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, textureID[5]);
			glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);
			glVertex3f(-250 + 30 * i, -300, 0);
			glTexCoord2f(0, 1);
			glVertex3f(-250 + 30 * i, -280, 0);
			glTexCoord2f(1, 1);
			glVertex3f(-230 + 30 * i, -280, 0);
			glTexCoord2f(1, 0);
			glVertex3f(-230 + 30 * i, -300, 0);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "life: ", -WIDTH / 2 + 20, -HEIGHT / 2 + 20);
		drawCharacters(GLUT_BITMAP_TIMES_ROMAN_24, "rope: ", -WIDTH / 2 + 20, -HEIGHT / 2 + 60);
		drawInteger(GLUT_BITMAP_TIMES_ROMAN_24, ropeVec1.size() + ropeVec2.size(), -WIDTH / 2 + 75, -HEIGHT / 2 + 60);
		drawCharacters(GLUT_BITMAP_HELVETICA_18, "SPACE BAR: shoot", 100, -HEIGHT / 2 + 120);
		drawCharacters(GLUT_BITMAP_HELVETICA_18, "Arrow key: move", 100, -HEIGHT / 2 + 90);
		drawCharacters(GLUT_BITMAP_HELVETICA_18, "P: pause", 100, -HEIGHT / 2 + 60);
		drawCharacters(GLUT_BITMAP_HELVETICA_18, "ESC: exit", 100, -HEIGHT / 2 + 30);
		if (iBall.size() == 0)
			gameState++;
	}
	glutSwapBuffers();
}
void main(int argc, char** argv) {
	srand(time(NULL));
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("PANG");

	initialize();
	initTexture();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboardDown);
	glutSpecialFunc(specialKeyDown);
	glutIdleFunc(idle);

	// enter GLUT event processing cycle
	glutMainLoop();
}