#define _CRT_SECURE_NO_WARNINGS
#define GL_BGRA 0x80E1 //32비트 bmp를 사용
#define GL_BGR_EXT 0x80E0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include "GL/glut.h"

//사용하는 함수들 선언
void light();
void fog();
GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
void texture();
void textureN();
void init();
void home();
void sphere();
void build();
void mydisplay();
void keyboard(unsigned char key, int x, int y);

//Texture를 Loading하는데 필요한 변수들
unsigned char header[54];
unsigned int dataPos;
unsigned int width, height;
unsigned int imagesize;
unsigned char *data;
//Texture를 initialize하는데 필요한 변수들 
GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
GLuint textures[5]; //텍스처 숫자
GLubyte *pBytes; // 데이터를 가리킬 포인터
BITMAPINFO *info; //비트맵 헤더를 저장할 변수

//Texture 껐다 켰다 할 때 쓸 변수
bool tex = TRUE;   //default값은 킴

//안개의 색을 결정 할 변수
const GLfloat fc[] = { 0.1,0.1,0.1,0.1 };
//안개의 정도를 결정 할 변수
GLfloat den= 0.1f;

//LookAt변수
GLfloat eye1 = 30.0;
GLfloat eye2 = 120.0;
GLfloat eye3 = -250.0;

void light()  //빛을 키는 함수
{
	//전체적인 빛을 킴
	glEnable(GL_LIGHTING);

	//GL_LIGHT0
	GLfloat ambient0[] = { 0.7f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuse0[] = { 0.3f, 0.5f, 0.5f, 1.0f };
	GLfloat	specular0[] = { 1.0f, 0.8f, 0.9f, 1.0f };
	GLfloat lightPos0[] = { 60.0, 20.0, -30.0, 1.0f };

	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//GL_LIGHT1
	GLfloat ambient1[] = { 0.5f, 0.7f, 0.9f, 0.5f };
	GLfloat diffuse1[] = { 0.5f, 0.1f, 0.1f, 0.4f };
	GLfloat	specular1[] = { 0.8f, 0.7f, 0.8f, 0.9f };
	GLfloat lightPos1[] = { -50.0, 30.0, 30.0, 1.0f };

	glEnable(GL_LIGHT1);

	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	//GL_LIGHT2
	GLfloat ambient2[] = { 0.75f, 0.2f, 0.5f, 0.1f };
	GLfloat diffuse2[] = { 0.25f, 0.8f, 0.5f, 0.8f };
	GLfloat	specular2[] = { 0.2f, 0.4f, 0.5f, 0.3f };
	GLfloat lightPos2[] = { 30.0, 40.0, -80.0, 1.0f };

	glEnable(GL_LIGHT2);

	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);

	//material Light
	GLfloat material_Ka[] = { 0.75, 0.18, 0.80, 1.00 };
	GLfloat material_Kd[] = { 0.30, 0.34, 0.81, 1.00 };
	GLfloat material_Ks[] = { 0.34, 0.56, 0.17, 1.00 };
	GLfloat material_Ke[] = { 0.50, 0.10, 0.40, 0.00 };
	GLfloat material_Se = 70;

	/*물체들의 색을 분홍색 계열로 바꿈
	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material_Ka);
	glMateriali(GL_FRONT, GL_SHININESS, 2);
	*/
}

void fog()  //안개를 키는 함수
{
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fc);   //안개의 색을 결정
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 8.0f);   //안개의 최대 짙은 정도
	glFogf(GL_FOG_END, 0.5f);     //안개의 최대 옅은 정도
}


GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info)  //texture를 loading
{
	FILE *file = fopen(filename, "rb");    //파일을 열어옴
	if (!file) { printf("이미지가 열리지않습니다\n"); }  //파일자체가 열리는지 확인
	
	if (fread(header, 1, 54, file) != 54)   //파일의 처음부분이 54바이트 헤더
	{
		printf("BMP파일이 아닙니다\n");
	}
	else printf("이미지를 불러옵니다.\n");

	if (header[0] != 'B' || header[1] != 'M')       //bmp파일인지 아닌지 확인
	{
		printf("BMP파일이 아닙니다");
	}
	else printf("이미지를 제대로 불러옵니다.\n");

	// 바이트 배열에서 int 변수를 읽음 
	dataPos = *(int*)&(header[0x0A]);
	imagesize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// 정보가 누락되는 것을 방지
	if (imagesize == 0) imagesize = width * height * 3;
	if (dataPos == 0) dataPos = 54;

	// 버퍼 생성
	data = new unsigned char[imagesize];

	// 파일에서 버퍼로 실제 데이터 넣기
	fread(data, 1, imagesize, file);

	// texture가 GPU안에 있으므로 파일을 닫음
	fclose(file);
	return data;
}
	
void texture()   // texture initialization -> Linear
{
	glGenTextures(5, textures); // 5개의 텍스처의 이름을 지정

	// 1번째 텍스쳐
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	pBytes = LoadDIBitmap("기본.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 2번째 텍스쳐
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("moon.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate
	
	// 3번째 텍스쳐
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	pBytes = LoadDIBitmap("태양.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 4번째 텍스쳐
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	pBytes = LoadDIBitmap("비늘.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 5번째 텍스쳐
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	pBytes = LoadDIBitmap("바닥.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

}
void textureN()    // texture initialization -> Nearest
{
	glGenTextures(5, textures); // 5개의 텍스처의 이름을 지정

	// 1번째 텍스처
	glBindTexture(GL_TEXTURE_2D, textures[0]);  
	pBytes = LoadDIBitmap("기본.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 2번째 텍스처
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("moon.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);      
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 3번째 텍스처
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	pBytes = LoadDIBitmap("태양.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 4번째 텍스처
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	pBytes = LoadDIBitmap("비늘.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 5번째 텍스처
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	pBytes = LoadDIBitmap("바닥.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate
	  
}

void init()
{
	glClearColor(0.0, 0.7, 1.0, 1.0);         // 배경색, 맨끝은 투명도(상관무)

	glViewport(0.0, 0.0, 500.0, 500.0);        //전체 공간에서 클리핑해서 보여줄 부분을 선택

	glEnable(GL_DEPTH_TEST);         //hidden surfae removal,

	light(); //빛을 키는 함수 호출

	texture(); //텍스처를 불러움

}

void home()  //사각형 건물들을 만들 때 사용
{
	glutSolidCube(5);
}

void sphere()  //구를 사용한 건물들을 만들 때 사용
{
	//텍스처를 구에 매핑
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	
	glutSolidSphere(5.0, 20.0, 20.0);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}
void build()     //normalvector를 사용할 건물
{
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glBegin(GL_QUADS);
	glNormal3f(0.0, -1.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0, -5.0, 5.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0, -5.0, 5.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0, -5.0, -5.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0, -5.0, -5.0);

	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0, -5.0, -5.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-5.0, -5.0, -5.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0, 5.0, -5.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0, 5.0, -5.0);

	glNormal3f(1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0, 5.0, -5.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0, -5.0, -5.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0, -5.0, 5.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0, 5.0, 5.0);

	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(5.0, 5.0, 5.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0, -5.0, 5.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0, -5.0, 5.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0, 5.0, 5.0);

	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0, 5.0, 5.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0, 5.0, -5.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0, -5.0, -5.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0, 5.0, -5.0);

	glNormal3f(0.0, 1.0, -0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0, 5.0, -5.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(5.0, 5.0, -5.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(5.0, 5.0, 5.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-5.0, 5.0, 5.0);

	glBindTexture(GL_TEXTURE_2D, textures[0]);  //다시 기본이미지로 바꿔줌

	glEnd();
}

void mydisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 3차원 공간에서 프레임버퍼에 이전에 있던걸 다 지워줌

	glMatrixMode(GL_PROJECTION);              //카메라에관한것	
	glLoadIdentity();                         //카메라에관한것 -> initialization
	gluPerspective(55.0, 1.0, 0.1, 500.0);      // 내가 그린 물체들을 표현하는 범위

	gluLookAt(eye1, eye2, eye3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //내 카메라가 어느 위치에서 어느 곳을 어느 방향으로 볼 것인지를 결정
	
	glMatrixMode(GL_MODELVIEW);               //물체를 그릴것
	glLoadIdentity();

	//texture를 키고 끌 때 사용
	if (tex == TRUE)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	glEnable(GL_FOG);
	glFogf(GL_FOG_DENSITY, den);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textures[4]); 
	//밑의 사각형 모양의 땅
	glPushMatrix();
	glNormal3f(0.0, 0.0, 1.0); //평면 바닥의 normal vector
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(80.0, 0.0, 80.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-80.0, 0.0, 80.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-80.0, 0.0, -80.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(80.0, 0.0, -80.0);
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	//ㄴ자 건물
	glPushMatrix();
	glTranslatef(-10.0, 5.0, -25.0);
	glScalef(1.0, 2.0, 1.5);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5.0, -2.5, 0.0);
	glPushMatrix();
	glTranslatef(-10.0, 5.0, -25.0);
	glScalef(1.0, 1.0, 1.5);
	home();
	glPopMatrix();
	glPopMatrix();

	//ㄴ자 건물 뒤의 네모난 빌딩
	glPushMatrix();
	glTranslatef(-10.0, 12.0, -14.0);
	glScalef(1.0, 5.0, 1.5);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10.0, 25.0, -14.0);
	glScalef(1.8, 0.5, 1.5);
	home();
	glPopMatrix();

	//정가운데의 최고고층빌딩
	glPushMatrix();
	glTranslatef(5.0, 20.0, 8.0);
	glScalef(2.5, 8.0, 2.0);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.0, -5.0, 0.0);
	glScalef(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(10.0, 20.0, 8.0);
	glScalef(2.5, 6.0, 2.0);
	home();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-17.2, -5.0, 0.0);
	glScalef(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(10.0, 20.0, 8.0);
	glScalef(2.5, 6.0, 2.0);
	home();
	glPopMatrix();
	glPopMatrix();

	//모아이 석상 모양의 건물
	glPushMatrix();
	glTranslatef(20.0, 7.5, -25.0);
	glScalef(2.0, 3.0, 1.5);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(17.0, 5.0, -13.0);
	glScalef(0.5, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(20.0, 7.5, -25.0);
	glScalef(2.0, 3.0, 1.5);
	home();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(13.0, 9.5, -25.0);
	glScalef(1.0, 1.5, 1.0);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(13.0, 7.5, -6.2);
	glScalef(0.5, 0.5, 1.0);
	glPushMatrix();
	glTranslatef(13.0, 1.0, -25.0);
	glScalef(1.0, 1.5, 1.0);
	home();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16.0, 11.5, -30.7);
	glScalef(0.4, 0.4, 0.4);
	sphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(16.0, 11.5, -30.7);
	glScalef(0.4, 0.4, 0.4);
	sphere();
	glPopMatrix();
	glPopMatrix();

	//바벨 모양의 건물
	glPushMatrix();
	glTranslatef(5.0, 8.0, 50.0);
	glScalef(2.5, 2.5, 2.5);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5.0, 26.0, 50.0);
	glScalef(1.5, 5.0, 1.5);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 32.0, 0.0);
	glPushMatrix();
	glTranslatef(5.0, 8.0, 50.0);
	glScalef(2.5, 2.5, 2.5);
	home();
	glPopMatrix();
	glPopMatrix();

	//직육면체 - 구 - 직육면체 건물
	glPushMatrix();
	glTranslatef(40.0, 7.5, -20.0);
	glScalef(2.0, 2.5, 2.0);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(40.0, 18.5, -20.0);
	glScalef(1.0, 1.0, 1.0);
	sphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 22.0, 0.0);
	glPushMatrix();
	glTranslatef(40.0, 7.5, -20.0);
	glScalef(2.0, 2.5, 2.0);
	home();
	glPopMatrix();
	glPopMatrix();

	//직육면체위에 구가 올려진 건물(직육면체에 normal vector 사용) - 왼쪽 앞
	glPushMatrix();
	glTranslatef(50.0, 15.0, -50.0);
	glScalef(1.0, 3.0, 1.0);
	build();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50.0, 35.0, -50.0);
	glScalef(1.0, 1.0, 1.0);
	sphere();
	glPopMatrix();

	//직육면체위에 구가 올려진 건물(직육면체에 normal vector 사용) - 오른쪽 앞
	glPushMatrix();
	glTranslatef(-100.0, 0.0, -20.0);
	glPushMatrix();
	glTranslatef(50.0, 15.0, -30.0);
	glScalef(1.0, 3.0, 1.0);
	build();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50.0, 35.0, -30.0);
	glScalef(1.0, 1.0, 1.0);
	sphere();
	glPopMatrix();
	glPopMatrix();

	//직육면체위에 구가 올려진 건물(직육면체에 normal vector 사용) - 오른쪽 뒤
	glPushMatrix();
	glTranslatef(-100.0, 0.0, 90.0);
	glPushMatrix();
	glTranslatef(50.0, 15.0, -30.0);
	glScalef(1.0, 3.0, 1.0);
	build();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50.0, 35.0, -30.0);
	glScalef(1.0, 1.0, 1.0);
	sphere();
	glPopMatrix();
	glPopMatrix();

	//직육면체위에 구가 올려진 건물(직육면체에 normal vector 사용) - 왼쪽 뒤
	glPushMatrix();
	glTranslatef(-0.5, 0.0, 90.0);
	glPushMatrix();
	glTranslatef(50.0, 15.0, -30.0);
	glScalef(1.0, 3.0, 1.0);
	build();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50.0, 35.0, -30.0);
	glScalef(1.0, 1.0, 1.0);
	sphere();
	glPopMatrix();
	glPopMatrix();

	//4개의 기둥을 잇는 직육면체 - 앞
	glPushMatrix();
	glTranslatef(0.0, 35.0, -50.0);
	glScalef(18.2, 1.0, 1.0);
	home();
	glPopMatrix();

	//4개의 기둥을 잇는 직육면체 - 뒤
	glPushMatrix();
	glTranslatef(0.0, 35.0, 60.0);
	glScalef(18.0, 1.0, 1.0);
	home();
	glPopMatrix();

	//4개의 기둥을 잇는 직육면체 - 왼
	glPushMatrix();
	glTranslatef(50.0, 35.0, 5.0);
	glScalef(1.0, 1.0, 20.0);
	home();
	glPopMatrix();

	//4개의 기둥을 잇는 직육면체 - 오
	glPushMatrix();
	glTranslatef(-50.0, 35.0, 5.0);
	glScalef(1.0, 1.0, 20.0);
	home();
	glPopMatrix();
	/*
	glEnable(GL_FOG);
	glFogf(GL_FOG_DENSITY, den);
	*/
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	//오른쪽 태양
	glPushMatrix();
	glTranslatef(-120.0, 120.0, 5.0);
	glScalef(5.0, 5.0, 5.0);
	sphere();
	glPopMatrix();

	//왼쪽 태양
	glPushMatrix();
	glTranslatef(240.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(-120.0, 120.0, 5.0);
	glScalef(5.0, 5.0, 5.0);
	sphere();
	glPopMatrix();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[3]);
	//외계 물고기 1
	glPushMatrix();
	glTranslatef(120.0, 15.0, 0.0);
	glScalef(2.5, 2.0, 2.0);
	sphere();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glPushMatrix();
	glTranslatef(130.0, 20.0, 5.0);
	glScalef(0.5, 0.5, 0.5);
	sphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, -10.0);
	glPushMatrix();
	glTranslatef(130.0, 20.0, 5.0);
	glScalef(0.5, 0.5, 0.5);
	sphere();
	glPopMatrix();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[3]);

	glPushMatrix();
	glTranslatef(120.0, 3.0, 0.0);
	glScalef(2.0, 1.0, 1.0);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 24.0, 0.0);
	glPushMatrix();
	glTranslatef(120.0, 3.0, 0.0);
	glScalef(2.0, 1.0, 1.0);
	home();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(122.0, 17.0, 13.0);
	glScalef(1.0, 1.0, 1.5);
	home();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, -25.5);
	glPushMatrix();
	glTranslatef(122.0, 17.0, 13.0);
	glScalef(1.0, 1.0, 1.5);
	home();
	glPopMatrix();
	glPopMatrix();

	//외계 물고기 2
	glPushMatrix();
	glTranslatef(-200.0, 30.0, 15.0);

	glPushMatrix();
	glTranslatef(120.0, 15.0, 0.0);
	glScalef(2.5, 2.0, 2.0);
	sphere();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[1]);

	glPushMatrix();
	glTranslatef(130.0, 20.0, 5.0);
	glScalef(0.5, 0.5, 0.5);
	sphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, -10.0);
	glPushMatrix();
	glTranslatef(130.0, 20.0, 5.0);
	glScalef(0.5, 0.5, 0.5);
	sphere();
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_FOG);
	
	glBindTexture(GL_TEXTURE_2D, 0);   //texture mapping 종료

	glFlush();

	glutSwapBuffers();     // 더블버퍼링
}

