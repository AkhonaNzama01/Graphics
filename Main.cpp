#include<iostream>
#include<windows.h>
#include<GL/glut.h>
#include<GL/freeglut.h>
#include<GL/GL.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>

#define _CRT_SECURE_NO_DEPRECATE
/*******Define constant variables for program******/
GLfloat light_position1[] = { 0,28,
-20,1.0 };
GLfloat model_ambient[] = { 0.05f,0.05f,0.05f,1.0f };
GLfloat mat_specular[] = { 0.8f,1.0f,1.0f,1.0f };
GLfloat mat_shininess[] = { 5.0f };
GLfloat mat_ambient[] = { 0.1f,0.1f,0.1f,1.0f };
GLfloat white_light[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_position0[] = { 0.28f,20.0f,1.0f };
GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat mat_diffuse1[] = { 0.1f, 0.5f, 0.8f, 1.0f };
GLfloat no_shininess[] = { 0.0f };
GLfloat sound[] = { 0.9f,0.9f,0.9f,1.0f };
GLint WinWidth;
GLint WinHeight;
/*viewpoint*/
typedef struct EyePoint {
	GLfloat x;
	GLfloat y;
	GLfloat z;
}EyePoint;
EyePoint myEye;
EyePoint vPoint;
GLfloat pro_up_down = 29.0f;
GLfloat vAngle = 0;
void reshape(int we, int he)
{
	WinWidth = we;
	WinHeight = he;
		glViewport(0, 0, (GLsizei)we, (GLsizei)he);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, (GLfloat)we / (GLfloat)he, 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(myEye.x, myEye.y, myEye.z, vPoint.x + 30 * sin(vAngle), vPoint.y, -30 *
		cos(vAngle), 0.0f, 1.0f, 0.0f);
}
#define BMP_Header_Length 54
void grab(void) {
	FILE* pDummyFile;
	FILE* pWritingFile;
	GLubyte* pPixelData;
	GLubyte BMP_Header[BMP_Header_Length];
	GLint i, j;
	GLint PixelDataLength;
	// Calculate the actual length of the pixel data
	i = WinWidth * 3; // Get the pixel data length of each row
	while (i % 4 != 0) // Supplement data until i is a multiple of 4
		++i;
	PixelDataLength = i * WinHeight;
	pPixelData = (GLubyte*)malloc(PixelDataLength);
		if (pPixelData == 0)
			exit(0);
	// Open the files using fopen_s
	if (fopen_s(&pDummyFile, "classroom.bmp", "rb") != 0) {
		exit(0);
	}
	if (fopen_s(&pWritingFile, "grab.bmp", "wb") != 0) {
		exit(0);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glReadPixels(0, 0, WinWidth, WinHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE,
			pPixelData);
	// The file header of the new file is copied as the file header of the new file
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET);
	i = WinWidth;
	j = WinHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);
	fseek(pWritingFile, 0, SEEK_END);
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}

