#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"


bool textureMode = true;
bool lightMode = true;
bool FLAG_ROTATE = 0;
bool FLAG_ALPHA_CANAL = 0;
bool FLAG_NUMBER_RABOT = 0;
float a = 0;				
float b = 0;	

//для перемещения
double X_D = 0,Y_D = 0,Z_D = 0;
bool FLAG_TOCHNOST_PEREMECH = 0;
double Z_begin = Z_D , Z_end = Z_D;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры
//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor4d(0.9, 0.8, 0,1);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor4d(0, 0, 0,1);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света
//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	// перемещение объекта
	if (key == 37)	//<--
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D+=0.5;
			 Y_D-=0.5;}
			else 
			{X_D+=0.05;
			 Y_D-=0.05;}
		}
	if (key == 38)	//^
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D-=0.5;
			 Y_D-=0.5;}
			else 
			{X_D-=0.05;
			 Y_D-=0.05;}
		}
	if (key == 39)	//-->
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D-=0.5;
			 Y_D+=0.5;}
			else 
			{X_D-=0.05;
			 Y_D+=0.05;}
		}
	if (key == 40)	//v
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{X_D+=0.5;
			 Y_D+=0.5;}
			else 
			{X_D+=0.05;
			 Y_D+=0.05;}
		}
	if (key == 33)	//PgUp
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{Z_D+=0.5;}
			else 
			{Z_D+=0.05;}
			Z_begin = Z_D;
		}
	if (key == 34)	//PgDn
		{
			if (FLAG_TOCHNOST_PEREMECH == 0)
			{Z_D-=0.5;}
			else 
			{Z_D-=0.05;}
			Z_begin = Z_D;
		}

	//-------------------------
	if (key == 16)
		{
			FLAG_TOCHNOST_PEREMECH = !FLAG_TOCHNOST_PEREMECH;
		}

	if (key == 'V')	//
		{
			FLAG_ROTATE = !FLAG_ROTATE;
			a = 0;
			b = 0;
		}
	if (key == 'B')	//
		{
			FLAG_ALPHA_CANAL = !FLAG_ALPHA_CANAL;
		}
	//=========================

	if (key == '1')	//-->
		{
			FLAG_NUMBER_RABOT = 0;
		}

	if (key == '2')	//-->
		{
			FLAG_NUMBER_RABOT = 1;
		}

}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	//OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::LoadBMP("texture_1v.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR более крутотенечка
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

//---------------------------------------------------------------------------------------------------------
double PI1 = 3.1415926535897932384626433832795;

double Mas_NORMAL[]={0,0,0};
double Vektor[] = {0,0,0};
double vektor_a[] = {0,0,0};
double vektor_b[] = {0,0,0};


void Krug(double x, double y, double z, double Radius , double color1,double color2,double color3,double alpha1)
{ 
glNormal3d(0,0,1);
int Count = 100 ;
double radius_textur = (double)89/(double)1024;
double centr_textur[] = {(double)933/(double)1024,1-((double)933/(double)1024)};
 glBegin( GL_TRIANGLE_FAN );
   glColor4d(color1,color2,color3,alpha1);
   glTexCoord2d(centr_textur[0],centr_textur[1]);
   glVertex3d( x, y , z ); 
         for( int i = 0; i <= Count; i++ ) 
		 {
		   double alpha = (double)i / (double)Count * PI1 * Radius;
		   glTexCoord2d(centr_textur[0] + cos( alpha)*radius_textur,centr_textur[1] + sin( alpha )* radius_textur);
           glVertex3d( cos( alpha ) * Radius, sin( alpha ) * Radius, z );
         }
    glEnd();
}

void NORMAL(double a[], double b[])
{
	double ax = a[0];
	double ay = a[1];
	double az = a[2];

	double bx = b[0];
	double by = b[1];
	double bz = b[2];

	// i 
	double chast1 = ay*bz;
	double chast5 = az*by;
	double nx = chast1 - chast5;

	// j
	double chast2 = az*bx;
	double chast6 = ax*bz;
	double ny = (chast2 - chast6);

	//  z
	double chast3 = ax*by;
	double chast4 = ay*bx;
	double nz = (chast3 - chast4);
	
	double Dlina = sqrt(pow(nx,2) + pow(ny,2) + pow(nz,2));

	Mas_NORMAL[0] = nx/Dlina;
	Mas_NORMAL[1] = (ny)/Dlina;
	Mas_NORMAL[2] = nz/Dlina;
}

void COORD_VEKTOR(double A[],double B[])
{
	double x = A[0]-B[0];
	double y = A[1]-B[1];
	double z = A[2]-B[2];
	Vektor[0] = x; 
	Vektor[1] = y;
	Vektor[2] = z;
}

//для линий
void LINE(double ONE[], double TWO[])
{
	glBegin(GL_LINES);

	glVertex3dv(ONE);
	glVertex3dv(TWO);

	glEnd();
}
//для треугольников ПОСЛЕ ТЕКСТУР ЛЕЖИТ ЗДЕСЬ ПРОСТО ТАК
/*void TRIANGLE(double One[], double Two[], double Three[])
{
	glBegin(GL_TRIANGLES);
	glVertex3dv(One);
	glVertex3dv(Two);
	glVertex3dv(Three);
	glEnd();
}*/
//для определения длины отрезка
double Dlina_line(double A[], double B[])
{
	double peremennaya = sqrt(pow(A[0] - B[0],2) + pow(A[1] - B[1],2) + pow(A[2]-B[2],2));
	return peremennaya;
}

