/* 
[CG hw2 stencil buffer]
You can focus on the part "You may need to do something here", and also "object_class.h"
If needed, please feel free to modify the code structure and all the source code we provided.
*/
#include <stdio.h>
#include <stdlib.h>
#include "../GL/glut.h"
#include "object_class.h"
#include "image_class.h"

#define obj_total 3

float constant = 100; //drawClip向量的大小

namespace{
	//const double CamaraPos[]={1.5, 0.5 ,18.0};
	const double CamaraPos[]={2.0, 1.0 ,18.0};
	const double BarrierPos[]={-2.0,-2.0,-0.5};

	float LightPos[] = { 2.4f, 2.2f, 2.9f };				//global position
	float LightAmb[] = { 0.1f, 0.1f, 0.3f, 1.0f };			// Ambient Light Values
	float LightDif[] = { 0.8f, 0.8f, 1.0f, 1.0f };			// Diffuse Light Values
	float LightSpc[] = { 0.5f, 0.5f, 0.5f, 1.0f };			// Specular Light Values

	float localPos[3][3];                                   // Local Light Position

	float MatAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Ambient Values
	float MatDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Diffuse Values
	float MatSpc[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Material - Specular Values
	float MatShn[] = { 0.1f };								// Material - Shininess
	GLUquadric* q;

	int obj_counter=0;
	object_class* obj_ptr[obj_total];
	object_class obj1("../Data/Object.txt");
	object_class obj2("../Data/Object1.txt");
	object_class obj3("../Data/Object2.txt");
	
	image_class* image;

	bool shadow_mode=true;
}

void globalToLocal(float* global_light) // Global Light Position To Local Light Position 
{
	GLfloat GL4[4] = { global_light[0],global_light[1],global_light[2],1.0 };
	GLfloat m[16];
	GLfloat BL4[3];  //
	
	for(int i = 0 ; i < 3 ; i++)
	{
		glPushMatrix();
		glLoadIdentity();
		
		glRotatef(-1*(obj_ptr[i]->rotation[2]), 0, 0, 1);
		glRotatef(-1*(obj_ptr[i]->rotation[1]), 0, 1, 0);
		glRotatef(-1*(obj_ptr[i]->rotation[0]), 1, 0, 0);
		glTranslatef(-1*(obj_ptr[i]->position[0]), -1*(obj_ptr[i]->position[1]), -1*(obj_ptr[i]->position[2]));
		glScalef(1.0 / (obj_ptr[i]->scale[0]), 1.0 / (obj_ptr[i]->scale[1]), 1.0 / (obj_ptr[i]->scale[2]));
		
		glGetFloatv(GL_MODELVIEW_MATRIX, m); // Get Inverse Matrix
		glPopMatrix();
		
		BL4[0] = m[0] * GL4[0] + m[4] * GL4[1] + m[8] * GL4[2] + m[12] * GL4[3];
		BL4[1] = m[1] * GL4[0] + m[5] * GL4[1] + m[9] * GL4[2] + m[13] * GL4[3];
		BL4[2] = m[2] * GL4[0] + m[6] * GL4[1] + m[10] * GL4[2] + m[14] * GL4[3];
		
		localPos[i][0] = BL4[0];
		localPos[i][1] = BL4[1];
		localPos[i][2] = BL4[2];
		
	}
}

void DrawGLRoom()										// Draw The Room (Box)
{
	
	glDisable(GL_LIGHTING);
	glColor3d(1.0,1.0,1.0);
	glEnable(GL_TEXTURE_2D);
	image[0].BuildTexture();
	glBegin(GL_QUADS);									// Begin Drawing Quads
	// Floor
	glNormal3f(0.0f, 1.0f, 0.0f);					// Normal Pointing Up
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Back Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Front Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Front Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Back Right
	glEnd();
	image[1].BuildTexture();
	glBegin(GL_QUADS);
	// Ceiling
	glNormal3f(0.0f, -1.0f, 0.0f);					// Normal Point Down
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Front Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Back Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Back Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Front Right
	glEnd();
	image[2].BuildTexture();
	glBegin(GL_QUADS);
	// Front Wall
	glNormal3f(0.0f, 0.0f, 1.0f);					// Normal Pointing Away From Viewer
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Top Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Bottom Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Bottom Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Top Right
	glEnd();
	image[3].BuildTexture();
	glBegin(GL_QUADS);
	// Back Wall
	glNormal3f(0.0f, 0.0f, -1.0f);					// Normal Pointing Towards Viewer
	glTexCoord2d(0.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Top Right
	glTexCoord2d(0.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Bottom Right
	glTexCoord2d(1.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Bottom Left
	glTexCoord2d(1.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Top Left
	glEnd();
	image[4].BuildTexture();
	glBegin(GL_QUADS);
	// Left Wall
	
	glNormal3f(1.0f, 0.0f, 0.0f);					// Normal Pointing Right
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Top Front
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Bottom Front
	glTexCoord2d(1.0,0.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Bottom Back
	glTexCoord2d(1.0,1.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Top Back
	glEnd();
	image[5].BuildTexture();
	glBegin(GL_QUADS);
	
	// Right Wall
	glNormal3f(-1.0f, 0.0f, 0.0f);					// Normal Pointing Left
	glTexCoord2d(0.0,1.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Top Back
	glTexCoord2d(0.0,0.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Bottom Back
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Bottom Front
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Top Front
	glEnd();										// Done Drawing Quads
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);				//指定使用不透明黑色清背景
	
	glShadeModel(GL_SMOOTH);
	q = gluNewQuadric();
	gluQuadricNormals(q, GL_SMOOTH);					// Enable Smooth Normal Generation
	//gluQuadricDrawStyle(q, GLU_FILL);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_LIGHT1);// Enable Light1
	glEnable(GL_LIGHTING);// Enable Lighting
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);// Set Light1 Position
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);// Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);// Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);// Set Light1 Specular

	glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmb);			// Set Material Ambience
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDif);			// Set Material Diffuse
	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpc);		// Set Material Specular
	glMaterialfv(GL_FRONT, GL_SHININESS, MatShn);		// Set Material Shininess

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	
	obj_ptr[0] = &obj1;
	obj_ptr[0]->scaling(0.5f, 0.5f, 0.5f);
	obj_ptr[1] = &obj2;
	obj_ptr[1]->scaling(0.7f, 0.7f, 0.7f);
	obj_ptr[1]->position[0] = -1.5;
	obj_ptr[1]->position[2] = 1.5;
	obj_ptr[2] = &obj3;
	obj_ptr[2]->scaling(0.9f, 0.9f, 0.9f);
	obj_ptr[2]->position[0] = 1.5;
	obj_ptr[2]->position[2] = -1.5;
	
	obj_counter=0;

	image=(image_class*) operator new(6*sizeof(image_class));
	new (image+0) image_class("../Data/bottom.bmp");
	new (image+1) image_class("../Data/ceiling.bmp");
	new (image+2) image_class("../Data/front.bmp");
	new (image+3) image_class("../Data/back.bmp");
	new (image+4) image_class("../Data/left.bmp");
	new (image+5) image_class("../Data/right.bmp");
	
}

void idle(){
	//printf("idle\n");
	glutPostRedisplay();
}
//---------負責視窗及繪圖內容的比例---------
void WindowSize(int w, int h)
{
	printf("目前視窗大小為%d X %d\n", w, h);
	glViewport(0, 0, w, h);							//當視窗長寬改變時，畫面也跟著變
	glMatrixMode(GL_PROJECTION);					//選擇投影矩陣模式
	glLoadIdentity();
	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);
	gluLookAt(CamaraPos[0], CamaraPos[1], CamaraPos[2], 0, 0, 0, 0, 1, 0);				//螢幕鏡頭的座標及方向
	glMatrixMode(GL_MODELVIEW);						//model模式最常用，沒事就切回這個模式
	glLoadIdentity();
}

//---------獲取鍵盤輸入---------
void Keyboard(unsigned char key, int x, int y)
{
	printf("你所按按鍵的碼是%x\t此時視窗內的滑鼠座標是(%d,%d)\n", key, x, y);
	if(key==0x62) shadow_mode=!shadow_mode;//b 開關陰影
	if(key==0x76)//v 選擇物體
	{
		obj_counter=(obj_counter+1)%3;
		printf("Selecting Object: %d\n", obj_counter+1);
	}
	if(key==0x61) LightPos[0]-=0.1f;//a
	if(key==0x64) LightPos[0]+=0.1f;//d
	if(key==0x77) LightPos[1]+=0.1f;//w
	if(key==0x73) LightPos[1]-=0.1f;//s
	if(key==0x71) LightPos[2]-=0.1f;//q
	if(key==0x65) LightPos[2]+=0.1f;//e

	if(key==0x6a) obj_ptr[obj_counter]->rotate_speed[0]-=0.2f;//j
	if(key==0x6c) obj_ptr[obj_counter]->rotate_speed[0]+=0.2f;//l
	if(key==0x69) obj_ptr[obj_counter]->rotate_speed[1]+=0.2f;//i
	if(key==0x6b) obj_ptr[obj_counter]->rotate_speed[1]-=0.2f;//k
	if(key==0x75) obj_ptr[obj_counter]->rotate_speed[2]-=0.2f;//u
	if(key==0x6f) obj_ptr[obj_counter]->rotate_speed[2]+=0.2f;//o

	if(key==0x34) {obj_ptr[obj_counter]->position[0]-=0.2f;}//numpad4
	if(key==0x36) {obj_ptr[obj_counter]->position[0]+=0.2f;}//numpad6
	if(key==0x35) {obj_ptr[obj_counter]->position[1]-=0.2f;}//numpad5
	if(key==0x38) {obj_ptr[obj_counter]->position[1]+=0.2f;}//numpad8
	if(key==0x37) {obj_ptr[obj_counter]->position[2]-=0.2f;}//numpad7
	if(key==0x39) {obj_ptr[obj_counter]->position[2]+=0.2f;}//numpad9

	if(key==0x20) std::cout<<"light pos:"<<LightPos[0]<<'\t'<<LightPos[1]<<'\t'<<LightPos[2]<<'\n';//space
	
}

void drawClip(void) // Draw Shadow Clips
{
	float f; // Check if the plane facing the light
	float vector[3];
	float vector2[3];
	float vector3[3];

	for (int i = 0; i < 3; i++)
	{
		glLoadIdentity();
		glPushMatrix();
		
		// local to global
		glScalef(obj_ptr[i]->scale[0], obj_ptr[i]->scale[1], obj_ptr[i]->scale[2]);
		glTranslatef(obj_ptr[i]->position[0], obj_ptr[i]->position[1], obj_ptr[i]->position[2]);
		glRotatef(obj_ptr[i]->rotation[0], 1, 0, 0);
		glRotatef(obj_ptr[i]->rotation[1], 0, 1, 0);
		glRotatef(obj_ptr[i]->rotation[2], 0, 0, 1);

		for (int j = 0; j < obj_ptr[i]->plane_count; j++)
		{
			f = obj_ptr[i]->planeEq[j][0] * localPos[i][0] + obj_ptr[i]->planeEq[j][1] * localPos[i][1] + obj_ptr[i]->planeEq[j][2] * localPos[i][2] + obj_ptr[i]->planeEq[j][3];
			
			if (f > 0)
			{
				vector[0] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][0] - localPos[i][0]; //av
				vector[1] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][1] - localPos[i][1];
				vector[2] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][2] - localPos[i][2];

				vector2[0] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][0] - localPos[i][0]; //bv
				vector2[1] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][1] - localPos[i][1];
				vector2[2] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][2] - localPos[i][2];

				vector3[0] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][0] - localPos[i][0]; //cv
				vector3[1] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][1] - localPos[i][1];
				vector3[2] = obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][2] - localPos[i][2];

				glBegin(GL_QUADS);

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][0], obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][1], obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][2]); //b

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][0], obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][1], obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][2]); //a

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][0] + vector[0] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][1] + vector[1] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][2] + vector[2] * constant); //a'

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][0] + vector2[0] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][1] + vector2[1] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][2] + vector2[2] * constant); //b'

				glEnd();

				glBegin(GL_QUADS);

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][0], obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][1], obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][2]); //a

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][0], obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][1], obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][2]); //c

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][0] + vector3[0] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][1] + vector3[1] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][2] + vector3[2] * constant); //c'

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][0] + vector[0] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][1] + vector[1] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][0]][2] + vector[2] * constant); //a'

				glEnd();

				glBegin(GL_QUADS);

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][0], obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][1], obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][2]); //c

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][0], obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][1], obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][2]); //b

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][0] + vector2[0] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][1] + vector2[1] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][1]][2] + vector2[2] * constant); //b'

				glVertex3f(obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][0] + vector3[0] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][1] + vector3[1] * constant, obj_ptr[i]->pos[obj_ptr[i]->plane[j][2]][2] + vector3[2] * constant); //c'

				glEnd();

			}
		}
		glPopMatrix();
	}
}

