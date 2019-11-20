#define _CRT_SECURE_NO_WARNINGS
#define GL_BGRA 0x80E1 //32��Ʈ bmp�� ���
#define GL_BGR_EXT 0x80E0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include "GL/glut.h"

//����ϴ� �Լ��� ����
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

//Texture�� Loading�ϴµ� �ʿ��� ������
unsigned char header[54];
unsigned int dataPos;
unsigned int width, height;
unsigned int imagesize;
unsigned char *data;
//Texture�� initialize�ϴµ� �ʿ��� ������ 
GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
GLuint textures[5]; //�ؽ�ó ����
GLubyte *pBytes; // �����͸� ����ų ������
BITMAPINFO *info; //��Ʈ�� ����� ������ ����

//Texture ���� �״� �� �� �� ����
bool tex = TRUE;   //default���� Ŵ

//�Ȱ��� ���� ���� �� ����
const GLfloat fc[] = { 0.1,0.1,0.1,0.1 };
//�Ȱ��� ������ ���� �� ����
GLfloat den= 0.1f;

//LookAt����
GLfloat eye1 = 30.0;
GLfloat eye2 = 120.0;
GLfloat eye3 = -250.0;

void light()  //���� Ű�� �Լ�
{
	//��ü���� ���� Ŵ
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

	/*��ü���� ���� ��ȫ�� �迭�� �ٲ�
	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material_Ka);
	glMateriali(GL_FRONT, GL_SHININESS, 2);
	*/
}

void fog()  //�Ȱ��� Ű�� �Լ�
{
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fc);   //�Ȱ��� ���� ����
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 8.0f);   //�Ȱ��� �ִ� £�� ����
	glFogf(GL_FOG_END, 0.5f);     //�Ȱ��� �ִ� ���� ����
}


GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info)  //texture�� loading
{
	FILE *file = fopen(filename, "rb");    //������ �����
	if (!file) { printf("�̹����� �������ʽ��ϴ�\n"); }  //������ü�� �������� Ȯ��
	
	if (fread(header, 1, 54, file) != 54)   //������ ó���κ��� 54����Ʈ ���
	{
		printf("BMP������ �ƴմϴ�\n");
	}
	else printf("�̹����� �ҷ��ɴϴ�.\n");

	if (header[0] != 'B' || header[1] != 'M')       //bmp�������� �ƴ��� Ȯ��
	{
		printf("BMP������ �ƴմϴ�");
	}
	else printf("�̹����� ����� �ҷ��ɴϴ�.\n");

	// ����Ʈ �迭���� int ������ ���� 
	dataPos = *(int*)&(header[0x0A]);
	imagesize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// ������ �����Ǵ� ���� ����
	if (imagesize == 0) imagesize = width * height * 3;
	if (dataPos == 0) dataPos = 54;

	// ���� ����
	data = new unsigned char[imagesize];

	// ���Ͽ��� ���۷� ���� ������ �ֱ�
	fread(data, 1, imagesize, file);

	// texture�� GPU�ȿ� �����Ƿ� ������ ����
	fclose(file);
	return data;
}
	