void Figura_One(double A[], double B[], double C[], double D[], double E[], double F[], double G[], double H[])
{
	glBegin(GL_TRIANGLES);

		//Фигура 1

	glTexCoord2d((double)701/(double)1024,1-((double)389/(double)1024));
	glVertex3dv(A);

	glTexCoord2d((double)816/(double)1024,1-((double)241/(double)1024));
	glVertex3dv(B);

	glTexCoord2d((double)934/(double)1024,1-((double)274/(double)1024));
	glVertex3dv(H);

		//Фигура 2
	
	glTexCoord2d((double)816/(double)1024,1-((double)241/(double)1024));
	glVertex3dv(B);

	glTexCoord2d((double)934/(double)1024,1-((double)274/(double)1024));
	glVertex3dv(H);

	glTexCoord2d((double)965/(double)1024,1-((double)183/(double)1024));
	glVertex3dv(D);

		//Фигура 3

	glTexCoord2d((double)934/(double)1024,1-((double)274/(double)1024));
	glVertex3dv(H);

	glTexCoord2d((double)964/(double)1024,1-((double)183/(double)1024));
	glVertex3dv(D);

	glTexCoord2d((double)1021/(double)1024,1-((double)361/(double)1024));
	glVertex3dv(E);

		//Фигура 4

	glTexCoord2d((double)936/(double)1024,1-((double)509/(double)1024));
	glVertex3dv(G);

	glTexCoord2d((double)1021/(double)1024,1-((double)509/(double)1024));
	glVertex3dv(F);

	glTexCoord2d((double)1021/(double)1024,1-((double)361/(double)1024));
	glVertex3dv(E);

	/*	//Фигура 5
	glVertex3dv(H);
	glVertex3dv(E);
	glVertex3dv(G);*/

		//Фигура 6

	glTexCoord2d((double)816/(double)1024,1-((double)241/(double)1024));
	glVertex3dv(B);

	glTexCoord2d((double)758/(double)1024,1-((double)64/(double)1024));
	glVertex3dv(C);

	glTexCoord2d((double)964/(double)1024,1-((double)183/(double)1024));
	glVertex3dv(D);


	glEnd();
}
void Figura_Two(double A[], double B[], double C[], double D[], double E[], double F[], double G[], double H[])
{	
	glBegin(GL_TRIANGLES);

		//Фигура 1

	glTexCoord2d((double)321/(double)1024,1-((double)389/(double)1024));
	glVertex3dv(A);

	glTexCoord2d((double)205/(double)1024,1-((double)241/(double)1024));
	glVertex3dv(B);

	glTexCoord2d((double)89/(double)1024,1-((double)274/(double)1024));
	glVertex3dv(H);

		//Фигура 2
	
	glTexCoord2d((double)205/(double)1024,1-((double)241/(double)1024));
	glVertex3dv(B);

	glTexCoord2d((double)89/(double)1024,1-((double)274/(double)1024));
	glVertex3dv(H);

	glTexCoord2d((double)58/(double)1024,1-((double)183/(double)1024));
	glVertex3dv(D);

		//Фигура 3

	glTexCoord2d((double)89/(double)1024,1-((double)274/(double)1024));
	glVertex3dv(H);

	glTexCoord2d((double)58/(double)1024,1-((double)183/(double)1024));
	glVertex3dv(D);

	glTexCoord2d((double)2/(double)1024,1-((double)361/(double)1024));
	glVertex3dv(E);

		//Фигура 4
	glTexCoord2d((double)87/(double)1024,1-((double)509/(double)1024));
	glVertex3dv(G);

	glTexCoord2d((double)2/(double)1024,1-((double)509/(double)1024));
	glVertex3dv(F);

	glTexCoord2d((double)2/(double)1024,1-((double)361/(double)1024));
	glVertex3dv(E);
	
	/*	//Фигура 5
	glVertex3dv(H);
	glVertex3dv(E);
	glVertex3dv(G);*/

		//Фигура 6

	glTexCoord2d((double)205/(double)1024,1-((double)241/(double)1024));
	glVertex3dv(B);

	glTexCoord2d((double)264/(double)1024,1-((double)64/(double)1024));
	glVertex3dv(C);

	glTexCoord2d((double)58/(double)1024,1-((double)183/(double)1024));
	glVertex3dv(D);


	glEnd();
}
void Soedinenie(double z, double z1)
{
	glBegin(GL_QUADS);

	double A[]={5.5,0,z}, B[]={3,2,z}, C[]={0,1,z}, D[]={2,4.5,z}, E[]={5,5.5,z}, F[]={7.5,5.5,z}, G[]={7.5,4,z}, H[]={3.5,4,z};
	double A1[]={5.5,0,z1}, B1[]={3,2,z1}, C1[]={0,1,z1}, D1[]={2,4.5,z1}, E1[]={5,5.5,z1}, F1[]={7.5,5.5,z1}, G1[]={7.5,4,z1}, H1[]={3.5,4,z1};

	//-->>Построение нормали
	COORD_VEKTOR(A,A1);
	double vektor_a1[] = {-Vektor[0],-Vektor[1],Vektor[2]};
	COORD_VEKTOR(H1,A1);
	double vektor_b1[] = {-Vektor[0],-Vektor[1],Vektor[2]};
	NORMAL(vektor_a1,vektor_b1);
	glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
	//--<<
	//К фигуре 1
	glTexCoord2d((double)2/(double)1024,1-((double)573/(double)1024));
	glVertex3dv(A1);
	glTexCoord2d((double)2/(double)1024,1-((double)514/(double)1024));
	glVertex3dv(A);
	glTexCoord2d((double)266/(double)1024,1-((double)514/(double)1024));
	glVertex3dv(H);
	glTexCoord2d((double)266/(double)1024,1-((double)573/(double)1024));
	glVertex3dv(H1);

	//-->>Построение нормали
	COORD_VEKTOR(A,A1);
	double vektor_a2[] = {Vektor[0],Vektor[1],Vektor[2]};
	COORD_VEKTOR(B1,A1);
	double vektor_b2[] = {Vektor[0],Vektor[1],Vektor[2]};
	NORMAL(vektor_a2,vektor_b2);
	glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
	//--<<

	glTexCoord2d((double)381/(double)1024,1-((double)639/(double)1024));
	glVertex3dv(A1);
	glTexCoord2d((double)381/(double)1024,1-((double)580/(double)1024));
	glVertex3dv(A);
	glTexCoord2d((double)194/(double)1024,1-((double)580/(double)1024));
	glVertex3dv(B);
	glTexCoord2d((double)194/(double)1024,1-((double)639/(double)1024));
	glVertex3dv(B1);

		//внутренняя
		/*glVertex3dv(B1);
		glVertex3dv(B);
		glVertex3dv(H);
		glVertex3dv(H1);*/

	//К фигуре 2

		//внутренняя
		/*glVertex3dv(B1);
		glVertex3dv(B);
		glVertex3dv(H);
		glVertex3dv(H1);

		//внутренняя
		glVertex3dv(B1);
		glVertex3dv(B);
		glVertex3dv(D);
		glVertex3dv(D1);

		//внутренняя
		glVertex3dv(H1);
		glVertex3dv(H);
		glVertex3dv(D);
		glVertex3dv(D1);*/

	//К фигуре 3

		//-->>Построение нормали
	COORD_VEKTOR(D,D1);
	double vektor_a3[] = {Vektor[0],Vektor[1],Vektor[2]};
	COORD_VEKTOR(E1,D1);
	double vektor_b3[] = {Vektor[0],Vektor[1],Vektor[2]};
	NORMAL(vektor_a3,vektor_b3);
	glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
	//--<<

	glTexCoord2d((double)431/(double)1024,1-((double)705/(double)1024));
	glVertex3dv(D1);
	glTexCoord2d((double)431/(double)1024,1-((double)646/(double)1024));
	glVertex3dv(D);
	glTexCoord2d((double)246/(double)1024,1-((double)646/(double)1024));
	glVertex3dv(E);
	glTexCoord2d((double)246/(double)1024,1-((double)705/(double)1024));
	glVertex3dv(E1);

		//внутренняя
		/*glVertex3dv(E1);
		glVertex3dv(E);
		glVertex3dv(H);
		glVertex3dv(H1);*/

		//внутренняя
	/*	glVertex3dv(D1);
		glVertex3dv(D);
		glVertex3dv(H);
		glVertex3dv(H1);*/

	//К фигуре 4

		//-->>Построение нормали
	COORD_VEKTOR(E,E1);
	double vektor_a4[] = {Vektor[0],Vektor[1],Vektor[2]};
	COORD_VEKTOR(F1,E1);
	double vektor_b4[] = {Vektor[0],Vektor[1],Vektor[2]};
	NORMAL(vektor_a4,vektor_b4);
	glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
	//--<<

	glTexCoord2d((double)239/(double)1024,1-((double)705/(double)1024));
	glVertex3dv(E1);
	glTexCoord2d((double)239/(double)1024,1-((double)646/(double)1024));
	glVertex3dv(E);
	glTexCoord2d((double)94/(double)1024,1-((double)646/(double)1024));
	glVertex3dv(F);
	glTexCoord2d((double)94/(double)1024,1-((double)705/(double)1024));
	glVertex3dv(F1);

	//-->>Построение нормали
	COORD_VEKTOR(F,F1);
	double vektor_a5[] = {Vektor[0],Vektor[1],Vektor[2]};
	COORD_VEKTOR(G1,F1);
	double vektor_b5[] = {Vektor[0],Vektor[1],Vektor[2]};
	NORMAL(vektor_a5,vektor_b5);
	glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
	//--<<

	glTexCoord2d((double)87/(double)1024,1-((double)705/(double)1024));
	glVertex3dv(F1);
	glTexCoord2d((double)87/(double)1024,1-((double)646/(double)1024));
	glVertex3dv(F);
	glTexCoord2d((double)2/(double)1024,1-((double)646/(double)1024));
	glVertex3dv(G);
	glTexCoord2d((double)2/(double)1024,1-((double)705/(double)1024));
	glVertex3dv(G1);

		//внутренняя
		/*glVertex3dv(G1);
		glVertex3dv(G);
		glVertex3dv(E);
		glVertex3dv(E1);*/

	//К фигуре 5
	/*glVertex3dv(G1);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(H1);*/

		//внутренняя
		/*glVertex3dv(G1);
		glVertex3dv(G);
		glVertex3dv(E);
		glVertex3dv(E1);

		//внутренняя
		glVertex3dv(E1);
		glVertex3dv(E);
		glVertex3dv(H);
		glVertex3dv(H1);*/

	//К фигуре 6
		/*
	glVertex3dv(C1);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(D1);
	*/

		//-->>Построение нормали
		COORD_VEKTOR(B,B1);
	double vektor_a6[] = {Vektor[0],Vektor[1],Vektor[2]};
	COORD_VEKTOR(C1,B1);
	double vektor_b6[] = {Vektor[0],Vektor[1],Vektor[2]};
	NORMAL(vektor_a6,vektor_b6);
	glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
	//--<<

	glTexCoord2d((double)2/(double)1024,1-((double)639/(double)1024));
	glVertex3dv(C1);
	glTexCoord2d((double)2/(double)1024,1-((double)580/(double)1024));
	glVertex3dv(C);
	glTexCoord2d((double)187/(double)1024,1-((double)580/(double)1024));
	glVertex3dv(B);
	glTexCoord2d((double)187/(double)1024,1-((double)639/(double)1024));
	glVertex3dv(B1);

		//внутренняя
		/*glVertex3dv(B1);
		glVertex3dv(B);
		glVertex3dv(D);
		glVertex3dv(D1);*/

	glEnd();
}

