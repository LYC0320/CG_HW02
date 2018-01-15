//Texture����
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>	//�n�򥦭ɥ�BITMAPFILEHEADER�o�Ӹ�Ƶ��c
#include "../GL/glut.h"
#include <iostream>


//---------�Ϥ�����---------
class image_class
{
public:
	image_class(const char*);
	~image_class()
	{
		if( image!=NULL )
		{
			operator delete(image);
			glDeleteTextures(1,&my_texture_id);
		}
	}
	void BuildTexture()			//�bOpenGL�إߧ���K��
	{
		glBindTexture(GL_TEXTURE_2D,my_texture_id);
	}

	int width;					//�Ϥ����e��
	int height;					//�Ϥ�������
	unsigned char *image;		//���V�@�Ӱ}�C�A�O�ઽ����OpenGL�ϥΪ��Ϥ���ƤF
	GLuint my_texture_id;
	
};

//---------²����Ū��BMP��k---------
image_class::image_class(const char* file_name)
{
	FILE				*fp;
	BITMAPFILEHEADER	mapHeader;			//BMP�ɪ����Y���c�A�o��o���c�~�ા�DInfo���j�p
	BITMAPINFO			mapInfo;			//�o��oInfo���c�~�ા�D�Ϥ����j�p�B���e

	unsigned int		InfoSize;			//Info��size
	unsigned int		size;				//�Ϥ���size

	fp = fopen(file_name, "rb");				//�}��bmp�ɮ�
	if( fp==NULL )
	{
		std::cout<<"no such file,load bmp failed.\n";
		system("pause");
		exit(-1);
	}

	fread(&mapHeader, sizeof(mapHeader), 1, fp);					//Ū��BMP�ɪ����Y
	InfoSize = mapHeader.bfOffBits - sizeof(mapHeader);				//�o��Info��size
	fread(&mapInfo, InfoSize, 1, fp);								//Ū��BMP�ɪ�Info
	size = mapInfo.bmiHeader.biSizeImage;							//�o���Ϥ����j�p

	image = (unsigned char*) operator new(size*sizeof(unsigned char));//�t�m�O���鵹�Ϥ��ϥ�
	fread(image, 1, size, fp);										//Ū���Ϥ�

	fclose(fp);

	width = mapInfo.bmiHeader.biWidth;
	height= mapInfo.bmiHeader.biHeight;

	glGenTextures(1,&my_texture_id);
	glBindTexture(GL_TEXTURE_2D, my_texture_id);
	glTexImage2D(	GL_TEXTURE_2D,
						0,
						3,
						width,
						height,
						0,
						GL_BGR_EXT,
						GL_UNSIGNED_BYTE,
						image);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}