void drawShadow(void) //Draw Shadow
{

	glDepthMask(GL_FALSE);									//關閉深度測試
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);    //關閉顏色緩存的寫入  
	glEnable(GL_STENCIL_TEST);								//使用模板緩存  
	glDisable(GL_LIGHTING);									//關閉燈光  

	glFrontFace(GL_CCW);									//逆時針正面

	glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
	glStencilOp(GL_KEEP, GL_KEEP,GL_INCR );					//設置模板函數(正面+1)

	drawClip();												//draw shadow clips

	glFrontFace(GL_CW);										//順時針正面
	glStencilOp(GL_KEEP, GL_KEEP,  GL_DECR);				//正面-1

	drawClip();												//draw shadow clips

	glFrontFace(GL_CCW);									//還原

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		//開啟混和
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);		//開啟顏色緩存

	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFF);					// if ( ref & mask ) != ( stencil & mask )

	//畫蓋過整個場景的陰影
	glPushMatrix();
		glLoadIdentity();
		glColor4f(0, 0, 0, 0.4);
		glBegin(GL_QUADS);
		glVertex3f(10, 10, 15);
		glVertex3f(-10, 10, 15);
		glVertex3f(-10, -10, 15);
		glVertex3f(10, -10, 15);
		glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);									//關閉混和
	glDepthMask(GL_TRUE);									//開啟深度測試
	glEnable(GL_LIGHTING);									//開啟光源
	glDisable(GL_STENCIL_TEST);								//關閉stencil test
}