void Okrugnost(double D[], double C[], double M[], double O[], double R)
{
	double z = D[2];
	double z1 = D[2]-1;

	double radius_textur =0.11620492839524828891858820954788;
	double centr_textur[] = {0.15879296875, 1-0.1233173828125};

	//-->> Вычисление углов
	double UgolD = acos((pow(Dlina_line(C, O), 2) + pow(Dlina_line(D, O), 2) - pow(Dlina_line(C, D), 2)) / (2 * Dlina_line(C, O)*Dlina_line(D, O)));
	double Dopka[] = { O[0] - R,O[1], O[2] };
	double UgolC = acos((pow(Dlina_line(Dopka, O), 2) + pow(Dlina_line(D, O), 2) - pow(Dlina_line(Dopka, D), 2)) / (2 * Dlina_line(Dopka, O)*Dlina_line(D, O))/*-0.055*/);
	//--<<

	double D1[] = { D[0],D[1],z1};
	double C1[] = { C[0],C[1],z1};
	double M1[] = { M[0],M[1],z1};
	double O1[] = { O[0],O[1],z1};

	double Tocka_begin[] = { D[0],D[1],D[2] };
	double Tocka_begin_down[] = { D1[0],D1[1],D1[2] };

	double TEXTURE_Tocka_begin[] = {(double)965/(double)1024,1-((double)183/(double)1024) };
	double TEXTURE_Tocka_begin_down[] = { (double)58/(double)1024,1-((double)183/(double)1024) };

	double LATERAL_TEXTURE_begin[] = {(double)2/(double)1024,1-((double)778/(double)1024) };
	double LATERAL_TEXTURE_begin_down[] = {(double)2/(double)1024,1-((double)837/(double)1024) };

	double j = UgolC+0.5;

	double flag_podscheta = 0;

	for (double i = acos(-1) - UgolC ; i < acos(-1) - UgolC  + UgolD; i+=0.0005)
	{
		glBegin(GL_TRIANGLES);

		flag_podscheta+=1;

		//-->>Построение нормали
		glNormal3d(0,0,1);
		//--<<

		//-->>Верх
		double Tochka_real[] = { 0,0,z};
		Tochka_real[0] = O[0] + R*cos(i);		//приращение координат
		Tochka_real[1] = O[1] + R*sin(i);

		double TEXTURE_Tochka_real[] = { 0,0};
		TEXTURE_Tochka_real[0] = centr_textur[0] - radius_textur*cos(j);	
		TEXTURE_Tochka_real[1] = centr_textur[1] - radius_textur*sin(j);
		
		//=================================
		//-->>Построение нормали
		glNormal3d(0,0,-1);
		//--<<
		glColor4d(0.58,0,0.827,1);
		//-->>Низ
		double Tochka_real_down[] = { 0,0,z1};
		Tochka_real_down[0] = O1[0] + R*cos(i);		//приращение координат
		Tochka_real_down[1] = O1[1] + R*sin(i);
		
		double TEXTURE_Tochka_real_down[] = { 0,0};
		j+=0.000517;
		TEXTURE_Tochka_real_down[0] = centr_textur[0] + radius_textur*cos(-j);	
		TEXTURE_Tochka_real_down[1] = centr_textur[1] - radius_textur*sin(j);


		double LATERAL_TEXTURE_real[] ={0,LATERAL_TEXTURE_begin[1]};
		double LATERAL_TEXTURE_real_down[] = {0,LATERAL_TEXTURE_begin_down[1]};
		LATERAL_TEXTURE_real[0] = (LATERAL_TEXTURE_begin[0] + ((0.35820595322550424400054552416517/6155.0))); 
		//!!!Должно делиться на 6155(количество построений) - ошибка в расчете радиуса и длины окружности
		LATERAL_TEXTURE_real_down[0] = LATERAL_TEXTURE_real[0] ;

		//=================================
		glTexCoord2d((double)58/(double)1024,1-((double)183/(double)1024));
		glVertex3dv(D1);

		glTexCoord2d(TEXTURE_Tocka_begin_down[0],TEXTURE_Tocka_begin_down[1]);
		glVertex3dv(Tocka_begin_down);

		glTexCoord2d(TEXTURE_Tochka_real_down[0],TEXTURE_Tochka_real_down[1]);
		glVertex3dv(Tochka_real_down);

		//==================================

		//--<<




		//-->>Построение нормали
		COORD_VEKTOR(Tocka_begin,Tocka_begin_down);
		double vektor_a1[] = {-Vektor[0],-Vektor[1],Vektor[2]};
		COORD_VEKTOR(Tochka_real_down,Tocka_begin_down);
		double vektor_b1[] = {-Vektor[0],-Vektor[1],Vektor[2]};
		NORMAL(vektor_a1,vektor_b1);
		glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
		//--<<
		glColor4d(0.933,0.416,0.314,1);
		//-->>Боковые 

		//=================================

		glTexCoord2d(LATERAL_TEXTURE_begin[0],LATERAL_TEXTURE_begin[1]);
		glVertex3dv(Tocka_begin); //2

		glTexCoord2d(LATERAL_TEXTURE_real[0],LATERAL_TEXTURE_real[1]);
		glVertex3dv(Tochka_real); //3

		glTexCoord2d(LATERAL_TEXTURE_begin_down[0],LATERAL_TEXTURE_begin_down[1]);
		glVertex3dv(Tocka_begin_down); //1

		//==================================

		//=================================

		glTexCoord2d(LATERAL_TEXTURE_real[0],LATERAL_TEXTURE_real[1]);
		glVertex3dv(Tochka_real);
		
		glTexCoord2d(LATERAL_TEXTURE_real_down[0],LATERAL_TEXTURE_real_down[1]);
		glVertex3dv(Tochka_real_down); //4

		glTexCoord2d(LATERAL_TEXTURE_begin_down[0],LATERAL_TEXTURE_begin_down[1]);
		glVertex3dv(Tocka_begin_down);

		//==================================

		//--<<



				//-->>Переопределение точек   //==== переход к след. шагу
					Tocka_begin[0] = Tochka_real[0];
					Tocka_begin[1] = Tochka_real[1];
					Tocka_begin_down[0] = Tochka_real_down[0];
					Tocka_begin_down[1] = Tochka_real_down[1];


					TEXTURE_Tocka_begin[0] = TEXTURE_Tochka_real[0];
					TEXTURE_Tocka_begin[1] = TEXTURE_Tochka_real[1];
					TEXTURE_Tocka_begin_down[0] = TEXTURE_Tochka_real_down[0];
					TEXTURE_Tocka_begin_down[1] = TEXTURE_Tochka_real_down[1];


					LATERAL_TEXTURE_begin[0] = LATERAL_TEXTURE_real[0];
					LATERAL_TEXTURE_begin_down[0] = LATERAL_TEXTURE_real_down[0];

				//--<<
		glEnd();
	}
}