void keyboard(unsigned char key, int x, int y)    //Keyboard control
{
	switch (key) {
	case '0':                      //0을 누르면 모든 빛이 off     
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
	case '1':                      //1을 누르면 1번빛만 on                     
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
	case '2':                      //2를 누르면 0번(메인빛)과 1번빛이 on  
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		break;
	case '3':                      //3을 누르면 모든 빛이 on
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		break;

	case 'a':                      //a 누르면 카메라 위치의 x값이 5만큼 감소
		eye1 -= 5.0;
		break;
	case 'd':                      //d 누르면 카메라 위치의 x값이 5만큼 증가
		eye1 += 5.0;
		break;
	case 'w':                      //w 누르면 카메라 위치의 y값이 5만큼 감소
		eye2 -= 5.0;
		break;
	case 's':                      //s 누르면 카메라 위치의 y값이 5만큼 증가
		eye2 += 5.0;
		break;
	case 'z':                      //z 누르면 카메라 위치의 y값이 5만큼 증가
		eye3 += 5.0;
		break;
	case 'c':                      //c 누르면 카메라 위치의 y값이 5만큼 감소
		eye3 -= 5.0;
		break;

	case '9':                      //9를 누르면 프로그램 종료
		exit(0);
		break;

	case '4':                      //4를 누르면 texture가 켜짐
		tex = TRUE;
		break;

	case '5':                      //5를 누르면 texture가 꺼짐
		tex = FALSE;
		break;

	case 'o':                      //o를 누르면 texture filtering이 Linear로 됨
		texture();
		break;

	case 'p':                      //o를 누르면 texture filtering이 nearest로 됨
		textureN();
		break;

	case 'k':                      //k를 누르면 fog가 짙어짐
		if (den <= 1.0) 
			den += 0.02;
		break;

	case 'l':                      //l을 누르면 fog가 옅어짐
		if (den <= 1.0) 
			den -= 0.02;
		break;
	}
	glutPostRedisplay();  //glutDisplayFunc에 등록된 함수를 호출
}

int main(int argc, char**argv)
{
	//glut initialization(초기화)
	glutInit(&argc, argv);
	//actual window size
	glutInitWindowSize(500, 500);
	//initial window location, top-left corner
	glutInitWindowPosition(0, 0);
	//create window with title 
	glutCreateWindow("HW3_14010976_LeeHyunSik");
	//더블버퍼 사용, 트루컬러모드, 깊이버퍼 사용
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//KeyboardFunc
	glutKeyboardFunc(keyboard);
	//call mydisplay() function
	glutDisplayFunc(mydisplay);
	//call init() function
	init();
	//main event loop, do not use exit()
	glutMainLoop();
}