//---------描繪畫面---------
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	//清理緩衝區
	glMatrixMode(GL_MODELVIEW);						//選擇矩陣模式

	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);// Set Light1 Position

	/* You may need to do something here */

	glLoadIdentity();//重設為單位矩陣，畫房間
	DrawGLRoom();
	glTranslated(BarrierPos[0],BarrierPos[1],BarrierPos[2]);
	glutSolidSphere(1.0,16,16);//小球，只是個障礙物
	
	glLoadIdentity();	
	for(int i=0; i < 3; i++){
		glPushMatrix();
		glScalef(obj_ptr[i]->scale[0],obj_ptr[i]->scale[1],obj_ptr[i]->scale[2]);
		glTranslatef(obj_ptr[i]->position[0],obj_ptr[i]->position[1],obj_ptr[i]->position[2]);
		glRotatef(obj_ptr[i]->rotation[0], 1, 0, 0);
		glRotatef(obj_ptr[i]->rotation[1], 0, 1, 0);
		glRotatef(obj_ptr[i]->rotation[2], 0, 0, 1);
		obj_ptr[i]->draw(); //draw the objects
		glPopMatrix();
	}

	if(shadow_mode){
		/* You may need to do something here */
		drawShadow();
	}
		
	//單純讓使用者知道光源在哪
	glPushMatrix();

	glLoadIdentity();//重置，畫光
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glColor4f(0.0f, 0.6f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	gluSphere(q, 0.1f, 16, 8);//光
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);

	glPopMatrix();
	
	for(int i=0;i < 3;i++){
		obj_ptr[i]->rotation[0]+=obj_ptr[i]->rotate_speed[0];
		obj_ptr[i]->rotation[1]+=obj_ptr[i]->rotate_speed[1];
		obj_ptr[i]->rotation[2]+=obj_ptr[i]->rotate_speed[2];
	}
	
	globalToLocal(LightPos);
	
	glutSwapBuffers();								//更新畫面
}

int main()
{
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);		//描繪模式使用雙緩衝區以及紅綠藍Alpha顏色順序
	glutInitWindowSize(600, 600);					//視窗長寬
	glutInitWindowPosition(300, 200);				//視窗左上角的位置
	glutCreateWindow("HW2_shadow_0656632");			//建立視窗並打上標題
	init();
	//下面三個與Callback函式有關
	glutReshapeFunc(WindowSize);					//當視窗改變大小時會獲取新的視窗長寬
	glutKeyboardFunc(Keyboard);						//獲取鍵盤輸入訊息
	//glutSpecialFunc(SpecialInput);//可以用來得到上下左右等特殊鍵，不過用不到
	glutDisplayFunc(Display);						//負責描繪
	glutIdleFunc(idle);
	glutMainLoop();									//進入主迴圈

	for(int i=0;i<6;i++) image[i].~image_class();//叫的土地要清掉
	operator delete(image);

	return 0;
}