void Okrugnost2(double G[], double H[], double E[], double O[], double R)
{
	double z = G[2];
	double z1 = G[2]-1;

	double radius_textur =0.25448546786231290721832370979896;
	double centr_textur[] = { 0.3130751953125, 1-0.3842568359375};

	//-->> Вычисление углов
	double UgolG = acos((pow(Dlina_line(H, O), 2) + pow(Dlina_line(G, O), 2) - pow(Dlina_line(H, G), 2)) / (2 * Dlina_line(H, O)*Dlina_line(G, O)));
		double Dopka[] = { O[0] - R,O[1], O[2] };
	double UgolD = acos((pow(Dlina_line(Dopka, O), 2) + pow(Dlina_line(G, O), 2) - pow(Dlina_line(Dopka, G), 2)) / (2 * Dlina_line(Dopka, O)*Dlina_line(G, O))/*-0.055*/);
	//--<<


	double G1[] = { G[0],G[1],z1};
	double H1[] = { H[0],H[1],z1};
	double E1[] = { E[0],E[1],z1};
	double O1[] = { O[0],O[1],z1};

	double Tocka_begin[] = { G[0],G[1],G[2] };
	double Tocka_begin_down[] = { G1[0],G1[1],G1[2] };

	double TEXTURE_Tocka_begin_down[] = {(double)87/(double)1024,1-((double)509/(double)1024) };

	double LATERAL_TEXTURE_begin[] = {(double)249/(double)1024,1-((double)712/(double)1024) };
	double LATERAL_TEXTURE_begin_down[] = {(double)249/(double)1024,1-((double)771/(double)1024) };

	double j = UgolD+1.55;

	double flag_poscheta_2_0 = 0;

	for (double i = acos(1) + UgolD ; i < acos(1) + UgolD  + UgolG; i+=0.0005)
	{
		glBegin(GL_TRIANGLES);

		flag_poscheta_2_0+=1;

		//-->>Построение нормали
		glNormal3d(0,0,1);
		//--<<

		//-->>Верх
		double Tochka_real[] = { 0,0,z};
		Tochka_real[0] = O[0] + R*cos(i);		//приращение координат
		Tochka_real[1] = O[1] + R*sin(i);

		//--<<

		//-->>Построение нормали
		glNormal3d(0,0,-1);
		//--<<
		glColor4d(0.58,0,0.827,1);
		//-->>Низ
		double Tochka_real_down[] = { 0,0,z1};
		Tochka_real_down[0] = O1[0] + R*cos(i);		//приращение координат
		Tochka_real_down[1] = O1[1] + R*sin(i);
		
		double TEXTURE_Tochka_real_down[] = { 0,0};
		j+=0.000515;
		TEXTURE_Tochka_real_down[0] = centr_textur[0] + radius_textur*cos(j);	
		TEXTURE_Tochka_real_down[1] = centr_textur[1] - radius_textur*sin(j);
		

		double LATERAL_TEXTURE_real[] ={0,LATERAL_TEXTURE_begin[1]};
		double LATERAL_TEXTURE_real_down[] = {0,LATERAL_TEXTURE_begin_down[1]};
		LATERAL_TEXTURE_real[0] = (LATERAL_TEXTURE_begin[0] - ((0.24554400239396859594170461161966/2000.0))); 
		//!!!Должно делиться на 1960(количество построений) - ошибка в расчете радиуса и длины окружности
		LATERAL_TEXTURE_real_down[0] = LATERAL_TEXTURE_real[0] ;



		
		if (i >= acos(1) + UgolD + 0.0005){



		//=================================
		glTexCoord2d((double)2/(double)1024,1-((double)361/(double)1024));
		glVertex3dv(E1);

		glTexCoord2d(TEXTURE_Tocka_begin_down[0],TEXTURE_Tocka_begin_down[1]);
		glVertex3dv(Tocka_begin_down);

		glTexCoord2d(TEXTURE_Tochka_real_down[0],TEXTURE_Tochka_real_down[1]);
		glVertex3dv(Tochka_real_down);

		//==================================
		}
		//--<<





		//-->>Построение нормали
		COORD_VEKTOR(Tocka_begin,Tocka_begin_down);
		double vektor_a1[] = {Vektor[0],Vektor[1],Vektor[2]};
		COORD_VEKTOR(Tochka_real_down,Tocka_begin_down);
		double vektor_b1[] = {Vektor[0],Vektor[1],Vektor[2]};
		NORMAL(vektor_a1,vektor_b1);
		glNormal3d(Mas_NORMAL[0],Mas_NORMAL[1],Mas_NORMAL[2]);
		//--<<
		glColor4d(0.933,0.416,0.314,1);
		//-->>Боковые 
		if (i >= acos(1) + UgolD + 0.0005){

		//=================================
		
		glTexCoord2d(LATERAL_TEXTURE_begin[0],LATERAL_TEXTURE_begin[1]);
		glVertex3dv(Tocka_begin);
		
		glTexCoord2d(LATERAL_TEXTURE_real[0],LATERAL_TEXTURE_real[1]);
		glVertex3dv(Tochka_real);

		glTexCoord2d(LATERAL_TEXTURE_begin_down[0],LATERAL_TEXTURE_begin_down[1]);
		glVertex3dv(Tocka_begin_down);

		//==================================

		//=================================

		glTexCoord2d(LATERAL_TEXTURE_real[0],LATERAL_TEXTURE_real[1]);
		glVertex3dv(Tochka_real);

		glTexCoord2d(LATERAL_TEXTURE_real_down[0],LATERAL_TEXTURE_real_down[1]);
		glVertex3dv(Tochka_real_down);

		glTexCoord2d(LATERAL_TEXTURE_begin_down[0],LATERAL_TEXTURE_begin_down[1]);
		glVertex3dv(Tocka_begin_down);

		//==================================

		}

		//--<<
				//-->>Переопределение точек
					Tocka_begin[0] = Tochka_real[0];
					Tocka_begin[1] = Tochka_real[1];
					Tocka_begin_down[0] = Tochka_real_down[0];
					Tocka_begin_down[1] = Tochka_real_down[1];
				//--<<

					TEXTURE_Tocka_begin_down[0] = TEXTURE_Tochka_real_down[0];
					TEXTURE_Tocka_begin_down[1] = TEXTURE_Tochka_real_down[1];




				LATERAL_TEXTURE_begin[0] = LATERAL_TEXTURE_real[0];
				LATERAL_TEXTURE_begin_down[0] = LATERAL_TEXTURE_real_down[0];


		glEnd();
	}
}
//---------------------------------------------------------------------------------------------------------------
void Okrugnost_VERCH(double D[], double C[], double M[], double O[], double R)
{
	double z = D[2];
	double z1 = D[2]-1;

	double radius_textur =0.11602109155793422;
	double centr_textur[] = {0.8406708984375, 1-0.1228935546875};

	//-->> Вычисление углов
	double UgolD = acos((pow(Dlina_line(C, O), 2) + pow(Dlina_line(D, O), 2) - pow(Dlina_line(C, D), 2)) / (2 * Dlina_line(C, O)*Dlina_line(D, O)));
	double Dopka[] = { O[0] - R,O[1], O[2] };
	double UgolC = acos((pow(Dlina_line(Dopka, O), 2) + pow(Dlina_line(D, O), 2) - pow(Dlina_line(Dopka, D), 2)) / (2 * Dlina_line(Dopka, O)*Dlina_line(D, O))/*-0.055*/);
	//--<<

	double Tocka_begin[] = { D[0],D[1],D[2] };

	double TEXTURE_Tocka_begin[] = {(double)965/(double)1024,1-((double)183/(double)1024) };

	double j = UgolC+0.5;

	double flag_podscheta01 = 0;

	for (double i = acos(-1) - UgolC ; i < acos(-1) - UgolC  + UgolD; i+=0.0005)
	{
		glBegin(GL_TRIANGLES);

		flag_podscheta01+=1;

		//-->>Построение нормали
		glNormal3d(0,0,1);
		//--<<

		//-->>Верх
		double Tochka_real[] = { 0,0,z};
		Tochka_real[0] = O[0] + R*cos(i);		//приращение координат
		Tochka_real[1] = O[1] + R*sin(i);

		double TEXTURE_Tochka_real[] = { 0,0};
		j+=0.000517;
		TEXTURE_Tochka_real[0] = centr_textur[0] - radius_textur*cos(j);	
		TEXTURE_Tochka_real[1] = centr_textur[1] - radius_textur*sin(j);
		
		//=================================

		glTexCoord2d((double)965/(double)1024,1-((double)183/(double)1024));
		glVertex3dv(D);

		glTexCoord2d(TEXTURE_Tocka_begin[0],TEXTURE_Tocka_begin[1]);
		glVertex3dv(Tocka_begin);

		glTexCoord2d(TEXTURE_Tochka_real[0],TEXTURE_Tochka_real[1]);
		glVertex3dv(Tochka_real);

		//==================================

		//--<<

		
				//-->>Переопределение точек   //==== переход к след. шагу
					Tocka_begin[0] = Tochka_real[0];
					Tocka_begin[1] = Tochka_real[1];

					TEXTURE_Tocka_begin[0] = TEXTURE_Tochka_real[0];
					TEXTURE_Tocka_begin[1] = TEXTURE_Tochka_real[1];
					//Tocka_begin_down[0] = Tochka_real_down[0];
					//Tocka_begin_down[1] = Tochka_real_down[1];



				//--<<
		glEnd();
	}
}