int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}
GLuint load_texture(const char* classroom_file)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLint last_texture_ID = 0;
	GLuint texture_ID = 0;
	FILE* pFile = NULL;
	// Open a file using fopen_s
	if (fopen_s(&pFile, classroom_file, "rb") != 0)
	{
		return 0;
	}
	// Read the width and height of an image in a file
	
		fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);
	// Calculate the number of bytes occupied by each line of pixels and the total pixel bytes
	{
	GLint line_bytes = width * 3;
	while (line_bytes % 4 != 0)
	++line_bytes;
	total_bytes = line_bytes * height;
	}
		// Allocate memory based on total pixel bytes
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}
	// Read pixel data
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}
	// Don't forget to close the file when done
	fclose(pFile);
	// Bind new texture, load texture and set texture parameters
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
			GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);
	free(pixels);
	return texture_ID;
}
GLuint texblackboard, texwindow, texdesk, texsound;
GLuint texceiling, texdoor, texfloor, texbackwall, texpole;
GLuint texairfro, texairback, texgaodi, texsdesk, texclock;
/*Draw classroom*/
void drawscence()
{
	//Set material related parameters
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	//First draw the ceiling
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texceiling);
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-40.0f, 30.0f, 30.0f);
	glTexCoord2f(0.0f, 3.0f);
	glVertex3f(-40.0f, 30.0f, -30.0f);
	glTexCoord2f(6.0f, 3.0f);
	glVertex3f(40.0f, 30.0f, -30.0f);
	glTexCoord2f(6.0f, 0.0f);
	glVertex3f(40.0f, 30.0f, 30.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

		//paint the floor
		glColor3f(0.8f, 1.0f, 0.8f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glVertex3f(-40.0f, 0.0f, 30.0f);
	glVertex3f(-40.0f, 0.0f, -30.0f);
	glVertex3f(40.0f, 0.0f, -30.0f);
	glVertex3f(40.0f, 0.0f, 30.0f);
	glEnd();
	//Draw the left wall and the left window
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f); //used to define the normal vector
	glVertex3f(-40.0f, 0.0f, 30.0f);
	glVertex3f(-40.0f, 30.0f, 30.0f);
	glVertex3f(-40.0f, 30.0f, -30.0f);
	glVertex3f(-40.0f, 0.0f, -30.0f);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texwindow);
	glColor3f(0.74902f, 0.847059f, 0.847059f);
	for (int n = 0; n <= 1; n++)
	{
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.1f, 0.1f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-39.9f, 10.0f, -8.0f + n * 18.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-39.9f, 20.0f, -8.0f + n * 18.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-39.9f, 20.0f, -18.0f + n * 18.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-39.9f, 10.0f, -18.0f + n * 18.0f);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	//Draw the right wall and the right window
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); //used to define the normal vector
	glVertex3f(40.0f, 0.0f, 30.0f);
	glVertex3f(40.0f, 30.0f, 30.0f);
	glVertex3f(40.0f, 30.0f, -30.0f);
	glVertex3f(40.0f, 0.0f, -30.0f);
	glEnd();
	
		glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texwindow);
	glColor3f(0.74902f, 0.847059f, 0.847059f);
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(1.0f, 0.0f); glVertex3f(39.5, 10, 10);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(39.5, 20, 10);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(39.5, 20, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(39.5, 10, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//draw back wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-40.0f, 30.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(40.0f, 30.0f, 30.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(40.0f, 0.0f, 30.0f);
	glEnd();
	//draw the front wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(
		-40.0f, 0.0f,
		-30.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(
		-40.0f, 30.0f,
		-30.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(40.0f, 30.0f,
		
		- 30.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(40.0f, 0.0f,
		-30.0f);
	glEnd();
	//draw a clock
	glBindTexture(GL_TEXTURE_2D, texclock);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(23.0f, 18.0f,
		-29.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(23.0f, 20.0f,
		-29.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(25.0f, 20.0f,
		-29.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(25.0f, 18.0f,
		-29.8f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//Draw the air conditioner to texture the air conditioner
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texairfro);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(33.0f, 0.0f,
		-26.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(33.0f, 15.0f,
		-26.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(37.0f, 15.0f,
		-26.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(37.0f, 0.0f,
		-26.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texairback);
	glBegin(GL_QUADS); glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(33.0f, 0.0f,
		-26.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(33.0f, 15.0f,
		-26.0f);
	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(33.0f, 15.0f,
			-29.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(33.0f, 0.0f,
		-29.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//Draw the front two stone pillars on both sides of the classroom
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texpole);
	for (int i = 0; i <= 1; i++)
	{
		glColor3f(0.658824f, 0.658824f, 0.658824f);
		//upper surface of stone pillar
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f + i * 78.0f, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f + i * 78.0f, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0f + i * 78.0f, 30.0f, -6.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0f + i * 78.0f, 30.0f, -4.0f);
		glEnd();
		//front face of stone pillar
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(static_cast<GLfloat>(-40.0 + i * 78),
			0.0f, static_cast<GLfloat>(-4.0f));
		glTexCoord2f(0.0f, 1.0f); glVertex3f(static_cast<GLfloat>(-40.0 + i * 78),
			static_cast<GLfloat>(30.0), static_cast<GLfloat>(-4.0f));
		glTexCoord2f(1.0f, 1.0f); glVertex3f(static_cast<GLfloat>(-38.0 + i * 78),
			static_cast<GLfloat>(30.0), static_cast<GLfloat>(-4.0f));
		glTexCoord2f(1.0f, 0.0f); glVertex3f(static_cast<GLfloat>(-38.0 + i * 78),
			0.0f, static_cast<GLfloat>(-4.0f));
		glEnd();
		//back surface of stone pillar
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); // Used to define the normal vector
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(static_cast<GLfloat>(-40.0 + i * 78), 0.0f, -6.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(static_cast<GLfloat>(-40.0 + i * 78),
			static_cast<GLfloat>(30.0), -6.0f);
	
			glTexCoord2f(1.0f, 1.0f);
		glVertex3f(static_cast<GLfloat>(-38.0 + i * 78),
			static_cast<GLfloat>(30.0), -6.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(static_cast<GLfloat>(-38.0 + i * 78), 0.0f, -6.0f);
		glEnd();
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); // Used to define the normal vector
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-38.0f + i * 76.0f, 0.0f, -4.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0f + i * 76.0f, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0f + i * 76.0f, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0f + i * 76.0f, 0.0f, -6.0f);
		glEnd();
	}
	//Draw the stone pillars on both sides of the classroom and the two behind
	for (int j = 0; j <= 1; j++)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		// Upper surface of the stone pillar
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f); // Used to define the normal vector
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f + j * 78.0f, 30.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f + j * 78.0f, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0f + j * 78.0f, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0f + j * 78.0f, 30.0f, 14.0f);
		glEnd();
		//front face of stone pillar
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f); // used to define the normal vector
		GLfloat xOffset = -40.0f + j * 78.0f; // Ensure that the values are of type 
	    GLfloat yOffset = 0.0f; 
		GLfloat zOffset = 14.0f; 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(xOffset, yOffset, zOffset);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(xOffset, yOffset + 30.0f, zOffset);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(xOffset + 2.0f, yOffset + 30.0f,
			zOffset); // Adjust x-coordinate
			glTexCoord2f(1.0f, 0.0f); glVertex3f(xOffset + 2.0f, yOffset, zOffset); // Adjust x - coordinate
			glEnd();
		//back surface of stone pillar
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); // Used to define the normal vector
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f + j * 78.0f, 0.0f, 12.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f + j * 78.0f, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0f + j * 78.0f, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0f + j * 78.0f, 0.0f, 12.0f);
		glEnd();
		//right surface
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); // Used to define the normal vector
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-38.0f + j * 76.0f, 0.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0f + j * 76.0f, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0f + j * 76.0f, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0f + j * 76.0f, 0.0f, 12.0f);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	//drawing blackboard
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texblackboard);
	glColor3f(0.184314f, 0.309804f, 0.184314f); // Note the 'f' suffix to make these values GLfloat
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.5f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-20.0f, 8.0f, -29.9f); // Make sure all vertex coordinates are of type GLfloat
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-20.0f, 18.0f, -29.9f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(20.0f, 18.0f, -29.9f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(20.0f, 8.0f, -29.9f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

		//Draw the lamp above the blackboard
   GLfloat blacklight[] = { 0.9f, 0.9f, 0.9f, 1.0f }; 
 glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(-15.0f, 20.4f, -29.5f); // Add 'f' to the values to make them GLfloat
	glScalef(8.0f, 0.8f, 1.0f); // Add 'f' to the values to make them GLfloat
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
		blacklight);
	glutSolidCube(1.0f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(12.0f, 20.4f, -29.5f); // Add 'f' to the values to make them GLfloat
	glScalef(8.0f, 0.8f, 1.0f); // Add 'f' to the values to make them GLfloat
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
		blacklight);
	glutSolidCube(1.0f);
	glPopMatrix();
	//Draw and texture a plateau in front of the classroom
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texgaodi);
	//paste on top
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(
		-30.0f, 1.5f,
		-22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(
		-30.0f, 1.5f,
		-30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f,
		-30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 1.5f,
		-22.0f);
	glEnd();
	//stick left
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(
		-30.0f, 0,
		-22.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(
			-30.0f, 1.5f,
			-22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(
		-30.0f, 1.5f,
		-30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(
		-30.0f, 0,
		-30.0f);
	glEnd();
	//stick to the front
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(
		-30.0f, 0,
		-22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(
		-30.0f, 1.5f,
		-22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f,
		-22.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 0,
		-22.0f);
	glEnd();
	//paste right
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(30.0f, 0,
		-22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(30.0f, 1.5f,
		-22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f,
		-30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 0,
		-30.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//draw podium
	//stick podium texture
	glBindTexture(GL_TEXTURE_2D, texsdesk);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glNormal3f(0.5f, 0.35f, 0.05f); //used to define the normal vector
		glTexCoord2f(0.0f, 0.0f); glVertex3f(
			-7.5f, 1.5f,
			-24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(
		-7.5f, 9.5f,
		-24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f,
		-24.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 1.5f,
		-24.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(7.5f, 1.5f,
		-24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -28.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 1.5f, -28.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -28.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -28.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -26.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -26.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glEnd();
	//painted door
	glColor3f(0.5f, 0.35f, 0.05f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texdoor);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); //used to define the normal vector
	glTexCoord2f(0.0f, 0.0f); glVertex3f(39.9f, 0.0f, -25.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(39.9f, 14.0f, -25.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(39.9f, 14.0f, -19.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(39.9f, 0.0f, -19.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//draw sound
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(-37.5, 26.25f, -5.5f);
	glScalef(1.0f, 1.5f, 1.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
		sound);
	glutSolidCube(1.0f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(37.5, 26.25f, -5.5f);
	glScalef(1.0f, 1.5f, 1.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
		sound);
	glutSolidCube(1.0f);
	glPopMatrix();
}
/*************draw projector****************/
void drawprojector()
{
	GLfloat sqrt2 = static_cast<GLfloat>(sqrt(2.0));
	GLfloat pro_up_down = 0.0f; 
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 1.0f); // used to define the normal vector
	glVertex3f(-40.0f, 30.0f, -30.0f + 10.0f * sqrt2);
	glVertex3f(-40.0f + 10.0f * sqrt2, 30.0f, -30.0f);
	glVertex3f(-40.0f + 10.0f * sqrt2, pro_up_down, -30.0f);
	glVertex3f(-40.0f, pro_up_down, -30.0f + 10.0f * sqrt2);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(4.0f);
	glBegin(GL_LINES);

		glVertex3f(-25.0f, 30.0f, -15.0f);
	glVertex3f(-25.0f, 25.0f, -15.0f);
	glEnd();
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glTranslatef(-25.0f, 24.0f, -15.0f);
	glScalef(4.0f, 2.0f, 2.0f);
	glutSolidCube(1.0f);
	glPopMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-40.0f, 30.0f, -30.0f + 10.0f * sqrt2);
	glVertex3f(-40.0f + 10.0f * sqrt2, 30.0f, -30.0f);
	glVertex3f(-40.0f + 10.0f * sqrt2, pro_up_down, -30.0f);
	glVertex3f(-40.0f, pro_up_down, -30.0f + 10.0f * sqrt2);
	glEnd();
}
void drawdesks()
{
	// Draw table
	GLfloat desk[] = { 1.0f, 0.9647f, 0.56078f }; // Added 'f' to make these values GLfloat
		for (int y = 0; y <= 4; y++)
		{
			for (int x = 0; x <= 1; x++)
			{
				// On the table
				glColor4f(1.0f, 0.9647f, 0.56078f, 1.0f);
				glPushMatrix();
				glTranslatef(-20.0f + x * 40.0f, 8.1f, -17.5f + y * 8.0f);
				glScalef(10.0f, 0.2f, 3.0f);
				glMaterialfv(GL_FRONT_AND_BACK,
				GL_AMBIENT_AND_DIFFUSE, desk);
				glutSolidCube(1.0f);
				glPopMatrix();
				// Under the table
				glColor4f(1.0f, 0.9647f, 0.56078f, 1.0f); 
				glPushMatrix();
				glTranslatef(-20.0f + x * 40.0f, 6.1f, -17.5f + y * 8.0f);
				glScalef(9.0f, 0.2f, 3.0f);
				glutSolidCube(1.0f);
				glPopMatrix();
				// In front of the table
				glColor4f(1.0f, 0.9647f, 0.56078f, 1.0f); 
			
				glPushMatrix();
				glTranslatef(-20.0f + x * 40.0f, 7.0f, -18.9f + y * 8.0f);
				glScalef(10.0f, 2.0f, 0.2f);
				glMaterialfv(GL_FRONT_AND_BACK,
					GL_AMBIENT_AND_DIFFUSE, desk);
				glutSolidCube(1.0f);
				glPopMatrix();
				// Table legs
				glColor3f(0.0f, 0.0f, 0.0f);
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(-25.0f + x * 40.0f, 6.0f, -19.0f + y * 8.0f);
				glVertex3f(-25.0f + x * 40.0f, 0.0f, -19.0f + y * 8.0f);
				glEnd();
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(-15.0f + x * 40.0f, 6.0f, -19.0f + y * 8.0f);
				glVertex3f(-15.0f + x * 40.0f, 0.0f, -19.0f + y * 8.0f);
				glEnd();
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(-25.0f + x * 40.0f, 0.0f, -18.0f + y * 8.0f);
				glVertex3f(-25.0f + x * 40.0f, 0.0f, -20.0f + y * 8.0f);
				glEnd();
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(-15.0f + x * 40.0f, 0.0f, -18.0f + y * 8.0f);
				glVertex3f(-15.0f + x * 40.0f, 0.0f, -20.0f + y * 8.0f);
				glEnd();
			}
			//drawing chair
			void drawchairs();
			
			{
			GLfloat chair[] = { 0.1f, 0.67f, 0.62f };
			for (int j = 0; j <= 4; j++)
			{
			for (int i = 0; i <= 1; i++)
			{
				// Draw the bottom of the chair
				glColor3f(0.90f, 0.47f, 0.14f);
				glPushMatrix();
				glTranslatef(static_cast<GLfloat>(-20 + i * 40),
				3.1f, static_cast<GLfloat>(-14.5 + j * 8));
				glScalef(10.0f, 0.2f, 3.0f);
				glMaterialfv(GL_FRONT_AND_BACK,
				GL_AMBIENT_AND_DIFFUSE, chair);
				glutSolidCube(1.0f);
				glPopMatrix();
				// Draw chair back
				GLfloat red = 0.90f;
				GLfloat green = 0.47f;
				GLfloat blue = 0.14f;
				glColor3f(red, green, blue);
				glPushMatrix();
				glTranslatef(static_cast<GLfloat>(-20 + i * 40),
				5.0f, static_cast<GLfloat>(-13 + j * 8));
				glScalef(10.0f, 4.0f, 0.2f);
				glMaterialfv(GL_FRONT_AND_BACK,
				GL_AMBIENT_AND_DIFFUSE, chair);
				glutSolidCube(1.0f);
				glPopMatrix();
				//draw chair legs
				glColor3f(0.0, 0.0, 0.0);
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(static_cast<GLfloat>(-25 + i * 40), 3.0f,
				static_cast<GLfloat>(-13 + j * 8));
				glVertex3f(static_cast<GLfloat>(-25 + i * 40), 0.0f,
				static_cast<GLfloat>(-13 + j * 8));
				glEnd();
				glColor3f(0.0, 0.0, 0.0);
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(-15.0f + static_cast<GLfloat>(i) * 40.0f,
				3.0f, -13.0f + static_cast<GLfloat>(j) * 8.0f);
				glVertex3f(-15.0f + static_cast<GLfloat>(i) * 40.0f,
				0.0f, -13.0f + static_cast<GLfloat>(j) * 8.0f);
				glEnd();
				glColor3f(0.0, 0.0, 0.0);
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(-25.0f + i * 40.0f, 0.0f, -12.5f + j * 8.0f);
				glVertex3f(-25.0f + i * 40.0f, 0.0f, -13.5f + j * 8.0f);
				glEnd();
				glColor3f(0.0, 0.0, 0.0);
				glBegin(GL_LINES);
				glLineWidth(3.0f);
				glVertex3f(static_cast<GLfloat>(-15 + i * 40), 0.0f,
				static_cast<GLfloat>(-12.5 + j * 8));
				glVertex3f(static_cast<GLfloat>(-15 + i * 40), 0.0f,
				static_cast<GLfloat>(-13.5 + j * 8));
				glEnd();
				}
			//draw the bottom of the chair
			glColor3f(static_cast<GLfloat>(0.90),
			static_cast<GLfloat>(0.47), static_cast<GLfloat>(0.14));
			glPushMatrix();
			glTranslatef(0.0f, 3.1f, -14.5f + static_cast<GLfloat>(j) *
			8.0f);
			glScalef(20.0f, 0.2f, 3.0f);
			glMaterialfv(GL_FRONT_AND_BACK,
			GL_AMBIENT_AND_DIFFUSE, chair);
			glutSolidCube(1.0f);
			glPopMatrix();
			//draw chair back
			glColor3f(0.90f, 0.47f, 0.14f); 
			glPushMatrix();
			glTranslatef(0.0f, 5.0f, -13.0f + static_cast<GLfloat>(j) *
			8.0f);
			glScalef(20.0f, 4.0f, 0.2f);
			glMaterialfv(GL_FRONT_AND_BACK,
			GL_AMBIENT_AND_DIFFUSE, chair);
			glutSolidCube(1.0f);
			glPopMatrix();
			//draw chair legs
			glColor3f(0.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glLineWidth(3.0f);
			glVertex3f(-10.0f, 3.0f, -13.0f + j * 8.0f);
			glVertex3f(-10.0f, 0.0f, -13.0f + j * 8.0f);
			glEnd();
			glColor3f(0.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glLineWidth(3.0f);
			glVertex3f(10.0f, 3.0f, -13.0f + j * 8.0f);
			glVertex3f(10.0f, 0.0f, -13.0f + j * 8.0f);
			glEnd();
			glColor3f(0.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glLineWidth(3.0f);
			glVertex3f(-10.0f, 0.0f, -12.5f + j * 8.0f);
			glVertex3f(-10.0f, 0.0f, -13.5f + j * 8.0f);
			glEnd();
			glColor3f(0.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glLineWidth(3.0f);
			glVertex3f(10.0f, 0.0f, -12.5f + j * 8.0f);
			glVertex3f(10.0f, 0.0f, -13.5f + j * 8.0f);
			glEnd();
			}
			void myDisplay();
			{
				// clear screen
				glClear(GL_COLOR_BUFFER_BIT |
				GL_DEPTH_BUFFER_BIT);
				//call draw function
				drawscence();
				drawprojector();
				drawdesks();
				drawchairs();
				glFlush();
				}

			int main(int argc, char* argv[])
				
					{
				myEye.x = 0;
				
				myEye.y = 15;
				myEye.z = 25;
				vPoint.x = 0;
				vPoint.y = 15;
				vPoint.z = -30;
				vAngle = 0;
				glEnable(GL_DEPTH_TEST);
				glutInit(&argc, argv);
				glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
				glutInitWindowPosition(400, 0);
				glutInitWindowSize(800, 600);
				glutCreateWindow("classroom");
				initial();
				glutDisplayFunc(&myDisplay);
				glutReshapeFunc(reshape);
				glutKeyboardFunc(OnKeyboard);
				glutSpecialFunc(OnSpecial);
				glutIdleFunc(OnIdle);
				print();
				glutMainLoop();
				return;
				}
			}
	    }





















			