void texture()   // texture initialization -> Linear
{
	glGenTextures(5, textures); // 5���� �ؽ�ó�� �̸��� ����

	// 1��° �ؽ���
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	pBytes = LoadDIBitmap("�⺻.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 2��° �ؽ���
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("moon.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate
	
	// 3��° �ؽ���
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	pBytes = LoadDIBitmap("�¾�.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 4��° �ؽ���
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	pBytes = LoadDIBitmap("���.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 5��° �ؽ���
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	pBytes = LoadDIBitmap("�ٴ�.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

}
void textureN()    // texture initialization -> Nearest
{
	glGenTextures(5, textures); // 5���� �ؽ�ó�� �̸��� ����

	// 1��° �ؽ�ó
	glBindTexture(GL_TEXTURE_2D, textures[0]);  
	pBytes = LoadDIBitmap("�⺻.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 2��° �ؽ�ó
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("moon.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);      
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 3��° �ؽ�ó
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	pBytes = LoadDIBitmap("�¾�.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 4��° �ؽ�ó
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	pBytes = LoadDIBitmap("���.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate

	// 5��° �ؽ�ó
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	pBytes = LoadDIBitmap("�ٴ�.bmp", &info);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pBytes);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);   //modulate
	  
}

void init()
{
	glClearColor(0.0, 0.7, 1.0, 1.0);         // ����, �ǳ��� ����(�����)

	glViewport(0.0, 0.0, 500.0, 500.0);        //��ü �������� Ŭ�����ؼ� ������ �κ��� ����

	glEnable(GL_DEPTH_TEST);         //hidden surfae removal,

	light(); //���� Ű�� �Լ� ȣ��

	texture(); //�ؽ�ó�� �ҷ���

}

void home()  //�簢�� �ǹ����� ���� �� ���
{
	glutSolidCube(5);
}

void sphere()  //���� ����� �ǹ����� ���� �� ���
{
	//�ؽ�ó�� ���� ����
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	
	glutSolidSphere(5.0, 20.0, 20.0);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}
void build()     //normalvector�� ����� �ǹ�
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

	glBindTexture(GL_TEXTURE_2D, textures[0]);  //�ٽ� �⺻�̹����� �ٲ���

	glEnd();
}

void mydisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // 3���� �������� �����ӹ��ۿ� ������ �ִ��� �� ������

	glMatrixMode(GL_PROJECTION);              //ī�޶󿡰��Ѱ�	
	glLoadIdentity();                         //ī�޶󿡰��Ѱ� -> initialization
	gluPerspective(55.0, 1.0, 0.1, 500.0);      // ���� �׸� ��ü���� ǥ���ϴ� ����

	gluLookAt(eye1, eye2, eye3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //�� ī�޶� ��� ��ġ���� ��� ���� ��� �������� �� �������� ����
	
	glMatrixMode(GL_MODELVIEW);               //��ü�� �׸���
	glLoadIdentity();

	//texture�� Ű�� �� �� ���
	if (tex == TRUE)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	glEnable(GL_FOG);
	glFogf(GL_FOG_DENSITY, den);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textures[4]); 
	//���� �簢�� ����� ��
	glPushMatrix();
	glNormal3f(0.0, 0.0, 1.0); //��� �ٴ��� normal vector
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
	//���� �ǹ�
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

	//���� �ǹ� ���� �׸� ����
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

	//������� �ְ��������
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

	//����� ���� ����� �ǹ�
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

	//�ٺ� ����� �ǹ�
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

	//������ü - �� - ������ü �ǹ�
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

	//������ü���� ���� �÷��� �ǹ�(������ü�� normal vector ���) - ���� ��
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

	//������ü���� ���� �÷��� �ǹ�(������ü�� normal vector ���) - ������ ��
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

	//������ü���� ���� �÷��� �ǹ�(������ü�� normal vector ���) - ������ ��
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

	//������ü���� ���� �÷��� �ǹ�(������ü�� normal vector ���) - ���� ��
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

	//4���� ����� �մ� ������ü - ��
	glPushMatrix();
	glTranslatef(0.0, 35.0, -50.0);
	glScalef(18.2, 1.0, 1.0);
	home();
	glPopMatrix();

	//4���� ����� �մ� ������ü - ��
	glPushMatrix();
	glTranslatef(0.0, 35.0, 60.0);
	glScalef(18.0, 1.0, 1.0);
	home();
	glPopMatrix();

	//4���� ����� �մ� ������ü - ��
	glPushMatrix();
	glTranslatef(50.0, 35.0, 5.0);
	glScalef(1.0, 1.0, 20.0);
	home();
	glPopMatrix();

	//4���� ����� �մ� ������ü - ��
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
	//������ �¾�
	glPushMatrix();
	glTranslatef(-120.0, 120.0, 5.0);
	glScalef(5.0, 5.0, 5.0);
	sphere();
	glPopMatrix();

	//���� �¾�
	glPushMatrix();
	glTranslatef(240.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(-120.0, 120.0, 5.0);
	glScalef(5.0, 5.0, 5.0);
	sphere();
	glPopMatrix();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[3]);
	//�ܰ� ����� 1
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

	//�ܰ� ����� 2
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
	
	glBindTexture(GL_TEXTURE_2D, 0);   //texture mapping ����

	glFlush();

	glutSwapBuffers();     // ������۸�
}

void keyboard(unsigned char key, int x, int y)    //Keyboard control
{
	switch (key) {
	case '0':                      //0�� ������ ��� ���� off     
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
	case '1':                      //1�� ������ 1������ on                     
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
	case '2':                      //2�� ������ 0��(���κ�)�� 1������ on  
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		break;
	case '3':                      //3�� ������ ��� ���� on
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		break;

	case 'a':                      //a ������ ī�޶� ��ġ�� x���� 5��ŭ ����
		eye1 -= 5.0;
		break;
	case 'd':                      //d ������ ī�޶� ��ġ�� x���� 5��ŭ ����
		eye1 += 5.0;
		break;
	case 'w':                      //w ������ ī�޶� ��ġ�� y���� 5��ŭ ����
		eye2 -= 5.0;
		break;
	case 's':                      //s ������ ī�޶� ��ġ�� y���� 5��ŭ ����
		eye2 += 5.0;
		break;
	case 'z':                      //z ������ ī�޶� ��ġ�� y���� 5��ŭ ����
		eye3 += 5.0;
		break;
	case 'c':                      //c ������ ī�޶� ��ġ�� y���� 5��ŭ ����
		eye3 -= 5.0;
		break;

	case '9':                      //9�� ������ ���α׷� ����
		exit(0);
		break;

	case '4':                      //4�� ������ texture�� ����
		tex = TRUE;
		break;

	case '5':                      //5�� ������ texture�� ����
		tex = FALSE;
		break;

	case 'o':                      //o�� ������ texture filtering�� Linear�� ��
		texture();
		break;

	case 'p':                      //o�� ������ texture filtering�� nearest�� ��
		textureN();
		break;

	case 'k':                      //k�� ������ fog�� £����
		if (den <= 1.0) 
			den += 0.02;
		break;

	case 'l':                      //l�� ������ fog�� ������
		if (den <= 1.0) 
			den -= 0.02;
		break;
	}
	glutPostRedisplay();  //glutDisplayFunc�� ��ϵ� �Լ��� ȣ��
}

int main(int argc, char**argv)
{
	//glut initialization(�ʱ�ȭ)
	glutInit(&argc, argv);
	//actual window size
	glutInitWindowSize(500, 500);
	//initial window location, top-left corner
	glutInitWindowPosition(0, 0);
	//create window with title 
	glutCreateWindow("HW3_14010976_LeeHyunSik");
	//������� ���, Ʈ���÷����, ���̹��� ���
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