void Okrugnost2_VERCH(double G[], double H[], double E[], double O[], double R)
{
	double z = G[2];
	double z1 = G[2]-1;

	double radius_textur =0.24666838623234279848626946234131;
	double centr_textur[] = {0.6947412109375 , 1-0.3841826171875};

	//-->> Вычисление углов
	double UgolG = acos((pow(Dlina_line(H, O), 2) + pow(Dlina_line(G, O), 2) - pow(Dlina_line(H, G), 2)) / (2 * Dlina_line(H, O)*Dlina_line(G, O)));
		double Dopka[] = { O[0] - R,O[1], O[2] };
	double UgolD = acos((pow(Dlina_line(Dopka, O), 2) + pow(Dlina_line(G, O), 2) - pow(Dlina_line(Dopka, G), 2)) / (2 * Dlina_line(Dopka, O)*Dlina_line(G, O))/*-0.055*/);
	//--<<

	double Tocka_begin[] = { G[0],G[1],G[2] };

	double TEXTURE_Tocka_begin[] = {(double)936/(double)1024,1-((double)509/(double)1024) };

	double j = UgolD+1.55;

	double flag_poschata_2_1=0;

	for (double i = acos(1) + UgolD ; i < acos(1) + UgolD  + UgolG; i+=0.0005)
	{
		glBegin(GL_TRIANGLES);

		flag_poschata_2_1+=1;

		//-->>Построение нормали
		glNormal3d(0,0,1);
		//--<<

		//-->>Верх
		double Tochka_real[] = { 0,0,z};
		Tochka_real[0] = O[0] + R*cos(i);		//приращение координат
		Tochka_real[1] = O[1] + R*sin(i);

		double TEXTURE_Tochka_real[] = { 0,0};
		j+=0.000515;
		TEXTURE_Tochka_real[0] = centr_textur[0] - radius_textur*cos(j);	
		TEXTURE_Tochka_real[1] = centr_textur[1] - radius_textur*sin(j);

		if (i >= acos(1) + UgolD + 0.0005){

		//=================================

		glTexCoord2d((double)1021/(double)1024,1-((double)361/(double)1024));
		glVertex3dv(E);

		glTexCoord2d(TEXTURE_Tocka_begin[0],TEXTURE_Tocka_begin[1]);
		glVertex3dv(Tocka_begin);

		glTexCoord2d(TEXTURE_Tochka_real[0],TEXTURE_Tochka_real[1]);
		glVertex3dv(Tochka_real);

		//==================================
		}

		//--<<
				//-->>Переопределение точек
					Tocka_begin[0] = Tochka_real[0];
					Tocka_begin[1] = Tochka_real[1];

					TEXTURE_Tocka_begin[0] = TEXTURE_Tochka_real[0];
					TEXTURE_Tocka_begin[1] = TEXTURE_Tochka_real[1];
					
				//--<<

		glEnd();
	}
}
//==================================================================================================

void KRIVAYA_Bese(double A[], double B[], double C[], double D[], double Color1, double Color2, double Color3)
{
	double P[] = {0,0,0};

	glLineWidth(3);

	glBegin(GL_LINE_STRIP);

	//glEnable(GL_LINE_STIPPLE);
	glColor3d(0.1,0.1,0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	//glDisable(GL_LINE_STIPPLE);

	glColor3d(Color1,Color2,Color3);
	
	for (double t = 0.0; t <= 1.0; t += 0.01)
		{
			double Dop3 = (1-t);
			double Dop2 = pow((1-t),2);
			double Dop1 = pow((1-t),3);

			P[0] = Dop1*A[0] + 3*t*Dop2*B[0] + 3*t*t*Dop3*C[0] +  t*t*t*D[0];
			P[1] = Dop1*A[1] + 3*t*Dop2*B[1] + 3*t*t*Dop3*C[1] +  t*t*t*D[1];
			P[2] = Dop1*A[2] + 3*t*Dop2*B[2] + 3*t*t*Dop3*C[2] +  t*t*t*D[2];

			glVertex3dv(P);

		}



	glEnd();
}

void Postroenie_vektorov(double A[], double B[], double R1[], double R2[])
{
	glBegin(GL_LINES);
	glPushMatrix();
	glVertex2dv(A);glVertex2dv(R1);
	glPopMatrix();
	glPushMatrix();
	glVertex2dv(B);glVertex2dv(R2);
	glPushMatrix();
	glEnd();
}

void KRIVAYA_Ermit(double A[], double B[], double R1[], double R2[], double Color1, double Color2, double Color3)
{
	//===========================

	double vector1[] = {(R1[0]-A[0]),(R1[1]-A[1])};
	double vector2[] = {(R2[0]-B[0]),(R2[1]-B[1])};

	//===========================

	double P[] = {0,0};

	glLineWidth(3);

	glColor3d(0.1,0.1,0.1);
	Postroenie_vektorov(A,B,R1,R2);

	glBegin(GL_LINE_STRIP);

	//
	

	glColor3d(Color1,Color2,Color3);
	
	for (double t = 0.0; t <= 1.0; t += 0.01)
		{
			double Dop4 = (pow(t,3)-pow(t,2));
			double Dop3 = (pow(t,3)-2*pow(t,2)+t);
			double Dop2 = (-2*pow(t,3)+3*pow(t,2));
			double Dop1 = (2*pow(t,3)-3*pow(t,2)+1);

			P[0] = A[0]*Dop1 + B[0]*Dop2 + vector1[0]*Dop3 + vector2[0]*Dop4;
			P[1] = A[1]*Dop1 + B[1]*Dop2 + vector1[1]*Dop3 + vector2[1]*Dop4;

			glVertex2dv(P);

		}

	glEnd();



}

void KUBIK_1(double Coord_X, double Coord_Y, double Coord_Z, double storona) //координаты центра!!!
{
	double polustorona = storona/2.0;

	double A_V[]={Coord_X+polustorona, Coord_Y-polustorona, Coord_Z+polustorona},B_V[]={Coord_X-polustorona, Coord_Y-polustorona, Coord_Z+polustorona},C_V[]={Coord_X-polustorona*2, Coord_Y+polustorona * 2, Coord_Z+polustorona},D_V[]={Coord_X+polustorona, Coord_Y+polustorona, Coord_Z+polustorona};
	double A_N[]={Coord_X+polustorona, Coord_Y-polustorona, Coord_Z-polustorona},B_N[]={Coord_X-polustorona, Coord_Y-polustorona, Coord_Z-polustorona*2.5},C_N[]={Coord_X-polustorona, Coord_Y+polustorona, Coord_Z-polustorona },D_N[]={Coord_X+polustorona, Coord_Y+polustorona, Coord_Z-polustorona*2.5 };

	glColor4d(0.761,0.188,0.173,1.0);

	glBegin(GL_QUADS);
		//верх
	glNormal3d(0,0,1);
	glVertex3dv(A_V);
	glVertex3dv(B_V);
	glVertex3dv(C_V);
	glVertex3dv(D_V);
		//низ
	glNormal3d(0,0,-1);
	glVertex3dv(A_N);
	glVertex3dv(B_N);
	glVertex3dv(C_N);
	glVertex3dv(D_N);
		//бок1
	glNormal3d(0,-1,0);
	glVertex3dv(A_V);
	glVertex3dv(B_V);
	glVertex3dv(B_N);
	glVertex3dv(A_N);
		//бок2
	glNormal3d(-1,0,0);
	glVertex3dv(B_V);
	glVertex3dv(C_V);
	glVertex3dv(C_N);
	glVertex3dv(B_N);
		//бок3
	glNormal3d(0,1,0);
	glVertex3dv(C_V);
	glVertex3dv(D_V);
	glVertex3dv(D_N);
	glVertex3dv(C_N);
		//бок4
	glNormal3d(1,0,0);
	glVertex3dv(D_V);
	glVertex3dv(A_V);
	glVertex3dv(A_N);
	glVertex3dv(D_N);
		
		
	glEnd();

}

void OBJECT_01(double Coord_X, double Coord_Y, double Coord_Z,double storona_kubika/*единичная*/)
{
	glPushMatrix();
	glRotated(-90,0,0,1);
	KUBIK_1(Coord_X,Coord_Y,Coord_Z,storona_kubika);//центр
	double storona_kubika_1 = storona_kubika/1.2;
	KUBIK_1(Coord_X+storona_kubika_1+storona_kubika/7.0,Coord_Y,Coord_Z,storona_kubika_1);
	double storona_kubika_2 = storona_kubika/1.35;
	KUBIK_1(Coord_X+storona_kubika_1+storona_kubika_1+storona_kubika/10+storona_kubika/10,Coord_Y,Coord_Z,storona_kubika_2);//нос
	double storona_kubika_3 = storona_kubika*1.1;
	KUBIK_1(Coord_X-storona_kubika-storona_kubika/10,Coord_Y,Coord_Z,storona_kubika_3);//задний блок
	double storona_kubika_4 = storona_kubika/1.9;
	KUBIK_1(Coord_X,Coord_Y-storona_kubika-storona_kubika/23.0,Coord_Z,storona_kubika_4);
	KUBIK_1(Coord_X,Coord_Y+storona_kubika+storona_kubika/23.0,Coord_Z,storona_kubika_4);
	KUBIK_1(Coord_X,Coord_Y,Coord_Z-storona_kubika-storona_kubika/23.0,storona_kubika_4);
	KUBIK_1(Coord_X,Coord_Y,Coord_Z+storona_kubika+storona_kubika/23.0,storona_kubika_4);
	glPopMatrix();
}


double t = 0.0; 
int izm_one = 1;
double Tochka_begin[] = { 0,0,0 };
double Tochka_real[] = { 0,0,0 };

void DVIG_Bezie(double A[], double B[], double C[], double D[], double Point[])
{
	Point[0] = pow(1 - t, 3)*A[0] + 3 * t*pow(1 - t, 2)*B[0] + 3 * t*t*(1 - t)*C[0] + t*t*t*D[0];
	Point[1] = pow(1 - t, 3)*A[1] + 3 * t*pow(1 - t, 2)*B[1] + 3 * t*t*(1 - t)*C[1] + t*t*t*D[1];
	Point[2] = pow(1 - t, 3)*A[2] + 3 * t*pow(1 - t, 2)*B[2] + 3 * t*t*(1 - t)*C[2] + t*t*t*D[2];
	t += 0.005 * izm_one;
}

//================================================================================================================
//================================================================================================================

void Render(OpenGL *ogl)
{       	
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_ALPHA_TEST);


	//настройка материала
	/*GLfloat amb[] = { 0.2, 0.2, 0.1, 1. }; //материал
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };*/
	//-----------My
	GLfloat amb[] = { 0.3, 0.3, 0.3, 1. }; //материал
	GLfloat dif[] = { 0.902, 0.902, 0.902, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };


	GLfloat sh = 0.1f * 256;
	///-------------------My
	GLfloat em[] = {0, 0, 0, 1.0};
	//GLfloat em[] = {0.184, 0.02, 0.412, 0.5};//СИРЕНЕВЫЙ СВЕТ ОТ ПРИЗМЫ

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);
	//-----------------my
	glMaterialfv(GL_FRONT, GL_EMISSION, em);

    //чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  

	
		//Тудунь-сюдунь
	glTranslated(X_D,Y_D,Z_D);
	
	double z = 2;
	double z1 = 1;
	double zDOP = z+1;

	double A[]={5.5,0,z}, B[]={3,2,z}, C[]={0,1,z}, D[]={2,4.5,z}, E[]={5,5.5,z}, F[]={7.5,5.5,z}, G[]={7.5,4,z}, H[]={3.5,4,z} , M[]={-0.5,4,z} , N[]={5.5,4.5,z}; 
	double A1[]={5.5,0,z1}, B1[]={3,2,z1}, C1[]={0,1,z1}, D1[]={2,4.5,z1}, E1[]={5,5.5,z1}, F1[]={7.5,5.5,z1}, G1[]={7.5,4,z1}, H1[]={3.5,4,z1} , M1[]={-0.5,4,z1} , N1[]={5.5,4.5,z}; 
	double CenterOkr[]= {1.05645161290322,2.71774193548387,z}, CenterOkr2[]= {5.5,0.25,z};
	double Radius = 2.0166128451752217286124510401587 , Radius2 = 4.2499999999999992241563975935144;

	if (FLAG_NUMBER_RABOT == 0){
	glPushMatrix();
	glTranslated(3,3.5,z-0.5);
	glScaled(0.2,0.2,0.2);
	Krug(0,0,0,2 , 1,1,1,1.0);
	glPopMatrix();
	//=======	
		
		glPushMatrix();
		glTranslatef (0,0,0);
		glTranslated(0,0,z+1);
		//===============================================
		//Красотуля))---
		if (FLAG_ROTATE == 1)
		glRotatef(b+=2, 0,0,1);

		glTranslatef (0,0,0);
		Krug(0,0,1,2 , 0.5,0.5,0.5,1.0);
		glPopMatrix();
//================================================


	glPushMatrix();
	glTranslatef (3,3.5,0);
  //================
	//Красотуля))---
		if (FLAG_ROTATE == 1)
				glRotatef(a+=0.5, 0,0,1);
	glTranslatef (-3,-3.5,0);
  //===

	glColor4d(0.5,0.5,0.5,1);
	glNormal3d(0,0,-1);
	Figura_Two(A1,B1,C1,D1,E1,F1,G1,H1);

	glColor4d(0.5,0.9,0.5,1);
	Soedinenie(z,z1);

	glColor4d(0,0,0,1);
	glColor4d(0.933,0.416,0.314,1);
	Okrugnost(D, C, M, CenterOkr, Radius);
	Okrugnost2(H, G, E, CenterOkr2, Radius2);
	//Okrugnost_VERCH(D, C, M, CenterOkr, Radius);

	if (FLAG_ALPHA_CANAL == 0) 
		{
		glColor4d(0.2,0.3,0.6,1);
		glNormal3d(0,0,1);
		Figura_One(A,B,C,D,E,F,G,H);
		Okrugnost_VERCH(D, C, M, CenterOkr, Radius);
		Okrugnost2_VERCH(H, G, E, CenterOkr2, Radius2);
	}

	//========================

	if (FLAG_ALPHA_CANAL == 1)
	{glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_ALPHA_TEST);


	glColor4d(0.2,0.3,0.6,0.5);
	glNormal3d(0,0,1);
	Figura_One(A,B,C,D,E,F,G,H);
	Okrugnost_VERCH(D, C, M, CenterOkr, Radius);
	Okrugnost2_VERCH(H, G, E, CenterOkr2, Radius2);
	
	//glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	}
	//=========================

	glPopMatrix();
	}
	//glDisable(GL_ALPHA_TEST);
	//glDisable(GL_BLEND);

	//================================================================================================================

	if (FLAG_NUMBER_RABOT == 1)
	{
		//double A[]={4,0}, B[]={1,0.5}, C[]={4.5,4.5},D[]={0,6}, E[]={5,7}, F[]={5.5,5};
		double /*A[]={0,0,0}, B[]={-15,20,40}, C[]={25,50,-10},D[]={-7,-6,10},*/ E[]={5,7}, F[]={5.5,5};

		double A1[]={1.5,1.5},A1R1[]={-0.5*4,3*3}, B1[]={4,3.5},B1R2[]={1,4.5};
	
		double C1[]={2,6},C1R1[]={1.5*1.5,7.5*1.5}, D1[]={5,4.5},D1R2[]={5.5,6*2};

		glPushMatrix();

	double A[] = { 0,0,0 };
	double B[] = { -15*1.5,20 * 1.5,40 * 1.5 };
	double C[] = { 25 * 1.5,50 * 1.5,-10 * 1.5 };
	double D[] = { -7,-6,10 };

	if (FLAG_ALPHA_CANAL == 1)
	{
		DVIG_Bezie(A, B, C, D, Tochka_real);
		if (t >= 1.0 || t <= 0)
			izm_one *= -1;  //порядок изменения направления движения
		glPushMatrix();
			double Vecktor_[] = { Tochka_real[0] - Tochka_begin[0],Tochka_real[1] - Tochka_begin[1],Tochka_real[2] - Tochka_begin[2] };
			Vecktor_[0] /= pow(pow(Vecktor_[0], 2) + pow(Vecktor_[1], 2) + pow(Vecktor_[2], 2), 0.5);
			Vecktor_[1] /= pow(pow(Vecktor_[0], 2) + pow(Vecktor_[1], 2) + pow(Vecktor_[2], 2), 0.5);
			Vecktor_[2] /= pow(pow(Vecktor_[0], 2) + pow(Vecktor_[1], 2) + pow(Vecktor_[2], 2), 0.5);
			glTranslated(Tochka_real[0], Tochka_real[1], Tochka_real[2]);
			double Ugol = acos(Vecktor_[2] / (pow((pow(Vecktor_[0], 2) + pow(Vecktor_[1], 2) + pow(Vecktor_[2], 2)), 0.5)));

			glPushMatrix();
				glRotated(Ugol * 180 / PI, -Vecktor_[1], Vecktor_[0], 0);
							/*glTranslated(0,-(0+1+0.5+1.0/10.0),0);
							  glRotated(90, 1, 0, 0);
							  OBJECT_01(0,0,0,1.0);*/
						KUBIK_1(0,0,0,1.2);

			glPopMatrix();
		glPopMatrix();

		Tochka_begin[0] = Tochka_real[0];
		Tochka_begin[1] = Tochka_real[1];
		Tochka_begin[2] = Tochka_real[2];

	}
		glPopMatrix();


	glPushMatrix();

	KUBIK_1(0,0,0,0.35);//передаем координаты центра + размер единичной стороны

	KRIVAYA_Bese(A,B,C,D,	0.341,0.078,0.667);
	KRIVAYA_Bese(D,C,E,F,	0.851,0.353,0.502);

	glTranslated(0,0,2);

	KRIVAYA_Ermit(A1,B1,A1R1,B1R2,	0.341,0.078,0.667);
	KRIVAYA_Ermit(C1,D1,C1R1,D1R2,	0.851,0.353,0.502);

	glPopMatrix();

	}
	//================================================================================================================

	//текст сообщения вверху слева, если надоест - закоментировать, или заменить =)
	char c[500];  //максимальная длина сообщения
	sprintf_s(c, "(T)Текстуры - %d\n(L)Свет - %d\n\nУправление светом:\n"
		"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
		"R - установить камеру и свет в начальное положение\n"
		"F - переместить свет в точку камеры\n\n"
		"Special:\n"
		"V - вращение (beta)    B - прозрачность крышки\n"
		"1 - Призма	                  2 - Кривые\n", textureMode, lightMode);
	ogl->message = std::string(c);

}   //конец тела функции

